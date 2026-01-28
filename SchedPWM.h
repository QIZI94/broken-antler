#ifndef SCHEDULED_PWM_H
#define SCHEDULED_PWM_H

#include <inttypes.h>

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#ifndef SCHEDULED_PWM_TRACEBACK_ENTRY
#define SCHEDULED_PWM_TRACEBACK_ENTRY
#endif


#define FIXED_FORWARD_LIST_TRACEBACK_ENTRY SCHEDULED_PWM_TRACEBACK_ENTRY


#include "fixedforwardlist.h"



#define _DEFINE_HAS_METHOD_IMPLEMENTED(name)                         \
template <typename T, typename Sig>                           \
struct has_##name##_implemented;                                    \
                                                              \
template <typename T, typename R, typename... Args>           \
struct has_##name##_implemented<T, R (T::*)(Args...)> {              \
    typedef char yes[1];                                       \
    typedef char no[2];                                        \
                                                              \
    template <typename U>                                      \
    static yes& test(                                          \
        decltype(static_cast<R (U::*)(Args...)>(&U::name))*); \
                                                              \
    template <typename>                                        \
    static no& test(...);                                      \
                                                              \
    static constexpr bool value =                              \
        sizeof(test<T>(0)) == sizeof(yes);                    \
};                                                            \
                                                              \
template <typename T, typename R, typename... Args>           \
struct has_##name##_implemented<T, R (T::*)(Args...) const> {        \
    typedef char yes[1];                                       \
    typedef char no[2];                                        \
                                                              \
    template <typename U>                                      \
    static yes& test(                                          \
        decltype(static_cast<R (U::*)(Args...) const>(&U::name))*); \
                                                              \
    template <typename>                                        \
    static no& test(...);                                      \
                                                              \
    static constexpr bool value =                              \
        sizeof(test<T>(0)) == sizeof(yes);                    \
};

#define _HAS_METHOD_IMPLEMENTED_HELPER(DerivedClass, Method, ReturnType, Args) \
static_assert(\
	has_##Method##_implemented<DerivedClass, ReturnType (DerivedClass::*)Args>::value,\
	"Derived class doesn't implement method with signature '" #ReturnType " " #Method #Args\
);



template<size_t N, class Impl, typename LED_ID_TYPE,  typename STEP_STORAGE_TYPE, typename BRIGHTNESS_TYPE = uint8_t>
class ScheduledPWM{
private: // type definitions
	_DEFINE_HAS_METHOD_IMPLEMENTED(assignLed)
	_DEFINE_HAS_METHOD_IMPLEMENTED(unassignLed)
	_DEFINE_HAS_METHOD_IMPLEMENTED(processLedStep)
	_DEFINE_HAS_METHOD_IMPLEMENTED(setupNextIsrTime)
	_DEFINE_HAS_METHOD_IMPLEMENTED(isLedAssigned)
	_DEFINE_HAS_METHOD_IMPLEMENTED(isLedExclusive)
	_DEFINE_HAS_METHOD_IMPLEMENTED(isLedStepShared)
	_DEFINE_HAS_METHOD_IMPLEMENTED(onDutyCycleBegin)
	_DEFINE_HAS_METHOD_IMPLEMENTED(onDutyCycleEnd)

	
public: // static constants
	static constexpr size_t LED_COUNT = N;
	static constexpr uint64_t DEFAULT_MIN_BRIGHTNESS = 0;
	static constexpr uint64_t DEFAULT_MAX_BRIGHTNESS = UINT64_MAX - 10;
	static constexpr uint8_t GetOtherBufferIndex[2] {1,0};
public: // type definitions
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

	enum class BufferIndex : uint8_t{
		Active,
		Writable
	};

	//using ValueType = uint8_t;

public: // member functions

	ScheduledPWM(
		BrightnessType minBrightness = DEFAULT_MIN_BRIGHTNESS,
		BrightnessType maxBrightness = DEFAULT_MAX_BRIGHTNESS
	) : minBrightness(minBrightness), maxBrightness(maxBrightness){

		// compile-time check for presence of required implementation 
		/// void assignLed(LedID, BitStorageType&)
		_HAS_METHOD_IMPLEMENTED_HELPER(Impl, assignLed, void, (LedID, BitStorageType&));
		/// void unassignLed(LedID, BitStorageType&)
		_HAS_METHOD_IMPLEMENTED_HELPER(Impl, unassignLed, void, (LedID, BitStorageType&));
		/// void processLedStep(const BitStorageType&)
		_HAS_METHOD_IMPLEMENTED_HELPER(Impl, processLedStep, void, (BitStorageType&, const BitStorageType&));
		/// void setupNextIsrTime(BrightnessType)
		_HAS_METHOD_IMPLEMENTED_HELPER(Impl, setupNextIsrTime, void, (BrightnessType));
		/// isLedAssigned(LedID, const BitStorageType&) const
		_HAS_METHOD_IMPLEMENTED_HELPER(Impl, isLedAssigned, bool, (LedID, const BitStorageType&) const);
		/// bool isLedExclusive(LedID, const BitStorageType&) const
		_HAS_METHOD_IMPLEMENTED_HELPER(Impl, isLedExclusive, bool, (LedID, const BitStorageType&) const);
		/// bool isLedStepShared(LedID, const BitStorageType&, const BitStorageType&) const
		_HAS_METHOD_IMPLEMENTED_HELPER(Impl, isLedStepShared, bool, (LedID, const BitStorageType&, const BitStorageType&) const);
		/// void onDutyCycleBegin()
		_HAS_METHOD_IMPLEMENTED_HELPER(Impl, onDutyCycleBegin, void, ());
		/// void onDutyCycleEnd()
		_HAS_METHOD_IMPLEMENTED_HELPER(Impl, onDutyCycleEnd, void, ());
		

	}
	
	/**
	 * Set PWM Duty cycle for specific Led by brightness/time.
	 * 
	 * @param ledId used for unique identification of led
	 * @param brightness brightness based on which duty cycle time is setup
	*/
	void setLedPWM(LedID ledId, BrightnessType brightness){
		newIndex = activeStepsIndex;
		uint8_t writableBufferIndex = GetOtherBufferIndex[activeStepsIndex];
		StepList& steps = stepsBuffer[writableBufferIndex];

		
		 
		if(!writableBufferReady){
			steps = stepsBuffer[activeStepsIndex];
			writableBufferReady = true;
		}

		SCHEDULED_PWM_TRACEBACK_ENTRY
		brightness = minBrightness > brightness && brightness != 0  ? minBrightness : brightness;
		brightness = maxBrightness < brightness ? maxBrightness : brightness;
		
		StepNode* nodeToInsertAfter;
		StepNode* previousNode = steps.begin();
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
				}
				break;
			}
			PWMStep& previousStep = previousNode->value;
			PWMStep& currentStep = currentNode->value;
			StepNode* nextNode = currentNode->nextNode();

			if(!pastInstanceRemoved && isLedAssigned(ledId, currentStep.bitStorage)){
				if(isLedExclusive(ledId, currentStep.bitStorage)){
					previousStep.nextIsrTime = currentStep.nextIsrTime;
					currentNode = steps.removeAfter(previousNode);
					pastInstanceRemoved = true;
					continue;
				}
				else {
					unassignLed(ledId, currentStep.bitStorage);
					pastInstanceRemoved = true;
				}
			}

			if(nodeToInsertAfter == nullptr){
				if(previousStep.nextIsrTime > brightness){
					nodeToInsertAfter = previousNode;
					if(pastInstanceRemoved == true){
						break;
					}
					//assignLed(ledId, currentStep.bitStorage);
					
				}
				else if(previousStep.nextIsrTime == brightness){
					assignLed(ledId, currentStep.bitStorage);
					nodeToInsertAfter = steps.end();
					if(pastInstanceRemoved == true){
						break;
					}
				}
				
			}

			/*
			if(isLedExclusive(ledId, currentStep.bitStorage)){
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
					previousStep.nextIsrTime = currentStep.nextIsrTime;
					currentNode = steps.removeAfter(previousNode);
					pastInstanceRemoved = true;
					continue;
				}
			}
			

			if(previousStep.nextIsrTime < brightness){
				
				assignLed(ledId, currentStep.bitStorage);

			}
			else if(previousStep.nextIsrTime > brightness){
				if(nodeToInsertAfter == nullptr){
					
					nodeToInsertAfter = previousNode;
				}
				unassignLed(ledId, currentStep.bitStorage);
			}
			else{
				unassignLed(ledId, currentStep.bitStorage);
				nodeToInsertAfter = steps.end();
			}
			*/
			previousNode = currentNode;
			currentNode = nextNode;
		}
		if(nodeToInsertAfter != nullptr && nodeToInsertAfter != steps.end()){
			
			
			StepNode* newStepNode = steps.insertAfter(nodeToInsertAfter, PWMStep::make({}, nodeToInsertAfter->value.nextIsrTime));

			nodeToInsertAfter->value.nextIsrTime = brightness;
			assignLed(
				ledId,
				newStepNode->value.bitStorage		
			);
		}
		newIndex = writableBufferIndex;
	}
	
	/**
	 * This will process single step each time its called in user provided timmed interrupt.
	 * 
	 * @returns TRUE when last PWM Step has been processed, otherwise FALSE.
	*/
	bool pwmISR(){
		
		//PANIC("ScheduledPWM::pwmISR");
		StepList& steps = stepsBuffer[activeStepsIndex];
		if(currentStepNode == steps.begin()){
			onDutyCycleBegin();
		}
		const PWMStep& currentStep = currentStepNode->value;

		processLedStep(currentStep.bitStorage);
		setupNextIsrTime(currentStep.nextIsrTime);

		

		currentStepNode = currentStepNode->nextNode();
		if(currentStepNode == steps.end()){
			onDutyCycleEnd();
			if(activeStepsIndex != newIndex){
				activeStepsIndex = newIndex;
				writableBufferReady = false;
			}
			currentStepNode = stepsBuffer[activeStepsIndex].begin();
			stateStorage = {};
			return true;
		}
		
		return false;
	}

	BrightnessType computeBrightness(LedID ledID, BufferIndex bufferIndex) const {
		const StepList& steps = bufferIndex == BufferIndex::Active ? stepsBuffer[activeStepsIndex] : stepsBuffer[GetOtherBufferIndex[activeStepsIndex]];
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
				
				if(isLedAssigned(ledID, searchedStepNode->value.bitStorage)){
					break;
				}
			}
			
		}

		return foundLedIDsBrightness;
	}


	inline const StepList& getStepList(BufferIndex bufferIndex) const {
		return bufferIndex == BufferIndex::Active ? stepsBuffer[activeStepsIndex] : stepsBuffer[GetOtherBufferIndex[activeStepsIndex]];
	}



	void clear() {
		newIndex = activeStepsIndex;
		uint8_t writableBufferIndex = GetOtherBufferIndex[activeStepsIndex];
		const StepList& steps = stepsBuffer[writableBufferIndex];
		while(steps.size() > 1){
			
			currentStepNode = steps.removeAfter(steps.beforeBegin());			
		}
		newIndex = writableBufferIndex;
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
		static_cast<Impl*>(this)->processLedStep(stateStorage, stepStorage);
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

	
//private:
public:

	StepList stepsBuffer[2] = {
		StepList({PWMStep{}}),
		StepList({PWMStep{}})
	};
	StepNode* currentStepNode = stepsBuffer[0].begin();//steps.insertAfter(steps.beforeBegin(), {});
	BitStorageType stateStorage{};
	BrightnessType maxBrightness;
	BrightnessType minBrightness;
	volatile bool writableBufferReady = false;
	volatile uint8_t activeStepsIndex = 0;
	volatile uint8_t newIndex = 0;
	/*struct {
		bool writableBufferReady : 1;
		uint8_t activeStepsIndex : 1;
		uint8_t newIndex : 1;
		uint8_t _unused : 5;
	};*/
};

#undef _DEFINE_HAS_METHOD_IMPLEMENTED
#undef _HAS_METHOD_IMPLEMENTED_HELPER

template<class SCHEDULED_PWM>
class DimmingPWM {
	public:
	using ScheduledPWMImpl = SCHEDULED_PWM;
	using LedID = typename ScheduledPWMImpl::LedID;
	using BrightnessType = typename ScheduledPWMImpl::BrightnessType;


	struct DimmingState{
		int32_t accumulatedBrightness;
		int32_t tickRate;
		BrightnessType targetBrightness;
		LedID ledId;

	};
	using DimmingStateList = FixedForwardList<SCHEDULED_PWM::LED_COUNT, DimmingState>;
	using Node = typename DimmingStateList::Node;

	public: // constants
	static constexpr uint8_t SHIFT_SCALE = 16;

	public: // member functions 

	template<size_t N_STATES_TO_PROCESS = DimmingStateList::BUFFER_SIZE>
	bool process(ScheduledPWMImpl& schedPWM){
		SCHEDULED_PWM_TRACEBACK_ENTRY
		if(currentDimmingState == dimmingStates.end()){
			return true;
		}
		for(size_t processedCounter = 0; processedCounter < N_STATES_TO_PROCESS; ++processedCounter){

			DimmingState& dimmingState = currentDimmingState->value;
			BrightnessType currentBrightness = dimmingState.accumulatedBrightness >> SHIFT_SCALE;
			LedID ledId = dimmingState.ledId;
			//LedID ledId = dimmingState.ledId;
			//bool brightnessChanged = false;

			

			if(dimmingState.tickRate > 0 && currentBrightness >= dimmingState.targetBrightness){
				currentBrightness = dimmingState.targetBrightness;
				currentDimmingState = dimmingStates.removeAfter(previousDimmingState);
				
			}
			else if(dimmingState.tickRate < 0 && currentBrightness <= dimmingState.targetBrightness){
				currentBrightness = dimmingState.targetBrightness;
				currentDimmingState = dimmingStates.removeAfter(previousDimmingState);
			}
			else {
				
				//for(uint16_t compensate = 0; compensate < compensateTicks; ++compensate){
				int32_t compensatedTicks = dimmingState.tickRate * compensateTicks;
				dimmingState.accumulatedBrightness += dimmingState.tickRate * compensateTicks;
				
				//}
				
				//if((dimmingState.accumulatedBrightness >> SHIFT_SCALE) != currentBrightness){
					
				//}
				previousDimmingState = currentDimmingState;
				currentDimmingState = currentDimmingState->nextNode();
			}
			schedPWM.setLedPWM(dimmingState.ledId, currentBrightness);
			

			if(currentDimmingState == dimmingStates.end()){
				currentDimmingState = dimmingStates.begin();
				previousDimmingState = dimmingStates.beforeBegin();
				compensateTicks = 1;
				return true;
			}
			else {
				++compensateTicks;
			}

		}
		return false;
	}

	void setDimming(LedID ledId, BrightnessType startBrightness, BrightnessType targetBrightness, uint16_t ticks){
		SCHEDULED_PWM_TRACEBACK_ENTRY
		Node* end = dimmingStates.end();

		Node* found = nullptr;
		for(Node* searchedDimmingState = dimmingStates.begin(); searchedDimmingState != end; searchedDimmingState = searchedDimmingState->nextNode()){
			if(searchedDimmingState->value.ledId == ledId){
				found = searchedDimmingState;
				break;
			}
		}

		int32_t scaledDelta = ((int32_t)targetBrightness - (int32_t)startBrightness) << SHIFT_SCALE;
		if(found == nullptr){
			dimmingStates.insertAfter(
				dimmingStates.beforeBegin(),
				DimmingState{
					.accumulatedBrightness = ((int32_t)startBrightness) << SHIFT_SCALE,
					.tickRate = scaledDelta / ticks,
					.targetBrightness = targetBrightness,
					.ledId = ledId
				}
			);
			currentDimmingState = dimmingStates.begin();
		}
		else {
			DimmingState& reused = found->value;
			reused.accumulatedBrightness = ((int32_t)startBrightness) << SHIFT_SCALE;
			reused.tickRate = scaledDelta / ticks;
			reused.targetBrightness = targetBrightness;
			reused.ledId = ledId;
		}
	}


	DimmingStateList dimmingStates;
	Node* currentDimmingState = dimmingStates.begin();
	Node* previousDimmingState = dimmingStates.beforeBegin();
	uint16_t compensateTicks = 1;
};



#endif