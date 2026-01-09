#ifndef ARDUINO_PANIC
#define ARDUINO_PANIC

#include <Arduino.h>
#include <avr/wdt.h>

struct PanicTrace{
	static constexpr size_t INVALID_LINE_NUMBER = -1;
	const PanicTrace* previous = nullptr;
	const char* filename;
	const char* functionName;
	uint32_t line;
	PanicTrace(const char* filename, const char* functionName, uint32_t line = INVALID_LINE_NUMBER) : filename(filename), functionName(functionName), line(line){
		previous = *latestTrace();
		*latestTrace() = this;
	}
	~PanicTrace(){
		*latestTrace() = previous;
	}

	void print(bool printDescription = true) const {
		if(printDescription){
			Serial.println(F("Traceback (most recent call last):"));
		}

		if(previous){
			previous->print(false);
		}

		Serial.print(' ');
		Serial.print(filename);
		if(line != INVALID_LINE_NUMBER){
			Serial.print(':');
			Serial.print(line);
		}
		Serial.print(F(" -> "));
		Serial.println(functionName);


	}

	static const PanicTrace** latestTrace(){
		static const PanicTrace* latest = nullptr;
		return &latest;
	}
	static void printLatest(){
		const PanicTrace* latest = *latestTrace();
		if(latest == nullptr){
			Serial.println(F("No Traceback"));
		}
		else {
			latest->print();
			Serial.println();
		}
	}
};




[[noreturn]] inline void arduino_panic_(const char* msg) {
    cli();                     // stop interrupts
	PanicTrace::printLatest();
	Serial.print(F("PANIC! -> "));
    Serial.println(msg);       // prints directly from flash
    Serial.flush();            // wait until bytes are sent

    wdt_enable(WDTO_2S);     // force hardware reset
    while (true) {}
}

[[noreturn]] inline void arduino_panic_(const __FlashStringHelper* msg) {
    cli();                     // stop interrupts
	PanicTrace::printLatest();
    Serial.print(F("PANIC! -> "));
	Serial.println(msg);       // prints directly from flash
    Serial.flush();            // wait until bytes are sent

    wdt_enable(WDTO_2S);     // force hardware reset
    while (true) {}
}

[[noreturn]] inline void arduino_raw_panic_(const char* msg) {
    cli();                     // stop interrupts
    Serial.println(msg);       // prints directly from flash
    Serial.flush();            // wait until bytes are sent

    wdt_enable(WDTO_2S);     // force hardware reset
    while (true) {}
}

[[noreturn]] inline void arduino_raw_panic_(const __FlashStringHelper* msg) {
    cli();                     // stop interrupts
    Serial.println(msg);       // prints directly from flash
    Serial.flush();            // wait until bytes are sent

    wdt_enable(WDTO_2S);     // force hardware reset
    while (true) {}
}

#define PANIC(msg) arduino_panic_(msg)
#define PANIC_RAW(msg) arduino_raw_panic_(msg)

#endif