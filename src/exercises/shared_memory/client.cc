/*
 * This file is part of the demos-linux package.
 * Copyright (C) 2011-2021 Mark Veltzer <mark.veltzer@gmail.com>
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
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>	// for EXIT_SUCCESS, EXIT_FAILURE, exit(3)
#include <err_utils.h>	// for CHECK_NOT_M1(), CHECK_NOT_VOIDP()

const int CLIENTMESSAGESIZE=1024;
const int MAXMESSAGE=256;
const int MAXCLINTS=10;

struct data {
	int readOffset;
	int writeOffset;
	char message[CLIENTMESSAGESIZE];
};

void doChild(int semid, struct data * smdata, int myID) {
	char message[MAXMESSAGE];
	int* messageSizePTR;
	int* fromIDPTR;
	int messageSize;
	int spaceLeft;
	struct sembuf sops[2];
	while(true) {
		sops[0].sem_num=myID;
		sops[0].sem_op=-1;
		sops[0].sem_flg=0;
		CHECK_NOT_M1(semop(semid, sops, 1));
		if (smdata[myID].readOffset!=smdata[myID].writeOffset) {
			messageSizePTR=(int *)smdata[myID].message + smdata[myID].readOffset/sizeof(int);
			messageSize=*messageSizePTR;
			spaceLeft=CLIENTMESSAGESIZE - smdata[myID].readOffset;
			if (spaceLeft < messageSize) {
				memcpy(message, smdata[myID].message + smdata[myID].readOffset, spaceLeft);
				memcpy(message+spaceLeft, smdata[myID].message, messageSize - spaceLeft);
				smdata[myID].readOffset=messageSize - spaceLeft;
			} else {
				memcpy(message, smdata[myID].message + smdata[myID].readOffset, messageSize);
				smdata[myID].readOffset+=messageSize;
			}
			fromIDPTR=(int *)message + 1;
			printf("\n\nHey! this is the child\nI got message from: %d message: %s\n", *fromIDPTR, message+2*sizeof(int));
		}
	}
}

void doParent(int semid, struct data * smdata, int myID) {
	char message[MAXMESSAGE];
	int * messageSizePTR;
	int * FromIDPTR;
	char Sto[8];
	int toID;
	int reminder;
	int spaceLeft;
	struct sembuf sops[2];
	while(true) {
		printf("To: ");
		CHECK_CHARP(fgets(Sto, 7, stdin), Sto);
		toID=atoi(Sto);
		printf("Message to %d: ", toID);

		CHECK_CHARP(fgets(message+sizeof(int)*2, sizeof(message) - sizeof(int)*2 -1, stdin), message+sizeof(int)*2);

		messageSizePTR=(int *)message;
		*messageSizePTR=strlen(message+sizeof(int)*2) + sizeof(int)*2;

		FromIDPTR=(int *)message+1;
		*FromIDPTR=myID;

		reminder=*messageSizePTR % sizeof(int);
		if (reminder > 0)
			*messageSizePTR+=sizeof(int)-reminder;
		sops[0].sem_num=toID;
		sops[0].sem_op=1;
		sops[0].sem_flg=0;
		CHECK_NOT_M1(semop(semid, sops, 1));

		spaceLeft=CLIENTMESSAGESIZE - smdata[toID].writeOffset;
		if (*messageSizePTR > spaceLeft) {
			memcpy(smdata[toID].message + smdata[toID].writeOffset, message, spaceLeft);
			memcpy(smdata[toID].message, message + spaceLeft, *messageSizePTR - spaceLeft);
			smdata[toID].writeOffset=*messageSizePTR - spaceLeft;
		} else {
			memcpy(smdata[toID].message + smdata[toID].writeOffset, message, *messageSizePTR);
			smdata[toID].writeOffset+=*messageSizePTR;
		}
	}
}

int main(int argc, char** argv, char** envp) {
	if(argc < 2) {
		fprintf(stderr, "%s: usage: %s MyID\n", argv[0], argv[0]);
		exit(errno);
	}
	int myID=atoi(argv[1]);
	if(myID>=MAXCLINTS || myID<0) {
		fprintf(stderr, "MyID must be 0-%d\n", MAXCLINTS);
		exit(errno);
	}
	key_t key=CHECK_NOT_M1(ftok("/etc/passwd", 'x'));
	int semid=CHECK_NOT_M1(semget(key, 0, 0));
	int shmid=CHECK_NOT_M1(shmget(key, sizeof(struct data), 0));
	struct data* smdata=(struct data*)CHECK_NOT_VOIDP(shmat(shmid, NULL, 0), (void*)-1);
	smdata[myID].readOffset=0;
	smdata[myID].writeOffset=0;
	if(CHECK_NOT_M1(fork())) {
		doChild(semid, smdata, myID);
	} else {
		doParent(semid, smdata, myID);
	}
	return EXIT_SUCCESS;
}
