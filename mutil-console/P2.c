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
	semid = semget((key_t)CRITICAL, 0, 0666);
	if (semid<0)
		err_quit("semget error");
	
	/* this semphore is to block P3, util Phase2 ends */
	runp3 = semget((key_t)RUNP3, 0, 0666);
	if (runp3<0)
		err_quit("semget error");

	/* this smephore is to block P1,P2 when is Phase3 */
	runp1p2 = semget((key_t)RUNP1P2, 0, 0666);
	if (runp1p2<0)
		err_quit("semget error");

	
	sharedtable_id=shmget((key_t)SHAREDTABLE, 0, 0666);
	if (sharedtable_id<0)
		err_quit("shmget error");
	if ( (sharedtable_ptr=shmat(sharedtable_id, 0, 0)) ==(void*)-1)
		err_quit("shmat error");

	/* Phase 1 */
	int table[10];
	filltable_and_sort(table, 10);

	semphore_p(semid);
	print(table, "after Phase1, P2's table", 10);
	semphore_v(semid);

			
	/* Phase 2*/
	fusion(semid, table, sharedtable_ptr);

			
	/* wait for Phase3 end */
	semphore_p(runp1p2);

			
	/* Phase 4 */
	int i;
	int idx=0;
	for (i=0; i<20; i++) {
		if (sharedtable_ptr[i]<0)
			table[idx]=sharedtable_ptr[i],idx++;
	}

	semphore_p(semid);
	print(table, "after Phase4, P2's table", 10);
	semphore_v(semid);
			


    return 0;
}

