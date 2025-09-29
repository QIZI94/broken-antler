#ifndef TIMER_H
#define TIMER_H
#include "timedexecution.h"


using StaticTimer10ms = StaticTimer<10>;
using TimedExecution10ms = TimedExecution<StaticTimer10ms>;






extern void initTimers();


#endif