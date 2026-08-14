//#include "SoftPWM_timer.h"
//#include "SoftPWM.h"
#include <Arduino.h>

#include "animationshandler.h"
#include "timer.h"
#include "audiosampler.h"

#include "ledpwm.h"


#define PROGMEM_READ_STRUCTURE(p_dst, p_src) do { memcpy_P(p_dst, p_src, sizeof(*p_dst));} while (0)

#define TICKS_PROCESSING_SCALER (1)
#define DURATION_TO_TICKS(duration) (((duration)>>TICKS_PROCESSING_SCALER))

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
    const AnimationStep* volatile currentStep;

	public:
	const AnimationDef* volatile animationDef = nullptr;
	volatile AnimationDirection direction = AnimationDirection::BACKWARD;
	volatile AnimationRunModeState state = AnimationRunModeState::STOP;
	
};

static constexpr uint32_t DIMMING_PROCESSING_INTERVAL = 0x01 << TICKS_PROCESSING_SCALER;
using LedsDimming = DimmingPWM<LedsPWM>;
static LedsDimming ledsDimming;
static TimedExecution1ms dimmingProcessingTimer;

static LedAnimationStateTimer ledAnimationTimers[size_t(LedPosition::NUM_OF_ALL_LEDS)];
static TimedExecution1ms audioLinkSamplerTimer;
static TimedExecution1ms actionAnimationTimer;
static uint8_t animationSpeedShift = 0;
static volatile uint8_t activeAnimationsTimersCount = 0;

LedsDimming::DimmingStableStateList::IndexType ledIdToLedIndex(LedsDimming::StableIndex ledId){
	
	switch(ledId){
		case 3:  return 0;
		case 11: return 1;
		case 7:  return 2;
		case 4:  return 3;
		case 9:  return 4;
		case 8:  return 5;
		case 13: return 6;
		case 12: return 7;
		case 15: return 8;
		case 14: return 9;
		case 17: return 10;
		case 16: return 11;
		
	}
}





static void audioLinkHandler(uint16_t rawAudioInput);

inline bool isAnimationRunning(){
	return activeAnimationsTimersCount != 0;
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

		using BufferIndex = LedsPWM::BufferIndex;
		//ledsDimming.setPaused(false);
		
		//uint8_t previousBlueBrightness = ledsPWM.computeBrightness(led.red.pin,BufferIndex::Writable);
		//uint8_t previousRedBrightness = ledsPWM.computeBrightness(led.blue.pin, BufferIndex::Writable);
		LedsDimming::StableIndex redLedIndex = ledIdToLedIndex(led.red.pin);
		LedsDimming::StableIndex blueLedIndex = ledIdToLedIndex(led.blue.pin);

		uint8_t blueBrightness = led.blue.convertBrightness(currentStep->brightness.blue);
		uint8_t redBrightness = led.red.convertBrightness(currentStep->brightness.red);
		//blueBrightness = blueBrightness < 2 ? blueBrightness : blueBrightness - DIMMING_PROCESSING_INTERVAL;
		//redBrightness = redBrightness < 2 ? redBrightness : redBrightness - DIMMING_PROCESSING_INTERVAL;
		
		uint16_t ticks = DURATION_TO_TICKS(duration - (duration < DIMMING_PROCESSING_INTERVAL ? 0 : DIMMING_PROCESSING_INTERVAL));
		ledsDimming.setDimming(redLedIndex, led.red.pin, ledsDimming.getCurrentBrightness(redLedIndex), redBrightness, ticks);
		ledsDimming.setDimming(blueLedIndex, led.blue.pin,ledsDimming.getCurrentBrightness(blueLedIndex), blueBrightness, ticks);


		//lastActiveIndex =  ledsPWM.getActiveIndex();
		//SoftPWMSetFadeTime(led.blue.pin, duration, duration);
		//SoftPWMSetFadeTime(led.red.pin, duration, duration);
		//setAnimationLed(led, currentStep->brightness);
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
			const SequentialAnimationStepSpan& steps = loadedAnimDef.stepSpan;
			

			LedAnimationStateTimer& animStateTimer = ledAnimationTimers[animationIndex];


			if(steps.empty()){
				continue;
			}

			animStateTimer.assignAnimation(
				animationIt,
				loadedAnimDef.direction,
				runOnce ? AnimationRunModeState::RUN_ONCE : AnimationRunModeState::RUN
			);
			
			switch(loadedAnimDef.direction){
				case AnimationDirection::BIDIRECTIONAL_FORWARD:
				case AnimationDirection::FORWARD:

					animStateTimer.current(steps.begin());
					break;
				
				case AnimationDirection::BIDIRECTIONAL_BACKWARD:
				case AnimationDirection::BACKWARD:
					animStateTimer.current(steps.rbegin());
					break;
				
			}

			animStateTimer.setup(handleLedAnimation, loadedAnimDef.initialDelay);
			activeCount++;
			
			animationIndex++;
			animationIt++;
			
			
		}
	}
	ledsDimming.clear();
	for(LedDef led : LED_AllLeds){
		//SoftPWMSetFadeTime(led.blue.pin, 0, 0);
		//SoftPWMSetFadeTime(led.red.pin, 0, 0);
		//setAnimationLed(led, LedBrightness::from(0), true);
	
		/*ledsDimming.stopDimming(ledsDimming.findDimmingHandle(led.red.pin));
		ledsDimming.stopDimming(ledsDimming.findDimmingHandle(led.blue.pin));*/

		ledsPWM.setLedPWM(led.red.pin, 0);
		ledsPWM.setLedPWM(led.blue.pin, 0);
	}

	activeAnimationsTimersCount = activeCount;

}

static void changeAnimation(const AnimationDef* animation, bool runOnce = false, uint8_t speedShift = 0){
	uint8_t activeCount = 0;

	for(LedAnimationStateTimer& animStateTimer : ledAnimationTimers){
		animStateTimer.disable();
	}

	if(animation != nullptr){
		//ledsDimming.setPaused(true);
		const AnimationDef* animationIt = animation;
		size_t animationIndex = 0;

		AnimationDef loadedAnimDef(LedPosition::LEFT_BACK, AnimationDirection::FORWARD, SequentialAnimationStepSpan(nullptr, nullptr));;
		
		animationSpeedShift = speedShift;
		while(true){
			PROGMEM_READ_STRUCTURE(&loadedAnimDef, animationIt);
			if(!loadedAnimDef.isValid()){
				break;
			}
			const SequentialAnimationStepSpan& steps = loadedAnimDef.stepSpan;
			if(steps.empty()){
				continue;
			}
			const LedDef& currentLed = LED_AllLeds[uint8_t(loadedAnimDef.ledPosition)];
			currentLed.blue.pin;
			LedAnimationStateTimer& animStateTimer = ledAnimationTimers[animationIndex];
			//animStateTimer.lastBrightness = LedBrightness::from(255);

			/*LedsDimming::Node* foundExistingDimmingHandle = ledsDimming.findDimmingHandle(currentLed.blue.pin);
			if(foundExistingDimmingHandle != ledsDimming.dimmingStates.beforeBegin()){
				animStateTimer.lastBrightness.blue = foundExistingDimmingHandle->nextNode()->value.accumulatedBrightness >> LedsDimming::SHIFT_SCALE;
				foundExistingDimmingHandle->nextNode()->value.targetBrightness = animStateTimer.lastBrightness.blue;
				foundExistingDimmingHandle->nextNode()->value.tickRate = 0;
				
			}
			else {
				animStateTimer.lastBrightness.blue = 255;
			}

			foundExistingDimmingHandle = ledsDimming.findDimmingHandle(currentLed.red.pin);
			if(foundExistingDimmingHandle != ledsDimming.dimmingStates.beforeBegin()){
				animStateTimer.lastBrightness.red = foundExistingDimmingHandle->nextNode()->value.accumulatedBrightness >> LedsDimming::SHIFT_SCALE;
				foundExistingDimmingHandle->nextNode()->value.targetBrightness = animStateTimer.lastBrightness.red;
				foundExistingDimmingHandle->nextNode()->value.tickRate = 0;
			}
			else {
				animStateTimer.lastBrightness.red = 255;
			}*/
			

			animStateTimer.assignAnimation(
				animationIt,
				loadedAnimDef.direction,
				runOnce ? AnimationRunModeState::RUN_ONCE : AnimationRunModeState::RUN
			);
			
			switch(loadedAnimDef.direction){
				case AnimationDirection::BIDIRECTIONAL_FORWARD:
				case AnimationDirection::FORWARD:

					animStateTimer.current(steps.begin());
					break;
				
				case AnimationDirection::BIDIRECTIONAL_BACKWARD:
				case AnimationDirection::BACKWARD:
					animStateTimer.current(steps.rbegin());
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
			//SoftPWMSetFadeTime(led.blue.pin, 0, 0);
			//SoftPWMSetFadeTime(led.red.pin, 0, 0);
			//setAnimationLed(led, LedBrightness::from(0), true);
			LedsDimming::StableIndex redLedIndex = ledIdToLedIndex(led.red.pin);
			LedsDimming::StableIndex blueLedIndex = ledIdToLedIndex(led.blue.pin);
			ledsDimming.setDimming(redLedIndex, led.red.pin, 0, 0, DURATION_TO_TICKS(0));
			ledsDimming.setDimming(blueLedIndex, led.blue.pin, 0, 0, DURATION_TO_TICKS(0));
		}
	}
	activeAnimationsTimersCount = activeCount;
}


void actionAnimationHandler(TimedExecution1ms&);




static const AnimationDef* newSelectedAnimation = nullptr;
static const AnimationDef* newIdleActionAnimation = nullptr;
static uint16_t newBassVolumeThreshold = 30;
static uint8_t newEarlyRepeatTriggerCount = 0;
static bool runOnlyOnce = false;




void setAnimation(const AnimationDef* newAnimation, bool runOnce){
	newSelectedAnimation = newAnimation;
	runOnlyOnce = runOnce;
}

void setupActionAnimation(const AnimationDef *idleAnimation) {
	newSelectedAnimation = idleAnimation;
	newIdleActionAnimation = idleAnimation;
}




uint8_t getActiveAnimationTimersCount(){
	return activeAnimationsTimersCount;
}

const AnimationDef* getCurrentAnimation(){
	const AnimationDef* currentAnimation = ledAnimationTimers[0].animationDef;
	if(currentAnimation == nullptr){
		return nullptr;
	}
	return activeAnimationsTimersCount != 0 ? currentAnimation : nullptr; 
}


static const AnimationDef* volatile idleActionAnimation = nullptr;
static const AnimationDef* volatile triggeredActionAnimation = nullptr;


static void startActionAnimation(const AnimationDef* idleAnimation){
	triggeredActionAnimation = nullptr;
	idleActionAnimation = idleAnimation;
	startAnimation(idleAnimation);
	actionAnimationTimer.setup(actionAnimationHandler, 1);
}

static void stopActionAnimation(){
	actionAnimationTimer.disable();
}

void triggerActionAnimation(const AnimationDef *actionAnimation) {
	triggeredActionAnimation = actionAnimation;
}


void initAnimations(){
	//SoftPWMBegin();
#ifdef TIFR2
	//SOFTPWM_TIMER_INIT(SOFTPWM_OCR);
#endif
	ledsPWM.begin();
	//initAudioSampler(A7, 32);
	//setAudioSampleHandler(audioLinkHandler);
	dimmingProcessingTimer.setup(
		[](TimedExecution1ms&){
			ledsDimming.process<6>(ledsPWM);
			dimmingProcessingTimer.restart(1);
			//Serial.println("~~~~HERE");
		},
		1
	);
	/*audioLinkSamplerTimer.setup(
		[](TimedExecution1ms&){
			handleAudioSampling();
			audioLinkSamplerTimer.restart(1); // 1 ms
		}
		,1000 //ms
	);*/
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
	
	//ledsDimming.setPaused(true);
}

void handleAnimations(){
	/*Serial.print("Tim: ");
	Serial.println(ledAnimationTimers[0].getTimer().getCurrentCountDown());
	Serial.print("Triggered");
	Serial.println(triggered);
	Serial.print("TriggeredAddr: 0x");
	Serial.println(int(triggeredActionAnimation), HEX);

	
	bool a = false;
	noInterrupts();
	TimedExecution1ms::List list;
	uint8_t idx = 0;
	for(auto it = list.cbegin(); it != list.cend(); ++it, ++idx){
		timedExecutions[idx] = it.timedExecution;
	}
	
	interrupts();
	timedExecutions[idx] = nullptr;
	for(auto timedExec : timedExecutions){
		if(timedExec == nullptr){
			break;
		}
		bool found = false;
		for(const auto execAndNamePair : timedExecutionToString){
			if(execAndNamePair.key == timedExec){
				Serial.println(execAndNamePair.value);
				found = true;
				break;
			}
		}
		if(!found){
			Serial.println("unknown");
		}
	}*/

	if(newSelectedAnimation != nullptr){
		noInterrupts();
		//animationChangeDebounce.reset(20);
		if(newIdleActionAnimation != nullptr){
			startActionAnimation(newIdleActionAnimation);
		}
		/*if(newBassAnimations != nullptr){
			uint8_t bassAnimLength = 0;
			const AudioLinkBassAnimation* bassAnimationIt = newBassAnimations;
			while(true){
				AudioLinkBassAnimation loadedBassAnimation;
				PROGMEM_READ_STRUCTURE(&loadedBassAnimation, bassAnimationIt++);
				if(!loadedBassAnimation.isValid()){
					break;
				}
				++bassAnimLength;
			}
			
			
			startAudioLink(
				AudioLink{
					.idleAnimation=newSelectedAnimation,
					.bassAnimations = newBassAnimations,
					.bassVolumeThreshold = newBassVolumeThreshold,
					.earlyRepeatTriggerCount = newEarlyRepeatTriggerCount,
					.bassAnimationsLength = bassAnimLength
				}
			);
		}*/
		else {
			//stopAudioLink();
			stopActionAnimation();
			startAnimation(newSelectedAnimation, runOnlyOnce);
		}
		
		newSelectedAnimation = nullptr;
		newIdleActionAnimation = nullptr;
		interrupts();
	}
	//if(dimmingTimer.isDown()){
	//	ledsDimming.process(ledsPWM);
	//	dimmingTimer.restart(DIMMING_PROCESSING_INTERVAL);
	//}
	//setAnimationLed(LED_LeftFront, 80, true);
	//setAnimationLed(LED_LeftMiddle, 80, true);
	//setAnimationLed(LED_LeftBack, 80, true);
}

// ACTION ANIMATION

void actionAnimationHandler(TimedExecution1ms&){
	//Serial.println("here");
	if(triggeredActionAnimation){
		//Serial.print("ACT: ");
		//Serial.println(ptrdiff_t(triggeredActionAnimation), HEX);
		
		//if(ledAnimationTimers[0].animationDef == idleActionAnimation){
			changeAnimation(triggeredActionAnimation, true);
		//}
		triggeredActionAnimation = nullptr;
	}
	else if(activeAnimationsTimersCount == 0){
		changeAnimation(idleActionAnimation, false);
	}
	actionAnimationTimer.restart(1);
	
}



