#ifndef SCHEDULED_PWM_ATMEGA328P_H
#define SCHEDULED_PWM_ATMEGA328P_H

#include <inttypes.h>



#include "panic.h"



#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define SCHEDULED_PWM_TRACEBACK_ENTRY /*\
PanicTrace __traceback_entry(__FILENAME__, __func__, __LINE__);*/


namespace detail{
	extern void panicOnStepError(const char* msg, size_t index, void* addr);
}
#define FIXED_FORWARD_LIST_ERROR_FN(msg, index, addr)/*\
	detail::panicOnStepError(((const char*) F(msg)), (index), (addr))*/



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


extern void initTIMER2();
extern void setNextIsrTimeForTIMER2(uint8_t brightness);




};


template<size_t N>
class ScheduledPWM_TIMER2 : public ScheduledPWM<N, ScheduledPWM_TIMER2<N>, SharedImpl::Pin,  SharedImpl::StateStorage, uint8_t>{
public:
	using ScheduledPWM = ScheduledPWM<N, ScheduledPWM_TIMER2<N>, SharedImpl::Pin,  SharedImpl::StateStorage, uint8_t>;
	using LedID = SharedImpl::Pin;
	using BitStorageType = SharedImpl::StateStorage;
	using BrightnessType = uint8_t;
	ScheduledPWM_TIMER2() : ScheduledPWM(5) {
		
	}
	void begin(){
		SharedImpl::initTIMER2();
		//setLedPWM(13, 5);
	/*	dimming.setDimming(13,100, 5, 10000>>1);
		dimming.setDimming(12,100, 1, 5000>>1);
		dimming.setDimming(2, 100, 50, 12000>>1);
		dimming.setDimming(3, 100, 50, 9000>>1);
		dimming.setDimming(4, 100, 50, 5000>>1);
		dimming.setDimming(5, 200, 101, 7000>>1);
		dimming.setDimming(6, 200, 101, 6000>>1);
		dimming.setDimming(8, 200, 101, 4000>>1);
		dimming.setDimming(9, 200, 101, 2000>>1);
		dimming.setDimming(10, 200, 101, 1000>>1);
		dimming.setDimming(11, 200, 101, 8000>>1);
		dimming.setDimming(A1, 100, 5, 0>>1);*/

		
	
	/*	setLedPWM(13, 1);
		setLedPWM(3, 4);
		setLedPWM(4, 8);
		setLedPWM(5, 12);
		setLedPWM(6, 16);
		setLedPWM(7, 20);
		setLedPWM(8, 24);*/
    

		/*dimming.setDimming(8, 90, 0, 5000>>3);
		dimming.setDimming(9, 80, 0, 5000>>3);
		dimming.setDimming(10, 60, 0, 5000>>3);
		dimming.setDimming(11, 50, 0, 5000>>3);
		dimming.setDimming(12, 40, 0, 5000>>3);*/

	}

	//void testImplementation();
//DimmingPWM<ScheduledPWM_TIMER2> dimming;

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
		stepStorage.applyState(xoredStep, ownedPins);
	}


	void setupNextIsrTime(BrightnessType nextTime) {
		SharedImpl::setNextIsrTimeForTIMER2(nextTime);
	}

	void onDutyCycleBegin() {
	}

	void onDutyCycleEnd(){

	}
	
	SharedImpl::PortMasks ownedPins = {};
	
	
	friend ScheduledPWM;
};


extern void testImplementation();
}

//inline ScheduledPWM SchedPWM;



extern void testScheduledPWM();

#endif