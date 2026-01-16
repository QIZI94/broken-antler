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

template<size_t N, class Impl, typename LED_ID_TYPE,  typename STEP_STORAGE_TYPE, typename BRIGHTNESS_TYPE = uint8_t>
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

	using StepList = FixedForwardList<LED_COUNT + 1, PWMStep, true>;
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
		StepNode* currentNode = previousNode->nextNode();
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
			StepNode* nextNode = currentNode->nextNode();
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
		Serial.print(steps.indexByNode(currentStepNode->nextNode()));
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

		

		currentStepNode = currentStepNode->nextNode();
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
		const StepNode* cend = steps.cend();
		BrightnessType foundLedIDsBrightness = 0;
		if(isLedAssigned(ledID, searchedStepNode->value.bitStorage)){
			while(1){
				foundLedIDsBrightness = searchedStepNode->value.nextIsrTime;
				searchedStepNode = searchedStepNode->nextNode();//steps.nextNode(searchedStepNode);
				
				if(searchedStepNode == cend){
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

private: // member functions
	/**
	 * Assign Led to scheduled step storage in PWM Cycle.
	 * This has to be assigned in a way for isLedAssigned() and isLedExclusive to detect assigned Led
	 * (called from setLedPWM())
	 * 
	 * @param ledId used for unique identification of led
	 * @param stepStorage writable step storage to assign Led into
	*/
	void assignLed(LedID ledId, BitStorageType& stepStorage){
		static_cast<Impl*>(this)->assignLed(ledId, stepStorage);
	}
	/**
	 * Unassigned Led to scheduled step storage in PWM Cycle.
	 * This has to be unassigned in a way for isLedAssigned() and isLedExclusive to not detect unassigned Led
	 * (called from setLedPWM())
	 * 
	 * @param ledId used for unique identification of led
	 * @param stepStorage writable step storage to unassign Led from
	*/
	void unassignLed(LedID ledId, BitStorageType& stepStorage){
		static_cast<Impl*>(this)->unassignLed(ledId, stepStorage);
	}
	/**
	 * Read and apply stepStorage to turn ON or OFF all described Leds.
	 * (called from pwmISR())
	 *  
	 * @param stepStorage read only step storage 
	*/
	void processLedStep(const BitStorageType& stepStorage){
		static_cast<Impl*>(this)->processLedStep(stepStorage);
	}

	/**
	 * Use next time for scheduling next timer interrupt
	 * (called from pwmISR())
	 *  
	 * @param nextTime next scheduled timmed interupt to apply next step 
	*/
	void setupNextIsrTime(BrightnessType nextTime){
		static_cast<Impl*>(this)->setupNextIsrTime(nextTime);
	}

	/**
	 * Check if specific Led is assigned.
	 * (called from setLedPWM())
	 *  
	 * @param ledId used for unique identification of Led
	 * @param stepStorage read only step storage to check unique Led presence
	 * @returns If Led is assigned in stepStorage return TRUE, otherwise return FALSE.
	*/
	bool isLedAssigned(LedID ledId, const BitStorageType& stepStorage) const {
		return static_cast<const Impl*>(this)->isLedAssigned(ledId, stepStorage);
	}

	/**
	 * Check if Led is only one assigned in stepStorage.
	 * (called from setLedPWM())
	 *  
	 * @param ledId used for unique identification of Led
	 * @param stepStorage read only step storage to check unique Led presence
	 * @returns If Led is uniqually assigned in stepStorage return TRUE, otherwise return FALSE.
	*/
	bool isLedExclusive(LedID ledId, const BitStorageType& stepStorage) const{
		return static_cast<const Impl*>(this)->isLedExclusive(ledId, stepStorage);
	}

	bool isLedStepShared(LedID ledId, const BitStorageType& previousStepStorage, const BitStorageType& currentStepStorage) const{
		return static_cast<const Impl*>(this)->isLedStepShared(ledId, previousStepStorage, currentStepStorage);
	}

	/**
	 * Called on PWM Duty Cycle start.
	 * (called from pwmISR())
	*/
	void onDutyCycleBegin(){
		static_cast<Impl*>(this)->onDutyCycleBegin();
	}

	/**
	 * Called on last PWM Duty Cycle after after last step is processed.
	 * (called from pwmISR())
	*/
	void onDutyCycleEnd(){
		static_cast<Impl*>(this)->onDutyCycleEnd();
	}

	
private:

	StepList steps;
	StepNode* currentStepNode = steps.insertAfter(steps.beforeBegin(), {});
	BrightnessType maxBrightness;
	BrightnessType minBrightness;
};

namespace SPWM_ATmega328P{

namespace SharedImpl{

using Pin = uint8_t;
enum PortIndex : uint8_t {
		PORTD_INDEX,
		PORTB_INDEX,
		PORTC_INDEX,
		PORT_COUNT
};
struct PortIndexAndMask{
	PortIndex portIndex = PortIndex::PORT_COUNT;
	uint8_t mask = 0;
};

using PortMasks = uint8_t [PortIndex::PORT_COUNT];


namespace helpers{
	inline constexpr PortIndexAndMask pinToPortIndexAndMask(SharedImpl::Pin pin){
		PortIndexAndMask portIndexAndMask;
		if(pin >= 14){
			portIndexAndMask.portIndex = PortIndex::PORTC_INDEX;
			portIndexAndMask.mask = 0x01 << (pin - 14);
		}
		else if(pin >= 8){
			portIndexAndMask.portIndex = PortIndex::PORTB_INDEX;
			portIndexAndMask.mask = 0x01 << (pin - 8);
		}
		else {
			portIndexAndMask.portIndex = PortIndex::PORTD_INDEX;
			portIndexAndMask.mask = 0x01 << pin;
		}
		return portIndexAndMask;
	}
	template<size_t N>
	struct PinMasksAndIndexesTable{
		PortIndexAndMask lookup[N];
	};

	template<size_t N>
	inline constexpr PinMasksAndIndexesTable<N> makePinMasksAndIndexesTable(){
		PinMasksAndIndexesTable<N> table;

		for(SharedImpl::Pin pin = 0; pin < N; ++pin){
			table.lookup[pin] = pinToPortIndexAndMask(pin);
		}

		return table;
	}

	static constexpr auto pinMasksAndIndexesTable = makePinMasksAndIndexesTable<22>();


	inline constexpr PortIndex otherPortIndexes[PortIndex::PORT_COUNT][PortIndex::PORT_COUNT - 1]{
		{PortIndex::PORTB_INDEX, PortIndex::PORTC_INDEX},
		{PortIndex::PORTD_INDEX, PortIndex::PORTC_INDEX},
		{PortIndex::PORTD_INDEX, PortIndex::PORTB_INDEX}
	};

	inline volatile uint8_t** GetOrderedPortsArray(){
		//#define SPWM_ATmega328P_TEST
		#ifdef SPWM_ATmega328P_TEST
			static volatile uint8_t fakePort[PortIndex::PORT_COUNT]{0,0b1000};
			static volatile uint8_t* portsPtr[PortIndex::PORT_COUNT]{
				&fakePort[PortIndex::PORTD_INDEX],
				&fakePort[PortIndex::PORTB_INDEX],
				&fakePort[PortIndex::PORTC_INDEX]
			};
		#else
			static volatile uint8_t* portsPtr[PortIndex::PORT_COUNT]{
				&PORTD,
				&PORTB,
				&PORTC
			};
		#endif
		return portsPtr;
	}
}

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



class ScheduledPWM_TIMER2 : public ScheduledPWM<12, ScheduledPWM_TIMER2, SharedImpl::Pin,  SharedImpl::StateStorage, uint8_t>{
public:

	ScheduledPWM_TIMER2() : ScheduledPWM(10) {
		
	}
	void begin(){
		SharedImpl::initTIMER2();
	}

	void testImplementation();


private:
	void assignLed(LedID ledId, BitStorageType& stepStorage) {
		stepStorage.assign(ledId, ownedPins);
	}

	void unassignLed(LedID ledId, BitStorageType& stepStorage) {
		stepStorage.unassign(ledId, ownedPins);
	}

	bool isLedAssigned(LedID ledId, const BitStorageType& stepStorage) const {
		return stepStorage.isAssigned(ledId);
	}
	bool isLedExclusive(LedID ledId, const BitStorageType& stepStorage) const {
		return stepStorage.isExclusive(ledId);
	}

	bool isLedStepShared(LedID ledId, const BitStorageType& previousStepStorage, const BitStorageType& currentStepStorage) const {
		return previousStepStorage.isSharedWith(ledId, currentStepStorage);
	}

	void processLedStep(const BitStorageType& stepStorage) {
		stepStorage.applyState(ownedPins);
	}


	void setupNextIsrTime(BrightnessType nextTime) {
		SharedImpl::setNextIsrTimeForTIMER2(nextTime);
	}

	void onDutyCycleBegin() {
		//delayMicroseconds(25);
		
		setLedPWM(2, cycleBrightness);
		setLedPWM(3, cycleBrightness);
		
		setLedPWM(7, cycleBrightness);
		setLedPWM(8, cycleBrightness);

		setLedPWM(12, cycleBrightness);
		setLedPWM(13, cycleBrightness);
		setLedPWM(2, cycleBrightness);
		setLedPWM(3, cycleBrightness);
		
		setLedPWM(7, cycleBrightness);
		setLedPWM(8, cycleBrightness);

		setLedPWM(12, cycleBrightness);
		setLedPWM(13, cycleBrightness);
		//Serial.println("HERE");
		if(cycleBrightness == 255){
			direction = -1;
		}
		else if(cycleBrightness == 1){
			direction = 1;
		}
		cycleBrightness += direction;
	}

	void onDutyCycleEnd(){

	}
	
	uint8_t cycleBrightness = 1;
	int8_t direction = 1;
	SharedImpl::PortMasks ownedPins = {};
	
	friend ScheduledPWM;
};

inline ScheduledPWM_TIMER2 SchedPWM_TIMER2;
extern void testImplementation();
}

//inline ScheduledPWM SchedPWM;



extern void testScheduledPWM();

#endif