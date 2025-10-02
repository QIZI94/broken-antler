#include <SoftPWM_timer.h>
#include <SoftPWM.h>

#include <avr/interrupt.h>
#include "timer.h"


#include "buttonhandler.h"
#include "animationshandler.h"
#include "animations.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BUTTON_HANDLER_SAMPLING_TIME_MS uint16_t(1)

volatile bool buttonInterruptHappen = false;
volatile ButtonEvent lastState = ButtonEvent::RELEASED;

TimedExecution1ms buttonHandlerTimer;

void timedButtonHandler(TimedExecution1ms&){
	handleButtonEvents();
	buttonHandlerTimer.restart(BUTTON_HANDLER_SAMPLING_TIME_MS);
}

void printButtonHandler(ButtonEvent state){
	lastState = state;
	buttonInterruptHappen = true;
}
/*
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
*/

void setup()
{
	Serial.begin(115200);
	Serial.println("Begining initialization");



	initButtonHandler(A6);
	buttonHandlerTimer.setup(timedButtonHandler, BUTTON_HANDLER_SAMPLING_TIME_MS);
	

	initAnimations();
	initAnimationsSwitcher();
	initTimers();
/*

	if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();*/

	//setButtonHandlerFunc(printButtonHandler);

	// Initialize

	//digitalWrite(2, HIGH);

	//pinMode(3, INPUT);
	//digitalWrite(3, HIGH);
	//pinMode(A4, OUTPUT);
	//digitalWrite(A4, HIGH);

	//pinMode(A5, INPUT);
	/*
  SoftPWMBegin();
  Serial.begin(115200);
  // Create and set pin 13 to 0 (off)
  SoftPWMSet(13, 40);
  SoftPWMSet(12, 80);
  SoftPWMSet(11, 40);
  SoftPWMSet(10, 80);
  SoftPWMSet(9 ,  40);
  SoftPWMSet(8 ,  80);
  SoftPWMSet(7 ,  60);
  SoftPWMSet(6 ,  80);
  SoftPWMSet(5 ,  40);
  SoftPWMSet(4 ,  80);
  SoftPWMSet(A2 ,  40);
  SoftPWMSet(A1 ,  80);
  

  // Set fade time for pin 13 to 100 ms fade-up time, and 500 ms fade-down time
  SoftPWMSetFadeTime(13, 1000, 1000);
  SoftPWMSetFadeTime(12, 1000, 1000);
  SoftPWMSetFadeTime(11, 1000, 1000);
  SoftPWMSetFadeTime(10, 1000, 1000);
  SoftPWMSetFadeTime(9 , 1000, 1000);
  SoftPWMSetFadeTime(8 , 1000, 1000);
  SoftPWMSetFadeTime(7 , 1000, 1000);
  SoftPWMSetFadeTime(6 , 1000, 1000);
  SoftPWMSetFadeTime(5 , 1000, 1000);
  SoftPWMSetFadeTime(4 , 1000, 1000);
  SoftPWMSetFadeTime(A2 , 1000, 1000);
  SoftPWMSetFadeTime(A1 , 1000, 1000);
  */
  //pinMode(A3, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(2), buttonISR, CHANGE);
}

bool isOn = false;


void loop()
{
	//Serial.println(analogRead(A6));
  
	 // Turn on - set to 100%
  /*SoftPWMSetPercent(13, 100);
  SoftPWMSetPercent(12, 100);
  SoftPWMSetPercent(11, 100);
  SoftPWMSetPercent(10, 100);
  SoftPWMSetPercent(9 , 100);
  SoftPWMSetPercent(8 , 100);
  SoftPWMSetPercent(7 , 100);
  SoftPWMSetPercent(6 , 100);
  SoftPWMSetPercent(5 , 100);
  SoftPWMSetPercent(4 , 100);
  SoftPWMSetPercent(A2 , 100);
  SoftPWMSetPercent(A1 , 100);

  // Wait for LED to turn on - you could do other tasks here
  delay(1000);

  // Turn off - set to 0%
  //SoftPWMSetPercent(13, 10);
  SoftPWMSetPercent(12, 10);
  SoftPWMSetPercent(11, 10);
  SoftPWMSetPercent(10, 10);
  SoftPWMSetPercent(9 , 10);
  SoftPWMSetPercent(8 , 10);
  SoftPWMSetPercent(7 , 10);
  SoftPWMSetPercent(6 , 10);
  SoftPWMSetPercent(5 , 10);
  SoftPWMSetPercent(4 , 10);
  SoftPWMSetPercent(A2 , 10);
  SoftPWMSetPercent(A1 , 10);
  delay(1000);*/
  //Serial.println(digitalRead(2));
  if(buttonInterruptHappen){
	//Serial.println(millis());
	
	
	switch(lastState){
		case ButtonEvent::PRESSED:
			Serial.println("ButtonState::PRESSED");
			break;
		case ButtonEvent::RELEASED:
			Serial.println("ButtonState::RELEASED");
			break;
		case ButtonEvent::LONG_PRESSED:
			Serial.println("ButtonState::LONG_PRESS");
			break;
	}
	buttonInterruptHappen = false;
	
  }

  handleAnimations();
  //Serial.println(analogRead(A7));
  //attachInterrupt(digitalPinToInterrupt(A3))
}
