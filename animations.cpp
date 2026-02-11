
#include "animations.h"
#include "animationshandler.h"
#include "buttonhandler.h"
#include "timer.h"
#include "eepromstorage.h"


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
	PERCENTAGE_TO_BRIGHTNESS(35,35),
	PERCENTAGE_TO_BRIGHTNESS(50, 80),
	PERCENTAGE_TO_BRIGHTNESS(35, 35),
};

constexpr LedBrightness bassRedStepPatternBrightnesses[] = {
	PERCENTAGE_TO_BRIGHTNESS(35,35),
	PERCENTAGE_TO_BRIGHTNESS(25, 80),
	PERCENTAGE_TO_BRIGHTNESS(35, 35),
};

constexpr LedBrightness bassBlueStepPatternBrightnesses[] = {
	PERCENTAGE_TO_BRIGHTNESS(35,35),
	PERCENTAGE_TO_BRIGHTNESS(50, 25),
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

static const PROGMEM AudioLinkBassAnimation audioLinkAnimations[] = DEFINE_AUDIO_LINK_BASS_ANIM(
	AudioLinkBassAnimation{.bassAnimation = bassAnimation, .repeatingBassAnimations = repeatedBassAnimation},
	AudioLinkBassAnimation{.bassAnimation = bassAnimation, .repeatingBassAnimations = repeatedBassAnimation},
	AudioLinkBassAnimation{.bassAnimation = bassAnimation, .repeatingBassAnimations = repeatedBassAnimation},
	AudioLinkBassAnimation{.bassAnimation = bassRedAnimation, .repeatingBassAnimations = repeatedRedBassAnimation},
	AudioLinkBassAnimation{.bassAnimation = bassBlueAnimation, .repeatingBassAnimations = repeatedBlueBassAnimation}
	//AudioLinkBassAnimation{.bassAnimation = bassColorRotateAnimation, .repeatingBassAnimations = repeatedColorRotateBassAnimation},
	//AudioLinkBassAnimation{.bassAnimation = bassColorRotateAnimation2, .repeatingBassAnimations = repeatedColorRotateBassAnimation2}
);


union AnimationStatePersistentStorage {
	static constexpr uint8_t AUDIO_LINK_ON_BIT_COUNT = 1;
	static constexpr uint8_t EYES_BRIGHTNESS_BIT_COUNT = 3;
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
volatile static size_t selectionIndex = 0;
volatile static bool longPressed = false;
volatile static bool timedPress = false;

volatile AnimationStatePersistentStorage animationStatePersistentStorage{0};
volatile bool enableStoreTimer = false;
static StaticTimer1ms lastAnimationStoreTimer;


uint8_t lastEyesBlueBrightness = 0;
uint8_t lastEyesRedBrightness = 0;


const LedBrightness eyesBrightnessLevels[] = {
	{.blue = 0, .red = 0},
	{.blue = 1, .red = 10},
	{.blue = 5, .red = 25},
	{.blue = 50, .red = 200}
};
static constexpr uint8_t eyesBrightnessLevelsLength = LENGTH_OF_CONST_ARRAY(eyesBrightnessLevels);
const LedBrightness* lastEyesBrightnessPtr = &eyesBrightnessLevels[0];

void buttonSwitchAnimationHandler(ButtonEvent buttonEvent){
	bool startStoreTimer = false;
	if(buttonEvent == ButtonEvent::RELEASED && !longPressed && !timedPress){
		setAnimation(animationList[selectionIndex]);
		animationStatePersistentStorage.selectionIndex = selectionIndex;
		animationStatePersistentStorage.audioLinkOn = 0;
		startStoreTimer = true;
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
		setAudioLink(idleFlow, 0, audioLinkAnimations);
		animationStatePersistentStorage.audioLinkOn = 1;
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
		timedPressTimer.disable();
	}


	if(startStoreTimer){
		enableStoreTimer = true;
		lastAnimationStoreTimer.restart(storeTime);
	}
}

void initAnimationsSwitcher(){
	// Perform compile time check for max values supported by EEPROM storage structure
	static_assert(1 <= AnimationStatePersistentStorage::MaxValueFromBitCount(AnimationStatePersistentStorage::AUDIO_LINK_ON_BIT_COUNT),
		"audioLinkOn is not 'bool'"
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

	if(animationStatePersistentStorage.audioLinkOn == 1){
		setAudioLink(idleFlow, 0, audioLinkAnimations);
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
}

void handleAnimationsPersistentStorage(){
	if(enableStoreTimer == true && lastAnimationStoreTimer.isDown()){
		if(!storeToEEPROM(animationStatePersistentStorage.value)){
			Serial.print(F("Failed to "));
		}

		Serial.println(F("store EEPROM animation state"));
		
		enableStoreTimer = false;
	}
}

