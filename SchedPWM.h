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
	static constexpr uint64_t DEFAULT_MIN_BRIGHTNESS = 0;
	static constexpr uint64_t DEFAULT_MAX_BRIGHTNESS = UINT64_MAX - 10;
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
	ScheduledPWM(
		BrightnessType minBrightness = DEFAULT_MIN_BRIGHTNESS,
		BrightnessType maxBrightness = DEFAULT_MAX_BRIGHTNESS
	) : minBrightness(minBrightness), maxBrightness(maxBrightness){}
	
	/**
	 * Set PWM Duty cycle for specific Led by brightness/time.
	 * 
	 * @param ledId used for unique identification of led
	 * @param brightness brightness based on which duty cycle time is setup
	*/
	void setLedPWM(LedID ledId, BrightnessType brightness);
	
	/**
	 * This will process single step each time its called in user provided timmed interrupt.
	 * 
	 * @returns TRUE when last PWM Step has been processed, otherwise FALSE.
	*/
	bool pwmISR();
protected:
	/**
	 * Assign Led to scheduled step storage in PWM Cycle.
	 * This has to be assigned in a way for isLedAssigned() and isLedExclusive to detect assigned Led
	 * (called from setLedPWM())
	 * 
	 * @param ledId used for unique identification of led
	 * @param stepStorage writable step storage to assign Led into
	*/
	virtual void assignLed(LedID ledId, BitStorageType& stepStorage){
		stepStorage |= 0x01 << ledId;
	}
	/**
	 * Unassigned Led to scheduled step storage in PWM Cycle.
	 * This has to be unassigned in a way for isLedAssigned() and isLedExclusive to not detect unassigned Led
	 * (called from setLedPWM())
	 * 
	 * @param ledId used for unique identification of led
	 * @param stepStorage writable step storage to unassign Led from
	*/
	virtual void unassignLed(LedID ledId, BitStorageType& stepStorage){
		stepStorage &= ~(0x01 << ledId);
	}
	/**
	 * Read and apply stepStorage to turn ON or OFF all described Leds.
	 * (called from pwmISR())
	 *  
	 * @param stepStorage read only step storage 
	*/
	virtual void processLedStep(const BitStorageType& stepStorage){

	
	}

	/**
	 * Use next time for scheduling next timer interrupt
	 * (called from pwmISR())
	 *  
	 * @param nextTime next scheduled timmed interupt to apply next step 
	*/
	virtual void setupNextIsrTime(BrightnessType nextTime){
		
	}

	/**
	 * Check if specific Led is assigned.
	 * (called from setLedPWM())
	 *  
	 * @param ledId used for unique identification of Led
	 * @param stepStorage read only step storage to check unique Led presence
	 * @returns If Led is assigned in stepStorage return TRUE, otherwise return FALSE.
	*/
	virtual bool isLedAssigned(LedID ledId, const BitStorageType& stepStorage){
		return ((0x01 << ledId) & stepStorage) != 0;
	}

	/**
	 * Check if Led is only one assigned in stepStorage.
	 * (called from setLedPWM())
	 *  
	 * @param ledId used for unique identification of Led
	 * @param stepStorage read only step storage to check unique Led presence
	 * @returns If Led is uniqually assigned in stepStorage return TRUE, otherwise return FALSE.
	*/
	virtual bool isLedExclusive(LedID ledId, const BitStorageType& stepStorage){
		if(stepStorage == 0){
			return false;
		}
		BitStorageType inverseLedMask = ~(0x01 << ledId);
		return (inverseLedMask & stepStorage) == 0;
	}

	virtual bool isLedStepShared(LedID ledId, const BitStorageType& previousStepStorage, const BitStorageType& currentStepStorage){
		uint8_t xoredStep = previousStepStorage ^ currentStepStorage;
		Serial.println(xoredStep, BIN);
		return !isLedExclusive(ledId, xoredStep);
	}

	/**
	 * Called on PWM Duty Cycle start.
	 * (called from pwmISR())
	*/
	virtual void onDutyCycleBegin(){}

	/**
	 * Called on last PWM Duty Cycle after after last step is processed.
	 * (called from pwmISR())
	*/
	virtual void onDutyCycleEnd(){}

	
private:

	StepList steps;
	StepNode* currentStepNode = steps.insertAfter(steps.begin(), {});
	BrightnessType maxBrightness;
	BrightnessType minBrightness;
};


inline ScheduledPWM SchedPWM;

#endif