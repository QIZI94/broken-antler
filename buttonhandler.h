#ifndef BUTTON_H
#define BUTTON_H

#define BUTTON_PIN (2)
#define LONG_PRESS_TIME (2000)

enum ButtonEvent: uint8_t {
	
	PRESSED = 0x01,
	RELEASED = 0x02,
	LONG_PRESSED = 0x04,
	ALL_PRESSED_EVENTS = PRESSED | LONG_PRESSED,
	ALL_EVENTS = PRESSED | RELEASED | LONG_PRESSED
};

using ButtonHandlerFunc = void (*)(ButtonEvent);

extern void setButtonHandlerFunc(ButtonHandlerFunc buttonHandler);
extern void initButtonHandler(uint8_t buttonPin);
extern void handleButtonEvents();

#endif
