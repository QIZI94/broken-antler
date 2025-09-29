#include <SoftPWM_timer.h>
#include <SoftPWM.h>

#include "animationshandler.h"
#include "timer.h"

#define SOFTPWM_FREQ 120
#define SOFTPWM_OCR (F_CPU/(8UL*256UL*SOFTPWM_FREQ))


struct LedAnimationStateTimer : public TimedExecution1ms{
	void assignAnimation(const AnimationDef* animationDef){
		this->animationDef = animationDef;
		this->direction = animationDef->direction;
		current(nullptr);
	}

	const AnimationStep* current(const AnimationStep* newCurrentStep){
		currentStep = newCurrentStep;
		return currentStep;
	}

	const AnimationStep* current() const{
		return currentStep;
	}

	const AnimationStep* begin(){
        return animationDef->stepSpan.begin();
    }

	const AnimationStep* end(){
        return animationDef->stepSpan.end();
    }

    const AnimationStep* rbegin(){
        return animationDef->stepSpan.rbegin();
    }
    const AnimationStep* rend(){
        return animationDef->stepSpan.rend();
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
	const AnimationDef* animationDef;
	AnimationDirection direction;
	
};



static LedAnimationStateTimer ledAnimationTimers[size_t(LedPosition::NUM_OF_ALL_LEDS)];


static void setAnimationLed(LedDef led, uint8_t brightness, bool immediate = false){

	uint8_t redBrightness = led.red.convertBrightness(brightness);
	uint8_t blueBrightness = led.blue.convertBrightness(brightness);

	SoftPWMSetPercent(led.blue.pin, blueBrightness, immediate ? 1 : 0);
	SoftPWMSetPercent(led.red.pin, redBrightness, immediate ? 1 : 0);
}




static void handleLedAnimation(TimedExecution1ms& timer){
	LedAnimationStateTimer& processedAnimation =  reinterpret_cast<LedAnimationStateTimer&>(timer);  
    const AnimationStep* currentStep = processedAnimation.current();
	uint16_t duration = currentStep->duration;

	if(!currentStep->isDelay()){
		LedDef led = LED_AllLeds[size_t(processedAnimation.animationDef->ledPosition)];
		SoftPWMSetFadeTime(led.blue.pin, duration, duration);
		SoftPWMSetFadeTime(led.red.pin, duration, duration);
		setAnimationLed(led, currentStep->brightness);
	}

    switch(processedAnimation.direction){
		
        case AnimationDirection::FORWARD:
            if(processedAnimation.next() == processedAnimation.end()){
                processedAnimation.current(processedAnimation.begin());
            }
            break;
        case AnimationDirection::BIDIRECTIONAL_FORWARD:
            if(processedAnimation.next() == processedAnimation.end()){
                processedAnimation.current(processedAnimation.rbegin());
				if(processedAnimation.prev() == processedAnimation.rend()){
					processedAnimation.current(processedAnimation.rbegin());
				}
                processedAnimation.direction = AnimationDirection::BIDIRECTIONAL_BACKWARD;
            }
            break;
        
        case AnimationDirection::BACKWARD:
            if(processedAnimation.prev() == processedAnimation.rend()){
                processedAnimation.current(processedAnimation.rbegin());
            }
			break;
        case AnimationDirection::BIDIRECTIONAL_BACKWARD:
            if(processedAnimation.prev() == processedAnimation.rend()){
				processedAnimation.current(processedAnimation.begin());
				if(processedAnimation.next() == processedAnimation.end()){
					processedAnimation.current(processedAnimation.begin());
				}
                
                processedAnimation.direction = AnimationDirection::BIDIRECTIONAL_FORWARD;
            }
        	break;
    }
	timer.restart(duration);
}


static void startAnimation(const AnimationDef* animation){
	
	for(LedAnimationStateTimer& animStateTimer : ledAnimationTimers){
		animStateTimer.disable();
		animStateTimer.timer.disable();
	}


    const AnimationDef* animationIt = animation;
	size_t animationIndex = 0;

    while(animationIt->isValid()){

        LedAnimationStateTimer& animStateTimer = ledAnimationTimers[animationIndex];
		
		animStateTimer.assignAnimation(animationIt);
		
        switch(animationIt->direction){
            case AnimationDirection::BIDIRECTIONAL_FORWARD:
            case AnimationDirection::FORWARD:

                animStateTimer.current(animStateTimer.begin());
                break;
            
            case AnimationDirection::BIDIRECTIONAL_BACKWARD:
            case AnimationDirection::BACKWARD:
                animStateTimer.current(animStateTimer.rbegin());
                break;
            
        }

		animStateTimer.setup(handleLedAnimation, animationIt->initialDelay);
		
		animationIndex++;
        animationIt++;
		
    }
	for(LedDef led : LED_AllLeds){
		SoftPWMSetFadeTime(led.blue.pin, 0, 0);
		SoftPWMSetFadeTime(led.red.pin, 0, 0);
		setAnimationLed(led, 0, true);
	}

}


const AnimationDef* newSelectedAnimation = nullptr;

void setAnimation(const AnimationDef* newAnimation){
	newSelectedAnimation = newAnimation;
}
void initAnimations(){
	SoftPWMBegin();
#ifdef TIFR2
	SOFTPWM_TIMER_INIT(SOFTPWM_OCR);
#endif
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
		startAnimation(newSelectedAnimation);
		newSelectedAnimation = nullptr;
		interrupts();
	}
	//setAnimationLed(LED_LeftFront, 80, true);
	//setAnimationLed(LED_LeftMiddle, 80, true);
	//setAnimationLed(LED_LeftBack, 80, true);
}


