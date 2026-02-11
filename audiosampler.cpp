
#include <Arduino.h>
#include <avr/io.h>

#include "audiosampler.h"
#include "adcsampler.h"


#define AVG_SAMPLE_DIVISOR (4)
#define AVG_SAMPLE_COUNT (1<<AVG_SAMPLE_DIVISOR)

static void emptyAudioInputHandler(uint16_t avgSample, uint16_t avgOverTime, uint16_t baseline){}

volatile static AudioInputHandler audioInputHandler = emptyAudioInputHandler;

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



void initAudioSampler(uint8_t analogPin, uint8_t baselineRate){
	internalAudioAnalogPin = analogPin;
	avgBaselineRate = baselineRate;
}

void setAudioSampleHandler(AudioInputHandler audioHandler){
	audioInputHandler = audioHandler; 
}





void handleAudioSampling(){
	
	//averageRawSignal();
	//lastAudioSample = nonBlockingAnalogRead(internalAudioAnalogPin);
	noInterrupts();
	averagedReading = averagedAnalogRead(internalAudioAnalogPin);
	interrupts();
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
	uint16_t localLastSample = nonBlockingAnalogRead(A7);
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

