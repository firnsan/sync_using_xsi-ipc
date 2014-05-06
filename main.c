#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "utils.h"


void fusion(int semid, int *table, int *sharedtable);
void print(int *table, const char *msg, int num);
	
int main(int argc, char *argv[])
{
	int semid, runp3, runp1p2;
	int shmid, *shmptr;
	int sharedtable_id, *sharedtable_ptr;
	pid_t pid1, pid2, pid3;
	
	//create a sem
	/* 好奇怪为什么用这个自己的函数会permission denied 
	  if (semid=semphore_get(1, 0666|IPC_CREAT)<0)
	   err_quit("semphore_get error"); 
	*/
	
	semid = semget((key_t)1234, 1, 0666|IPC_CREAT);
	
	//init it by 1
	if (semphore_set(semid, 1) < 0)
		err_quit("semphore_set error");


	runp3 = semget((key_t)1236, 1, 0666|IPC_CREAT);
	
	//init it by 0
	if (semphore_set(runp3, 0) < 0)
		err_quit("semphore_set error");


	runp1p2 = semget((key_t)1238, 1, 0666|IPC_CREAT);
	
	//init it by 0
	if (semphore_set(runp1p2, 0) < 0)
		err_quit("semphore_set error");

	/*	shmid=shmget((key_t)2345, sizeof(int), 0666|IPC_CREAT);
	if ( (shmptr=shmat(shmid, 0, 0)) ==(void*)-1)
		err_quit("shmat error");
	*shmptr=0;*/

	sharedtable_id=shmget((key_t)3466, sizeof(int)*22, 0666|IPC_CREAT);
	if ( (sharedtable_ptr=shmat(sharedtable_id, 0, 0)) ==(void*)-1)
		err_quit("shmat error");
	bzero(sharedtable_ptr, sizeof(int)*22);
	
	pid2=fork();
	if (pid2<0)
		err_quit("fork error");
	else if (0==pid2) {
		pid3=fork();
		if (pid3<0)
			err_quit("fork error");
		else if(0==pid3) {
			
			/* the third process */

			semphore_p(runp3);
			srand(getpid());

			int rndnums[10];
			int i, j, rndnum, idx, is_ok;

			for (i=0; i<10; i++)
				rndnums[i]=-1;

			idx=0;
			while(1) {
				is_ok=1;
				rndnum=rand()%20;
				for (j=0;j<idx;j++)
					if (rndnums[j]==rndnum) {
						is_ok=0;
						break;
					}
				
				if (!is_ok)
					continue;

				rndnums[idx]=rndnum;
				idx++;
				if (idx==10)
					break;
			}
			
			for (i=0; i<10; i++) {
				rndnum=rndnums[i];
				sharedtable_ptr[rndnum]=-sharedtable_ptr[rndnum];
			}

			print(sharedtable_ptr, "after Phase 3, sharedtable", 20);
			sleep(1);
			
			semphore_v(runp1p2);
			semphore_v(runp1p2);
			
		} else {
			/* the second process */

			int table[10];
			filltable_and_sort(table, 10);

			semphore_p(semid);
			print(table, "after Phase1, P2's table", 10);
			semphore_v(semid);
			
			sleep(4);
			fusion(semid, table, sharedtable_ptr);
			
			semphore_p(runp1p2);

			int i;
			int idx=0;
			for (i=0; i<20; i++) {
				if (sharedtable_ptr[i]<0)
					table[idx]=sharedtable_ptr[i],idx++;
			}

			semphore_p(semid);
			print(table, "after Phase4, P2's table", 10);
			semphore_v(semid);
			
			
		}
			
	} else {
		/*the first process */
		
		int table[10];
		filltable_and_sort(table, 10);

		
		semphore_p(semid);
		print(table, "after Phase 1, P1's table", 10);
		semphore_v(semid);

		
		/*
		semphore_p(semid);
		if (0==*shmptr)
			*shmptr=table[0];
		semphore_v(semid);
		
		int i=0;
		while (1) {
			if (i==10)
				break;
			semphore_p(semid);
			if(table[i]>*shmptr) {
				semphore_v(semid);
				usleep(100);
			}
			else {
				*shmptr=table[i];
				add2sharedtable(sharedtable_ptr, table[i]);
				i++;
				semphore_v(semid);
			}
				
		 } */
		sleep(4);
		fusion(semid, table, sharedtable_ptr);
		print(sharedtable_ptr, "after Phase2, sharedtable", 20);

		//let p3 run;
		semphore_v(runp3);
		semphore_p(runp1p2);

		int i;
		int idx=0;
		for (i=0; i<20; i++) {
			if (sharedtable_ptr[i]>0)
				table[idx]=sharedtable_ptr[i],idx++;
		}

		semphore_p(semid);
		print(table, "after Phase4, P1's table", 10);
		semphore_v(semid);
}


    return 0;
}


void add2sharedtable(int *array, int new)
{
	int i;
	for (i=0; i<20; i++) {
		if (array[i]==0) {
			array[i]=new;
			break;
		}
	}
}


void fusion(int semid, int *table, int *sharedtable)
{
	semphore_p(semid);
	if (0==sharedtable[0]) {
		sharedtable[0]=table[0];
		sharedtable[20]=getpid();
		sharedtable[21]=0;
	}
	semphore_v(semid);
		
	int i=0;
	int idx;
	while (1) {
		if (i==10)
			break;
		semphore_p(semid);
		if (sharedtable[20]==getpid()) { /* this process have judged just now, continue*/
			semphore_v(semid);
			usleep(100);
			continue;
		} else if(sharedtable[20]==-1) {
			for(;i<10;i++)
				sharedtable[10+i]=table[i];
			semphore_v(semid);
			break;
		}
			
		idx=sharedtable[21];

		if(table[i]>sharedtable[idx]) {
			sharedtable[20]=getpid(); /* sign that i have judged it */
			semphore_v(semid);
			usleep(100);
		} else {
			//printf("%d: table[%d]=%d lt sharedtable[%d]=%d\n", getpid(), i, table[i], idx, sharedtable[idx]);
			//sleep(1);
			sharedtable[idx]=table[i];
			idx++;
			sharedtable[21]=idx;
			i++;
			if (10==i) {
				sharedtable[20]=-1; /*sign that my table is over*/
				semphore_v(semid);
			} else {
				sharedtable[idx]=table[i];
				sharedtable[20]=getpid(); /* sign that i have judged it */
				semphore_v(semid);
			}
		}
			
	}
}


void print(int *table, const char *msg, int num)
{
	int i;
	printf("%s:\n", msg);
	for (i=0; i<num; i++)
		printf("%d\n", table[i]);
}