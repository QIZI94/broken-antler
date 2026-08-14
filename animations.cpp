
#include "animations.h"
#include "animationshandler.h"
#include "buttonhandler.h"
#include "audiosampler.h"
#include "timer.h"
#include "eepromstorage.h"
#include "utils/variant.h"
#include "utils/orderedenumeratedarray.h"

#include "externaldev.h"

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


constexpr uint8_t DNB_MAX_BRIGHTNESS = 50;
constexpr uint8_t DNB_MIN_BRIGHTNESS = 20;
// DRUM AND BASS
constexpr uint16_t beatDuration = 340;
static const PROGMEM AnimationStep dnbSnaresTimingsSteps[] = {

	//snare
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MAX_BRIGHTNESS), .duration = 0},
	STEP_DELAY(20),
	//snare
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MIN_BRIGHTNESS), .duration = 0},
	STEP_DELAY(beatDuration - 20),

};
static const PROGMEM AnimationStep dnbSnaresFastTimingsSteps[] = {

	//snare
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MAX_BRIGHTNESS), .duration = 0},
	STEP_DELAY(10),
	//snare
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MIN_BRIGHTNESS), .duration = 0},
	STEP_DELAY(beatDuration - 10),

};
static const PROGMEM AnimationStep dnbKickTimingsSteps[] = {
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MAX_BRIGHTNESS), .duration = 0},
	STEP_DELAY(50),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MIN_BRIGHTNESS), .duration = 0},
	STEP_DELAY(beatDuration - 50),


	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MAX_BRIGHTNESS), .duration = 0},
	STEP_DELAY(50),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MIN_BRIGHTNESS), .duration = 0},
	STEP_DELAY(beatDuration - 50),

	STEP_DELAY(100),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MAX_BRIGHTNESS), .duration = 10},
	STEP_DELAY(10),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MIN_BRIGHTNESS), .duration = 10},
	STEP_DELAY(beatDuration - 30),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MAX_BRIGHTNESS), .duration = 10},
	STEP_DELAY(10),
	//kick
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(DNB_MIN_BRIGHTNESS), .duration = 10},
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

constexpr LedBrightness bassStepPatternBrightnesses[] = {
	PERCENTAGE_TO_BRIGHTNESS(25,25),
	PERCENTAGE_TO_BRIGHTNESS(100, 100),
	PERCENTAGE_TO_BRIGHTNESS(35, 35),
};

constexpr LedBrightness bassRedStepPatternBrightnesses[] = {
	PERCENTAGE_TO_BRIGHTNESS(25,25),
	PERCENTAGE_TO_BRIGHTNESS(15, 100),
	PERCENTAGE_TO_BRIGHTNESS(35, 35),
};

constexpr LedBrightness bassBlueStepPatternBrightnesses[] = {
	PERCENTAGE_TO_BRIGHTNESS(25,10),
	PERCENTAGE_TO_BRIGHTNESS(100, 10),
	PERCENTAGE_TO_BRIGHTNESS(35, 35),
};
constexpr float repeatBassDivisor = 1.6;
static const PROGMEM AnimationStep fastFlowAnimSteps[] = {
    AnimationStep{.brightness = bassStepPatternBrightnesses[0], .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = bassStepPatternBrightnesses[1], .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = bassStepPatternBrightnesses[2], .duration = bassStepPatternDurations[2]},
	STEP_DELAY(20)
	//STEP_DELAY(10000)
    
};
static const PROGMEM AnimationStep fastRedFlowAnimSteps[] = {
    AnimationStep{.brightness = bassRedStepPatternBrightnesses[0], .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = bassRedStepPatternBrightnesses[1], .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = bassRedStepPatternBrightnesses[2], .duration = bassStepPatternDurations[2]},
	STEP_DELAY(20)
	//STEP_DELAY(10000)
    
};

static const PROGMEM AnimationStep fastBlueFlowAnimSteps[] = {
    AnimationStep{.brightness = bassBlueStepPatternBrightnesses[0], .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = bassBlueStepPatternBrightnesses[1], .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = bassBlueStepPatternBrightnesses[2], .duration = bassStepPatternDurations[2]},
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

static const PROGMEM AnimationDef bassRedAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fastRedFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fastRedFlowAnimSteps), 40),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fastRedFlowAnimSteps), 80),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fastRedFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fastRedFlowAnimSteps), 40),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fastRedFlowAnimSteps), 80)
);

static const PROGMEM AnimationDef bassBlueAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fastBlueFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fastBlueFlowAnimSteps), 40),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fastBlueFlowAnimSteps), 80),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fastBlueFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fastBlueFlowAnimSteps), 40),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fastBlueFlowAnimSteps), 80)
);

static const PROGMEM AnimationDef bassColorRotateAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fastRedFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 40),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fastBlueFlowAnimSteps), 80),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fastRedFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 40),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fastBlueFlowAnimSteps), 80)
);

static const PROGMEM AnimationDef bassColorRotateAnimation2[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fastBlueFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 40),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fastRedFlowAnimSteps), 80),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fastBlueFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 40),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fastRedFlowAnimSteps), 80)
);

static const PROGMEM AnimationStep fasterFlowAnimSteps[] = {
    AnimationStep{.brightness = bassStepPatternBrightnesses[0], .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = bassStepPatternBrightnesses[1], .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = bassStepPatternBrightnesses[2], .duration = bassStepPatternDurations[2]},
	
	//STEP_DELAY(10000)
    
};
static const PROGMEM AnimationStep fasterFlowWithDelayAnimSteps[] = {
    AnimationStep{.brightness = bassStepPatternBrightnesses[0], .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = bassStepPatternBrightnesses[1], .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = bassStepPatternBrightnesses[2], .duration = bassStepPatternDurations[2]},
	STEP_DELAY(10/repeatBassDivisor)
	//STEP_DELAY(10000)
    
};

static const PROGMEM AnimationStep fasterRedFlowAnimSteps[] = {
    AnimationStep{.brightness = bassRedStepPatternBrightnesses[0], .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = bassRedStepPatternBrightnesses[1], .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = bassRedStepPatternBrightnesses[2], .duration = bassStepPatternDurations[2]},
	
	//STEP_DELAY(10000)
    
};
static const PROGMEM AnimationStep fasterRedFlowWithDelayAnimSteps[] = {
    AnimationStep{.brightness = bassRedStepPatternBrightnesses[0], .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = bassRedStepPatternBrightnesses[1], .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = bassRedStepPatternBrightnesses[2], .duration = bassStepPatternDurations[2]},
	STEP_DELAY(10/repeatBassDivisor)
	//STEP_DELAY(10000)
    
};

static const PROGMEM AnimationStep fasterBlueFlowAnimSteps[] = {
    AnimationStep{.brightness = bassBlueStepPatternBrightnesses[0], .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = bassBlueStepPatternBrightnesses[1], .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = bassBlueStepPatternBrightnesses[2], .duration = bassStepPatternDurations[2]},
	
	//STEP_DELAY(10000)
    
};
static const PROGMEM AnimationStep fasterBlueFlowWithDelayAnimSteps[] = {
    AnimationStep{.brightness = bassBlueStepPatternBrightnesses[0], .duration = bassStepPatternDurations[0]},
    AnimationStep{.brightness = bassBlueStepPatternBrightnesses[1], .duration = bassStepPatternDurations[1]},
	AnimationStep{.brightness = bassBlueStepPatternBrightnesses[2], .duration = bassStepPatternDurations[2]},
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

static const PROGMEM AnimationDef repeatedRedBassAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fasterRedFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fasterRedFlowAnimSteps), 40 / repeatBassDivisor),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fasterRedFlowWithDelayAnimSteps), 80 / repeatBassDivisor),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fasterRedFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fasterRedFlowAnimSteps), 40 / repeatBassDivisor),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fasterRedFlowWithDelayAnimSteps), 80 / repeatBassDivisor)
);

static const PROGMEM AnimationDef repeatedBlueBassAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fasterBlueFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fasterBlueFlowAnimSteps), 40 / repeatBassDivisor),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fasterBlueFlowWithDelayAnimSteps), 80 / repeatBassDivisor),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fasterBlueFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fasterBlueFlowAnimSteps), 40 / repeatBassDivisor),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fasterBlueFlowWithDelayAnimSteps), 80 / repeatBassDivisor)
);
static const PROGMEM AnimationDef repeatedColorRotateBassAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fasterRedFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fasterFlowAnimSteps), 40 / repeatBassDivisor),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fasterBlueFlowWithDelayAnimSteps), 80 / repeatBassDivisor),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fasterRedFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fasterFlowAnimSteps), 40 / repeatBassDivisor),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fasterBlueFlowWithDelayAnimSteps), 80 / repeatBassDivisor)
);
static const PROGMEM AnimationDef repeatedColorRotateBassAnimation2[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fasterBlueFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fasterFlowAnimSteps), 40 / repeatBassDivisor),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fasterRedFlowWithDelayAnimSteps), 80 / repeatBassDivisor),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fasterBlueFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fasterFlowAnimSteps), 40 / repeatBassDivisor),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fasterRedFlowWithDelayAnimSteps), 80 / repeatBassDivisor)
);




static const PROGMEM AnimationStep slowFlowColorRotationAnimSteps[] = {

    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(20,70), .duration = 1400},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(40,40), .duration = 1400},
	AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(60,20), .duration = 1400},

    
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


static constexpr utils::KeyValuePair<AnimationPreset, const AnimationDef*> animationEnumeratedList[] = {
	{AnimationPreset::ALL_LEDS_ON, 				allLedsOnAnim},
	{AnimationPreset::LEFT_RIGHT, 				leftRightFlowAnim},
	{AnimationPreset::SLOW_BREATHING, 			slowBreathingAnimation},
	{AnimationPreset::LEFT_RIGHT_FLOW, 			leftRightFlowAnim},
	{AnimationPreset::DMB_BEAT, 				leftRightFlowAnim},
	{AnimationPreset::FLOW, 					leftRightFlowAnim},
	{AnimationPreset::SEGMENTED_FLOW, 			leftRightFlowAnim},
	{AnimationPreset::IDLE_FLOW, 				leftRightFlowAnim},
	{AnimationPreset::IDLE_FLOW_COLOR_ROTATION, idleFlowColorRotation},
	{AnimationPreset::VIU_VIU_POLICE, 			viuviuPoliceAnimation},
	{AnimationPreset::TURN_OFF_LEDS, 			turnOffLedsAnimation},
	
};

static constexpr utils::KeyValuePair<AudioLinkIdlePreset, const AnimationDef*> idleAnimationEnumeratedList[] = {
	{AudioLinkIdlePreset::IDLE_FLOW_COLOR_ROTATION, idleFlowColorRotation},	
};

static constexpr utils::KeyValuePair<AudioLinkBassPreset, const AnimationDef*> bassAnimationEnumeratedList[] = {
	{AudioLinkBassPreset::FAST_FLOW, 				bassAnimation},	
	{AudioLinkBassPreset::FAST_FLOW_RED, 			bassRedAnimation},
	{AudioLinkBassPreset::FAST_FLOW_BLUE, 			bassBlueAnimation},
	{AudioLinkBassPreset::REPEAT_FAST_FLOW, 		repeatedBassAnimation},	
	{AudioLinkBassPreset::REPEAT_FAST_FLOW_RED, 	repeatedRedBassAnimation},
	{AudioLinkBassPreset::REPEAT_FAST_FLOW_BLUE, 	repeatedBlueBassAnimation},
};




static const auto FUTURE_animationList = utils::MakeOrderedArrayFromEnumeratedArray(animationEnumeratedList);
static const auto FUTURE_idleAnimationList = utils::MakeOrderedArrayFromEnumeratedArray(idleAnimationEnumeratedList);
static const auto FUTURE_bassAnimationList = utils::MakeOrderedArrayFromEnumeratedArray(bassAnimationEnumeratedList);

static constexpr utils::KeyValuePair<AudioLinkBassPreset, AudioLinkBassPreset> bassPresetPicker[] = {
	{AudioLinkBassPreset::FAST_FLOW, AudioLinkBassPreset::REPEAT_FAST_FLOW},
	{AudioLinkBassPreset::FAST_FLOW, AudioLinkBassPreset::REPEAT_FAST_FLOW},
	{AudioLinkBassPreset::FAST_FLOW, AudioLinkBassPreset::REPEAT_FAST_FLOW},
	{AudioLinkBassPreset::FAST_FLOW_RED, AudioLinkBassPreset::REPEAT_FAST_FLOW_RED},
	{AudioLinkBassPreset::FAST_FLOW_BLUE, AudioLinkBassPreset::REPEAT_FAST_FLOW_BLUE}
};
static constexpr uint8_t bassPresetPickerSize = LENGTH_OF_CONST_ARRAY(bassPresetPicker);

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

static const PROGMEM AudioLinkBassAnimation audioLinkAnimations[] = DEFINE_AUDIO_LINK_BASS_ANIM(
	AudioLinkBassAnimation{.bassAnimation = bassAnimation, .repeatingBassAnimations = repeatedBassAnimation},
	AudioLinkBassAnimation{.bassAnimation = bassAnimation, .repeatingBassAnimations = repeatedBassAnimation},
	AudioLinkBassAnimation{.bassAnimation = bassAnimation, .repeatingBassAnimations = repeatedBassAnimation},
	AudioLinkBassAnimation{.bassAnimation = bassRedAnimation, .repeatingBassAnimations = repeatedRedBassAnimation},
	AudioLinkBassAnimation{.bassAnimation = bassBlueAnimation, .repeatingBassAnimations = repeatedBlueBassAnimation}
	//AudioLinkBassAnimation{.bassAnimation = bassColorRotateAnimation, .repeatingBassAnimations = repeatedColorRotateBassAnimation},
	//AudioLinkBassAnimation{.bassAnimation = bassColorRotateAnimation2, .repeatingBassAnimations = repeatedColorRotateBassAnimation2}
);


static const PROGMEM AnimationStep allBlueOn[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(50, 0), .duration = 0},
	STEP_DELAY(1000)
};

static const PROGMEM AnimationDef audioLinkHighSensitivityIndicatorAnim[] = DEFINE_ANIMATION(
	ALL_LEDS_ANIMATION_HELPER(AnimationDirection::FORWARD, MAKE_SPAN(allBlueOn))
);

static const PROGMEM AnimationStep allRedOn[] = {
    AnimationStep{.brightness = PERCENTAGE_TO_BRIGHTNESS(0, 50), .duration = 0},
	STEP_DELAY(1000)
};

static const PROGMEM AnimationDef audioLinkLowSensitivityIndicatorAnim[] = DEFINE_ANIMATION(
	ALL_LEDS_ANIMATION_HELPER(AnimationDirection::FORWARD, MAKE_SPAN(allRedOn))
);

enum AudioLinkSensitivity : uint16_t{
	DISABLED,
	HIGH_SENSITIVITY,
	LOW_SENSITIVITY,
	SENSITIVITY_MODES_COUNT
};
union AnimationStatePersistentStorage {
	static constexpr uint8_t AUDIO_LINK_ON_BIT_COUNT = 2;
	static constexpr uint8_t EYES_BRIGHTNESS_BIT_COUNT = 2;
	static constexpr uint8_t SELECTION_INDEX_BIT_COUNT = 4;

	static constexpr uint64_t MaxValueFromBitCount(uint8_t bitCount){
		return (0x01 << bitCount) - 1;
	}
	static constexpr int64_t MaxValueFromBitCount(int8_t bitCount){
		return (0x01 << bitCount) - 1;
	}
	struct{
		uint16_t audioLinkOn	: AUDIO_LINK_ON_BIT_COUNT;
		uint16_t eyesBrightness	: EYES_BRIGHTNESS_BIT_COUNT;
		uint16_t selectionIndex	: SELECTION_INDEX_BIT_COUNT;
	};
	uint16_t value;
};



constexpr uint16_t storeTime = 7000;

static TimedExecution1ms timedPressTimer;
static TimedExecution1ms delayAnimationStartTimer;
static TimedExecution1ms audioLinkSamplerTimer;
volatile static size_t selectionIndex = 0;
volatile static bool longPressed = false;
volatile static bool timedPress = false;

volatile AnimationStatePersistentStorage animationStatePersistentStorage{0};
volatile bool enableStoreTimer = false;
static StaticTimer1ms lastAnimationStoreTimer;


struct AudioLinkSettings{
	uint16_t bassVolumeThreshold;
	uint8_t earlyRepeatTriggerCount;
} static audioLinkSettings;


const static LedBrightness eyesBrightnessLevels[] = {
	{.blue = 0, .red = 0},
	{.blue = 1, .red = 10},
	{.blue = 5, .red = 25},
	{.blue = 50, .red = 200}
};
static constexpr uint8_t eyesBrightnessLevelsLength = LENGTH_OF_CONST_ARRAY(eyesBrightnessLevels);
static const LedBrightness* lastEyesBrightnessPtr = &eyesBrightnessLevels[0];

Variant<AnimationPreset, AudioLinkIdlePreset, AudioLinkBassPreset> currentAnimationTrigger;

void delayedAnimationHandler(TimedExecution1ms&){
	switch(currentAnimationTrigger){
		case currentAnimationTrigger.indexOf<AnimationPreset>():
			setAnimation(animationList[uint8_t(currentAnimationTrigger.get<AnimationPreset>())]);
			break;
		case currentAnimationTrigger.indexOf<AudioLinkIdlePreset>():
			//setAudioLink(idleAnimationList[uint8_t(currentAnimationTrigger.get<AudioLinkIdlePreset>())]);
			break;
		case currentAnimationTrigger.indexOf<AudioLinkBassPreset>():
			triggerActionAnimation(FUTURE_bassAnimationList[uint8_t(currentAnimationTrigger.get<AudioLinkBassPreset>())]);
			//playBassAnimation(bassAnimationList[uint8_t(currentAnimationTrigger.get<AudioLinkBassPreset>())]);
			break;
		default:
			break;
	}
	//currentAnimationTrigger.reset();
}

static void timedLateStartAudioLink(TimedExecution1ms&){
	uint16_t bassVolumeThreshold;
	switch (animationStatePersistentStorage.audioLinkOn)
	{
		case AudioLinkSensitivity::HIGH_SENSITIVITY:
			bassVolumeThreshold = 40;
			break;
		case AudioLinkSensitivity::LOW_SENSITIVITY:
			bassVolumeThreshold = 105;
			break;
		default:

			break;
	}
	setupActionAnimation(idleFlowColorRotation);
	audioLinkSettings.bassVolumeThreshold = bassVolumeThreshold;
	audioLinkSettings.earlyRepeatTriggerCount = 0;
	audioLinkSamplerTimer.setup(
		[](TimedExecution1ms&){
			handleAudioSampling();
			audioLinkSamplerTimer.restart(1); // 1 ms
		},
		1000
	);
	//setAudioLink(idleFlowColorRotation, 0, audioLinkAnimations, bassVolumeThreshold);
}

static void startAudioLinkPreset(AudioLinkSensitivity bassVolumeMode){
	uint16_t bassVolumeThreshold;
	switch (bassVolumeMode)
	{
		case AudioLinkSensitivity::HIGH_SENSITIVITY:
			setAnimation(audioLinkHighSensitivityIndicatorAnim);
			break;
		case AudioLinkSensitivity::LOW_SENSITIVITY:
			setAnimation(audioLinkLowSensitivityIndicatorAnim);
			break;
		default:

			break;
	}
	timedPressTimer.setup(timedLateStartAudioLink, 1000);
	
}

static void buttonSwitchAnimationHandler(ButtonEvent buttonEvent){
	bool startStoreTimer = false;
	if(buttonEvent == ButtonEvent::RELEASED && !longPressed && !timedPress){
		audioLinkSamplerTimer.disable();
		setAnimation(animationList[selectionIndex]);
		animationStatePersistentStorage.selectionIndex = selectionIndex;
		animationStatePersistentStorage.audioLinkOn = AudioLinkSensitivity::DISABLED;
		startStoreTimer = true;
		selectionIndex++;
		if(animationListLength <= selectionIndex){
			selectionIndex = 0;
		}
	}
	// do nothing upon releasing button after LONG_PRESSED
	else if(longPressed == true){
		longPressed = false;
	}
	else if(buttonEvent == ButtonEvent::LONG_PRESSED){
		longPressed = true;
		
		uint16_t currentAudioLinkSensitivity = animationStatePersistentStorage.audioLinkOn;
		if(currentAudioLinkSensitivity != AudioLinkSensitivity::DISABLED){
			if(AudioLinkSensitivity::SENSITIVITY_MODES_COUNT == ++currentAudioLinkSensitivity){
				currentAudioLinkSensitivity = AudioLinkSensitivity::HIGH_SENSITIVITY;
			}
		}
		else {
			currentAudioLinkSensitivity = AudioLinkSensitivity::HIGH_SENSITIVITY;
		}

		startAudioLinkPreset(AudioLinkSensitivity(currentAudioLinkSensitivity));	
		animationStatePersistentStorage.audioLinkOn = currentAudioLinkSensitivity;
		startStoreTimer = true;
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
		animationStatePersistentStorage.eyesBrightness = lastEyesBrightnessPtr - (&eyesBrightnessLevels[0]);
		startStoreTimer = true;
	}
	else{
		
		timedPress = false;
		if(timedPressTimer.getExecFunction() != timedLateStartAudioLink){
			timedPressTimer.disable();
		}
	}


	if(startStoreTimer){
		enableStoreTimer = true;
		lastAnimationStoreTimer.restart(storeTime);
	}
}
static void audioLinkHandler(uint16_t avgSample, uint16_t avgOverTime, uint16_t baseline);




void initAnimationsSwitcher(){
	initAudioSampler(A7, 32);
	setAudioSampleHandler(audioLinkHandler);
	/*simulateBassTimer.setup([](TimedExecution1ms& timer){
			uartMessageManager.handler.sendDeferredMessage(UniformMessage::Type::TIMED_EVENT);
			timer.restart(100);
		},
	1000
	);*/
	
	//startAnimationPreset(AnimationPreset::FLOW, 4);
	// Perform compile time check for max values supported by EEPROM storage structure
	static_assert(AudioLinkSensitivity::SENSITIVITY_MODES_COUNT <= AnimationStatePersistentStorage::MaxValueFromBitCount(AnimationStatePersistentStorage::AUDIO_LINK_ON_BIT_COUNT),
		"amount of sensitivity modes are over maximum EEPROM storage'"
	);
	static_assert((LENGTH_OF_CONST_ARRAY(eyesBrightnessLevels) - 1) <= AnimationStatePersistentStorage::MaxValueFromBitCount(AnimationStatePersistentStorage::EYES_BRIGHTNESS_BIT_COUNT),
		"eyesBrightness levels selection are over maximum EEPROM storage"
	);
	static_assert((LENGTH_OF_CONST_ARRAY(animationList) - 1) <= AnimationStatePersistentStorage::MaxValueFromBitCount(AnimationStatePersistentStorage::SELECTION_INDEX_BIT_COUNT),
		"Animations selectionIndex is over maximum EEPROM storage"
	);

	
	if(!loadFromEEPROM(animationStatePersistentStorage.value)){
		Serial.println(F("EEPROM Failed"));
		animationStatePersistentStorage.value = 0;
	}
	AudioLinkSensitivity loadedAudioLinkSensitivity = AudioLinkSensitivity(animationStatePersistentStorage.audioLinkOn);
	if(loadedAudioLinkSensitivity != AudioLinkSensitivity::DISABLED){
		startAudioLinkPreset(loadedAudioLinkSensitivity);
	}
	else {
		selectionIndex = animationStatePersistentStorage.selectionIndex;
		setAnimation(animationList[selectionIndex]);
		++selectionIndex;
		if(animationListLength <= selectionIndex){
			selectionIndex = 0;
		}
	}

	lastEyesBrightnessPtr = &eyesBrightnessLevels[animationStatePersistentStorage.eyesBrightness];

	/*Serial.println(lastAnimationState.audioLinkOn);
	Serial.println(lastAnimationState.eyesBrightness);
	Serial.println(lastAnimationState.selectionIndex);*/

	setButtonHandlerFunc(buttonSwitchAnimationHandler);

	analogWrite(LED_Eye.blue.pin, lastEyesBrightnessPtr->blue);
	analogWrite(LED_Eye.red.pin, lastEyesBrightnessPtr->red);

	// test audiolink
	//setupActionAnimation(idleFlowColorRotation);
	/*startAudioLinkPreset(AudioLinkSensitivity::LOW_SENSITIVITY);
	simulateBassTimer.setup([](TimedExecution1ms& timer){
			uartMessageManager.handler.sendDeferredMessage(UniformMessage::Type::TIMED_EVENT);
			//triggerActionAnimation(bassAnimation);
			triggerAudioLinkBass(AudioLinkBassPreset::FAST_FLOW, 4);
			timer.restart(2000);
		},
	50
	);*/
	//tst.restart(2000);
}

void handleAnimationsPersistentStorage(){
	/*Serial.print("sim: ");
	Serial.println(simulateBassTimer.isEnabled());*/

	/*if(tst.isDown()){
		noInterrupts();
		triggerActionAnimation(bassAnimation);
		tst.restart(2000);
		interrupts();
	}*/

	if(enableStoreTimer == true && lastAnimationStoreTimer.isDown()){
		if(!storeToEEPROM(animationStatePersistentStorage.value)){
			Serial.print(F("Failed to "));
		}

		Serial.println(F("store EEPROM animation state"));
		
		enableStoreTimer = false;
	}
}

void startAnimationPreset(AnimationPreset animPreset, uint16_t delayMs){
	currentAnimationTrigger = animPreset;
	delayAnimationStartTimer.setup(delayedAnimationHandler, delayMs);
}

void startAudioLink(AudioLinkIdlePreset idlePreset, uint16_t delayMs){
	currentAnimationTrigger = idlePreset;
	delayAnimationStartTimer.setup(delayedAnimationHandler, delayMs);
}

void triggerAudioLinkBass(AudioLinkBassPreset bassPreset, uint16_t delayMs){
	currentAnimationTrigger = bassPreset;
	delayAnimationStartTimer.setup(delayedAnimationHandler, delayMs);
}

static uint8_t bassAnimationSwitchCounter = 0;
static void audioLinkHandler(uint16_t avgSample, uint16_t avgOverTime, uint16_t baseline){

	static LowPassFilterFixed bassFilter3(120.0, 1024);
	static HighPassFilterFixed highBassFilter3(100.0, 1024);
	
	//int filteredLowpass250 = (int)bassFilter1.filter((float)rawSample);
	int16_t filteredLowpass80 = bassFilter3.filter(highBassFilter3.filter(avgSample) + baseline);
	//filteredLowpass80 = bassFilter3.filter(filteredLowpass80);
	//filteredLowpass80 = bassFilter3.filter(filteredLowpass80);
	uint16_t lowPass120 = filteredLowpass80 < 0 ? 0 : filteredLowpass80;
	//Serial.println("here");
	//triggerActionAnimation(bassAnimation);
	if(((lowPass120 > baseline && (lowPass120 - baseline) > audioLinkSettings.bassVolumeThreshold))){
			if(delayAnimationStartTimer.isEnabled()){
				return;
			}
			if(
				getCurrentAnimation() == idleFlowColorRotation
			){
				triggerAudioLinkBass(bassPresetPicker[bassAnimationSwitchCounter].key, 4);
				uartMessageManager.handler.sendDeferredMessage(UniformMessage::Type::TIMED_EVENT);
				//triggerActionAnimation(FUTURE_bassAnimationList[size_t(bassPresetPicker[bassAnimationSwitchCounter].key)]);
				//triggerAudioLinkBass(AudioLinkBassPreset::FAST_FLOW, 4);
			}
			// repeat bass animation
			else if(getActiveAnimationTimersCount() <= audioLinkSettings.earlyRepeatTriggerCount){		
				if(
					getCurrentAnimation() != idleFlowColorRotation
				){
					triggerAudioLinkBass(bassPresetPicker[bassAnimationSwitchCounter].value, 4);
					uartMessageManager.handler.sendDeferredMessage(UniformMessage::Type::TIMED_EVENT);
					//triggerActionAnimation(FUTURE_bassAnimationList[size_t(bassPresetPicker[bassAnimationSwitchCounter].value)]);					
				}
			}
			

	}

	
	if(bassPresetPickerSize <= ++bassAnimationSwitchCounter){
		bassAnimationSwitchCounter = 0;
	}
}
