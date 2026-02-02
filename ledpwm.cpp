#include "ledpwm.h"


ISR(TIMER2_COMPB_vect) {
	ledsPWM.pwmISR();
}