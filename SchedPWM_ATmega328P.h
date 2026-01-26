#ifndef SCHEDULED_PWM_ATMEGA328P_H
#define SCHEDULED_PWM_ATMEGA328P_H

#include <inttypes.h>



#include "panic.h"



#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define SCHEDULED_PWM_TRACEBACK_ENTRY \
PanicTrace __traceback_entry(__FILENAME__, __func__, __LINE__);


namespace detail{
	extern void panicOnStepError(const char* msg, size_t index, void* addr);
}
#define FIXED_FORWARD_LIST_ERROR_FN(msg, index, addr)\
	detail::panicOnStepError(((const char*) F(msg)), (index), (addr))



#include "SchedPWM.h"


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
	void applyState(StateStorage& xored, const PortMasks& ownedPins) const;
};


static void initTIMER2();
static void setNextIsrTimeForTIMER2(uint8_t brightness);




};



class ScheduledPWM_TIMER2 : public ScheduledPWM<12, ScheduledPWM_TIMER2, SharedImpl::Pin,  SharedImpl::StateStorage, uint8_t>{
public:

	ScheduledPWM_TIMER2() : ScheduledPWM(1) {
		
	}
	void begin(){
		SharedImpl::initTIMER2();
		//setLedPWM(13, 5);
		/*dimming.setDimming(13,100, 10, 10000>>3);
		dimming.setDimming(2, 100, 10, 10000>>3);
		dimming.setDimming(3, 100, 10, 10000>>3);
		dimming.setDimming(4, 100, 10, 10000>>3);
		dimming.setDimming(5, 200, 101, 10000>>3);
		dimming.setDimming(6, 200, 101, 10000>>3);
		dimming.setDimming(7, 200, 101, 10000>>3);*/
	
		setLedPWM(13, 1);
		setLedPWM(3, 4);
		setLedPWM(4, 8);
		setLedPWM(5, 12);
		setLedPWM(6, 16);
		setLedPWM(7, 20);
		setLedPWM(8, 24);
    

		/*dimming.setDimming(8, 90, 0, 5000>>3);
		dimming.setDimming(9, 80, 0, 5000>>3);
		dimming.setDimming(10, 60, 0, 5000>>3);
		dimming.setDimming(11, 50, 0, 5000>>3);
		dimming.setDimming(12, 40, 0, 5000>>3);*/

	}

	void testImplementation();
DimmingPWM<ScheduledPWM_TIMER2> dimming;

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

	void processLedStep(BitStorageType& xoredStep, const BitStorageType& stepStorage) {
		/*computeBrightness(3);
		computeBrightness(4);
		computeBrightness(5);
		computeBrightness(6);*/
		/*computeBrightness(7);
		computeBrightness(8);

		computeBrightness(13);*/
		stepStorage.applyState(xoredStep, ownedPins);
	}


	void setupNextIsrTime(BrightnessType nextTime) {
		SharedImpl::setNextIsrTimeForTIMER2(nextTime);
	}

	void onDutyCycleBegin() {
		//delayMicroseconds(25);
		
		/*setLedPWM(2, cycleBrightness);
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
		cycleBrightness += direction;*/
		//dimming.process(*this);
		/*if((dimming.currentDimmingState->value.accumulatedBrightness >> 10) > 90){
			dimming.setDimming(13, 89,0, 60000 >> 8);
		}*/
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