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
//#pragma GCC optimize("O3", "inline-functions", "tree-vectorize", "unroll-loops")


#include <avr/interrupt.h>
#include "timer.h"


#include "buttonhandler.h"
#include "animationshandler.h"
#include "animations.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "adcsampler.h"
#include "audiosampler.h"
#include "rtc.h"
#include "SchedPWM.h"

// remove me after done
#include "fixedforwardlist.h"

#include "panic.h"


#define BUTTON_HANDLER_SAMPLING_TIME_MS uint16_t(10)

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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

TimedExecution1ms testTime;

 static uint32_t startTime = 0;

volatile static uint32_t testTimeTook = 0;

void setup()
{
	Serial.begin(115200);
	Serial.println("Begining initialization");

	initSamplerADC();

	initButtonHandler(A6);
	buttonHandlerTimer.setup(timedButtonHandler, BUTTON_HANDLER_SAMPLING_TIME_MS);
	

	
	initAnimationsSwitcher();
	initTimers();
  	initAnimations();
	initRTC();

	/*FixedForwardList<10, int> list {10};
	using Node = FixedForwardList<10, int>::Node;
	for(const auto& a : list.nodes){
		Serial.print(a.nextIndex());
		Serial.print(" ");
		Serial.println(a.value);
		
	}



	Node* node = list.insertAfter(list.begin(), 1000);
	Node* node2 = list.insertAfter(node, 2000);
	Node* node3 = list.insertAfter(node2, 3000);

	list.removeAfter(node);
	list.removeAfter(node);
	list.insertAfter(node, 4000);
	list.removeAfter(list.end());
	list.removeAfter(list.end());

	 list.insertAfter(list.begin(), 2000);
	list.insertAfter(list.begin(), 3000);
	list.insertAfter(list.begin(), 3000);
	list.insertAfter(list.begin(), 4000);
	Serial.println(">>>>>>>>>>>>>>>>>>>>>>");

	Node* it = list.begin();
	while (it != list.end()){
		Serial.print(it->nextIndex());
		Serial.print(" ");
		Serial.println(it->value);
		it = list.nextNode(it);
	}
	
	Serial.println("-------------------------");

	


	for(auto& a : list.nodes){
		Serial.print(a.nextIndex());
		Serial.print(" ");
		Serial.println(a.value);
		
	}*/
	/*
	for(const auto& a : SchedPWM.steps){
		Serial.print("bitStorage: ");
		Serial.println(a.bitStorage);
		Serial.print("nextIndex: ");
		Serial.println(a.nextIndex);
		Serial.print("nextIsrTime: ");
		Serial.println(a.nextIsrTime);
		Serial.println();

	}
	Serial.println("---------------------");
	SchedPWM.setLedPWM(4, 50);
	SchedPWM.setLedPWM(3, 30);


	for(const auto& a : SchedPWM.steps){
		Serial.print("bitStorage: ");
		Serial.println(a.bitStorage);
		Serial.print("nextIndex: ");
		Serial.println(a.nextIndex);
		Serial.print("nextIsrTime: ");
		Serial.println(a.nextIsrTime);
		Serial.println();

	}*/
	Serial.println();
	
	//SchedPWM.steps.insertAfter(SchedPWM.steps.begin(), {});
	/*SchedPWM.pwmISR();
	//Serial.println(SchedPWM.steps.indexByNode(SchedPWM.steps.begin()));
	SchedPWM.setLedPWM(4, 50);
	
	SchedPWM.setLedPWM(3, 30);
	while(!SchedPWM.pwmISR());
	SchedPWM.setLedPWM(2, 10);
	while(!SchedPWM.pwmISR());
	//SchedPWM.pwmISR();
	SchedPWM.setLedPWM(4, 70);
	while(!SchedPWM.pwmISR());
	SchedPWM.setLedPWM(3, 20);
	while(!SchedPWM.pwmISR());
	SchedPWM.setLedPWM(2, 40);
	while(!SchedPWM.pwmISR());
	SchedPWM.setLedPWM(2, 20);
	while(!SchedPWM.pwmISR());
	SchedPWM.setLedPWM(2, 255);
	SchedPWM.setLedPWM(5, 255);
	while(!SchedPWM.pwmISR());

	Serial.print("Brightness: ");
	Serial.println(SchedPWM.computeBrightness(3));*/

	testScheduledPWM();

	//Serial.print("AAAA: ");
	//Serial.println(SchedPWM.isLedExclusive(0,0x02));

	/*testTime.setup([](TimedExecution1ms&){
		uint32_t cur = micros();
		testTimeTook = cur - startTime;
		startTime = cur;
		testTime.restart(1);
	}, 1, true);*/

/*
	if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
		Serial.println(F("SSD1306 allocation failed"));
		//for(;;); // Don't proceed, loop forever
	}
	else {
		  display.display();
			display.clearDisplay();
			display.setTextColor(WHITE);
			display.println("HERE");
			display.display();
	}*/

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.

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

static int lastTime  = 0;
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
  //if((micros() - lastTime) > 1000){
	//averageRawSignal();
	//handleAudioSampling();
	//Serial.println("here");
	//lastTime = micros();
 //}
 //debugAudioSampler();
 //delay(1);
  handleAnimations();
  //Serial.println(testTimeTook);
  delay(100);
  //SchedPWM.pwmISR();
  //PANIC("ARDUINO PANIC!!");
  //Serial.println(analogRead(A7));
  //attachInterrupt(digitalPinToInterrupt(A3))
}
