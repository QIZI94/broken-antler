#ifndef SCHEDULED_PWM_H
#define SCHEDULED_PWM_H

#include "leddefinition.h"



#ifdef __AVR_ATmega328P__
#define ERROR_MSG_LITERAL(literal_str) ((const char*) F(literal_str))
#include "fixedforwardlist.h"
#endif 


class ScheduledPWM{
private: // static functions
	static void panicOnStepError(const char* msg, size_t index, void* addr);
public: // static variables
	static constexpr uint8_t LED_COUNT = uint8_t(LedPosition::NUM_OF_ALL_LEDS) * 2;
	static constexpr uint64_t DEFAULT_MIN_BRIGHTNESS = 0;
	static constexpr uint64_t DEFAULT_MAX_BRIGHTNESS = UINT64_MAX - 10;
public: // types
	using BrightnessType = uint8_t;
	using LedID = uint8_t;
	using BitStorageType = uint16_t;

	struct PWMStep {
		BitStorageType bitStorage{};
		BrightnessType nextIsrTime = 0;
		static PWMStep make(const BitStorageType& bitStorage, const BrightnessType& nextIsrTime){
			PWMStep created;
			created.bitStorage = bitStorage;
			created.nextIsrTime = nextIsrTime;
			return created;
		}
	};

	using StepList = FixedForwardList<LED_COUNT + 1, PWMStep, uint8_t, panicOnStepError>;
	using StepNode = StepList::Node;

	//using ValueType = uint8_t;



public: // member functions
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

	inline const StepList& getStepList() const {
		return steps;
	}

	BrightnessType computeBrightness(LedID ledID) const;

	void clear() {
		while(steps.size() > 1){
			currentStepNode = steps.removeAfter(steps.beforeBegin());			
		}
	}

protected: // member functions
	/**
	 * Assign Led to scheduled step storage in PWM Cycle.
	 * This has to be assigned in a way for isLedAssigned() and isLedExclusive to detect assigned Led
	 * (called from setLedPWM())
	 * 
	 * @param ledId used for unique identification of led
	 * @param stepStorage writable step storage to assign Led into
	*/
	virtual void assignLed(LedID ledId, BitStorageType& stepStorage) = 0;
	/**
	 * Unassigned Led to scheduled step storage in PWM Cycle.
	 * This has to be unassigned in a way for isLedAssigned() and isLedExclusive to not detect unassigned Led
	 * (called from setLedPWM())
	 * 
	 * @param ledId used for unique identification of led
	 * @param stepStorage writable step storage to unassign Led from
	*/
	virtual void unassignLed(LedID ledId, BitStorageType& stepStorage) = 0;
	/**
	 * Read and apply stepStorage to turn ON or OFF all described Leds.
	 * (called from pwmISR())
	 *  
	 * @param stepStorage read only step storage 
	*/
	virtual void processLedStep(const BitStorageType& stepStorage) = 0;

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
	virtual bool isLedAssigned(LedID ledId, const BitStorageType& stepStorage) const = 0;

	/**
	 * Check if Led is only one assigned in stepStorage.
	 * (called from setLedPWM())
	 *  
	 * @param ledId used for unique identification of Led
	 * @param stepStorage read only step storage to check unique Led presence
	 * @returns If Led is uniqually assigned in stepStorage return TRUE, otherwise return FALSE.
	*/
	virtual bool isLedExclusive(LedID ledId, const BitStorageType& stepStorage) const = 0;

	virtual bool isLedStepShared(LedID ledId, const BitStorageType& previousStepStorage, const BitStorageType& currentStepStorage) const = 0;

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
	StepNode* currentStepNode = steps.insertAfter(steps.beforeBegin(), {});
	BrightnessType maxBrightness;
	BrightnessType minBrightness;
};


//inline ScheduledPWM SchedPWM;



extern void testScheduledPWM();

#endif