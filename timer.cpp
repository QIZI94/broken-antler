#include "Arduino.h"

#include "timer.h"



ISR(TIMER1_COMPA_vect) {	
  TimedExecution10ms::StaticTimerBase::tickAllTimers();
  TimedExecution10ms::executeAllTimedExecutions();
}

void initTimers(){
	
	#ifdef TCCR1A
	cli(); // disable interrupts during setup

	// Configure Timer 1 interrupt
	// F_clock = 16e6 Hz, prescaler = 64, Fs = 100 Hz
	TCCR1A = 0;
	TCCR1B = 1<<WGM12 | 0<<CS12 | 1<<CS11 | 1<<CS10;
	TCNT1 = 0;          // reset Timer 1 counter
	// OCR1A = ((F_clock / prescaler) / Fs) - 1 = 2499
	OCR1A = 2499;       // Set sampling frequency Fs = 100 Hz
	TIMSK1 = 1<<OCIE1A; // Enable Timer 1 interrupt
	
	sei(); // re-enable interrupts
	#endif
}