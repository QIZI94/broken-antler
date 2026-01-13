#include "SchedPWM.h"


#include <string.h>





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




class TestSchedPWM: protected ScheduledPWM<12, uint8_t, uint16_t, uint8_t>  {
protected:
	virtual void assignLed(LedID ledId, BitStorageType& stepStorage) override {
		stepStorage |= 0x01 << ledId;
	}

	virtual void unassignLed(LedID ledId, BitStorageType& stepStorage) override {
		stepStorage &= ~(0x01 << ledId);
	}

	virtual void processLedStep(const BitStorageType& stepStorage) override {

	
	}


	virtual void setupNextIsrTime(BrightnessType nextTime) override {
		
	}


	virtual bool isLedAssigned(LedID ledId, const BitStorageType& stepStorage) const override {
		return ((0x01 << ledId) & stepStorage) != 0;
	}


	virtual bool isLedExclusive(LedID ledId, const BitStorageType& stepStorage) const override {
		if(stepStorage == 0){
			return false;
		}
		BitStorageType inverseLedMask = ~(0x01 << ledId);
		return (inverseLedMask & stepStorage) == 0;
	}

	bool isLedStepShared(LedID ledId, const BitStorageType& previousStepStorage, const BitStorageType& currentStepStorage) const override {
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
		
		//SchedPWM.pwmISR();
		//Serial.println(SchedPWM.steps.indexByNode(SchedPWM.steps.begin()));
		//SchedPWM.setLedPWM(4, 50);
		
		TEST_STATEMENT(
			setLedPWM(4, 50),
			stepList.isEqual(
				{
					PWMStep::make(0b10000, 50),
					PWMStep::make(0b00000, 0),
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
					PWMStep::make(0b10000, 50),
					PWMStep::make(0b00000, 0),
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
					PWMStep::make(0b11000, 30),
					PWMStep::make(0b10000, 50),
					PWMStep::make(0b00000, 0),
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
					PWMStep::make(0b11000, 30),
					PWMStep::make(0b10000, 70),
					PWMStep::make(0b00000, 0),
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
					PWMStep::make(0b11000, 20),
					PWMStep::make(0b10000, 70),
					PWMStep::make(0b00000, 0),
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
					PWMStep::make(0b10100, 40),
					PWMStep::make(0b10000, 70),
					PWMStep::make(0b00000, 0),
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
					PWMStep::make(0b10000, 70),
					PWMStep::make(0b00000, 0),
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
					PWMStep::make(0b110100, 70),
					PWMStep::make(0b100100, 245),
					PWMStep::make(0b000000, 0),
				},
				comparePWMStep
			)
		);

		TEST_CMP(computeBrightness(2), getMaxBrightness());
		TEST_CMP(computeBrightness(5), getMaxBrightness());

		
		//Serial.print("Brightness: ");
		//Serial.println(SchedPWM.computeBrightness(3));
		while(!pwmISR());
		clear();
		while(!pwmISR());
		Serial.println("DONE");
	}

};

void testScheduledPWM(){
	TestSchedPWM testSched;

	testSched.test();
}