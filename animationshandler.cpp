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
  "align-functions=2", "align-jumps=2",      "align-loops=2", "inline-all-stringops" \
)*/
#pragma GCC optimize("O3", "inline-functions", "tree-vectorize", "unroll-loops")
#include <SoftPWM_timer.h>
#include <SoftPWM.h>
#include <Arduino.h>

#include "animationshandler.h"
#include "timer.h"
#include "audiosampler.h"

#define SOFTPWM_FREQ 120
#define SOFTPWM_OCR (F_CPU/(8UL*256UL*SOFTPWM_FREQ))

#define PROGMEM_READ_STRUCTURE(p_dst, p_src) do { memcpy_P(p_dst, p_src, sizeof(*p_dst));} while (0)

enum class AnimationRunModeState : uint8_t {
	RUN,
	RUN_ONCE,
	STOP
};


struct LedAnimationStateTimer : public TimedExecution1ms{
	void assignAnimation(const AnimationDef* animationDef, AnimationDirection direction, AnimationRunModeState state = AnimationRunModeState::RUN){
		this->animationDef = animationDef;
		this->direction = direction;
		this->state = state;
		current(nullptr);
	}

	const AnimationStep* current(const AnimationStep* newCurrentStep){
		currentStep = newCurrentStep;
		return currentStep;
	}

	const AnimationStep* current() const{
		return currentStep;
	}

    const AnimationStep* next(){
        return ++currentStep;
    }
    const AnimationStep* prev(){
        return --currentStep;
    }
    
	private:
    const AnimationStep* currentStep;

	public:
	const AnimationDef* animationDef = nullptr;
	AnimationDirection direction = AnimationDirection::BACKWARD;
	AnimationRunModeState state = AnimationRunModeState::STOP;
	
	
};



static LedAnimationStateTimer ledAnimationTimers[size_t(LedPosition::NUM_OF_ALL_LEDS)];
static TimedExecution1ms audioLinkSamplerTimer;
static volatile uint8_t activeAnimationsTimersCount = 0;

const AnimationDef audioLinkFeature[] = DEFINE_ANIMATION(
	ALL_LEDS_ANIMATION_HELPER(AnimationDirection::FORWARD, SequentialAnimationStepSpan(nullptr, nullptr))
);

extern void audioLinkHandler(uint16_t rawAudioInput);

static void setAnimationLed(LedDef led, uint8_t brightness, bool immediate = false){

	uint8_t redBrightness = led.red.convertBrightness(brightness);
	uint8_t blueBrightness = led.blue.convertBrightness(brightness);

	SoftPWMSetPercent(led.blue.pin, blueBrightness, immediate ? 1 : 0);
	SoftPWMSetPercent(led.red.pin, redBrightness, immediate ? 1 : 0);
}

inline bool isAnimationRunning(){
	/*for(const LedAnimationStateTimer& animTimer : ledAnimationTimers){
		if(animTimer.isEnabled()){
			return true;
		}
	}
	return false;*/
	return activeAnimationsTimersCount > 0;
}



static void handleLedAnimation(TimedExecution1ms& timer){
	LedAnimationStateTimer& processedAnimation =  reinterpret_cast<LedAnimationStateTimer&>(timer);
	if(processedAnimation.state == AnimationRunModeState::STOP){
		//processedAnimation.animationDef = nullptr;
		//processedAnimation.current(nullptr);
		activeAnimationsTimersCount--;
		return;
	}
	AnimationStep loadedStep = {};
	PROGMEM_READ_STRUCTURE(&loadedStep, processedAnimation.current());
    const AnimationStep* currentStep = &loadedStep;
	uint16_t duration = currentStep->duration;
	
	AnimationDef loadedAnimDef(LedPosition::LEFT_BACK, AnimationDirection::FORWARD, SequentialAnimationStepSpan(nullptr, nullptr));
	PROGMEM_READ_STRUCTURE(&loadedAnimDef, processedAnimation.animationDef);
	const SequentialAnimationStepSpan* steps = &loadedAnimDef.stepSpan;
	if(!currentStep->isDelay()){
		LedDef led = LED_AllLeds[size_t(loadedAnimDef.ledPosition)];
		SoftPWMSetFadeTime(led.blue.pin, duration, duration);
		SoftPWMSetFadeTime(led.red.pin, duration, duration);
		setAnimationLed(led, currentStep->brightness);
	}

	if(processedAnimation.state == AnimationRunModeState::RUN_ONCE){
		switch(processedAnimation.direction){
			case AnimationDirection::FORWARD:
			case AnimationDirection::BIDIRECTIONAL_FORWARD:
				if(processedAnimation.next() == steps->end()){
					processedAnimation.state = AnimationRunModeState::STOP;
				}
				break;
			case AnimationDirection::BACKWARD:
			case AnimationDirection::BIDIRECTIONAL_BACKWARD:
				if(processedAnimation.prev() == steps->rend()){
					processedAnimation.state = AnimationRunModeState::STOP;
				}
				break;
		}
	}
	else {

		switch(processedAnimation.direction){
			
			case AnimationDirection::FORWARD:
				if(processedAnimation.next() == steps->end()){
					processedAnimation.current(steps->begin());
				}
				break;
			case AnimationDirection::BIDIRECTIONAL_FORWARD:
				if(processedAnimation.next() == steps->end()){
					processedAnimation.current(steps->rbegin());
					if(processedAnimation.prev() == steps->rend()){
						processedAnimation.current(steps->rbegin());
					}
					processedAnimation.direction = AnimationDirection::BIDIRECTIONAL_BACKWARD;
				}
				break;
			
			case AnimationDirection::BACKWARD:
				if(processedAnimation.prev() == steps->rend()){
					processedAnimation.current(steps->rbegin());
				}
				break;
			case AnimationDirection::BIDIRECTIONAL_BACKWARD:
				if(processedAnimation.prev() == steps->rend()){
					processedAnimation.current(steps->begin());
					if(processedAnimation.next() == steps->end()){
						processedAnimation.current(steps->begin());
					}
					
					processedAnimation.direction = AnimationDirection::BIDIRECTIONAL_FORWARD;
				}
				break;
		}
	}
	timer.restart(duration);
}


static void startAnimation(const AnimationDef* animation, bool runOnce = false){
	uint8_t activeCount = 0;
	for(LedAnimationStateTimer& animStateTimer : ledAnimationTimers){
		animStateTimer.disable();
	}

	if(animation != nullptr){
		const AnimationDef* animationIt = animation;
		size_t animationIndex = 0;

		AnimationDef loadedAnimDef(LedPosition::LEFT_BACK, AnimationDirection::FORWARD, SequentialAnimationStepSpan(nullptr, nullptr));;
		

		while(true){
			PROGMEM_READ_STRUCTURE(&loadedAnimDef, animationIt);
			if(!loadedAnimDef.isValid()){
				break;
			}
			const SequentialAnimationStepSpan* steps = &loadedAnimDef.stepSpan;

			LedAnimationStateTimer& animStateTimer = ledAnimationTimers[animationIndex];
			
			animStateTimer.assignAnimation(
				animationIt,
				loadedAnimDef.direction,
				runOnce ? AnimationRunModeState::RUN_ONCE : AnimationRunModeState::RUN
			);
			
			switch(loadedAnimDef.direction){
				case AnimationDirection::BIDIRECTIONAL_FORWARD:
				case AnimationDirection::FORWARD:

					animStateTimer.current(steps->begin());
					break;
				
				case AnimationDirection::BIDIRECTIONAL_BACKWARD:
				case AnimationDirection::BACKWARD:
					animStateTimer.current(steps->rbegin());
					break;
				
			}

			animStateTimer.setup(handleLedAnimation, loadedAnimDef.initialDelay);
			activeCount++;
			
			animationIndex++;
			animationIt++;
			
			
		}
	}
	for(LedDef led : LED_AllLeds){
		SoftPWMSetFadeTime(led.blue.pin, 0, 0);
		SoftPWMSetFadeTime(led.red.pin, 0, 0);
		setAnimationLed(led, 0, true);
	}

	activeAnimationsTimersCount = activeCount;

}

static void changeAnimation(const AnimationDef* animation, bool runOnce = false){
	uint8_t activeCount = 0;
	if(animation != nullptr){
		
		const AnimationDef* animationIt = animation;
		size_t animationIndex = 0;

		AnimationDef loadedAnimDef(LedPosition::LEFT_BACK, AnimationDirection::FORWARD, SequentialAnimationStepSpan(nullptr, nullptr));;
		

		while(true){
			PROGMEM_READ_STRUCTURE(&loadedAnimDef, animationIt);
			if(!loadedAnimDef.isValid()){
				break;
			}
			const SequentialAnimationStepSpan* steps = &loadedAnimDef.stepSpan;

			LedAnimationStateTimer& animStateTimer = ledAnimationTimers[animationIndex];
			
			animStateTimer.assignAnimation(
				animationIt,
				loadedAnimDef.direction,
				runOnce ? AnimationRunModeState::RUN_ONCE : AnimationRunModeState::RUN
			);
			
			switch(loadedAnimDef.direction){
				case AnimationDirection::BIDIRECTIONAL_FORWARD:
				case AnimationDirection::FORWARD:

					animStateTimer.current(steps->begin());
					break;
				
				case AnimationDirection::BIDIRECTIONAL_BACKWARD:
				case AnimationDirection::BACKWARD:
					animStateTimer.current(steps->rbegin());
					break;
				
			}

			animStateTimer.setup(handleLedAnimation, loadedAnimDef.initialDelay);
			activeCount++;
			
			animationIndex++;
			animationIt++;
			
		}
	}
	else {
		for(LedAnimationStateTimer& animStateTimer : ledAnimationTimers){
			animStateTimer.disable();
		}
		for(LedDef led : LED_AllLeds){
			SoftPWMSetFadeTime(led.blue.pin, 0, 0);
			SoftPWMSetFadeTime(led.red.pin, 0, 0);
			setAnimationLed(led, 0, true);
		}
	}
	activeAnimationsTimersCount = activeCount;
}



const AnimationDef* newSelectedAnimation = nullptr;
bool runOnlyOnce = false;

void setAnimation(const AnimationDef* newAnimation, bool runOnce){
	newSelectedAnimation = newAnimation;
	runOnlyOnce = runOnce;
}

extern void audioLinkHandler(uint16_t avgSample, uint16_t avgOverTime, uint16_t baseline);



void initAnimations(){
	SoftPWMBegin();
#ifdef TIFR2
	SOFTPWM_TIMER_INIT(SOFTPWM_OCR);
#endif
	initAudioSampler(A7, 32);
	setAudioSampleHandler(audioLinkHandler);
	audioLinkSamplerTimer.setup(
		[](TimedExecution1ms&){
			handleAudioSampling();
			audioLinkSamplerTimer.restart(1); // 1 ms
		}
		,1000 //ms
	);
	/*SoftPWMSetFadeTime(LED_LeftFront.blue.pin,0, 0);
	SoftPWMSetFadeTime(LED_LeftFront.red.pin, 0, 0);
	setAnimationLed(LED_LeftFront, 7, true);*/
	
	/*pinMode(LED_EyeLeft.blue.pin, OUTPUT);
	pinMode(LED_EyeLeft.red.pin, OUTPUT);
	pinMode(LED_EyeRight.blue.pin, OUTPUT);
	pinMode(LED_EyeRight.red.pin, OUTPUT);


	analogWrite(LED_EyeLeft.blue.pin, 20);
	analogWrite(LED_EyeLeft.red.pin, 20);
	analogWrite(LED_EyeRight.blue.pin, 20);
	analogWrite(LED_EyeRight.red.pin, 20);*/


	//setAnimationLed(LED_EyeLeft, 50, true);
	
}

void handleAnimations(){
	if(newSelectedAnimation != nullptr){
		noInterrupts();
		//animationChangeDebounce.reset(20);
		startAnimation(newSelectedAnimation, runOnlyOnce);
		
		if(newSelectedAnimation == audioLinkFeature){

		}
		newSelectedAnimation = nullptr;
		interrupts();
	}
	//setAnimationLed(LED_LeftFront, 80, true);
	//setAnimationLed(LED_LeftMiddle, 80, true);
	//setAnimationLed(LED_LeftBack, 80, true);
}



// AUDIOLINK

static const PROGMEM AnimationStep fastFlowAnimSteps[] = {
    AnimationStep{.brightness = 25, .duration = 50},
    AnimationStep{.brightness = 95, .duration = 25},
	AnimationStep{.brightness = 25, .duration = 25},
	//STEP_DELAY(10000)
    
};

static const PROGMEM AnimationDef fastFlowAnimation[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 40),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 80),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 40),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(fastFlowAnimSteps), 80)
);

static const PROGMEM AnimationStep slowFlowAnimSteps[] = {
    AnimationStep{.brightness = 30, .duration = 10},
    AnimationStep{.brightness = 60, .duration = 1500},
	AnimationStep{.brightness = 30, .duration = 1300},
	//STEP_DELAY(10000)
    
};

static const PROGMEM AnimationDef slowFlow[] = DEFINE_ANIMATION(
	AnimationDef(LedPosition::LEFT_FRONT,  		AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 0),
	AnimationDef(LedPosition::LEFT_MIDDLE,  	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 350),
	AnimationDef(LedPosition::LEFT_BACK,  		AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 700),
	AnimationDef(LedPosition::RIGHT_FRONT,   	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 0),
	AnimationDef(LedPosition::RIGHT_MIDDLE, 	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 350),
	AnimationDef(LedPosition::RIGHT_BACK,    	AnimationDirection::FORWARD, MAKE_SPAN(slowFlowAnimSteps), 700)
);




static uint8_t stayOn1 = 0;
void audioLinkHandler(uint16_t avgSample, uint16_t avgOverTime, uint16_t baseline){
	
	static LowPassFilterFixed bassFilter3(120.0, 1024);
	static HighPassFilterFixed highBassFilter3(80.0, 1024);
	
	//int filteredLowpass250 = (int)bassFilter1.filter((float)rawSample);
	int16_t filteredLowpass80 = bassFilter3.filter(highBassFilter3.filter(avgSample) + baseline);
	//filteredLowpass80 = bassFilter3.filter(filteredLowpass80);
	//filteredLowpass80 = bassFilter3.filter(filteredLowpass80);
	uint16_t lowPass120 = filteredLowpass80 < 0 ? 0 : filteredLowpass80;

	//Serial.print("rawSample: ");
	//Serial.println(filteredLowpass80);
	/*Serial.print(" avgSample: ");
	Serial.println(avgSample);*/
	/*Serial.print(" avgOverTime: ");
	Serial.print(avgOverTime);
	Serial.print(" baseline: ");
	Serial.print(baseline);
	Serial.print(" lowpass: ");
	Serial.print(filteredLowpass80);
	Serial.print(" duffraw: ");
	Serial.print(rawSample - baseline);
	Serial.print(" duffavg: ");
	Serial.println(avgSample - baseline);*/
	//Serial.print(" lowpass: ");
	//Serial.println(lowPass120);
	
	//SoftPWMSetFadeTime(LED_LeftFront.blue.pin,0, 0);
	//SoftPWMSetFadeTime(LED_LeftFront.red.pin, 0, 0);
	if(((lowPass120 > baseline && (lowPass120 - baseline) > 10))){
	//if(filteredLowpass80 > 400){
	//Serial.println("bass");
		
		
		//SoftPWMSetFadeTime(LED_LeftFront.blue.pin,7, 7);
		//SoftPWMSetFadeTime(LED_LeftFront.red.pin, 7, 7);
		

		//if(stayOn1 == 0){
			//Serial.print("rawSample: ");
			//Serial.println(lowPass120);
			//SoftPWMSetFadeTime(LED_LeftFront.blue.pin,30, 30);
			//SoftPWMSetFadeTime(LED_LeftFront.red.pin, 30, 30);
			//setAnimationLed(LED_LeftFront, 80);
			if(activeAnimationsTimersCount != 0 && ledAnimationTimers[0].animationDef != fastFlowAnimation){
				changeAnimation(fastFlowAnimation, true);
			}
			
			//digitalWrite(A1, HIGH);
			//digitalWrite(A0, HIGH);
			//stayOn1=50;
		//}
		//else {
		//	stayOn1--;
		//}
	}
	else {
		//digitalWrite(A1, LOW);
		//digitalWrite(A0, LOW);
		//SoftPWMSetFadeTime(LED_LeftFront.blue.pin,30, 30);
		//SoftPWMSetFadeTime(LED_LeftFront.red.pin, 30, 30);
		//setAnimationLed(LED_LeftFront, 30);
		if(activeAnimationsTimersCount == 0){
			changeAnimation(slowFlow);
		}
		//stayOn1=30;
		
	}
}



