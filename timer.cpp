#include "Arduino.h"

#include "timer.h"

#include <TimerOne.h>


void timerOneIsr(){
	TimedExecution1ms::StaticTimerBase::tickAllTimers();
  	TimedExecution1ms::executeAllTimedExecutions();

}
void initTimers(){
	Timer1.initialize(1000);
	Timer1.attachInterrupt(timerOneIsr);

}