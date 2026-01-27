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


#include <string.h>

#include <inttypes.h>



//#define DEBUG_SCHED_PMW

static const char PROGMEM NOT_EQUAL_TO[] = " != ";
static const char PROGMEM EQUAL_TO[] = " == ";
static const char PROGMEM SEPARATOR[] = "-------------------";
static const char PROGMEM TEST_FAILED[] = "--> TEST FAILED <--";
static const char PROGMEM SUCCESSFUL[] = "' - SUCCESSFUL";
static const char PROGMEM SINGLE_QUOTE[] = "'";


#define TEST_CMP(LEFT, RIGHT)\
Serial.println((const __FlashStringHelper *)SEPARATOR);\
if(LEFT != RIGHT){\
	Serial.println((const __FlashStringHelper *)TEST_FAILED);\
	Serial.print(F(#LEFT));\
	Serial.print((const __FlashStringHelper *) NOT_EQUAL_TO);\
	Serial.println(F(#RIGHT));\
	PANIC_RAW((const __FlashStringHelper *)SINGLE_QUOTE);\
}\
else{\
	Serial.print(F(#LEFT));\
	Serial.print((const __FlashStringHelper *) EQUAL_TO);\
	Serial.print(F(#RIGHT));\
	Serial.println((const __FlashStringHelper *)SUCCESSFUL);\
}

#define TEST_STATEMENT(INIT_STATEMENT, TEST_STATEMENT_TRUE)\
Serial.println((const __FlashStringHelper *)SEPARATOR);\
INIT_STATEMENT;\
if(!TEST_STATEMENT_TRUE){\
	Serial.println((const __FlashStringHelper *)TEST_FAILED);\
	Serial.print('\'');\
	Serial.print(F(#INIT_STATEMENT));\
	PANIC_RAW((const __FlashStringHelper *)SINGLE_QUOTE);\
}\
else{\
	Serial.print('\'');\
	Serial.print(F(#INIT_STATEMENT));\
	Serial.println((const __FlashStringHelper *)SUCCESSFUL);\
}




class TestSchedPWM: protected ScheduledPWM<12, TestSchedPWM, uint8_t, uint16_t, uint8_t>  {
friend ScheduledPWM;
protected:
	void assignLed(LedID ledId, BitStorageType& stepStorage) {
		stepStorage |= 0x01 << ledId;
	}

	void unassignLed(LedID ledId, BitStorageType& stepStorage) {
		stepStorage &= ~(0x01 << ledId);
	}

	void processLedStep(BitStorageType& xoredStep, const BitStorageType& stepStorage) {
		
	
	}


	void setupNextIsrTime(BrightnessType nextTime) {
		
	}


	bool isLedAssigned(LedID ledId, const BitStorageType& stepStorage) const {
		return ((0x01 << ledId) & stepStorage) != 0;
	}


	bool isLedExclusive(LedID ledId, const BitStorageType& stepStorage) const {
		if(stepStorage == 0){
			return false;
		}
		BitStorageType inverseLedMask = ~(0x01 << ledId);
		return (inverseLedMask & stepStorage) == 0;
	}

	bool isLedStepShared(LedID ledId, const BitStorageType& previousStepStorage, const BitStorageType& currentStepStorage) const {
		uint8_t xoredStep = previousStepStorage ^ currentStepStorage;
#ifdef DEBUG_SCHED_PMW
		Serial.print(previousStepStorage, BIN);
		Serial.print('^');
		Serial.print(currentStepStorage, BIN);
		Serial.print('=');
		Serial.println(xoredStep, BIN);
#endif
		if(previousStepStorage == currentStepStorage){
			return false;
		}
		return !isLedExclusive(ledId, xoredStep);
	}

	void onDutyCycleBegin(){
		
	}

	void onDutyCycleEnd(){
		
	}


public:
	void test(){
		SCHEDULED_PWM_TRACEBACK_ENTRY
		
		

		auto comparePWMStep = [](const PWMStep& left, const PWMStep& right){
			bool isStorageEqual = left.bitStorage == right.bitStorage;
			bool isNextIsrTimeEqual = left.nextIsrTime == right.nextIsrTime;

			if(!isStorageEqual){
				Serial.print(F("bitStorage: "));
				Serial.print(left.bitStorage, BIN);
				Serial.print(F(" != "));
				Serial.println(right.bitStorage, BIN);
			}
			if(!isNextIsrTimeEqual){
				Serial.print(F("nextIsrTime: "));
				Serial.print(left.nextIsrTime);
				Serial.print(F(" != "));
				Serial.println(right.nextIsrTime);
			}

			return isStorageEqual && isNextIsrTimeEqual;
		};

		const StepList& stepList = getStepList();
		/*
		auto printForwardList = [](const StepList& toPrint, BitStorageType mask){
			
			for(const StepList::Node* node = toPrint.cbegin(); node != toPrint.cend(); node = node->nextNode()){
				mask ^= node->value.bitStorage;
				Serial.print('(');
				Serial.print(node->value.nextIsrTime);
				Serial.print(')');
				Serial.print(node->value.bitStorage, BIN);
				Serial.print('[');
				for(uint8_t shift = 0; shift < 8; ++shift){
					if(node->value.bitStorage & 0x01 << shift){
						Serial.print((int)shift);
						Serial.print(',');
					}
				}
				Serial.print(']');
				Serial.print('{');
				Serial.print(mask,BIN);
				Serial.println('}');
			}
		};

		setLedPWM(2, 20);
		setLedPWM(3, 30);
		setLedPWM(3, 10);
		setLedPWM(4, 15);
		setLedPWM(5, 20);

		printForwardList(stepList, referenceStepStorage);

		return;*/
		
		//SchedPWM.pwmISR();
		//Serial.println(SchedPWM.steps.indexByNode(SchedPWM.steps.begin()));
		//SchedPWM.setLedPWM(4, 50);
		
		TEST_STATEMENT(
			setLedPWM(4, 50),
			stepList.isEqual(
				{
					PWMStep::make(0b10000, 50),
					PWMStep::make(0b10000, 0),
				},
				comparePWMStep
			)
		);
		

		TEST_CMP(computeBrightness(4), 50)
		
		
		TEST_STATEMENT(
			setLedPWM(3, 30),
			stepList.isEqual(
				{
					PWMStep::make(0b11000, 30),
					PWMStep::make(0b01000, 50),
					PWMStep::make(0b10000, 0),
				},
				comparePWMStep
			)
		);


		TEST_CMP(computeBrightness(3), 30)

		TEST_STATEMENT(
			setLedPWM(2, 10),
			stepList.isEqual(
				{
					PWMStep::make(0b11100, 10),
					PWMStep::make(0b00100, 30),
					PWMStep::make(0b01000, 50),
					PWMStep::make(0b10000, 0),
				},
				comparePWMStep
			)
		);

		TEST_CMP(computeBrightness(2), 10)
		
		TEST_STATEMENT(
			setLedPWM(4, 70),
			stepList.isEqual(
				{
					PWMStep::make(0b11100, 10),
					PWMStep::make(0b00100, 30),
					PWMStep::make(0b01000, 70),
					PWMStep::make(0b10000, 0),
				},
				comparePWMStep
			)
		);

		TEST_CMP(computeBrightness(4), 70)

		TEST_STATEMENT(
			setLedPWM(3, 20),
			stepList.isEqual(
				{
					PWMStep::make(0b11100, 10),
					PWMStep::make(0b00100, 20),
					PWMStep::make(0b01000, 70),
					PWMStep::make(0b10000, 0),
				},
				comparePWMStep
			)
		);

		TEST_CMP(computeBrightness(3), 20)

		//SchedPWM.pwmISR();
		TEST_STATEMENT(
			setLedPWM(2, 40),
			stepList.isEqual(
				{
					PWMStep::make(0b11100, 20),
					PWMStep::make(0b01000, 40),
					PWMStep::make(0b00100, 70),
					PWMStep::make(0b10000, 0),
				},
				comparePWMStep
			)
		);

		TEST_CMP(computeBrightness(2), 40);

		TEST_STATEMENT(
			setLedPWM(2, 20),
			stepList.isEqual(
				{
					PWMStep::make(0b11100, 20),
					PWMStep::make(0b01100, 70),
					PWMStep::make(0b10000, 0),
				},
				comparePWMStep
			)
		);

		TEST_CMP(computeBrightness(2), 20);

		TEST_STATEMENT(
			setLedPWM(2, 255);setLedPWM(5, 255),
			stepList.isEqual(
				{
					PWMStep::make(0b111100, 20),
					PWMStep::make(0b001000, 70),
					PWMStep::make(0b010000, 245),
					PWMStep::make(0b100100, 0),
				},
				comparePWMStep
			)
		);

		TEST_CMP(computeBrightness(2), getMaxBrightness());
		TEST_CMP(computeBrightness(5), getMaxBrightness());

		TEST_STATEMENT(
			setLedPWM(4, 0),
			stepList.isEqual(
				{
					PWMStep::make(0b101100, 20),
					PWMStep::make(0b001000, 245),
					PWMStep::make(0b100100, 0),
				},
				comparePWMStep
			)
		);

		TEST_STATEMENT(
			setLedPWM(4, 70),
			stepList.isEqual(
				{
					PWMStep::make(0b111100, 20),
					PWMStep::make(0b001000, 70),
					PWMStep::make(0b010000, 245),
					PWMStep::make(0b100100, 0),
				},
				comparePWMStep
			)
		);

		TEST_STATEMENT(
			setLedPWM(2, 0),
			stepList.isEqual(
				{
					PWMStep::make(0b111000, 20),
					PWMStep::make(0b001000, 70),
					PWMStep::make(0b010000, 245),
					PWMStep::make(0b100000, 0),
				},
				comparePWMStep
			)
		);

		
		//Serial.print("Brightness: ");
		//Serial.println(SchedPWM.computeBrightness(3));
		constexpr StepList::SizeType EXPECTED_SIZE = 4;
		
		
		TEST_CMP(getStepList().size(), EXPECTED_SIZE);


		StepList::SizeType stepCount = 1;
		while(!pwmISR()){
			++stepCount;
		}
		constexpr StepList::SizeType FIRST_PWM_ISR_ITERATED_STEP_COUNT = EXPECTED_SIZE;
		TEST_CMP(getStepList().size(), FIRST_PWM_ISR_ITERATED_STEP_COUNT);
		
		stepCount = 1;
		while(!pwmISR()){
			++stepCount;
		}

		constexpr StepList::SizeType SECOND_PWM_ISR_ITERATED_STEP_COUNT = EXPECTED_SIZE;
		TEST_CMP(getStepList().size(), SECOND_PWM_ISR_ITERATED_STEP_COUNT);
		clear();
		stepCount = 1;
		while(!pwmISR()){
			++stepCount;
		}
		constexpr StepList::SizeType CLEARED_PWM_ISR_ITERATED_STEP_COUNT = 1;
		TEST_CMP(getStepList().size(), CLEARED_PWM_ISR_ITERATED_STEP_COUNT);

		Serial.println(F("DONE"));
	}

};

void testScheduledPWM(){
	TestSchedPWM testSched;

	testSched.test();
}