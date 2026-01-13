#ifndef SCHEDULED_PWM_H
#define SCHEDULED_PWM_H

#include <inttypes.h>

#include "leddefinition.h"

#include "panic.h"

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define SCHEDULED_PWM_TRACEBACK_ENTRY \
PanicTrace __traceback_entry(__FILENAME__, __func__, __LINE__);


#define FIXED_FORWARD_LIST_TRACEBACK_ENTRY SCHEDULED_PWM_TRACEBACK_ENTRY

namespace detail{
	extern void panicOnStepError(const char* msg, size_t index, void* addr);
}

#define FIXED_FORWARD_LIST_ERROR_FN(msg, index, addr)\
	detail::panicOnStepError(((const char*) F(msg)), (index), (addr))

#ifdef __AVR_ATmega328P__

#include "fixedforwardlist.h"
#endif 

template<size_t N, typename LED_ID_TYPE, typename STEP_STORAGE_TYPE, typename BRIGHTNESS_TYPE = uint8_t>
class ScheduledPWM{
	
public: // static variables
	static constexpr size_t LED_COUNT = N;
	static constexpr uint64_t DEFAULT_MIN_BRIGHTNESS = 0;
	static constexpr uint64_t DEFAULT_MAX_BRIGHTNESS = UINT64_MAX - 10;
public: // types
	using BrightnessType = BRIGHTNESS_TYPE;
	using LedID = LED_ID_TYPE;
	using BitStorageType = STEP_STORAGE_TYPE;

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

	using StepList = FixedForwardList<LED_COUNT + 1, PWMStep>;
	using StepNode = typename StepList::Node;

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
	void setLedPWM(LedID ledId, BrightnessType brightness){
		SCHEDULED_PWM_TRACEBACK_ENTRY
		brightness = minBrightness > brightness && brightness != 0  ? minBrightness : brightness;
		brightness = maxBrightness < brightness ? maxBrightness : brightness;
		StepNode* previousNode = steps.begin();
		
		StepNode* nodeToInsertAfter;
		if(brightness == 0){
			unassignLed(ledId, previousNode->value.bitStorage);
			nodeToInsertAfter = steps.end();
		}
		else {
			assignLed(ledId, previousNode->value.bitStorage);
			nodeToInsertAfter = nullptr;
		}
		StepNode* currentNode = steps.nextNode(previousNode);
		bool pastInstanceRemoved = false;
		
		while(1){

			if(currentNode == steps.end()){
				if(nodeToInsertAfter == nullptr){
					nodeToInsertAfter = previousNode;
	#ifdef DEBUG_SCHED_PMW
					Serial.println("Hint insert after end");
	#endif
				}
				break;
			}
			PWMStep& previousStep = previousNode->value;
			PWMStep& currentStep = currentNode->value;
			StepNode* nextNode = steps.nextNode(currentNode);
	#ifdef DEBUG_SCHED_PMW
			Serial.print("Input mask: ");
			Serial.print(0x01 << ledId, BIN);
			Serial.print(" Step mask: ");
			Serial.println(currentNode->value.bitStorage, BIN);
	#endif
			if(isLedExclusive(ledId, currentStep.bitStorage)){
	#ifdef DEBUG_SCHED_PMW
				Serial.println("removing unique");
	#endif
				//previousStep.nextIsrTime = currentStep.nextIsrTime;
				currentNode = steps.removeAfter(previousNode);
				continue;
			}
			if(
				!pastInstanceRemoved &&
				!isLedAssigned(ledId, currentStep.bitStorage) && 
				nextNode != steps.end()
			){
				if(!isLedStepShared(ledId, previousStep.bitStorage, currentStep.bitStorage)){

				
	#ifdef DEBUG_SCHED_PMW
					Serial.println("removing previous");
	#endif
					previousStep.nextIsrTime = currentStep.nextIsrTime;
					currentNode = steps.removeAfter(previousNode);
					pastInstanceRemoved = true;
					continue;
				}
	#ifdef DEBUG_SCHED_PMW
				Serial.println("Attempted removing previous");
	#endif
			}
			

			if(previousStep.nextIsrTime < brightness){
				
				assignLed(ledId, currentStep.bitStorage);

			}
			else if(previousStep.nextIsrTime > brightness){
				if(nodeToInsertAfter == nullptr){
					
					nodeToInsertAfter = previousNode;
	#ifdef DEBUG_SCHED_PMW
					Serial.print("Hint insert after greater: ");
					Serial.println(previousStep.nextIsrTime);
	#endif
				}
				unassignLed(ledId, currentStep.bitStorage);
			}
			else{
				unassignLed(ledId, currentStep.bitStorage);
				nodeToInsertAfter = steps.end();
			}

			previousNode = currentNode;
			currentNode = nextNode;
		}
		if(nodeToInsertAfter != nullptr && nodeToInsertAfter != steps.end()){
			PWMStep& referenceStep = nodeToInsertAfter->value;
			
			StepNode* newStepNode = steps.insertAfter(nodeToInsertAfter, referenceStep);
	#ifdef DEBUG_SCHED_PMW
			Serial.print("New step index: ");
			Serial.println(steps.indexByNode(newStepNode));
	#endif
			referenceStep.nextIsrTime = brightness;
			unassignLed(
				ledId,
				newStepNode->value.bitStorage		
			);
		}
	}
	
	/**
	 * This will process single step each time its called in user provided timmed interrupt.
	 * 
	 * @returns TRUE when last PWM Step has been processed, otherwise FALSE.
	*/
	bool pwmISR(){
		
		//PANIC("ScheduledPWM::pwmISR");
		if(currentStepNode == steps.begin()){
			onDutyCycleBegin();
		}
		const PWMStep& currentStep = currentStepNode->value;
	#ifdef DEBUG_SCHED_PMW
		Serial.print('(');
		Serial.print(size_t(currentStepNode));
		Serial.print(')');
		Serial.print('[');
		Serial.print(steps.indexByNode(currentStepNode));
		Serial.print(']');
		Serial.print(F(" Next: "));
		Serial.print(currentStepNode->nextIndex());
		Serial.print(" LedMask: ");
		Serial.print(currentStep.bitStorage, BIN);
		Serial.print(" Brightness: ");
		Serial.print(currentStep.nextIsrTime);
		Serial.print(" [");
		for(int i = 0; i < 8; i++){
			uint8_t ledMask = 0x01<<i & 0xFE;
			if(currentStep.bitStorage & ledMask){
				Serial.print(size_t(i));
				Serial.print(',');
			}
		}
		Serial.println(']');
	#endif
		processLedStep(currentStep.bitStorage);
		setupNextIsrTime(currentStep.nextIsrTime);

		

		currentStepNode = steps.nextNode(currentStepNode);
		if(currentStepNode == steps.end()){
			onDutyCycleEnd();
			currentStepNode = steps.begin();
	#ifdef DEBUG_SCHED_PMW
			Serial.println();
	#endif
			return true;
		}
		
		return false;
		
	}

	BrightnessType computeBrightness(LedID ledID) const {
		const StepNode* searchedStepNode = steps.cbegin();
		BrightnessType foundLedIDsBrightness = 0;
		if(isLedAssigned(ledID, searchedStepNode->value.bitStorage)){
			while(1){
				foundLedIDsBrightness = searchedStepNode->value.nextIsrTime;
				searchedStepNode = steps.nextNode(searchedStepNode);
				
				if(searchedStepNode == steps.cend()){
					// this should not happen, put error here
					break;
				}
				
				if(!isLedAssigned(ledID, searchedStepNode->value.bitStorage)){
					break;
				}
			}
			
		}

		return foundLedIDsBrightness;
	}


	inline const StepList& getStepList() const {
		return steps;
	}



	void clear() {
		while(steps.size() > 1){
			currentStepNode = steps.removeAfter(steps.beforeBegin());			
		}
	}

	BrightnessType getMinBrightness() const {
		return minBrightness;
	}

	BrightnessType getMaxBrightness() const {
		return maxBrightness;
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

namespace SPWM_ATmega328P{

struct SharedImpl{
	using Pin = uint8_t;
	enum PortIndex : uint8_t {
			PORTD_INDEX,
			PORTB_INDEX,
			PORTC_INDEX,
			PORT_COUNT
	};

	using PortMasks = uint8_t [PortIndex::PORT_COUNT];
	struct StateStorage{
		
		PortMasks digitalPortStates = {};

		void assign(Pin pin, PortMasks& ownedPins);
		void unassign(Pin pin, PortMasks& ownedPins);
		bool isAssigned(Pin pin) const;
		bool isExclusive(Pin pin) const;
		bool isSharedWith(Pin pin, const StateStorage& other) const;
		void applyState(const PortMasks& ownedPins) const;
	};

	static void initTIMER2();
	static void setNextIsrTimeForTIMER2(uint8_t brightness);
	

};

class ScheduledPWM_TIMER2 : public ScheduledPWM<12, SharedImpl::Pin,  SharedImpl::StateStorage, uint8_t>{
public:
	ScheduledPWM_TIMER2() : ScheduledPWM(10) {

	}
	void begin(){
		SharedImpl::initTIMER2();
	}

	void testImplementation();


protected:
	void assignLed(LedID ledId, BitStorageType& stepStorage) override {
		stepStorage.assign(ledId, ownedPins);
	}

	void unassignLed(LedID ledId, BitStorageType& stepStorage) override {
		stepStorage.unassign(ledId, ownedPins);
	}

	bool isLedAssigned(LedID ledId, const BitStorageType& stepStorage) const override {
		return stepStorage.isAssigned(ledId);
	}
	bool isLedExclusive(LedID ledId, const BitStorageType& stepStorage) const override {
		return stepStorage.isExclusive(ledId);
	}

	bool isLedStepShared(LedID ledId, const BitStorageType& previousStepStorage, const BitStorageType& currentStepStorage) const override {
		return previousStepStorage.isSharedWith(ledId, currentStepStorage);
	}

	void processLedStep(const BitStorageType& stepStorage) override {
		stepStorage.applyState(ownedPins);
	}


	void setupNextIsrTime(BrightnessType nextTime) override {
		SharedImpl::setNextIsrTimeForTIMER2(nextTime);
	}

	void onDutyCycleBegin() override {
		//delayMicroseconds(25);
		
		/*setLedPWM(2, cycleBrightness + 80);
		setLedPWM(3, cycleBrightness + 20);
		
		setLedPWM(7, cycleBrightness + 60);
		setLedPWM(8, cycleBrightness + 70);*/

		setLedPWM(12, cycleBrightness);
		setLedPWM(13, cycleBrightness);
		if(cycleBrightness == 255){
			direction = -1;
		}
		else if(cycleBrightness == 1){
			direction = 1;
		}
		cycleBrightness += direction;
	}
	
	uint8_t cycleBrightness = 1;
	int8_t direction = 1;
	SharedImpl::PortMasks ownedPins = {};
};

inline ScheduledPWM_TIMER2 SchedPWM_TIMER2;
extern void testImplementation();
}

//inline ScheduledPWM SchedPWM;



extern void testScheduledPWM();

#endif