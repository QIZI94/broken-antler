

#include "animations.h"
#include "animationshandler.h"
#include "buttonhandler.h"
#include <SoftPWM.h>

#define ALL_LEDS_ANIMATION_HELPER(direction, span) \
    AnimationDef(LedPosition::LEFT_FRONT,  (direction), (span)),\
	AnimationDef(LedPosition::LEFT_MIDDLE,  (direction), (span)),\
	AnimationDef(LedPosition::LEFT_BACK,  (direction), (span)),\
	AnimationDef(LedPosition::RIGHT_FRONT,   (direction), (span)),\
	AnimationDef(LedPosition::RIGHT_MIDDLE, (direction), (span)),\
	AnimationDef(LedPosition::RIGHT_BACK,    (direction), (span))


#define DEFINE_ANIMATION(...) \
{\
__VA_ARGS__,\
ANIM_END\
}
static const AnimationStep breathingAnimSteps[] = {
    
    AnimationStep{.brightness = 95, .duration = 700},
    AnimationStep{.brightness = 10, .duration = 800},
};
static const AnimationStep breathingAnimDelayedSteps[] = {
    STEP_DELAY(1000),
    AnimationStep{.brightness = 95, .duration = 2000},
    AnimationStep{.brightness = 10, .duration = 2000},
};

static const AnimationStep slowBreathingAnimSteps[] = {
    AnimationStep{.brightness = 0, .duration = 800},
    AnimationStep{.brightness = 95, .duration = 900},
    
};
static const AnimationStep flowAnimSteps[] = {
    AnimationStep{.brightness = 25, .duration = 700},
    AnimationStep{.brightness = 95, .duration = 800},
    
};
static const AnimationStep segmentedFlowAnimSteps[] = {
    AnimationStep{.brightness = 95, .duration = 300},
	AnimationStep{.brightness = 20, .duration = 300},
    
	STEP_DELAY(600)
    
};

static const AnimationStep leftRightFlowMidAnimSteps[] = {
    AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},


    
	
    
};


// LEFT RIGHT KIT-RIDER
constexpr uint16_t leftRightflowOffset = 1500;

static const AnimationStep leftRightFlowLeftFrontAnimSteps[] = {
    AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	STEP_DELAY(1000),
	AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	STEP_DELAY(leftRightflowOffset)
};
static const AnimationStep leftRightFlowLeftMiddleAnimSteps[] = {
	AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	STEP_DELAY(500),
	AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	STEP_DELAY(500 + leftRightflowOffset),
};
static const AnimationStep leftRightFlowLeftBackAnimSteps[] = {
	AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	//STEP_DELAY(rightFlowOffset),
	AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	STEP_DELAY(1000 + leftRightflowOffset)
	//STEP_DELAY(500)
};




static const AnimationStep leftRightFlowRightFrontAnimSteps[] = {
    AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	STEP_DELAY(1000+leftRightflowOffset),
	AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	//STEP_DELAY(500)
};
static const AnimationStep leftRightFlowRightMiddleAnimSteps[] = {
	AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	STEP_DELAY(500+leftRightflowOffset),
	AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	STEP_DELAY(500),
};
static const AnimationStep leftRightFlowRightBackAnimSteps[] = {
	AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	STEP_DELAY(leftRightflowOffset),
	AnimationStep{.brightness = 95, .duration = 500},
	AnimationStep{.brightness = 20, .duration = 500},
	STEP_DELAY(1000)
	//STEP_DELAY(500)
};



static const AnimationStep NoEffectSteps[] = {
	AnimationStep{.brightness = 95, .duration = 0},
	STEP_DELAY(1000)
};

static const AnimationStep disableSteps[] = {
	AnimationStep{.brightness = 0, .duration = 0},
	STEP_DELAY(1000)
};



// DRUM AND BASS
constexpr uint16_t beatDuration = 340;
static const AnimationStep dnbSnaresTimingsSteps[] = {

//snare
AnimationStep{.brightness = 100, .duration = 0},
STEP_DELAY(20),
//snare
AnimationStep{.brightness = 20, .duration = 0},
STEP_DELAY(beatDuration - 20),

};
static const AnimationStep dnbSnaresFastTimingsSteps[] = {

//snare
AnimationStep{.brightness = 100, .duration = 0},
STEP_DELAY(10),
//snare
AnimationStep{.brightness = 20, .duration = 0},
STEP_DELAY(beatDuration - 10),

};
static const AnimationStep dnbKickTimingsSteps[] = {
//kick
AnimationStep{.brightness = 100, .duration = 0},
STEP_DELAY(50),
//kick
AnimationStep{.brightness = 20, .duration = 0},
STEP_DELAY(beatDuration - 50),


//kick
AnimationStep{.brightness = 100, .duration = 0},
STEP_DELAY(50),
//kick
AnimationStep{.brightness = 20, .duration = 0},
STEP_DELAY(beatDuration - 50),

STEP_DELAY(100),
//kick
AnimationStep{.brightness = 100, .duration = 10},
STEP_DELAY(10),
//kick
AnimationStep{.brightness = 20, .duration = 10},
STEP_DELAY(beatDuration - 30),
//kick
AnimationStep{.brightness = 100, .duration = 10},
STEP_DELAY(10),
//kick
AnimationStep{.brightness = 20, .duration = 10},
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

static const AnimationDef allLedsOnAnim[] = DEFINE_ANIMATION(
	ALL_LEDS_ANIMATION_HELPER(AnimationDirection::FORWARD, MAKE_SPAN(NoEffectSteps))	
);

static const AnimationDef dmbBeatAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(dnbKickTimingsSteps)),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(dnbSnaresTimingsSteps)),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(dnbSnaresFastTimingsSteps)),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(dnbKickTimingsSteps)),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(dnbSnaresTimingsSteps)),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(dnbSnaresFastTimingsSteps))

);


static const AnimationDef leftRightAnim[] = DEFINE_ANIMATION(
		AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps), 400),
		AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps), 400),
		AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps), 400),
		AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps)),
		AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps)),
		AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(breathingAnimSteps))
);
static const AnimationDef segmentedFlowAnim[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 450),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 900),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 450),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(segmentedFlowAnimSteps), 900)
);
static const AnimationDef leftRightFlowAnim[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowLeftFrontAnimSteps), 0 + leftRightflowOffset/2),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowLeftMiddleAnimSteps), 250 + leftRightflowOffset/2),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowLeftBackAnimSteps), 500 + leftRightflowOffset/2),
	AnimationDef(LedPosition::RIGHT_BACK,   	AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowRightFrontAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowRightMiddleAnimSteps), 250),
	AnimationDef(LedPosition::RIGHT_FRONT,    	AnimationDirection::FORWARD, MAKE_SPAN(leftRightFlowRightBackAnimSteps), 500)
);

static const AnimationDef slowBreathingAnimation[] = DEFINE_ANIMATION(
	ALL_LEDS_ANIMATION_HELPER(AnimationDirection::BIDIRECTIONAL_FORWARD, MAKE_SPAN(slowBreathingAnimSteps))
);

static const AnimationDef flowAnimation[] = DEFINE_ANIMATION(
		AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 0),
		AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 300),
		AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 600),
		AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 0),
		AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 300),
		AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(flowAnimSteps), 600)
);

static const AnimationDef turnOffLedsAnimation[] = DEFINE_ANIMATION(
	ALL_LEDS_ANIMATION_HELPER(AnimationDirection::FORWARD, MAKE_SPAN(disableSteps))
);



static const AnimationDef* animationList[] = {
	allLedsOnAnim,
	leftRightAnim,
	slowBreathingAnimation,
	leftRightFlowAnim,
	dmbBeatAnimation,
	flowAnimation,
	segmentedFlowAnim,
	//animation2,
	//animation3,
	/*
	animation4,
	animation5,
	animation6,*/
	
	turnOffLedsAnimation

};

constexpr size_t animationListLength = LENGTH_OF_CONST_ARRAY(animationList);


volatile size_t selectionIndex = 0;
volatile bool eyesOn = false;
volatile bool longPressed = false;

void buttonSwitchAnimationHandler(ButtonEvent buttonEvent){
	if(buttonEvent == ButtonEvent::RELEASED && longPressed == false){
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
		eyesOn = !eyesOn;
		if(eyesOn){

			analogWrite(LED_Eye.blue.pin, 20);
			analogWrite(LED_Eye.red.pin, 100);
		}
		else {
			analogWrite(LED_Eye.blue.pin, 0);
			analogWrite(LED_Eye.red.pin, 0);
		}
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
	//setAnimation(turnOffLedsAnimation);
	setButtonHandlerFunc(buttonSwitchAnimationHandler);



	analogWrite(LED_Eye.blue.pin, 1);
	analogWrite(LED_Eye.red.pin, 1);


}

