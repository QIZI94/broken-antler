//#include "SoftPWM_timer.h"
//#include "SoftPWM.h"
#include <Arduino.h>

#include "animationshandler.h"
#include "timer.h"
#include "audiosampler.h"

#include "ledpwm.h"


#define PROGMEM_READ_STRUCTURE(p_dst, p_src) do { memcpy_P(p_dst, p_src, sizeof(*p_dst));} while (0)

#define TICKS_PROCESSING_SCALER (1)
#define DURATION_TO_TICKS(duration) ((duration>>TICKS_PROCESSING_SCALER))

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
	LedBrightness lastBrightness = LedBrightness::from(0);
	
};

static constexpr uint32_t DIMMING_PROCESSING_INTERVAL = 0x01 << TICKS_PROCESSING_SCALER;
using LedsDimming = DimmingPWM<LedsPWM>;
static LedsDimming ledsDimming;
static TimedExecution1ms dimmingProcessingTimer;

static LedAnimationStateTimer ledAnimationTimers[size_t(LedPosition::NUM_OF_ALL_LEDS)];
static TimedExecution1ms audioLinkSamplerTimer;
static uint8_t animationSpeedShift = 0;
static volatile uint8_t activeAnimationsTimersCount = 0;



extern void audioLinkHandler(uint16_t rawAudioInput);

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
		uint8_t previousBlueBrightness = processedAnimation.lastBrightness.blue;
		uint8_t previousRedBrightness = processedAnimation.lastBrightness.red;
		uint8_t blueBrightness = led.blue.convertBrightness(currentStep->brightness.blue);
		uint8_t redBrightness = led.red.convertBrightness(currentStep->brightness.red);
		uint16_t ticks = DURATION_TO_TICKS(duration);
		ledsDimming.setDimming(led.red.pin, previousRedBrightness == 255 ? ledsPWM.computeBrightness(led.red.pin) : previousRedBrightness, redBrightness, ticks);
		ledsDimming.setDimming(led.blue.pin,  previousBlueBrightness == 255 ? ledsPWM.computeBrightness(led.blue.pin) : previousBlueBrightness, blueBrightness, ticks);

		processedAnimation.lastBrightness.blue = blueBrightness;
		processedAnimation.lastBrightness.red = redBrightness;
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
	timer.restart(duration + 4);
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
			animStateTimer.lastBrightness = LedBrightness::from(0);

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
			animStateTimer.lastBrightness = LedBrightness::from(255);

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

			ledsDimming.setDimming(led.red.pin, 0, 0, DURATION_TO_TICKS(0));
			ledsDimming.setDimming(led.blue.pin, 0, 0, DURATION_TO_TICKS(0));
		}
	}
	activeAnimationsTimersCount = activeCount;
}





static const AnimationDef* newSelectedAnimation = nullptr;
static const AudioLinkBassAnimation* newBassAnimations = nullptr;
static uint8_t newEarlyRepeatTriggerCount = 0;
static bool runOnlyOnce = false;




void setAnimation(const AnimationDef* newAnimation, bool runOnce){
	newSelectedAnimation = newAnimation;
	runOnlyOnce = runOnce;
}

void setAudioLink(const AnimationDef* idleAnimation, uint8_t earlyRepeatTriggerCount, const AudioLinkBassAnimation* bassAnimations){
	newSelectedAnimation = idleAnimation;
	newEarlyRepeatTriggerCount = earlyRepeatTriggerCount;
	newBassAnimations = bassAnimations;
}

struct AudioLink{
	const AnimationDef* idleAnimation;
	const AudioLinkBassAnimation* bassAnimations;
	uint8_t earlyRepeatTriggerCount;
	uint8_t bassAnimationsLength;
} static audioLink;



void startAudioLink(const AudioLink& newAudioLink){
	audioLink = newAudioLink;
	startAnimation(newAudioLink.idleAnimation);
	audioLinkSamplerTimer.setup(
		[](TimedExecution1ms&){
			handleAudioSampling();
			audioLinkSamplerTimer.restart(1); // 1 ms
		}
		,1000 //ms
	);
}

void stopAudioLink(){
	audioLinkSamplerTimer.disable();
}



extern void audioLinkHandler(uint16_t avgSample, uint16_t avgOverTime, uint16_t baseline);

StaticTimer1ms dimmingTimer;
void initAnimations(){
	//SoftPWMBegin();
#ifdef TIFR2
	//SOFTPWM_TIMER_INIT(SOFTPWM_OCR);
#endif
	ledsPWM.begin();
	initAudioSampler(A7, 32);
	setAudioSampleHandler(audioLinkHandler);
	/*dimmingProcessingTimer.setup(
		[](TimedExecution1ms&){
			ledsDimming.process<1>(ledsPWM);
			dimmingProcessingTimer.restart(DIMMING_PROCESSING_INTERVAL);
			//Serial.println("~~~~HERE");
		},
		1
	);*/
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
	dimmingTimer.restart(1);
	//ledsDimming.setPaused(true);
}

void handleAnimations(){
	if(newSelectedAnimation != nullptr){
		noInterrupts();
		//animationChangeDebounce.reset(20);
		if(newBassAnimations != nullptr){
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
					.earlyRepeatTriggerCount = newEarlyRepeatTriggerCount,
					.bassAnimationsLength = bassAnimLength
				}
			);
		}
		else {
			stopAudioLink();
			startAnimation(newSelectedAnimation, runOnlyOnce);
		}
		
		newSelectedAnimation = nullptr;
		newBassAnimations = nullptr;
		interrupts();
	}
	if(dimmingTimer.isDown()){
		ledsDimming.process(ledsPWM);
		dimmingTimer.restart(DIMMING_PROCESSING_INTERVAL);
	}
	//setAnimationLed(LED_LeftFront, 80, true);
	//setAnimationLed(LED_LeftMiddle, 80, true);
	//setAnimationLed(LED_LeftBack, 80, true);
}



// AUDIOLINK


static uint8_t stayOn1 = 0;
static uint8_t bassAnimationSwitchCounter = 0;
void audioLinkHandler(uint16_t avgSample, uint16_t avgOverTime, uint16_t baseline){

	static LowPassFilterFixed bassFilter3(120.0, 1024);
	static HighPassFilterFixed highBassFilter3(100.0, 1024);
	
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
	if(((lowPass120 > baseline && (lowPass120 - baseline) > 30))){
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
			//bool isCurrentAnimationBass = ledAnimationTimers[0].animationDef == audioLink.bassAnimation;
			//bool isAnyAnimationRunning = activeAnimationsTimersCount != 0;
			//bool shouldBassAnimationRepeat = activeAnimationsTimersCount <= 2;
			// regular bass animation
			if(
				activeAnimationsTimersCount != 0 &&
				ledAnimationTimers[0].animationDef == audioLink.idleAnimation
			){
				AnimationDef* bassAnimation;
				PROGMEM_READ_STRUCTURE(&bassAnimation, &audioLink.bassAnimations[bassAnimationSwitchCounter].bassAnimation);

				changeAnimation(bassAnimation, true, 0);

			}
			// repeat bass animation
			else if(activeAnimationsTimersCount <= audioLink.earlyRepeatTriggerCount){

				
				
				if(
					ledAnimationTimers[0].animationDef != audioLink.idleAnimation
				){
					AnimationDef* repeatingBassAnimations;
					PROGMEM_READ_STRUCTURE(&repeatingBassAnimations, &audioLink.bassAnimations[bassAnimationSwitchCounter].repeatingBassAnimations);

					
					changeAnimation(repeatingBassAnimations, true, 0);
					
				}
			}
			
			/*if(activeAnimationsTimersCount != 0 && ledAnimationTimers[0].animationDef != audioLink.bassAnimation){
				changeAnimation(audioLink.bassAnimation, true);
			}*/
			
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
			changeAnimation(audioLink.idleAnimation, 0);
		}
		//stayOn1=30;
		
	}
	
	if(audioLink.bassAnimationsLength <= ++bassAnimationSwitchCounter){
		bassAnimationSwitchCounter = 0;
	}
}



