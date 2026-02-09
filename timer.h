#ifndef TIMER_H
#define TIMER_H
#include "timedexecution.h"


using StaticTimer1ms = StaticTimer<1, uint16_t>;
using TimedExecution1ms = TimedExecution<StaticTimer1ms>;

extern void initTimers();

extern void setRTC(uint32_t unixTime);
extern uint32_t rtcNow();

#endif