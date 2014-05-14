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



	/* wait for Phase1 and Phase2 end */
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


	/* wake up P1, P2 */
	semphore_v(runp1p2);
	semphore_v(runp1p2);


    return 0;
}

