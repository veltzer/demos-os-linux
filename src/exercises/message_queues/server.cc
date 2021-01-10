/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>	// for EXIT_SUCCESS, exit(3)
#include <err_utils.h>	// for CHECK_NOT_M1()

const int MSGSZ=128;
const int MAXQUEUE=32768;

typedef struct mymsgbuf
{
	long mtype;
	long fromID;
	char mtext[MSGSZ];
} message_buf;

static int childPID;
static int msqid;
static struct msqid_ds msgCtlBuf;

void TerminateChild(int gotsig) {
	CHECK_NOT_M1(kill(childPID, SIGKILL));
	exit(EXIT_SUCCESS);
}

void DoChild() {
	int msgsize;
	int currentQueueSize, biggestQueueSize;
	int i, biggestQID=0;
	message_buf rbuf;
	while(true) {
		CHECK_NOT_M1(msgctl(msqid, IPC_STAT, &msgCtlBuf));
		if(msgCtlBuf.msg_cbytes + MSGSZ+sizeof(long)>=msgCtlBuf.msg_qbytes) {
			// try to empty all queues. find the one with most messages and report it.
			biggestQueueSize=0;
			for(i=1; i<MAXQUEUE; i++) {
				currentQueueSize=0;
				while((msgsize=msgrcv(msqid, &rbuf, MSGSZ+sizeof(long), i, IPC_NOWAIT|MSG_NOERROR))!=-1) {
					currentQueueSize+=msgsize;
				}
				if(currentQueueSize>biggestQueueSize) {
					biggestQueueSize=currentQueueSize;
					biggestQID=i;
				}
			}
			printf("\n\nAll Queues cleared.\nBiggest queue was %d containing %d bytes.",
				biggestQID, biggestQueueSize);
		}
		CHECK_ZERO(sleep(5));
	}
	exit(EXIT_SUCCESS);
}

int main(int argc, char** argv, char** envp) {
	sigset_t emptymask;
	sigemptyset(&emptymask);
	struct sigaction SigAction;
	SigAction.sa_handler=TerminateChild;
	SigAction.sa_mask=emptymask;
	SigAction.sa_flags=0;
	CHECK_NOT_M1(sigaction(SIGINT, &SigAction, NULL));
	CHECK_NOT_M1(sigaction(SIGTERM, &SigAction, NULL));
	CHECK_NOT_M1(sigaction(SIGQUIT, &SigAction, NULL));
	CHECK_NOT_M1(sigaction(SIGABRT, &SigAction, NULL));
	key_t key=CHECK_NOT_M1(ftok("/etc/passwd", 'x'));
	msqid=CHECK_NOT_M1(msgget(key, IPC_CREAT|0666));
	printf("Queue created\n");
	childPID=CHECK_NOT_M1(fork());
	if(childPID==0) {
		DoChild();
	}
	while(true) {
		int Ans;
		printf("Main Menu\n");
		printf("=========\n");
		printf("What would you like to do?\n");
		printf("1. Show all the queue properties\n");
		printf("2. Stop the service (it's a strike).\n");
		printf("3. Change queue size\n");
		printf("4. Change permission\n-> ");
		CHECK_INT(scanf("%d", &Ans), 1);
		switch (Ans) {
		case 1:
			CHECK_NOT_M1(msgctl(msqid, IPC_STAT, &msgCtlBuf));
			fprintf(stderr, "msg_perm.uid=%d\n", msgCtlBuf.msg_perm.uid);
			fprintf(stderr, "msg_perm.gid=%d\n", msgCtlBuf.msg_perm.gid);
			fprintf(stderr, "msg_perm.cuid=%d\n", msgCtlBuf.msg_perm.cuid);
			fprintf(stderr, "msg_perm.cgid=%d\n", msgCtlBuf.msg_perm.cgid);
			fprintf(stderr, "msg_perm.mode=%#o, ", msgCtlBuf.msg_perm.mode);
			fprintf(stderr, "access permissions=%#o\n", msgCtlBuf.msg_perm.mode & 0777);
			fprintf(stderr, "msg_cbytes=%ld\n", msgCtlBuf.msg_cbytes);
			fprintf(stderr, "msg_qbytes=%ld\n", msgCtlBuf.msg_qbytes);
			fprintf(stderr, "msg_qnum=%ld\n", msgCtlBuf.msg_qnum);
			fprintf(stderr, "msg_lspid=%d\n", msgCtlBuf.msg_lspid);
			fprintf(stderr, "msg_lrpid=%d\n", msgCtlBuf.msg_lrpid);
			fprintf(stderr, "msg_stime=%s", msgCtlBuf.msg_stime ?
				ctime(&msgCtlBuf.msg_stime) : "Not Set\n");
			fprintf(stderr, "msg_rtime=%s", msgCtlBuf.msg_rtime ?
				ctime(&msgCtlBuf.msg_rtime) : "Not Set\n");
			fprintf(stderr, "msg_ctime=%s", ctime(&msgCtlBuf.msg_ctime));
			break;
		case 2:
			CHECK_NOT_M1(kill(childPID, SIGKILL));
			CHECK_NOT_M1(msgctl(msqid, IPC_RMID, NULL));
			exit(EXIT_SUCCESS);
			break;
		case 3:
			CHECK_NOT_M1(msgctl(msqid, IPC_STAT, &msgCtlBuf));
			printf("From %ld\nTo ", msgCtlBuf.msg_qbytes);
			CHECK_INT(scanf("%ld", &msgCtlBuf.msg_qbytes), 1);
			CHECK_NOT_M1(msgctl(msqid, IPC_SET, &msgCtlBuf));
			break;
		case 4:
			fprintf(stderr, "Enter msg_perm.uid: ");
			CHECK_INT(scanf ("%d", &msgCtlBuf.msg_perm.uid), 1);
			fprintf(stderr, "Enter msg_perm.gid: ");
			CHECK_INT(scanf("%d", &msgCtlBuf.msg_perm.gid), 1);
			fprintf(stderr, "Enter msg_perm.mode: ");
			CHECK_INT(scanf("%d", &msgCtlBuf.msg_perm.mode), 1);
			CHECK_NOT_M1(msgctl(msqid, IPC_SET, &msgCtlBuf));
			break;
		}
	}
	return EXIT_SUCCESS;
}
