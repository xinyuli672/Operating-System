// Testing two threads, but one yield and one sched
void test2() {
  int t, me;

  me = get_thread();  // T0
  t = create_thread(threadPrintYield, me);  //T1

  threadPrintSched(t);  // T0 running, called threadPrintSched(1)
  exit_thread();
}

void threadPrintSched(int t) {  // t = 0
  int i;
  for (i = 0; i < NUMYIELDS; i++) {
    Printf("T%d: iteration %d\n", get_thread(), i);
    sched_thread();
  }
}
void threadPrintYield(int t) {
  int i, yielder;
  for (i = 0; i < NUMYIELDS; i++) {
    Printf("T%d: iteration %d\n", get_thread(), i);
    yielder = yield_thread(t);
    Printf("Thread %d resumed by thread %d\n", get_thread(), yielder);
  }
}
