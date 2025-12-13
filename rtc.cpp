#include "rtc.h"
#include "timer.h"




static TimedExecution1ms rtcTimer;

static void resetTimerForRTC(TimedExecution1ms&){
	rtcTimer.restart(UINT32_MAX);
}


void initRTC() {
	rtcTimer.setup(resetTimerForRTC, UINT32_MAX);
}

void setRTC(uint32_t unixTime){
	// this is ok to do without stopping global interrupts
	// because chance that its done during reset of the timer is very small and would occur after 49 days of timer running.
	// In practical case this is not an issues
	rtcTimer.getTimer().reset(UINT32_MAX - unixTime);
}

uint32_t rtcRawNow(){
	return UINT32_MAX - rtcTimer.getTimer().getCurrentCountDown();
}

