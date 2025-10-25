
/*#pragma GCC optimize( \
  "O3", "inline-functions", "inline-functions-called-once", \
  "unswitch-loops", "peel-loops", "predictive-commoning", \
  "gcse-after-reload", "tree-loop-distribute-patterns", \
  "tree-slp-vectorize", "tree-loop-vectorize", "rename-registers", \
  "reorder-blocks", "reorder-blocks-and-partition", \
  "reorder-functions", "split-wide-types", "cprop-registers", \
  "ipa-cp-clone", "ipa-reference", "ipa-pure-const", "ipa-profile", "ipa-pta", \
  "tree-partial-pre", "tree-tail-merge", "ivopts", "web", \
  "cse-follow-jumps", "cse-skip-blocks", "reorder-blocks-algorithm=simple", \
  "split-paths", "vect-cost-model=dynamic", \
  "align-functions=2", "align-jumps=2", "align-loops=2", "inline-all-stringops" \
)*/
#pragma GCC optimize("O3", "inline-functions", "tree-vectorize", "unroll-loops")
#include <Arduino.h>
#include <avr/io.h>

#include "audiosampler.h"
#include "adcsampler.h"


#define AVG_SAMPLE_DIVISOR (4)
#define AVG_SAMPLE_COUNT (1<<AVG_SAMPLE_DIVISOR)

static void emptyAudioInputHandler(uint16_t avgSample, uint16_t avgOverTime, uint16_t baseline){}

volatile static AudioInputHandler audioInputHandler = emptyAudioInputHandler;
static uint16_t lastAudioSample = 0;
static uint16_t averagedReading = 0;
static uint16_t baselineSum = 0;
static uint16_t baselineAvg = 0;
static uint8_t internalAudioAnalogPin = 0xFF;
static uint8_t averageCounter = 1;
static uint8_t avgBaselineRate = 30;
static uint8_t averageSamplesIndex = 0;
static uint16_t audioSamplesAverage[AVG_SAMPLE_COUNT]{0};
static uint16_t audioSamplesAvgSum = 0;
static uint16_t audioSamplesAvg = 0;



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
	//lastAudioSample = nonBlockingAnalogRead(internalAudioAnalogPin);
	averagedReading = averagedAnalogRead(internalAudioAnalogPin);
	//delayMicroseconds(500);
	
	/*uint16_t averagedReading = 0;
  	for(int sampleIdx = 0; sampleIdx < 5; sampleIdx++){
    	lastAudioSample = uint16_t(analogRead(internalAudioAnalogPin));
    	averagedReading += lastAudioSample;
  	}
	averagedReading /= 5;*/
	//averageRawSignal();
	//uint16_t& averageSampleRef = audioSamplesAverage[averageSamplesIndex++];
	audioSamplesAvgSum -= audioSamplesAverage[averageSamplesIndex];
	audioSamplesAverage[averageSamplesIndex++] = averagedReading;
	audioSamplesAvgSum += averagedReading;

	audioSamplesAvg = audioSamplesAvgSum >> AVG_SAMPLE_DIVISOR;

	//Serial.println(averagedReading);

	/*if(averageCounter < avgBaselineRate){
		if(baselineAvg > averagedReading){
			baselineSum += averagedReading;
		}
		else {
			baselineSum += audioSamplesAvg;
		}
		averageCounter++;
	}
	else {
		baselineAvg = baselineSum = baselineSum >> 5;
		averageCounter = 1;
	}*/
	if(averageCounter < avgBaselineRate){
        baselineSum += baselineAvg > averagedReading ? averagedReading : audioSamplesAvg;		
        averageCounter++;
    }
	else {
		baselineAvg = baselineSum = baselineSum >> 5;
		averageCounter = 1;
	}
	

	
	
	


	if(averageSamplesIndex >= AVG_SAMPLE_COUNT){
		averageSamplesIndex = 0;
	}
	audioInputHandler(averagedReading, audioSamplesAvg, baselineAvg);
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
	noInterrupts();
	uint16_t localLastSample = lastAudioSample;
	uint16_t localAveragedSample = averagedReading;
	uint16_t localAvgOverTime = audioSamplesAvg;
	uint16_t localBaseline = baselineAvg;
	interrupts();
	Serial.print("rawSample:");
	Serial.print(localLastSample);
	Serial.print(",avgSample:");
	Serial.print(localAveragedSample);
	Serial.print(",avgOverTime:");
	Serial.print(localAvgOverTime);
	Serial.print(",baseline:");
	Serial.println(localBaseline);
	/*Serial.print(",diffraw:");
	Serial.print(lastAudioSample - baselineAvg);
	Serial.print("diffavg:");
	Serial.println(audioSamplesAvg - baselineAvg);*/
	
	
}

