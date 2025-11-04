#include <Arduino.h>
#include "adcsampler.h"

#define N_SAMPLES 20
volatile uint16_t adcAverageBufferA7[N_SAMPLES];
volatile uint16_t lastReadingADCA7 = 0;
volatile uint16_t lastReadingADCA6 = 0;
volatile uint16_t adcAverageBufferIndexA7 = 0;
volatile uint16_t adcAverageSumA7 = 0;
volatile bool discardAfterChannelChange = false;
ISR(ADC_vect) {
#ifdef ADC
	if(discardAfterChannelChange){
		discardAfterChannelChange = false;
		return;
	}
	if((ADMUX & 0x0F) == 6){
		lastReadingADCA6 = ADC;
		ADMUX = (ADMUX & 0xF0) | 7;
		discardAfterChannelChange = true;
		return;
	}


    lastReadingADCA7 = ADC;
#endif
	// remove oldest
	adcAverageSumA7 -= adcAverageBufferA7[adcAverageBufferIndexA7];   
    adcAverageBufferA7[adcAverageBufferIndexA7] = lastReadingADCA7;
    adcAverageSumA7 += lastReadingADCA7;  
	//Serial.println(adc_buf[buf_idx]);
	//Serial.println(buf_idx);
	adcAverageBufferIndexA7++;          // add newest
    if(adcAverageBufferIndexA7 >= N_SAMPLES){
		adcAverageBufferIndexA7 = 0;
#ifdef ADC	
		ADMUX = (ADMUX & 0xF0) | 6;
		discardAfterChannelChange = true;
#endif
	}
	//buf_idx = (buf_idx + 1) % N_SAMPLES;
	
}

void initSamplerADC(){
#ifdef ADC
	ADMUX = (1 << REFS0) | 7;

	// Enable ADC and set prescaler
	ADCSRA = (1 << ADEN) |             // Enable ADC
			(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128

	ADCSRB = 0;                        // Free-running mode
	ADCSRA |= (1 << ADATE) | (1 << ADIE); // Auto-trigger + interrupt enable

	ADCSRA |= (1 << ADSC);   
#endif          // Start first conversion
}

uint16_t nonBlockingAnalogRead(uint8_t pin){
	switch (pin){
		case A7:
			return lastReadingADCA7;
		case A6:
			return lastReadingADCA6;		
	}
	return 0;
}

uint16_t averagedAnalogRead(uint8_t pin){
	switch (pin){
		case A7:
			return adcAverageSumA7 / N_SAMPLES;
		case A6:
			return lastReadingADCA6;		
	}

	return 0;
}

uint16_t summedAnalogRead(uint8_t pin){
	switch (pin){
		case A7:
			return adcAverageSumA7;
		case A6:
			return lastReadingADCA6;		
	}

	return 0;
}
