/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2020 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * EXTRA_LINK_FLAGS=-lasound
 */
// Programmer: Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat May 9 22:03:40 PDT 2009
// Last Modified: Sun May 10 17:22:43 PDT 2009
// Filename: midiecho.c
// Syntax: C; ALSA 1.0
// $Smake: gcc -o %b %f -lasound
//
// Description:	Echo MIDI input notes transposed up a tritone.

#include <firstinclude.h>
#include <alsa/asoundlib.h>
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE
#include <stdbool.h>	// for true

// function declarations:
int getArgsExpected(int midicommand);
void errormessage(const char *format, ...);
void echomidi(snd_rawmidi_t* midiin, snd_rawmidi_t* midiout);

// /////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
	int status;	// for storing error codes
	int mode=SND_RAWMIDI_SYNC;	// automatic MIDI out flushing
	snd_rawmidi_t* midiin=NULL;	// structure to access MIDI input
	snd_rawmidi_t* midiout=NULL;	// structure to access MIDI output
	const char* portname="hw:1,0,0";// see alsarawportlist.c example program
	if ((argc > 1) && (strncmp("hw:", argv[1], 3)==0)) {
		portname=argv[1];
	}
	if ((status=snd_rawmidi_open(&midiin, &midiout, portname, mode)) < 0) {
		errormessage("Problem opening MIDI input: %s", snd_strerror(status));
		exit(EXIT_FAILURE);
	}
	printf("Type control-c to exit.\n");
	echomidi(midiin, midiout);
	// never gets here, but here is how to close MIDI gracefully:
	snd_rawmidi_close(midiin);
	snd_rawmidi_close(midiout);
	midiin=NULL;	// snd_rawmidi_close() does not clear invalid pointer,
	midiout=NULL;	// so might be a good idea to erase it after closing.
	return EXIT_SUCCESS;
}

// midiecho -- Echo MIDI notes out with an added transposition.

void echomidi(snd_rawmidi_t* midiin, snd_rawmidi_t* midiout) {
	unsigned char readbuffer;	// storage for input MIDI byte stream
	unsigned char buffer[1024]={0};	// storage space for incoming commands
	int dataptr=0;	// writing index in buffer
	int argsLeft=0;	// bytes left to read for a command
	int transpose=6;// transposition for note out
	int newnote;	// temp storage for output note
	int status;	// storage for error codes
	while(true) {
		if ((status=snd_rawmidi_read(midiin, &readbuffer, 1)) < 0) {
			errormessage("Problem reading MIDI input: %s", snd_strerror(status));
		}
		if (readbuffer & 0x80) {// a command byte has arrived
			argsLeft=getArgsExpected(readbuffer);
			dataptr=0;
			buffer[dataptr++]=readbuffer;
		} else {// a data byte has arrived
			if ((dataptr==0) || (argsLeft <=0)) {
				argsLeft=getArgsExpected(buffer[0]);
				dataptr=1;
			}
			buffer[dataptr++]=readbuffer;
			argsLeft--;
		}
		if ((argsLeft==0) && (((buffer[0] & 0xf0)==0x90) || ((buffer[0] & 0xf0)==0x80))) {
			newnote=buffer[1] + transpose;
			if ((newnote > 0) && (newnote < 128)) {
				buffer[1]=(unsigned char)newnote;
				if ((status=snd_rawmidi_write(midiout, buffer, 3)) < 0) {
					errormessage("Problem with MIDI out: %s", snd_strerror(status));
					exit(EXIT_FAILURE);
				} else if (((buffer[0] & 0xf0)==0x90) && ((buffer[2] & 0x0f)!=0)) {
					printf("New note: %d\n", newnote);
				}
			}
		}
		if (dataptr > 1000) {
			dataptr=0;
		}
	}
}

// getArgsExpected -- returns the number of MIDI data bytes which are
// expected to follow the given command. 0xf0 set of commands
// are not handled completely, particularly system exclusives.

int getArgsExpected(int midicommand) {
	switch (midicommand & 0xf0) {
	case 0x80: return 2;	// note off
	case 0x90: return 2;	// note on
	case 0xa0: return 2;	// aftertouch
	case 0xb0: return 2;	// continuous controller
	case 0xc0: return 1;	// patch change
	case 0xd0: return 1;	// channel pressure
	case 0xe0: return 1;	// pitch bend
	case 0xf0: return 0;	// not totally correct, esp. for sysex
	}
	return 0;
}

// error -- Print an error message.
void errormessage(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	putc('\n', stderr);
}
