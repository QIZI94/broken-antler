#ifndef LED_PWM_H
#define LED_PWM_H

#include "SchedPWM_ATmega328P.h"
#include "leddefinition.h"
using LedsPWM = SPWM_ATmega328P::ScheduledPWM_TIMER2<uint8_t(LedPosition::NUM_OF_ALL_LEDS) * 2>;
inline LedsPWM ledsPWM(6,240, 4);


#endif