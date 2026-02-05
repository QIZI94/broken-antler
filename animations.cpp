
#include "animations.h"
#include "animationshandler.h"
#include "buttonhandler.h"
#include "timer.h"


static const PROGMEM AnimationStep breathingAnimSteps[] = {
    
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 700},
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(10), .duration = 800},
};
static const PROGMEM AnimationStep breathingAnimDelayedSteps[] = {
    STEP_DELAY(1000),
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 2000},
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(10), .duration = 2000},
};

static const PROGMEM AnimationStep slowBreathingAnimSteps[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(0), .duration = 800},
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 900},
    
};
static const PROGMEM AnimationStep flowAnimSteps[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(25), .duration = 700},
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 800},
    
};
static const PROGMEM AnimationStep segmentedFlowAnimSteps[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 300},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 300},
    
	STEP_DELAY(600)
    
};

static const PROGMEM AnimationStep leftRightFlowMidAnimSteps[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},


    
	
    
};


// LEFT RIGHT KIT-RIDER
constexpr uint16_t leftRightflowOffset = 1500;

static const PROGMEM AnimationStep leftRightFlowLeftFrontAnimSteps[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	STEP_DELAY(1000),
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	STEP_DELAY(leftRightflowOffset)
};
static const PROGMEM AnimationStep leftRightFlowLeftMiddleAnimSteps[] = {
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	STEP_DELAY(500),
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	STEP_DELAY(500 + leftRightflowOffset),
};
static const PROGMEM AnimationStep leftRightFlowLeftBackAnimSteps[] = {
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	//STEP_DELAY(rightFlowOffset),
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	STEP_DELAY(1000 + leftRightflowOffset)
	//STEP_DELAY(500)
};




static const PROGMEM AnimationStep leftRightFlowRightFrontAnimSteps[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	STEP_DELAY(1000+leftRightflowOffset),
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	//STEP_DELAY(500)
};
static const PROGMEM AnimationStep leftRightFlowRightMiddleAnimSteps[] = {
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	STEP_DELAY(500+leftRightflowOffset),
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	STEP_DELAY(500),
};
static const PROGMEM AnimationStep leftRightFlowRightBackAnimSteps[] = {
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	STEP_DELAY(leftRightflowOffset),
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 500},
	STEP_DELAY(1000)
	//STEP_DELAY(500)
};

// VIU VIU POLICE
static const PROGMEM AnimationStep rightBlueViu[] = {
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(30, 0), .duration = 650},

	STEP_DELAY(100),
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(70, 0), .duration = 650},
	//STEP_DELAY(500)
};
static const PROGMEM AnimationStep leftRedViu[] = {
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(0, 35), .duration = 650},

	STEP_DELAY(100),
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(0, 100), .duration = 650},
	//STEP_DELAY(500)
};



static const PROGMEM AnimationStep NoEffectSteps[] = {
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(95), .duration = 0},
	STEP_DELAY(1000)
};

static const PROGMEM AnimationStep disableSteps[] = {
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(0), .duration = 0},
	STEP_DELAY(1000)
};



// DRUM AND BASS
constexpr uint16_t beatDuration = 340;
static const PROGMEM AnimationStep dnbSnaresTimingsSteps[] = {

	//snare
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(100), .duration = 0},
	STEP_DELAY(20),
	//snare
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 0},
	STEP_DELAY(beatDuration - 20),

};
static const PROGMEM AnimationStep dnbSnaresFastTimingsSteps[] = {

	//snare
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(100), .duration = 0},
	STEP_DELAY(10),
	//snare
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 0},
	STEP_DELAY(beatDuration - 10),

};
static const PROGMEM AnimationStep dnbKickTimingsSteps[] = {
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(100), .duration = 0},
	STEP_DELAY(50),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 0},
	STEP_DELAY(beatDuration - 50),


	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(100), .duration = 0},
	STEP_DELAY(50),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 0},
	STEP_DELAY(beatDuration - 50),

	STEP_DELAY(100),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(100), .duration = 10},
	STEP_DELAY(10),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 10},
	STEP_DELAY(beatDuration - 30),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(100), .duration = 10},
	STEP_DELAY(10),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20), .duration = 10},
	STEP_DELAY(beatDuration - 30),

};
/*
AnimationStep dnbTimingsSteps[] = {
	AnimationStep{.brightness = 100, .duration = 30},
	STEP_DELAY(50),
	AnimationStep{.brightness = 0, .duration = 0},
	STEP_DELAY(50),
	AnimationStep{.brightness = 100, .duration = 40},
	STEP_DELAY(80),
	AnimationStep{.brightness = 0, .duration = 30},
	STEP_DELAY(beatDuration - 50 - 50 - 40 - 30 - 80 )
};*/
/*
AnimationStep dnbTimingsSteps[] = {
	AnimationStep{.brightness = 100, .duration = 30},
	STEP_DELAY(50),
	AnimationStep{.brightness = 0, .duration = 0},
	STEP_DELAY(50),static const AnimationDef leftRightAnim[] = DEFINE_ANIMATION(
		AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps), 400),
		AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps), 400),
		AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps), 400),
		AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps)),
		AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps)),
		AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps))
);
	AnimationStep{.brightness = 100, .duration = 40},
	STEP_DELAY(80),
	AnimationStep{.brightness = 0, .duration = 0},
	STEP_DELAY(beatDuration - 50 - 80-30-30)
};*/

static const PROGMEM AnimationDef allLedsOnAnim[] = DEFINE_ANIMATION(
	ALL_LEDS_ANIMATION_HELPER(AnimationDirection::FORWARD, MAKE_SPAN(NoEffectSteps))	
);

static const PROGMEM AnimationDef dmbBeatAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(dnbKickTimingsSteps)),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(dnbSnaresTimingsSteps)),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(dnbSnaresFastTimingsSteps)),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(dnbKickTimingsSteps)),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(dnbSnaresTimingsSteps)),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(dnbSnaresFastTimingsSteps))

);


static const PROGMEM AnimationDef leftRightAnim[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps), 400),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps), 400),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps), 400),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps)),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps)),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps))
);
static const PROGMEM AnimationDef segmentedFlowAnim[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 450),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 900),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 450),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 900)
);
static const PROGMEM AnimationDef leftRightFlowAnim[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowLeftFrontAnimSteps), 0 + leftRightflowOffset/2),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowLeftMiddleAnimSteps), 250 + leftRightflowOffset/2),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowLeftBackAnimSteps), 500 + leftRightflowOffset/2),
	AnimationDef(LedPosition::RIGHT_BACK,   	AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowRightFrontAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowRightMiddleAnimSteps), 250),
	AnimationDef(LedPosition::RIGHT_FRONT,    	AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowRightBackAnimSteps), 500)
);

static const PROGMEM AnimationDef slowBreathingAnimation[] = DEFINE_ANIMATION(
	ALL_LEDS_ANIMATION_HELPER(AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(slowBreathingAnimSteps))
);

static const PROGMEM AnimationDef flowAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 300),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 600),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 300),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 600)
);

static const PROGMEM AnimationDef turnOffLedsAnimation[] = DEFINE_ANIMATION(
	ALL_LEDS_ANIMATION_HELPER(AnimationDirection::FORWARD, SequentialAnimationStepSpan(nullptr, nullptr))
);


static const PROGMEM AnimationDef viuviuPoliceAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(leftRedViu), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(leftRedViu), 0),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(leftRedViu), 0),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(rightBlueViu), 650 + 100),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(rightBlueViu), 650 + 100),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(rightBlueViu), 650 + 100)
);

// AUDIOLINK
constexpr uint8_t bassStepPatternDurations[]{
	50,20,20
};
constexpr float repeatBassDivisor = 1.6;
static const PROGMEM AnimationStep fastFlowAnimSteps[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(35,35), .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(50, 80), .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(35, 35), .duration = bassStepPatternDurations[2]},
	STEP_DELAY(20)
	//STEP_DELAY(10000)
    
};

static const PROGMEM AnimationDef bassAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 40),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 80),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 40),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 80)
);

static const PROGMEM AnimationStep fasterFlowAnimSteps[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(35,35), .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(50, 80), .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(35, 35), .duration = bassStepPatternDurations[2]},
	
	//STEP_DELAY(10000)
    
};
static const PROGMEM AnimationStep fasterFlowWithDelayAnimSteps[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(35,35), .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(50, 80), .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(35, 35), .duration = bassStepPatternDurations[2]},
	STEP_DELAY(10/repeatBassDivisor)
	//STEP_DELAY(10000)
    
};

static const PROGMEM AnimationDef repeatedBassAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fasterFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fasterFlowAnimSteps), 40 / repeatBassDivisor),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fasterFlowWithDelayAnimSteps), 80 / repeatBassDivisor),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fasterFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fasterFlowAnimSteps), 40 / repeatBassDivisor),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fasterFlowWithDelayAnimSteps), 80 / repeatBassDivisor)
);

static const PROGMEM AnimationStep slowFlowColorRotationAnimSteps[] = {

    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(50,90), .duration = 1400},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(70,70), .duration = 1400},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(90,50), .duration = 1400},

    
};

static const PROGMEM AnimationDef idleFlowColorRotation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(slowFlowColorRotationAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowColorRotationAnimSteps), 350),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(slowFlowColorRotationAnimSteps), 700),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowColorRotationAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowColorRotationAnimSteps), 350),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowColorRotationAnimSteps), 700)
);

static const PROGMEM AnimationStep slowFlowAnimSteps[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(35), .duration = 10},
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(80), .duration = 1500},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(35), .duration = 1300},
	//STEP_DELAY(10000)
    
};

static const PROGMEM AnimationDef idleFlow[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 350),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 700),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 350),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 700)
);





static const AnimationDef* animationList[] = {
	allLedsOnAnim,
	leftRightAnim,
	slowBreathingAnimation,
	leftRightFlowAnim,
	dmbBeatAnimation,
	flowAnimation,
	segmentedFlowAnim,
	idleFlow,
	idleFlowColorRotation,
	viuviuPoliceAnimation,

	
	turnOffLedsAnimation

};

constexpr size_t animationListLength = LENGTH_OF_CONST_ARRAY(animationList);

static TimedExecution1ms timedPressTimer;
volatile static size_t selectionIndex = 0;
volatile static bool eyesOn = false;
volatile static bool longPressed = false;
volatile static bool timedPress = false;

uint8_t lastEyesBlueBrightness = 0;
uint8_t lastEyesRedBrightness = 0;


const LedBrightness eyesBrightnessLevels[] = {
	{.blue = 0, .red = 0},
	{.blue = 5, .red = 25},
	{.blue = 50, .red = 200}
};
static constexpr uint8_t eyesBrightnessLevelsLength = LENGTH_OF_CONST_ARRAY(eyesBrightnessLevels);
const LedBrightness* lastEyesBrightnessPtr = &eyesBrightnessLevels[0];

void buttonSwitchAnimationHandler(ButtonEvent buttonEvent){

	if(buttonEvent == ButtonEvent::RELEASED && !longPressed && !timedPress){
		setAnimation(animationList[selectionIndex]);
		selectionIndex++;
		if(animationListLength <= selectionIndex){
			selectionIndex = 0;
		}
	}
	else if(longPressed == true){
		longPressed = false;
	}
	else if(buttonEvent == ButtonEvent::LONG_PRESSED){
		longPressed = true;
		setAudioLink(bassAnimation, repeatedBassAnimation, idleFlow,0);
	}
	if(buttonEvent == ButtonEvent::PRESSED){
		timedPressTimer.setup(
			[](TimedExecution1ms&){
				analogWrite(LED_Eye.blue.pin, 155);
				analogWrite(LED_Eye.red.pin, 255);

				timedPressTimer.setup(
					[](TimedExecution1ms&){
						analogWrite(LED_Eye.blue.pin, lastEyesBrightnessPtr->blue);
						analogWrite(LED_Eye.red.pin, lastEyesBrightnessPtr->red);
						timedPress = true;
					},
					50
				);
			},
			2000
		);
	}
	else if (buttonEvent == ButtonEvent::RELEASED && timedPress && !longPressed){
		timedPress = false;
		lastEyesBrightnessPtr++;

		if(lastEyesBrightnessPtr == &eyesBrightnessLevels[eyesBrightnessLevelsLength]){
			lastEyesBrightnessPtr = &eyesBrightnessLevels[0];
		}

		analogWrite(LED_Eye.blue.pin, lastEyesBrightnessPtr->blue);
		analogWrite(LED_Eye.red.pin, lastEyesBrightnessPtr->red);
	}
	else{
		timedPress = false;
		timedPressTimer.disable();
	}

	/*switch(state){
		
			case ButtonState::PRESSED:
				Serial.println("ButtonState::PRESSED");
				
				break;
			case ButtonState::RELEASED:
				
		
				Serial.println("ButtonState::RELEASED");
				break;
			case ButtonState::LONG_PRESS:
				Serial.println("ButtonState::LONG_PRESS");
				break;
		}*/
}


void initAnimationsSwitcher(){
	setAnimation(segmentedFlowAnim);
	//setAudioLink(bassAnimation, repeatedBassAnimation, idleFlow,0);
	setButtonHandlerFunc(buttonSwitchAnimationHandler);



	analogWrite(LED_Eye.blue.pin, lastEyesBrightnessPtr->blue);
	analogWrite(LED_Eye.red.pin, lastEyesBrightnessPtr->red);


}

