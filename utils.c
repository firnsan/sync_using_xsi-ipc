#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "utils.h"

void err_quit(const char *msg)
{
	//printf("at: %s:%d\n",__FILE__,__LINE__);
	perror(msg);
	exit(-1);
}

//create a semphore, or get a exsited semphore
int semphore_get(int num, int flag)
{
	int semid;
	//key_t key=ftok(".", 234);
	/*if ( (semid=semget((key_t)1234 ,1, IPC_CREAT|0666)) <0 )
		return -1;	
		return semid;*/
	return semget((key_t)1234, 1, 0666 |IPC_CREAT);
		//semget((key_t)1234, 1, 0666 |IPC_CREAT)
}

//init the semval
int semphore_set(int semid, int val)
{
	union semun un;
	un.val=val;
	if (semctl(semid, 0, SETVAL, un)<0)
		return -1;
	return 0;
}

//ask a resource
int semphore_p(int semid)
{
	struct sembuf sb;
	sb.sem_num=0;
	sb.sem_op=-1;
	sb.sem_flg=SEM_UNDO;
	if ( semop(semid, &sb, 1) < 0)
		return -1;
	return 0;
}

//release a resource
int semphore_v(int semid)
{
	struct sembuf sb;
	sb.sem_num=0;
	sb.sem_op=1;
	//sb.sem_flg=SEM_UNDO;
	if (semop(semid, &sb, 1) < 0)
		return -1;
	return 0;
}

void swap(int *a, int *b)
{
	*a=*a^*b;
	*b=*a^*b;
	*a=*a^*b;
}

void buble_sort(int *array, int num)
{
	int i,j;
	for (i=0; i<num-1; i++)
		for (j=0; j<num-1-i; j++)
			if (array[j]>array[j+1])
				swap(&array[j], &array[j+1]);
}

void filltable_and_sort(int *array, int num)
{
	int i=num;
	srand((unsigned int)getpid());//must set seed, or evrytime the rand num is the same
	while(i--) {
		array[i]=rand()%2009+1;
	}
	buble_sort(array, num);
}