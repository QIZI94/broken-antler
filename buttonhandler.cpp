
/*#pragma GCC optimize( \
  "O3", "inline-functions", "inline-functions-called-once", \
  "unswitch-loops", "peel-loops", "predictive-commoning", \
  "gcse-after-reload", "tree-loop-distribute-patterns", \
  "tree-slp-vectorize", "tree-loop-vectorize", "rename-registers", \
  "reorder-blocks", "reorder-blocks-and-partition", \
  "reorder-functions", "split-wide-types", "cprop-registers", \
  "ipa-cp-clone", "ipa-reference", "ipa-pure-const", "ipa-profile", "ipa-pta", \
  "tree-partial-pre", "tree-tail-merge", "ivopts", "web", \
  "cse-follow-jumps", "cse-skip-blocks", "reorder-blocks-algorithm=simple", \
  "split-paths", "vect-cost-model=dynamic", \
  "align-functions=2", "align-jumps=2", "align-loops=2", "inline-all-stringops" \
)*/
#pragma GCC optimize("O3", "inline-functions", "tree-vectorize", "unroll-loops")
#include <Arduino.h>

#include "buttonhandler.h"
#include "timer.h"
#include "adcsampler.h"


#define BUTTON_PIN (2)

enum InternalButtonEvent : uint8_t {
	NO_EVENT = 0x00,
	PRESSED_DEBOUNCE = 0x10,
	NOT_RELEASED = 0x20
};

void emptyButtonHandler(ButtonEvent){}

TimedExecution1ms buttonPressTimer;
volatile ButtonHandlerFunc buttonHandlerFuncPtr = emptyButtonHandler;




volatile uint8_t buttonEventMask = InternalButtonEvent::NO_EVENT;
volatile uint8_t internalButtonPin = 0xFF;


void buttonPress(TimedExecution1ms& timer){
	if(nonBlockingAnalogRead(internalButtonPin) > 800){
		buttonEventMask = ButtonEvent::PRESSED;
	}
}


void longPressHandlerFunc(TimedExecution1ms& timer){
	buttonEventMask = ButtonEvent::LONG_PRESSED;
}


void initButtonHandler(uint8_t buttonPin){
	buttonEventMask = InternalButtonEvent::NO_EVENT;
	internalButtonPin = buttonPin;
}

void setButtonHandlerFunc(ButtonHandlerFunc buttonHandler){
	buttonHandlerFuncPtr = buttonHandler;
}

void handleButtonEvents(){
	
	int analogButtonRead = nonBlockingAnalogRead(internalButtonPin);//analogRead(internalButtonPin);
	if(analogButtonRead > 800){
		constexpr uint8_t buttonPressedMask =  ButtonEvent::PRESSED |  ButtonEvent::LONG_PRESSED | InternalButtonEvent::PRESSED_DEBOUNCE | InternalButtonEvent::NOT_RELEASED;
		if(buttonPressTimer.isEnabled() == false && (buttonEventMask & buttonPressedMask) == 0){
      		buttonPressTimer.setup(buttonPress, 20);
			buttonEventMask = InternalButtonEvent::PRESSED_DEBOUNCE;
    	}
	}
	else if(buttonEventMask == InternalButtonEvent::NOT_RELEASED && (buttonEventMask & ButtonEvent::LONG_PRESSED) == 0){
		if(buttonPressTimer.isEnabled()){
			buttonPressTimer.disable();
		}
		buttonEventMask = ButtonEvent::RELEASED;
	}
	else if(buttonEventMask == InternalButtonEvent::PRESSED_DEBOUNCE){
		if(buttonPressTimer.isEnabled()){
			buttonPressTimer.disable();
		}
		buttonEventMask = InternalButtonEvent::NO_EVENT;
	}

	switch (buttonEventMask)
	{
		case ButtonEvent::PRESSED:
			buttonPressTimer.setup(longPressHandlerFunc, LONG_PRESS_TIME);
		case ButtonEvent::LONG_PRESSED: 
			
			buttonHandlerFuncPtr(ButtonEvent(buttonEventMask));
			buttonEventMask = InternalButtonEvent::NOT_RELEASED;
			break;
		case ButtonEvent::RELEASED:
			buttonHandlerFuncPtr(ButtonEvent(buttonEventMask));
			buttonEventMask = InternalButtonEvent::NO_EVENT;
		default:
			break;
	}
}