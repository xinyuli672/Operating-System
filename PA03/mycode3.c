/* mycode3.c: your portion of the kernel
 *
 *   	Below are procedures that are called by other parts of the kernel. 
 * 	Your ability to modify the kernel is via these procedures.  You may
 *  	modify the bodies of these procedures any way you desire (however,
 *  	you cannot change the interfaces).  
 */

#include "aux.h"
#include "sys.h"
#include "mycode3.h"

#define FALSE 0
#define TRUE 1

/* 	A sample semaphore table. You may change this any way you wish. 
 */

static struct {
	int valid;	// Is this a valid entry (was sem allocated)?
	int value;	// value of semaphore
	int blockedproc[MAXPROCS]; // list of processes blocked on this semephore
	int head;
	int tail;
	int size;  // used to check if the blocked processes list is empty
} semtab[MAXSEMS];


/* 	InitSem () is called when kernel starts up.  Initialize data
 *  	structures (such as semaphore table) and call any initialization
 *   	procedures here. 
 */

void InitSem ()
{
	int s;

	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {		// mark all sems free
		semtab[s].valid = FALSE;
	}
}

/* 	MySeminit (p, v) is called by the kernel whenever the system
 *  	call Seminit (v) is called.  The kernel passes the initial
 *  	value v, along with the process ID p of the process that called
 * 	Seminit.  MySeminit should allocate a semaphore (find free entry
 * 	in semtab and allocate), initialize that semaphore's value to v,
 *  	and then return the ID (i.e., index of allocated entry). 
 */

int MySeminit (int p, int v)
{
	int s;

	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {
		if (semtab[s].valid == FALSE) {
			break;
		}
	}
	if (s == MAXSEMS) {
		DPrintf ("No free semaphores.\n");
		return (-1);
	}

	semtab[s].valid = TRUE;
	semtab[s].value = v;
	semtab[s].head = 0;
	semtab[s].tail = -1;
	semtab[s].size = 0;

	return (s);
}

/*   	MyWait (p, s) is called by the kernel whenever the system call
 * 	Wait (s) is called. 
 */

void MyWait (p, s)
	int p;				// process
	int s;				// semaphore
{
	/* modify or add code any way you wish */
  if(semtab[s].value == 0){ // add the calling process to the blocked list on sem s and block
	semtab[s].tail = (++semtab[s].tail) % MAXPROCS;  //pre increment tail, wraps around, update tail position 
	semtab[s].blockedproc[semtab[s].tail] = p;  // add to list
	semtab[s].size++;
  DPrintf("size = %d head = %d tail = %d added %d to the queue\n",semtab[s].size, semtab[s].blockedproc[semtab[s].head],semtab[s].blockedproc[semtab[s].tail],p);
	Block(p);
  }else{
	semtab[s].value--;
  }
  
}

/*  	MySignal (p, s) is called by the kernel whenever the system call
 *  	Signal (s) is called.  
 */

void MySignal (p, s)
	int p;				// process
	int s;				// semaphore
{
  int pid;
	/* modify or add code any way you wish */
  //check if there's any blocking processes, if so, unblock the first 
  if(semtab[s].size > 0){
    pid = semtab[s].blockedproc[semtab[s].head];  //get the pid of the first process in the blocked processes list on sem s
    semtab[s].head = (++semtab[s].head) % MAXPROCS;  //update head position 
    semtab[s].size--;
    DPrintf("size = %d head = %d tail = %d unblocking: %d\n",semtab[s].size, semtab[s].blockedproc[semtab[s].head],semtab[s].blockedproc[semtab[s].tail],pid);
    Unblock(pid);
  }else{
    semtab[s].value++;
  }
}

