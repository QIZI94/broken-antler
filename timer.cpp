
#include "Arduino.h"

#include "timer.h"

static constexpr uint8_t TIMER_COMPARE_1MS = 244;

volatile static uint32_t rtcRaw = 0;

// triggers roughly 1 ms (1024 us) at overflow
ISR(TIMER0_COMPB_vect) {
	TimedExecution1ms::StaticTimerBase::tickAllTimers();
  	TimedExecution1ms::executeAllTimedExecutions();
	++rtcRaw;
}

void initTimers(){
	// timer0
#ifdef TIMSK0
	//OCR0B = TIMER_COMPARE_1MS;               
  	TIMSK0 |= (1 << OCIE0B);
#endif
	//Timer1.initialize(1000);
	//Timer1.attachInterrupt(timerOneIsr);

}

void setRTC(uint32_t unixTime){
	noInterrupts();
	rtcRaw = unixTime;
	interrupts();
}

uint32_t rtcNow(){
	
	noInterrupts();
	volatile const uint32_t now = rtcRaw;
	interrupts();
	return now;
}
