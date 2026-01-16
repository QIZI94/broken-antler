#include "SchedPWM.h"


#include <string.h>

#include "panic.h"



//#define DEBUG_SCHED_PMW


void detail::panicOnStepError(const char* msg, size_t index, void* addr){
	PanicTrace::printLatest();
	char indexAndAddrStr[40];
	sprintf(indexAndAddrStr, "PANIC! -> (%p):[%u] -> ", addr, index);
	Serial.print(indexAndAddrStr);
	PANIC_RAW((const __FlashStringHelper *)msg);	
}



namespace SPWM_ATmega328P{

	using PortIndex = SharedImpl::PortIndex;
	using PortMasks = SharedImpl::PortMasks;

	using PortIndexAndMask = SharedImpl::PortIndexAndMask;

	

	bool isPinExclusiveInMask(SharedImpl::Pin pin, const PortMasks& pinsStates){
		PortIndexAndMask pinPortIndexAndMask = SharedImpl::helpers::pinMasksAndIndexesTable.lookup[pin];
		for(PortIndex otherPortIdx : SharedImpl::helpers::otherPortIndexes[pinPortIndexAndMask.portIndex]){
			if(pinsStates[otherPortIdx] != 0){
				return false;
			}
		}
		uint8_t portStateStorage = pinsStates[pinPortIndexAndMask.portIndex];
		if(portStateStorage == 0){
			return false;
		}
		uint8_t inversePinMask = ~pinPortIndexAndMask.mask;
		return (inversePinMask & portStateStorage) == 0;
	}


	void SharedImpl::StateStorage::assign(Pin pin,  PortMasks& ownedPins){
		PortIndexAndMask pinPortIndexAndMask = helpers::pinMasksAndIndexesTable.lookup[pin];
		volatile uint8_t* ownedMaskPtr = &ownedPins[pinPortIndexAndMask.portIndex];
		if(((*ownedMaskPtr) & pinPortIndexAndMask.mask) == 0){
			pinMode(pin, OUTPUT);
			*ownedMaskPtr |= pinPortIndexAndMask.mask;
		}
		digitalPortStates[pinPortIndexAndMask.portIndex] |= pinPortIndexAndMask.mask;
		//ownedPins[pinPortIndexAndMask.portIndex] |= pinPortIndexAndMask.mask;
	}

	void SharedImpl::StateStorage::unassign(Pin pin, PortMasks& ownedPins){
		PortIndexAndMask pinPortIndexAndMask = helpers::pinMasksAndIndexesTable.lookup[pin];
		volatile uint8_t* ownedMaskPtr = &ownedPins[pinPortIndexAndMask.portIndex];
		if(((*ownedMaskPtr) & pinPortIndexAndMask.mask) == 0){
			pinMode(pin, OUTPUT);
			*ownedMaskPtr |= pinPortIndexAndMask.mask;
		}
		digitalPortStates[pinPortIndexAndMask.portIndex] &= ~pinPortIndexAndMask.mask;
		//ownedPins[pinPortIndexAndMask.portIndex] |= pinPortIndexAndMask.mask;
	}

	bool SharedImpl::StateStorage::isAssigned(Pin pin) const {
		/*if(true || pin != cachedPin){
			cachedPin = pin;
			cachedPinToPortAndMask = pinToPortIndexAndMask(pin);
		}*/
		PortIndexAndMask pinPortIndexAndMask = helpers::pinMasksAndIndexesTable.lookup[pin];
		//PortIndexAndMask pinPortIndexAndMask = pinToPortIndexAndMask(pin);
		return (digitalPortStates[pinPortIndexAndMask.portIndex] & pinPortIndexAndMask.mask) != 0;
	}

	bool SharedImpl::StateStorage::isExclusive(Pin pin) const {
		return isPinExclusiveInMask(pin, digitalPortStates);
	}

	bool SharedImpl::StateStorage::isSharedWith(Pin pin, const StateStorage &other) const {
		//uint8_t xoredStep = previousStepStorage ^ currentStepStorage;

		PortMasks xoredMasks;
		uint8_t equalCount = 0;
		for(uint8_t portIdx = 0; portIdx < PortIndex::PORT_COUNT; ++portIdx){
			uint8_t xoredMask = digitalPortStates[portIdx] ^ other.digitalPortStates[portIdx];
			if(xoredMask == 0){
				++equalCount;
			}
			xoredMasks[portIdx] = xoredMask;
		}

		if(equalCount == PortIndex::PORT_COUNT){
			return false;
		}
		return !isPinExclusiveInMask(pin, xoredMasks);
	}

	void SharedImpl::StateStorage::applyState(const PortMasks& ownedPins) const {
		static volatile uint8_t** portsPtr = helpers::GetOrderedPortsArray();
		for(uint8_t portIdx = 0; portIdx < PortIndex::PORT_COUNT; ++portIdx){
			volatile uint8_t* port = portsPtr[portIdx];
			uint8_t ownedMask = ownedPins[portIdx];
			uint8_t ownedPortMask = (*port) & (~ownedMask);
			ownedPortMask |= digitalPortStates[portIdx] & ownedMask;
			*port = ownedPortMask;
		}
	}

	void SharedImpl::setNextIsrTimeForTIMER2(uint8_t brightness){
		brightness = brightness == 1 ? 1 : brightness>>1;
		if(brightness < OCR2A){
			OCR2B = brightness;
		}
		else {
			constexpr uint8_t MAX_BRIGHTNESS = UINT8_MAX/2;
			OCR2B = MAX_BRIGHTNESS;
		}
	}
	void SharedImpl::initTIMER2(){
		cli();                      // stop interrupts during setup

		// Reset timer configuration
		TCCR2A = 0;
		TCCR2B = 0;
		TCNT2  = 0;

		// ---- Compare values ----
		OCR2A = 129;                // period reset (~120 Hz)
		OCR2B = 0;                 

		// ---- Mode: CTC ----
		// WGM22:0 = 010 → CTC with OCR2A as TOP
		TCCR2A |= (1 << WGM21);

		// ---- Prescaler: 1024 ----
		TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);

		// ---- Interrupts ----
		TIMSK2 = 0;                 // start clean
		TIMSK2 |= (1 << OCIE2B);    // enable ONLY Compare B interrupt
		// OCIE2A intentionally NOT set

		sei();                      // enable interrupts

	}
	ISR(TIMER2_COMPB_vect) {
		SchedPWM_TIMER2.pwmISR();
	}
	void ScheduledPWM_TIMER2::testImplementation(){
		//using StepNode = ScheduledPWM_TIMER2::StepNode;
		//using PWMStep = ScheduledPWM_TIMER2::PWMStep;
		static volatile uint8_t** portsPtr = SharedImpl::helpers::GetOrderedPortsArray();
		auto start = micros();
		setLedPWM(2, 10);
		setLedPWM(3, 20);
		setLedPWM(2, 30);
		setLedPWM(8, 50);
		setLedPWM(A1, 50);
		
		setLedPWM(13, 9);
		auto end = micros();
		
		computeBrightness(2);
		computeBrightness(3);
		computeBrightness(2);
		computeBrightness(8);
		computeBrightness(A1);
		
		computeBrightness(13);
		
		Serial.print("Time: ");
		Serial.println(end-start);

		//pinMode(13, OUTPUT);
		
		const StepList& stepList = getStepList();
		for(const StepNode* it = stepList.cbegin(); it != stepList.cend(); it=it->nextNode()){
			const PWMStep& step = it->value;
			Serial.print("Step state: ");
			Serial.print(step.bitStorage.digitalPortStates[PortIndex::PORTC_INDEX], BIN);
			Serial.print(' ');
			Serial.print(step.bitStorage.digitalPortStates[PortIndex::PORTB_INDEX], BIN);
			Serial.print(' ');
			Serial.print(step.bitStorage.digitalPortStates[PortIndex::PORTD_INDEX], BIN);
			Serial.print('(');
			Serial.print(step.nextIsrTime);
			Serial.print(')');
			Serial.print('[');
			for(uint8_t pin = 0; pin < 21; ++pin){
				if(step.bitStorage.isAssigned(pin)){
					Serial.print(pin);
					Serial.print(',');
				}
			}
			Serial.print(']');
			
			step.bitStorage.applyState(ownedPins);

			Serial.print(" Step port state: ");
			Serial.print(*portsPtr[PortIndex::PORTC_INDEX], BIN);
			Serial.print(' ');
			Serial.print(*portsPtr[PortIndex::PORTB_INDEX], BIN);
			Serial.print(' ');
			Serial.print(*portsPtr[PortIndex::PORTD_INDEX], BIN);

			Serial.print(" owned pins: ");
			Serial.print(ownedPins[PortIndex::PORTC_INDEX], BIN);
			Serial.print(' ');
			Serial.print(ownedPins[PortIndex::PORTB_INDEX], BIN);
			Serial.print(' ');
			Serial.println(ownedPins[PortIndex::PORTD_INDEX], BIN);
			delay(1500);
		}
		while(!pwmISR()){
			Serial.println("INSIDE TEST");
		}
		Serial.println("END");
		
	}

	void testImplementation(){
		ScheduledPWM_TIMER2 schedPWM;
		schedPWM.testImplementation();
		
		//SchedPWM_TIMER2.begin();
		
		//SchedPWM_TIMER2.setLedPWM(13, 2);
		//Serial.print("HEEEEEEER: ");
		//Serial.println(SchedPWM_TIMER2.currentStepNode->nextNode() != SchedPWM_TIMER2.steps.end());
	}

} // SPWM_ATmega328P



