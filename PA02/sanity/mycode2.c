/* mycode2.c: your portion of the kernel
 *
 *   	Below are procedures that are called by other parts of the kernel. 
 * 	Your ability to modify the kernel is via these procedures.  You may
 *  	modify the bodies of these procedures any way you wish (however,
 *  	you cannot change the interfaces).  
 */

#include "aux.h"
#include "sys.h"
#include "mycode2.h"

#define TIMERINTERVAL 1	// in ticks (tick = 10 msec)


/* 	A sample process table. You may change this any way you wish. 
 */

static struct {
	int valid;
	int pid;
	int priority; 
  int req;
  int stride;
  int pass;
  int cpu;
  int test;
}proctab[MAXPROCS];
int ts = 0;
int timer = 0;


/* 	InitSched () is called when the kernel starts up.  First, set the
 *  	scheduling policy (see sys.h). Make sure you follow the rules
 *   	below on where and how to set it.  Next, initialize all your data
 * 	structures (such as the process table).  Finally, set the timer
 *  	to interrupt after a specified number of ticks. 
 */

void InitSched ()
{
	int i;

	/* First, set the scheduling policy. You should only set it
	 * from within this conditional statement.  While you are working
	 * on this assignment, GetSchedPolicy () will return NOSCHEDPOLICY. 
	 * Thus, the condition will be true and you may set the scheduling
	 * policy to whatever you choose (i.e., you may replace ARBITRARY).  
	 * After the assignment is over, during the testing phase, we will
	 * have GetSchedPolicy () return the policy we wish to test (and
	 * the policy WILL NOT CHANGE during the entirety of a test).  Thus
	 * the condition will be false and SetSchedPolicy (p) will not be
	 * called, thus leaving the policy to whatever we chose to test
	 * (and so it is important that you NOT put any critical code in
	 * the body of the conditional statement, as it will not execute when
	 * we test your program). 
	 */
	if (GetSchedPolicy () == NOSCHEDPOLICY) {	// leave as is
		SetSchedPolicy (PROPORTIONAL);		// set policy here
	}
		
	/* Initialize all your data structures here */
	for (i = 0; i < MAXPROCS; i++) {
		proctab[i].valid = 0;
	}

	/* Set the timer last */
	SetTimer (TIMERINTERVAL);
}


/*  	StartingProc (p) is called by the kernel when the process
 * 	identified by PID p is starting. This allows you to record the
 * 	arrival of a new process in the process table, and allocate
 *  	any resources (if necessary).  Returns 1 if successful, 0 otherwise. 
 */

int StartingProc (p)
	int p;				// process that is starting
{
	int i;

  switch (GetSchedPolicy ()) {

  	case LIFO:
    	for (i = 0; i < MAXPROCS; i++) {
    		if (! proctab[i].valid) {
    			proctab[i].valid = 1;
    			proctab[i].pid = p;
    			ts = ts + 1;
    			proctab[i].priority = ts;
          
    			//DPrintf("\ni = %d priority = %d p = %d\n\n",i,proctab[i].priority,p);
    			DoSched();
    			return (1);
    		}
    	}	
      break;
      
    default:
    	for (i = 0; i < MAXPROCS; i++) {
    		if (! proctab[i].valid) {
    			proctab[i].valid = 1;
    			proctab[i].pid = p;
    			ts = ts + 1;
    			proctab[i].priority = ts;
          proctab[i].req = 0;
          proctab[i].pass = 0;
          proctab[i].stride = 0;
          proctab[i].test = 0;
    			//DPrintf("\ni = %d priority = %d p = %d\n\n",i,proctab[i].priority,p);
    			//DoSched();
    			return (1);
    		}
    	}	
      break; 
  }
  
	DPrintf ("Error in StartingProc: no free table entries\n");
	return (0);
}
			

/*   	EndingProc (p) is called by the kernel when the process
 * 	identified by PID p is ending.  This allows you to update the
 *  	process table accordingly, and deallocate any resources (if
 *  	necessary).  Returns 1 if successful, 0 otherwise. 
 */


int EndingProc (p)
	int p;				// process that is ending
{
	int i;

  switch (GetSchedPolicy ()) {

	case LIFO:
    for (i = 0; i < MAXPROCS; i++) {
		  if (proctab[i].valid && proctab[i].pid == p) {
			  proctab[i].valid = 0;
			  //DPrintf("\nProcess %d is ending\n\n", proctab[i].pid);
			  DoSched();
			  return (1);
		  }
	  }
		break;
   
   default: 
     for (i = 0; i < MAXPROCS; i++) {
		  if (proctab[i].valid && proctab[i].pid == p) {
			  proctab[i].valid = 0;
			  //DPrintf("\nProcess %d is ending\n\n", proctab[i].pid);
			  //DoSched();            
			  return (1);
		  }
	  }
    break;
  }

  DPrintf ("Error in EndingProc: can't find process %d\n", p);
  return (0);

}


/* 	SchedProc () is called by kernel when it needs a decision for
 * 	which process to run next. It calls the kernel function
 *  	GetSchedPolicy () which will return the current scheduling policy
 *   	which was previously set via SetSchedPolicy (policy).  SchedProc ()
 * 	should return a process PID, or if there are no processes to run. 
 */

int SchedProc ()
{
	int i;
	int found_first = 0;
	int min = 0;
	int max = 0;
	int p = 0;
	int index;
  int m = 0;



	switch (GetSchedPolicy ()) {

	case ARBITRARY:

		for (i = 0; i < MAXPROCS; i++) {
			if (proctab[i].valid) {
				return (proctab[i].pid);
			}
		}
		break;

	case FIFO:

		/* your code here */
		for (i = 0; i < MAXPROCS; i++){
			if(proctab[i].valid){
				if(!found_first || proctab[i].priority < min){
					found_first = 1;
					p = proctab[i].pid;
					min = proctab[i].priority;
					index = i;
				}
			}
		}
		//DPrintf("fifo: next pid = %d, index = %d, priority = %d \n",p,index,proctab[index].priority);

		if(found_first) {
			return(p);
		}	
		break;



	case LIFO:

		/* your code here */
		for (i = 0; i < MAXPROCS; i++){
			if(proctab[i].valid){
				if(!found_first || proctab[i].priority > max) {
					found_first = 1;
					p = proctab[i].pid;
					max = proctab[i].priority;
					index = i;
				}
			}
		}	

		
		if(found_first){
			return(p);
		} 
		break;
   

	case ROUNDROBIN:

		/* your code here */
   for(i = 0; i < MAXPROCS; i++){
     if(proctab[i].valid){
       if(!found_first || proctab[i].priority < min) {
					found_first = 1;
					p = proctab[i].pid;
					min = proctab[i].priority;
					index = i;
				}
			}
    }
    
   if(found_first){
      ts = ts + 1;
			proctab[index].priority = ts;

      //DPrintf("pid %d is called, time = %d\n", p, proctab[index].time);
			return(p);
		} 

		break;


	case PROPORTIONAL:
 
		/* your code here */
    for(i = 0; i < MAXPROCS; i++){
     if(proctab[i].valid){
      //look for the one with smallest pass value
      if(!found_first || proctab[i].pass < min) {
					found_first = 1;
					p = proctab[i].pid;
					min = proctab[i].pass;
					index = i;
      }
     }      
    }
    
    if(found_first){
			proctab[index].pass += proctab[index].stride;  //pass = pass + stride
      
      proctab[index].test += 1;
			return(p);
		}
    
		break;
	}	
	return (0);
}


/*  	HandleTimerIntr () is called by the kernel whenever a timer
 *  	interrupt occurs.  Timer interrupts should occur on a fixed
 * 	periodic basis.
 */

void HandleTimerIntr ()
{
	SetTimer (TIMERINTERVAL);

	switch (GetSchedPolicy ()) {	// is policy preemptive?

	case ROUNDROBIN:		// ROUNDROBIN is preemptive
    DoSched ();
    break;
    
	case PROPORTIONAL:		// PROPORTIONAL is preemptive
		DoSched ();		// make scheduling decision
    timer++;
    if(timer % 100 == 0){
      for(int i = 0; i<MAXPROCS; i++){
        if(proctab[i].valid){ 
          //DPrintf("\npid: %d, requested CPU: %d, cpu: %d, times called: %d\n",proctab[i].pid, proctab[i].req, proctab[i].cpu, proctab[i].test);
          proctab[i].test = 0;
        }
      }
      //DPrintf("--------------------------------------------------\n");
   }
		break;

	default:			// if non-preemptive, do nothing
		break;
	}
}

/* 	MyRequestCPUrate (p, n) is called by the kernel whenever a process
 *  	identified by PID p calls RequestCPUrate (n).  This is a request for
 *   	n% of CPU time, i.e., requesting a CPU whose speed is effectively
 * 	n% of the actual CPU speed. Roughly n out of every 100 quantums
 *  	should be allocated to the calling process. n must be at least
 *  	0 and must be less than or equal to 100.  MyRequestCPUrate (p, n)
 * 	should return 0 if successful, i.e., if such a request can be
 * 	satisfied, otherwise it should return -1, i.e., error (including if
 *  	n < 0 or n > 100). If MyRequestCPUrate (p, n) fails, it should
 *   	have no effect on scheduling of this or any other process, i.e., AS
 * 	IF IT WERE NEVER CALLED.
 */

int MyRequestCPUrate (p, n)
	int p;				// process whose rate to change
	int n;				// percent of CPU time
{
  int index;
  int sum = 0;
  int count = 0;
  int share = 0;
  int unused_cpu;
  int temp = 0;
  
	/* your code here */
  if(n>=0 && n<=100){
    for(int i = 0; i < MAXPROCS; i++){
     if(proctab[i].valid){
       if(proctab[i].pid == p){
         index = i;
       }else{  // if process A is making a second request for say 30 cpu , when calculating the total cpu usage, we should not count A itself. 
         sum += proctab[i].req;
       } 
     }
    }
    sum = sum + n;
    //check if allocation , counting n, will exceed 100% cpu usage
    if(sum <= 100){
      proctab[index].req = n;
      proctab[index].cpu = n;
      if(n == 0){
        proctab[index].stride = 0;
      }else{
        proctab[index].stride = 100000 / n;
      }
      unused_cpu = 100 - sum;
      //loop through the processes table, count how many processes are with req ==0
      for(int i = 0; i < MAXPROCS; i++){
       if(proctab[i].valid && proctab[i].req == 0){
         count++;
       }
      }
      //if there's any processes with req == 0, distribute unused cpu evenly to them 
      if(count != 0){
//        if(unused_cpu < count){
//          share = 1;
//          for(int i = 0; i < MAXPROCS; i++){
//           if(proctab[i].valid && proctab[i].req == 0 && temp <= unused_cpu){
////             temp++;
////             proctab[i].cpu = share;
////             proctab[i].stride = 100000 / share; 
//           }
//          }
//        }else{
          share = unused_cpu / count;
          //loop through the processes table again, look for the ones with req == 0, then assign them with 
          for(int i = 0; i < MAXPROCS; i++){
           if(proctab[i].valid && proctab[i].req == 0){
             proctab[i].cpu = share;
             proctab[i].stride = 100000 / share; 
           }
          //}
        }
             
      }      
      return(0);
    }
  }
	return (-1);
}
