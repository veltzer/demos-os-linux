/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2018 Mark Veltzer <mark.veltzer@gmail.com>
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

#include <firstinclude.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3), EXIT_FAILURE
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_CHARP()

const int MSGSZ=1024;

// note we cannot declare msgbuf and get away with it
typedef struct mymsgbuf {
	long mtype;
	long fromID;
	char mtext[MSGSZ];
} message_buf;

void doParent(int msqid, long myID)
{
	char SsubscriberID[8];
	message_buf sbuf;
	setbuf(stdin, 0);
	setbuf(stdout, 0);
	while(true) {
		printf("Call ID: ");
		CHECK_CHARP(fgets(SsubscriberID, sizeof(SsubscriberID)-1, stdin), SsubscriberID);
		sbuf.mtype=atoi(SsubscriberID);
		sbuf.fromID=myID;
		printf("Message to %ld: ", sbuf.mtype);
		CHECK_CHARP(fgets(sbuf.mtext, sizeof(sbuf.mtext) -1, stdin), sbuf.mtext);
		CHECK_NOT_M1(msgsnd(msqid, &sbuf, strlen(sbuf.mtext)+sizeof(long), 0));
	}
}

void doChild(int msqid, long myID)
{
	while(true) {
		message_buf rbuf;
		int msgsize=CHECK_NOT_M1(msgrcv(msqid, &rbuf, MSGSZ+sizeof(long), myID, 0));
		rbuf.mtext[msgsize-sizeof(long)]='\0';	// string was sent without null char.
		printf("Message from: %ld -> %s", rbuf.fromID, rbuf.mtext);
	}
}

int main(int argc, char** argv, char** envp) {
	long myID;
	if(argc!=2) {
		fprintf(stderr, "%s: usage: %s MirsID\n", argv[0], argv[0]);
		return EXIT_FAILURE;
	}
	if((myID=atoi(argv[1]))<1) {
		fprintf(stderr, "MirsID must be numeric positive greater than 0 and uniq (not checked)\n");
		return EXIT_FAILURE;
	}
	key_t key=CHECK_NOT_M1(ftok("/etc/passwd", 'x'));
	int msqid=CHECK_NOT_M1(msgget(key, 0));
	switch (CHECK_NOT_M1(fork())) {
	case 0:
		doChild(msqid, myID);
		return EXIT_SUCCESS;
	default:
		doParent(msqid, myID);
		return EXIT_SUCCESS;
	}
	return EXIT_SUCCESS;
}
