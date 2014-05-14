#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "mykey.h"
#include "utils.h"
	
int main(int argc, char *argv[])
{
	int semid, runp3, runp1p2;
	int shmid, *shmptr;
	int sharedtable_id, *sharedtable_ptr;
	
	/* this semphore is to contro the critical area between P1 and P2 */
	semid = semget((key_t)CRITICAL, 1, 0666|IPC_CREAT);
	if (semphore_set(semid, 1) < 0) 	//init it by 1
		err_quit("semphore_set error");

	/* this semphore is to block P3, util Phase2 ends */
	runp3 = semget((key_t)RUNP3, 1, 0666|IPC_CREAT);
	if (semphore_set(runp3, 0) < 0) 	//init it by 0
		err_quit("semphore_set error");

	/* this smephore is to block P1,P2 when is Phase3 */
	runp1p2 = semget((key_t)RUNP1P2, 1, 0666|IPC_CREAT);
	if (semphore_set(runp1p2, 0) < 0) //init it by 0;
		err_quit("semphore_set error");

	/* create a shared memory to store the shared table
	   and two variables to control fusion, so it's size is 20+2,
	   the [20] is to store P1 or P2's pid, and the [21] to store
	   the idx of the shared table when fusioning and sorting 
	*/
	sharedtable_id=shmget((key_t)SHAREDTABLE, sizeof(int)*22, 0666|IPC_CREAT);
	if ( (sharedtable_ptr=shmat(sharedtable_id, 0, 0)) ==(void*)-1)
		err_quit("shmat error");
	bzero(sharedtable_ptr, sizeof(int)*22);
		

	/* Phase 1*/
	int table[10];
	filltable_and_sort(table, 10);
		
	semphore_p(semid);
	print(table, "after Phase 1, P1's table", 10);
	semphore_v(semid);

	/* Phase 2*/
	fusion(semid, table, sharedtable_ptr);
	print(sharedtable_ptr, "after Phase2, sharedtable", 20);

	/* let p3 run */
	semphore_v(runp3);

	/* wait for Phase3 end*/
	semphore_p(runp1p2);

	/* Phase 4*/
	int i;
	int idx=0;
	for (i=0; i<20; i++) {
		if (sharedtable_ptr[i]>0)
			table[idx]=sharedtable_ptr[i],idx++;
	}
	semphore_p(semid);
	print(table, "after Phase4, P1's table", 10);
	semphore_v(semid);

    return 0;
}

