/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2022 Mark Veltzer <mark.veltzer@gmail.com>
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

/*
 * EXTRA_LINK_FLAGS=-lasound
 */
// Programmer: Craig Stuart Sapp <craig@ccrma.stanford.edu>
// Creation Date: Sat May 9 17:50:41 PDT 2009
// Last Modified: Sat May 9 18:14:05 PDT 2009
// Filename: alsarawportlist.c
// Syntax: C; ALSA 1.0
// $Smake: gcc -o %b %f -lasound
//
// Description:	Print available input/output MIDI ports using
// using ALSA rawmidi interface. Derived from
// amidi.c (An ALSA 1.0.19 utils program).
//
// First double-check that you have the ALSA system installed on your computer
// by running this command-line command:
// cat /proc/asound/version
// Which should return something like:
// Advanced Linux Sound Architecture Driver Version 1.0.17.
// This example program should work if the version number (1.0.17 in this
// case) is "1".
//
// Access to a MIDI port requires three integers:
// (1) A soundcard number
// (2) A device number
// (3) A sub-device number
//
// The following program shows how to get access to these three numbers.
// When you know these three numbers, you can open a particular MIDI
// input/output using snd_rawmidi_open(), using a string such as:
// hw:x,y,z
// where x, y, z are the three numbers which specify a particular
// MIDI input/output port. On my computer, this program prints this
// information:
//
// Dir Device Name
// ===================================
// IO hw:1,0,0 MidiSport 1x1 MIDI 1
//
// Where "hw:1,0,0" is the string needed to open the MidiSport 1x1 MIDI
// port in the function snd_rawmidi_open() for MIDI input/output.
//
// See the other example programs:
//
// alsarawportlist.c -- this file.
// alsarawmidiout.c -- How to write MIDI output to an external synth.
// alsarawmidiin.c -- How to read MIDI in blocking mode.
// midiinthread.c -- How to read MIDI using threads.
// nonblockinginput.c -- How to read MIDI input in non-blocking mode.
// midiecho.c -- Example of MIDI input and output at same time.
// amidi.c -- ALSA utility program used as a prototype.
//
// Some of the ALSA functions used in this program:
//
// int card;
// int snd_card_next(card&)
// Gives the next card number in the list of sound cards. Returns
// a negative value if there was an error. To find the first card
// in the list, set the input value to -1. To list all of the cards
// availble on the system, keep calling snd_card_next() until it sets the
// value of card back to -1. If the first call to snd_card_next() sets
// card to -1, then that means that there were no cards available. The
// return value may give a reason why none were found (such as the
// user does not have permission to access the sound devices).
// Related functions:
// int snd_card_get_name(int card, char** name) -- short name of card.
// int snd_card_get_longname(int card, char** name) -- long name of card.
//
// const char* snd_strerror(int error_number)
// Returns a string which describes an error number returned by
// any ALSA function (such as snd_card_next). Error are numbers
// less than zero.
//
// int snd_ctl_open(snd_ctl_t** ctl, const char* name, int mode)
// Opens a sound card when the name is set to "hw:#" where # is the
// card number. Modes: 0, or one of the following:
// SND_CTL_NONBLOCK==non-blocking mode.
// SND_CTL_ASYNC==Async notification flag.
// Returns a negative error code if there was a problem opening the card.
// Related function:
// snd_ctl_close(ctl) which does the opposite of snd_ctl_open().
//
// int device;
// int snd_ctl_rawmidi_next_device(snd_ctl_t* control, &device)
// Sets the next device number available on the give card controller.
// You set device to -1 for the first call to the function to get the
// first device number. The function works in a similar manner to
// snd_card_next(). If the function sets device to -1, then there
// are no more MIDI devices on the sound card. Returns a negative
// error code if there was a problem.
//
// Online documentation:
//
// http://www.alsa-project.org/alsa-doc/alsa-lib/rawmidi.html
// http://www.alsa-project.org/alsa-doc/alsa-lib/group___control.html
// * snd_ctl_open()
// * snd_ctl_close()
// http://www.alsa-project.org/alsa-doc/alsa-lib/control_8c.html
// * snd_card_next()
// * snd_card_get_name()
// * snd_card_get_longname()
// * snd_ctl_rawmidi_next_device()
//

#include <firstinclude.h>
#include <alsa/asoundlib.h>	// for snd_*(3)
#include <stdio.h>	// for putc(3)
#include <stdarg.h>	// for vfprintf(3), va_start(3), va_end(3), va_list(3)
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, exit(3)
#include <stdbool.h>	// for true(O)

// error -- print error message
void error(const char *format, ...) {
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	putc('\n', stderr);
	exit(EXIT_FAILURE);
}

// is_input -- returns true if specified card/device/sub can output MIDI data.
int is_input(snd_ctl_t *ctl, int card, int device, int sub) {
	snd_rawmidi_info_t *info;
	int status;

	snd_rawmidi_info_alloca(&info);
	snd_rawmidi_info_set_device(info, device);
	snd_rawmidi_info_set_subdevice(info, sub);
	snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
	if ((status=snd_ctl_rawmidi_info(ctl, info)) < 0 && status !=-ENXIO) {
		return status;
	} else if (status==0) {
		return 1;
	}
	return 0;
}

// is_output -- returns true if specified card/device/sub can output MIDI data.
int is_output(snd_ctl_t *ctl, int card, int device, int sub) {
	snd_rawmidi_info_t *info;
	int status;

	snd_rawmidi_info_alloca(&info);
	snd_rawmidi_info_set_device(info, device);
	snd_rawmidi_info_set_subdevice(info, sub);
	snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
	if ((status=snd_ctl_rawmidi_info(ctl, info)) < 0 && status !=-ENXIO) {
		return status;
	} else if (status==0) {
		return 1;
	}
	return 0;
}

// list_subdevice_info -- Print information about a subdevice
// of a device of a card if it can handle MIDI input and/or output.
void list_subdevice_info(snd_ctl_t *ctl, int card, int device) {
	snd_rawmidi_info_t *info;
	const char *name;
	const char *sub_name;
	int subs, subs_in, subs_out;
	int sub, in, out;
	int status;

	snd_rawmidi_info_alloca(&info);
	snd_rawmidi_info_set_device(info, device);

	snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
	snd_ctl_rawmidi_info(ctl, info);
	subs_in=snd_rawmidi_info_get_subdevices_count(info);
	snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
	snd_ctl_rawmidi_info(ctl, info);
	subs_out=snd_rawmidi_info_get_subdevices_count(info);
	subs=subs_in > subs_out ? subs_in : subs_out;

	sub=0;
	in=out=0;
	if ((status=is_output(ctl, card, device, sub)) < 0) {
		error("cannot get rawmidi information %d:%d: %s", card, device, snd_strerror(status));
	} else if (status)
		out=1;
	if (status==0) {
		if ((status=is_input(ctl, card, device, sub)) < 0) {
			error("cannot get rawmidi information %d:%d: %s", card, device, snd_strerror(status));
		}
	} else if (status)
		in=1;
	if (status==0)
		return;
	name=snd_rawmidi_info_get_name(info);
	sub_name=snd_rawmidi_info_get_subdevice_name(info);
	if (sub_name[0]=='\0') {
		if (subs==1) {
			printf("%c%c,hw:%d,%d,[%s]\n", in ? 'I' : ' ', out ? 'O' : ' ', card, device, name);
		} else
			printf("%c%c,hw:%d,%d,[%s] (%d subdevices)\n", in ? 'I' : ' ', out ? 'O' : ' ', card, device, name, subs);
	} else {
		sub=0;
		while(true) {
			printf("%c%c,hw:%d,%d,%d,[%s]\n", in ? 'I' : ' ', out ? 'O' : ' ', card, device, sub, sub_name);
			if (++sub >=subs)
				break;
			in=is_input(ctl, card, device, sub);
			out=is_output(ctl, card, device, sub);
			snd_rawmidi_info_set_subdevice(info, sub);
			if (out) {
				snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
				if ((status=snd_ctl_rawmidi_info(ctl, info)) < 0) {
					error("cannot get rawmidi information %d:%d:%d: %s", card, device, sub, snd_strerror(status));
				}
			} else {
				snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_INPUT);
				if ((status=snd_ctl_rawmidi_info(ctl, info)) < 0) {
					error("cannot get rawmidi information %d:%d:%d: %s", card, device, sub, snd_strerror(status));
				}
			}
			sub_name=snd_rawmidi_info_get_subdevice_name(info);
		}
	}
}

// list_midi_devices_on_card -- For a particular "card" look at all
// of the "devices/subdevices" on it and print information about it
// if it can handle MIDI input and/or output.
void list_midi_devices_on_card(int card) {
	snd_ctl_t *ctl;
	char name[32];
	int device=-1;
	int status;
	sprintf(name, "hw:%d", card);
	if ((status=snd_ctl_open(&ctl, name, 0)) < 0) {
		error("cannot open control for card %d: %s", card, snd_strerror(status));
	}
	do {
		status=snd_ctl_rawmidi_next_device(ctl, &device);
		if (status < 0) {
			error("cannot determine device number: %s", snd_strerror(status));
		}
		if (device >=0) {
			list_subdevice_info(ctl, card, device);
		}
	} while (device >=0);
	snd_ctl_close(ctl);
}

// print_midi_ports -- go through the list of available "soundcards",
// checking them to see if there are devices/subdevices on them which
// can read/write MIDI data.
void print_midi_ports(void) {
	int status;
	int card=-1;	// use -1 to prime the pump of iterating through card list
	if ((status=snd_card_next(&card)) < 0) {
		error("cannot determine card number: %s", snd_strerror(status));
	}
	if (card < 0) {
		error("no sound cards found");
	}
	printf("Dir Device Name\n");
	printf("====================================\n");
	while(card>=0) {
		list_midi_devices_on_card(card);
		if((status=snd_card_next(&card))<0) {
			error("cannot determine card number: %s", snd_strerror(status));
		}
	}
}

// print_card_list -- go through the list of available "soundcards"
// in the ALSA system, printing their associated numbers and names.
// Cards may or may not have any MIDI ports available on them (for
// example, a card might only have an audio interface).
void print_card_list(void) {
	int status;
	int card=-1;	// use -1 to prime the pump of iterating through card list
	char* longname=NULL;
	char* shortname=NULL;
	if ((status=snd_card_next(&card)) < 0) {
		error("cannot determine card number: %s", snd_strerror(status));
	}
	if (card < 0) {
		error("no sound cards found");
	}
	while (card >=0) {
		printf("Card %d:", card);
		if ((status=snd_card_get_name(card, &shortname)) < 0) {
			error("cannot determine card shortname: %s", snd_strerror(status));
		}
		if ((status=snd_card_get_longname(card, &longname)) < 0) {
			error("cannot determine card longname: %s", snd_strerror(status));
		}
		printf("\tLONG NAME: %s\n", longname);
		printf("\tSHORT NAME: %s\n", shortname);
		if ((status=snd_card_next(&card)) < 0) {
			error("cannot determine card number: %s", snd_strerror(status));
		}
	}
}

int main(int argc, char** argv, char** envp) {
	// print_card_list();
	print_midi_ports();
	return EXIT_SUCCESS;
}
