#ifndef RTC_H
#define RTC_H

#include <inttypes.h>

extern void initRTC();
extern void setRTC(uint32_t unixTime);
extern uint32_t rtcRawNow();


#endif