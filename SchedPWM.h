#ifndef SCHEDULED_PWM_H
#define SCHEDULED_PWM_H

#include "leddefinition.h"

#include "fixedforwardlist.h"



class ScheduledPWM{
public:
	using BrightnessType = uint8_t;
	using LedID = uint8_t;
	using BitStorageType = uint16_t;
	//using ValueType = uint8_t;
	static constexpr uint8_t LED_COUNT = uint8_t(LedPosition::NUM_OF_ALL_LEDS) * 2;

private:
	static void panicOnStepError(const char* msg, size_t index, void* addr);

private:
	struct PWMStep {
		BrightnessType nextIsrTime = 0;
		BitStorageType bitStorage{};
	};

	using StepList = FixedForwardList<LED_COUNT + 1, PWMStep, uint8_t, panicOnStepError>;
	using StepNode = StepList::Node;

public:
	
	void setLedPWM(LedID ledId, BrightnessType brightness);

	bool pwmISR();
//protected:

	virtual void begin(){

	}

	virtual void end(){

	}

	virtual void assignLed(LedID ledId, BitStorageType& stepStorage){
		stepStorage |= 0x01 << ledId;
	}

	virtual void unassignLed(LedID ledId, BitStorageType& stepStorage){
		stepStorage &= ~(0x01 << ledId);
	}

	virtual void processLedStep(const BitStorageType& stepStorage){
		Serial.print(" LedMask: ");
		Serial.print(stepStorage, BIN);
	}

	virtual void setupNextIsrTime(BrightnessType nextTime){
		Serial.print(" Brightness: ");
		Serial.println(nextTime);
	}
	virtual bool isLedAssigned(LedID ledId, const BitStorageType& stepStorage){
		return ((0x01 << ledId) & stepStorage) != 0;
	}
	virtual bool isLedExclusive(LedID ledId, const BitStorageType& stepStorage){
		BitStorageType inverseLedMask = ~(0x01 << ledId);
		return (inverseLedMask & stepStorage) == 0;
	}
	

	
//private:

	StepList steps;
	StepNode* currentStepNode = steps.begin();
	//PWMStep unallocatedStep
	BrightnessType maxBrightness = 250;
	BrightnessType minBrightness = 0;
	

};


inline ScheduledPWM SchedPWM;

#endif