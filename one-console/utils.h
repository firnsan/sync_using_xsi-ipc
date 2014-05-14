#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semphore_get(int num, int flag);
int semphore_set(int semid, int val);
int semphore_p(int semid);
int semphore_v(int semid);
void err_quit(const char *msg);
void filltable_and_sort(int *table, int num);
void fusion(int semid, int *table, int *sharedtable);
void print(int *table, const char *msg, int num);

#ifdef _SEM_SEMUN_UNDEFINED
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};
#endif
	

#endif