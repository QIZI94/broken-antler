
/*#pragma GCC optimize( \
  "O3", "inline-functions", "inline-functions-called-once", \
  "unswitch-loops", "peel-loops", "predictive-commoning", \
  "gcse-after-reload", "tree-loop-distribute-patterns", \
  "tree-slp-vectorize", "tree-loop-vectorize", "rename-registers", \
  "reorder-blocks", "reorder-blocks-and-partition", \
  "reorder-functions", "split-wide-types", "cprop-registers", \
  "ipa-cp-clone", "ipa-reference", "ipa-pure-const", "ipa-profile", "ipa-pta", \
  "tree-partial-pre", "tree-tail-merge", "ivopts", "web", \
  "cse-follow-jumps", "cse-skip-blocks", "reorder-blocks-algorithm=simple", \
  "split-paths", "vect-cost-model=dynamic", \
  "align-functions=2", "align-jumps=2", "align-loops=2", "inline-all-stringops" \
)*/
#pragma GCC optimize("O3", "inline-functions", "tree-vectorize", "unroll-loops")
#include "Arduino.h"

#include "timer.h"

#include <TimerOne.h>


ISR(TIMER0_COMPB_vect) {
	TimedExecution1ms::StaticTimerBase::tickAllTimers();
  	TimedExecution1ms::executeAllTimedExecutions();
}

void initTimers(){
	// timer0
#ifdef TIMSK0
	OCR0B = 244;               // triggers roughly 1 ms before overflow
  	TIMSK0 |= (1 << OCIE0B);   // enable Compare B interrupt
#endif
	//Timer1.initialize(1000);
	//Timer1.attachInterrupt(timerOneIsr);

}