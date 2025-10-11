
#include <Arduino.h>
#include <avr/io.h>

#include "audiosampler.h"
#include "adcsampler.h"


#define AVG_SAMPLE_DIVISOR (4)
#define AVG_SAMPLE_COUNT (1<<AVG_SAMPLE_DIVISOR)


volatile static AudioInputHandler audioInputHandler = nullptr;
volatile static uint16_t lastAudioSample = 0;
volatile static uint16_t averagedReading = 0;
static uint16_t baselineSum = 0;
static uint16_t baselineAvg = 0;
static uint8_t internalAudioAnalogPin = 0xFF;
static uint8_t averageCounter = 1;
static uint8_t avgBaselineRate = 30;
static uint16_t averageSamplesIndex = 0;
volatile static uint16_t audioSamplesAverage[AVG_SAMPLE_COUNT]{0};
volatile uint16_t audioSamplesAvgSum = 0;
volatile uint16_t audioSamplesAvg = 0;



/*
ISR(ADC_vect) {
#ifdef ADC
    lastAudioSample = ADC;
#endif
	// remove oldest
	running_sum -= adc_buf[buf_idx];   
    adc_buf[buf_idx] = lastAudioSample;
    running_sum += lastAudioSample;  
	//Serial.println(adc_buf[buf_idx]);
	//Serial.println(buf_idx);
	buf_idx++;          // add newest
    if(buf_idx >= N_SAMPLES){
		buf_idx = 0;
	}
	//buf_idx = (buf_idx + 1) % N_SAMPLES;
	
}

void adc_init_freerun() {
	#ifdef ADC
	ADMUX = (1 << REFS0) | 7;

	// Enable ADC and set prescaler
	ADCSRA = (1 << ADEN) |             // Enable ADC
			(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Prescaler 128

	ADCSRB = 0;                        // Free-running mode
	ADCSRA |= (1 << ADATE) | (1 << ADIE); // Auto-trigger + interrupt enable

	ADCSRA |= (1 << ADSC);   
	#endif          // Start first conversion
    // Discard first conversion (common AVR quirk)
  //  while (ADCSRA & (1 << ADSC)); // wait for first conversion
   // uint16_t dummy = ADC;         // discard

}*/


void initAudioSampler(uint8_t analogPin, uint8_t baselineRate){
	internalAudioAnalogPin = analogPin;
	avgBaselineRate = baselineRate;

/*	adc_init_freerun();
#ifdef ADCSRA
	//ADCSRA = (ADCSRA & 0xf8) | 0x04;
#endif
*/
}

void setAudioSampleHandler(AudioInputHandler audioHandler){
	audioInputHandler = audioHandler; 
}

uint16_t getLastRawAudioSample(){
	return lastAudioSample;
}



void handleAudioSampling(){
	
	//averageRawSignal();
	lastAudioSample = nonBlockingAnalogRead(internalAudioAnalogPin);
	averagedReading = averagedAnalogRead(internalAudioAnalogPin);
	//delayMicroseconds(500);
	
	/*uint16_t averagedReading = 0;
  	for(int sampleIdx = 0; sampleIdx < 5; sampleIdx++){
    	lastAudioSample = uint16_t(analogRead(internalAudioAnalogPin));
    	averagedReading += lastAudioSample;
  	}
	averagedReading /= 5;*/
	//averageRawSignal();

	audioSamplesAvgSum -= audioSamplesAverage[averageSamplesIndex];
	audioSamplesAverage[averageSamplesIndex] = averagedReading;
	audioSamplesAvgSum += averagedReading;

	audioSamplesAvg = audioSamplesAvgSum >> AVG_SAMPLE_DIVISOR;

	//Serial.println(averagedReading);

	if(averageCounter < avgBaselineRate){
		if(baselineAvg < averagedReading){
			baselineSum += averagedReading;
		}
		else {
			baselineSum += audioSamplesAvg;
		}
		
	}
	else {
		baselineAvg = baselineSum = baselineSum / avgBaselineRate;
		averageCounter = 1;
	}
	
	if(audioInputHandler != nullptr){
		audioInputHandler(lastAudioSample, averagedReading, audioSamplesAvg, baselineAvg);
	}
	averageCounter++;
	averageSamplesIndex++;

	if(averageSamplesIndex >= AVG_SAMPLE_COUNT){
		averageSamplesIndex = 0;
	}
	
}

void debugAudioSampler(){
	uint16_t avgReading = 0;
	uint16_t audioSample = 0;
  	/*for(int sampleIdx = 0; sampleIdx < N_SAMPLES; sampleIdx++){
		
		audioSample = adc_buf[sampleIdx];

		Serial.print("raw");
		Serial.print("[");
		Serial.print(sampleIdx);
		Serial.print("]: ");
		Serial.println(adc_buf[sampleIdx]);
    	avgReading += audioSample;
  	}*/
	Serial.print("rawSample: ");
	Serial.print(lastAudioSample);
	Serial.print(" avgSample: ");
	Serial.print(averagedReading);
	Serial.print(" avgOverTime: ");
	Serial.print(audioSamplesAvg);
	Serial.print(" baseline: ");
	Serial.print(baselineAvg);
	Serial.print(" duffraw: ");
	Serial.print(lastAudioSample - baselineAvg);
	Serial.print(" duffavg: ");
	Serial.println(audioSamplesAvg - baselineAvg);
	Serial.println(nonBlockingAnalogRead(A6));
}

