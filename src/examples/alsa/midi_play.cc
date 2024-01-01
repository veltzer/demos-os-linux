/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2024 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * demos-linux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * demos-linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with demos-linux. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>	// for fprintf(3)
#include <unistd.h>	// for sleep(3)
#include <stdlib.h>	// for EXIT_FAILURE, EXIT_SUCCESS
#include <signal.h>	// for signal(2)
#include <alsa/asoundlib.h>
#include <err_utils.h>	// for CHECK_ZERO(), CHECK_NOT_M1()

/*
 * An example of playing midi.
 *
 * Before running this example make sure that midi works for you.
 * 1. Make sure you have timidity running.
 *	s -ef | grep timidity
 * 2. Find the timidity ports (usually 128:0-4)
 *	aplaymidi -l
 * 3. play the demo midi file.
 *	aplaymidi -p 128:0 data/sample.midi
 *
 * The original example was taken from:
 * http://alsamodular.sourceforge.net/miniArp.c
 * http://alsamodular.sourceforge.net/alsa_programming_howto.html
 * EXTRA_LINK_FLAGS_AFTER=-lasound
 */

const int TICKS_PER_QUARTER=128;
const int MAX_SEQ_LEN=64;

static int queue_id, port_in_id, port_out_id, transpose, bpm0, bpm, tempo, swing, sequence[3][MAX_SEQ_LEN], seq_len;
static snd_seq_t *seq_handle;
static char* seq_filename;
static snd_seq_tick_time_t tick;

snd_seq_t *open_seq() {
	snd_seq_t *seq_handle;
	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
		fprintf(stderr, "Error opening ALSA sequencer.\n");
		exit(EXIT_FAILURE);
	}
	snd_seq_set_client_name(seq_handle, "miniArp");
	if ((port_out_id=snd_seq_create_simple_port(seq_handle, "miniArp", SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ, SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
		fprintf(stderr, "Error creating sequencer port.\n");
		exit(EXIT_FAILURE);
	}
	if ((port_in_id=snd_seq_create_simple_port(seq_handle, "miniArp", SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE, SND_SEQ_PORT_TYPE_APPLICATION)) < 0) {
		fprintf(stderr, "Error creating sequencer port.\n");
		exit(EXIT_FAILURE);
	}
	return(seq_handle);
}

void set_tempo() {
	snd_seq_queue_tempo_t *queue_tempo;
	snd_seq_queue_tempo_malloc(&queue_tempo);
	tempo=(int)(6e7 / ((double)bpm * (double)TICKS_PER_QUARTER) * (double)TICKS_PER_QUARTER);
	snd_seq_queue_tempo_set_tempo(queue_tempo, tempo);
	snd_seq_queue_tempo_set_ppq(queue_tempo, TICKS_PER_QUARTER);
	snd_seq_set_queue_tempo(seq_handle, queue_id, queue_tempo);
	snd_seq_queue_tempo_free(queue_tempo);
}

snd_seq_tick_time_t get_tick() {
	snd_seq_queue_status_t *status;
	snd_seq_tick_time_t current_tick;
	snd_seq_queue_status_malloc(&status);
	snd_seq_get_queue_status(seq_handle, queue_id, status);
	current_tick=snd_seq_queue_status_get_tick_time(status);
	snd_seq_queue_status_free(status);
	return(current_tick);
}

void init_queue() {
	queue_id=snd_seq_alloc_queue(seq_handle);
	snd_seq_set_client_pool_output(seq_handle, (seq_len<<1) + 4);
}

void clear_queue() {
	snd_seq_remove_events_t *remove_ev;
	snd_seq_remove_events_malloc(&remove_ev);
	snd_seq_remove_events_set_queue(remove_ev, queue_id);
	snd_seq_remove_events_set_condition(remove_ev, SND_SEQ_REMOVE_OUTPUT | SND_SEQ_REMOVE_IGNORE_OFF);
	snd_seq_remove_events(seq_handle, remove_ev);
	snd_seq_remove_events_free(remove_ev);
}

void arpeggio() {
	snd_seq_event_t ev;
	double dt;
	for(int l1=0; l1 < seq_len; l1++) {
		dt=(l1 % 2==0) ? (double)swing / 16384.0 : -(double)swing / 16384.0;
		snd_seq_ev_clear(&ev);
		snd_seq_ev_set_note(&ev, 0, sequence[2][l1] + transpose, 127, sequence[1][l1]);
		snd_seq_ev_schedule_tick(&ev, queue_id, 0, tick);
		snd_seq_ev_set_source(&ev, port_out_id);
		snd_seq_ev_set_subs(&ev);
		snd_seq_event_output_direct(seq_handle, &ev);
		tick+=(int)((double)sequence[0][l1] * (1.0 + dt));
	}
	snd_seq_ev_clear(&ev);
	ev.type=SND_SEQ_EVENT_ECHO;
	snd_seq_ev_schedule_tick(&ev, queue_id, 0, tick);
	snd_seq_ev_set_dest(&ev, snd_seq_client_id(seq_handle), port_in_id);
	snd_seq_event_output_direct(seq_handle, &ev);
}

void midi_action() {
	do {
		snd_seq_event_t *ev;
		snd_seq_event_input(seq_handle, &ev);
		switch (ev->type) {
		case SND_SEQ_EVENT_ECHO:
			arpeggio();
			break;
		case SND_SEQ_EVENT_NOTEON:
			clear_queue();
			transpose=ev->data.note.note - 60;
			tick=get_tick();
			arpeggio();
			break;
		case SND_SEQ_EVENT_CONTROLLER:
			if (ev->data.control.param==1) {
				bpm=(int)((double)bpm0 * (1.0 + (double)ev->data.control.value / 127.0));
				set_tempo();
			}
			break;
		case SND_SEQ_EVENT_PITCHBEND:
			swing=(double)ev->data.control.value;
			break;
		}
		snd_seq_free_event(ev);
	} while (snd_seq_event_input_pending(seq_handle, 0) > 0);
}

void parse_sequence() {
	FILE *f;
	char c;
	unsigned int pos=0;
	if (!(f=fopen(seq_filename, "r"))) {
		fprintf(stderr, "Couldn't open sequence file %s\n", seq_filename);
		exit(EXIT_FAILURE);
	}
	seq_len=0;
	while((c=fgetc(f))!=EOF) {
		pos++;
		switch (c) {
		case 'c':
			sequence[2][seq_len]=0; break;
		case 'd':
			sequence[2][seq_len]=2; break;
		case 'e':
			sequence[2][seq_len]=4; break;
		case 'f':
			sequence[2][seq_len]=5; break;
		case 'g':
			sequence[2][seq_len]=7; break;
		case 'a':
			sequence[2][seq_len]=9; break;
		case 'h':
			sequence[2][seq_len]=11; break;
		}
		c=fgetc(f); pos++;
		if(c==EOF) {
			break;
		}
		if(c=='#') {
			sequence[2][seq_len]++;
			c=fgetc(f); pos++;
		}
		sequence[2][seq_len]+=12*atoi(&c);
		c=fgetc(f); pos++;
		if(c==EOF) {
			break;
		}
		if(atoi(&c)==0) {
			fprintf(stderr, "error: atoi(&c)==0 with c=%s, pos=%u\n", &c, pos);
			exit(EXIT_FAILURE);
		}
		sequence[1][seq_len]=TICKS_PER_QUARTER / atoi(&c);
		c=fgetc(f); pos++;
		if(c==EOF) {
			break;
		}
		if(atoi(&c)==0) {
			fprintf(stderr, "error: atoi(&c)==0 with c=%s, pos=%u\n", &c, pos);
			exit(EXIT_FAILURE);
		}
		sequence[0][seq_len]=TICKS_PER_QUARTER / atoi(&c);
		seq_len++;
	}
	CHECK_ZERO_ERRNO(fclose(f));
}

void sigterm_exit(int sig) {
	clear_queue();
	// I removed this sleep...
	// CHECK_ZERO(sleep(2));
	snd_seq_stop_queue(seq_handle, queue_id, NULL);
	snd_seq_free_queue(seq_handle, queue_id);
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv, char** envp) {
	if (argc !=3) {
		fprintf(stderr, "%s: %s <beats per minute> <sequence file>\n", argv[0], argv[0]);
		fprintf(stderr, "%s: example 60 file.seq\n", argv[0]);
		return EXIT_FAILURE;
	}
	bpm0=atoi(argv[1]);
	seq_filename=argv[2];

	bpm=bpm0;
	parse_sequence();
	seq_handle=open_seq();
	init_queue();
	set_tempo();
	arpeggio();
	snd_seq_start_queue(seq_handle, queue_id, NULL);
	snd_seq_drain_output(seq_handle);
	int npfd=snd_seq_poll_descriptors_count(seq_handle, POLLIN);
	struct pollfd *pfd=(struct pollfd *)alloca(npfd * sizeof(struct pollfd));
	snd_seq_poll_descriptors(seq_handle, pfd, npfd, POLLIN);
	transpose=0;
	swing=0;
	tick=0;
	signal(SIGINT, sigterm_exit);
	signal(SIGTERM, sigterm_exit);
	arpeggio();
	while (true) {
		if (poll(pfd, npfd, 100000) > 0) {
			for(int i=0; i < npfd; i++) {
				if (pfd[i].revents > 0)
					midi_action();
			}
		}
	}
}
