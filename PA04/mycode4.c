/*   	Umix thread package
 *
 */

#include <setjmp.h>
#include <string.h>
#include "aux.h"
#include "umix.h"
#include "mycode4.h"

static int MyInitThreadsCalled = 0;	// 1 if MyInitThreads called, else 0

static struct thread {			// thread table
	int valid;			// 1 if entry is valid, else 0
	jmp_buf env;			// current context
	jmp_buf env_cpy;
	void (*func)();
	int param;
	int ts;
} thread[MAXTHREADS];

int timestamp;

int curr_tid, new_tid, prev_tid;
int CallFromExit = 0;
int CallFromSched = 0;

#define STACKSIZE	65536		// maximum size of thread stack

/* 	MyInitThreads () initializes the thread package. Must be the first
 *  	function called by any user program that uses the thread package.  
 */

void MyInitThreads ()
{
	if (MyInitThreadsCalled) {		// run only once
		Printf ("MyInitThreads: should be called only once\n");
		Exit ();
	}

	for (int i = 0; i < MAXTHREADS; i++) {	// initialize thread table
		thread[i].valid = 0;
	}

	thread[0].valid = 1;			// initialize thread 0
	curr_tid = 0;
	new_tid = 0;
	timestamp = 0;
	thread[0].ts = ++timestamp;
  prev_tid = -1;

		for (int j = 0; j < MAXTHREADS; j++){
      
			char stack[j * STACKSIZE];
			if (((int) &stack[STACKSIZE-1]) - ((int) &stack[0]) + 1 != STACKSIZE) {
				Printf ("Stack space reservation failed\n");
				Exit ();
			}
			if ((setjmp (thread[j].env_cpy)) == 0) {
				//memcpy(thread[j].env_cpy,thread[j].env, sizeof(jmp_buf));
			}else{
        //DPrintf("now executing tid %d, its param is %d\n",curr_tid,thread[curr_tid].param);
        //curr_tid = MyGetThread();
        //thread[curr_tid].ts = ++timestamp;
        DPrintf("tid %d ts = %d about to execute\n",curr_tid,thread[curr_tid].ts);
        (*(thread[curr_tid].func))(thread[curr_tid].param);  // execute func (param)
				MyExitThread ();
			}
		}
   //longjmp (thread[0].env, 1);	// back to thread 0
	
	MyInitThreadsCalled = 1;
	//DPrintf("now finish calling init thread\n");
  //DPrintf("curr_tid = %d\n",curr_tid);
}

/*  	MyCreateThread (f, p) creates a new thread to execute
 * 	f (p), where f is a function with no return value and
 * 	p is an integer parameter.  The new thread does not begin
 *  	executing until another thread yields to it. 
 */

int MyCreateThread (f, p)
	void (*f)();			// function to be executed
	int p;				// integer parameter
{
	if (! MyInitThreadsCalled) {
		Printf ("MyCreateThread: Must call MyInitThreads first\n");
		Exit ();
	}
 
	// new thead IDs should be integers that are assigned in increasing order.
	int found = 0; 
	for(int i = new_tid + 1; i < MAXTHREADS; i++){
		if(thread[i].valid == 0){
			thread[i].valid = 1;
			new_tid = i;
			found = 1;
			break;
		}
	}
	//we have to reuse available thread id from the beginning now 
	if(found == 0){
		for(int i = 0; i < MAXTHREADS; i++){
			if(thread[i].valid == 0){
				thread[i].valid = 1;
				new_tid = i;
				found = 1;
				break;
			}
		}
	}
	// if running out of space
	if(found == 0){
		return(-1);
	}
	memcpy(thread[new_tid].env, thread[new_tid].env_cpy, sizeof(jmp_buf));

	thread[new_tid].func = f;
	thread[new_tid].param = p;
	thread[new_tid].ts = ++timestamp;
 DPrintf("now created new thread: tid %d ts = %d\n",new_tid,thread[new_tid].ts);



	return (new_tid);		// done, return new thread ID
}

/*   	MyYieldThread (t) causes the running thread, call it T, to yield to
 * 	thread t. Returns the ID of the thread that yielded to the calling
 *  	thread T, or -1 if t is an invalid ID.  Example: given two threads
 *  	with IDs 1 and 2, if thread 1 calls MyYieldThread (2), then thread 2
 * 	will resume, and if thread 2 then calls MyYieldThread (1), thread 1
 * 	will resume by returning from its call to MyYieldThread (2), which
 *  	will return the value 2.
 */

int MyYieldThread (t)
	int t;				// thread being yielded to
{
	if (! MyInitThreadsCalled) {
		Printf ("MyYieldThread: Must call MyInitThreads first\n");
		Exit ();
	}

	if (t < 0 || t >= MAXTHREADS) {
		Printf ("MyYieldThread: %d is not a valid thread ID\n", t);
		return (-1);
	}
	if (! thread[t].valid) {
		Printf ("MyYieldThread: Thread %d does not exist\n", t);
		return (-1);
	}

	curr_tid = MyGetThread();
  DPrintf("Yielding function tid %d is yielding to %d\n",curr_tid, t);
  prev_tid = curr_tid; 

  if(t == curr_tid){
//		return t;
  DPrintf("yielding to itself\n");
	}
 
	if (setjmp (thread[curr_tid].env) == 0) {  //save current thread context
        thread[curr_tid].ts = ++timestamp;  // put to the end of queue
        thread[t].ts = ++timestamp; //remove from queue; put it to the very front
        curr_tid = t; //small t
        DPrintf("tid %d (new ts = %d) is yielding to tid %d (new ts = %d)\n",prev_tid,thread[prev_tid].ts,t,thread[curr_tid].ts);
        longjmp (thread[t].env, 1);		  //switch to yielding thread
  }

  
  if (CallFromExit){
    CallFromExit = 0;
    return -1;
  }else if(CallFromSched){
    CallFromSched = 0;
    return -1;
  }else{
    if(thread[prev_tid].valid)
      return prev_tid;
    else
      return -1;    
  }
}

/*   	MyGetThread () returns ID of currently running thread. 
 */

int MyGetThread ()
{
//DPrintf("----------\n");
	if (! MyInitThreadsCalled) {
		Printf ("MyGetThread: Must call MyInitThreads first\n");
		Exit ();
	}
	return curr_tid;

}

/* 	MySchedThread () causes the running thread to simply give up the
 *  	CPU and allow another thread to be scheduled.  Selecting which
 *  	thread to run is determined here.  Note that the same thread may
 * 	be chosen (as will be the case if there are no other threads). 
 */

void MySchedThread ()
{
	if (! MyInitThreadsCalled) {
		Printf ("MySchedThread: Must call MyInitThreads first\n");
		Exit ();
	}
	//FIFO
	int next_tid;
	int foundfirst = 0;
	int min = 0;
 int index = 0;
  thread[curr_tid].ts = ++timestamp;
	for(int i = 0; i < MAXTHREADS; i++){
		if(thread[i].valid && thread[i].ts > -1){
			if(!foundfirst || thread[i].ts < min) {
				foundfirst = 1;
				index = i;
				min = thread[i].ts;
			}
		}
	}
	if(!foundfirst){
		Exit();
	}
  next_tid = index;
  CallFromSched = 1;
	MyYieldThread(next_tid);

}

/* 	MyExitThread () causes the currently running thread to exit. 
 */

void MyExitThread ()
{
  //DPrintf("now inside MyExitThread\n"); 
	if (! MyInitThreadsCalled) {
		Printf ("MyExitThread: Must call MyInitThreads first\n");
		Exit ();
	}
  //DPrintf("succsfully called my exit thread\n");
	thread[curr_tid].valid = 0;
	thread[curr_tid].ts = -1;
 //DPrintf("tid %d has exited. valid = %d ; ts = %d\n",curr_tid,thread[curr_tid].valid, thread[curr_tid].ts);
 int noneleft = 1;
 for(int i = 0; i < MAXTHREADS; i++){
   if(thread[i].valid){
     noneleft = 0;
     break;
   }
 }
 if(noneleft){
  DPrintf("done----------------------------------------------\n");
  Exit();
 }else{
  CallFromExit = 1;
  MySchedThread();
 }
  
}

