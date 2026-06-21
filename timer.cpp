
#include "Arduino.h"

#include "timer.h"

static constexpr uint8_t TIMER_COMPARE_1MS = 244;

volatile static uint32_t rtcOffset = 0;

// triggers roughly 1 ms (1024 us) at overflow
ISR(TIMER0_COMPB_vect) {
	// enable other interrupts to nest
	// in order to give this interrupt used for software timers lower priority,
	// than more time sensitive ones like Software PWM
	interrupts();
	TimedExecution1ms::StaticTimerBase::tickAllTimers();
  	TimedExecution1ms::executeAllTimedExecutions();
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
	rtcOffset = unixTime - micros();
	interrupts();
}

uint32_t rtcNow(){
	
	noInterrupts();
	const uint32_t now = rtcOffset + micros();
	interrupts();
	return now;
}
