
#include "Arduino.h"

#include "timer.h"



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