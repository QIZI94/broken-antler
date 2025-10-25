#ifndef AUDIO_SAMPLER_H
#define AUDIO_SAMPLER_H

#include <inttypes.h>

using AudioInputHandler = void (*)(uint16_t avgSample, uint16_t avgOverTime, uint16_t baseline);
extern void initAudioSampler(uint8_t analogPin, uint8_t avgAndBaselineRate);
extern void setAudioSampleHandler(AudioInputHandler audioHandler);
extern uint16_t getLastRawAudioSample();
extern void handleAudioSampling();

extern void averageRawSignal();

extern void debugAudioSampler();


class LowPassFilterFixed {

  public:
	static constexpr int32_t SHIFT = 10;              // scale = 2^10 = 1024
	static constexpr int32_t SCALE = 1 << SHIFT;

    // default constructor
    LowPassFilterFixed() {}

    // custom constructor
    LowPassFilterFixed(float f_cutoff, float f_timePeriod = 1024) {
		setup(f_cutoff, f_timePeriod);
	}

	void setup(float f_cutoff, float f_timePeriod){
		float ts = f_timePeriod * 1e-6;
		float a_0 = -(((ts / 2) * 2 * PI * f_cutoff - 1) / ((ts / 2) * 2 * PI * f_cutoff + 1));
      	float b_0 = ((ts / 2) * 2 * PI * f_cutoff) / (1 + (ts / 2) * 2 * PI * f_cutoff);
		//constexpr uint32_t scaled_a = (1024 * a_0 * division_scaler) + (1024 * b_0 * division_scaler) + (1024 * b_0 * division_scaler);
		a_0_scaled = (a_0 * SCALE + 0.5);
		b_0_scaled = (b_0 * SCALE + 0.5);
	}

	

    int32_t old_raw = 0; // old raw val
    int32_t old_filtered = 0; // old filtered val
	int32_t a_0_scaled = 0;
	int32_t b_0_scaled = 0;

    //float ts = 0; // sampling time 1/sampling frequency
    

    // filter function
    // raw signal as input
	int32_t filter(int32_t raw) {
		// use power-of-two scale to replace divides with bit shifts



		int32_t raw_scaled = raw << SHIFT;

		// multiply → rescale with right shift instead of divide
		int32_t term1 = (a_0_scaled * old_filtered) >> SHIFT;
		int32_t term2 = (b_0_scaled * raw_scaled) >> SHIFT;
		int32_t term3 = (b_0_scaled * old_raw) >> SHIFT;

		int32_t filtered = term1 + term2 + term3;

		old_raw = raw_scaled;
		old_filtered = filtered;

		// convert back from fixed-point (right shift instead of divide)
		return filtered >> SHIFT;
	}
};

class LowPassFilter {

  public:

    // default constructor
    LowPassFilter() {}

    // custom constructor
    LowPassFilter(float f_cutoff) {
		setup(f_cutoff);
	}

	void setup(float f_cutoff){
		constexpr float ts = 1024* 1e-6;
		//constexpr float f_cutoff = 70;
		a_0 = -(((ts / 2) * 2 * PI * f_cutoff - 1) / ((ts / 2) * 2 * PI * f_cutoff + 1));
      	b_0 = ((ts / 2) * 2 * PI * f_cutoff) / (1 + (ts / 2) * 2 * PI * f_cutoff);
		//constexpr uint32_t scaled_a = (1024 * a_0 * division_scaler) + (1024 * b_0 * division_scaler) + (1024 * b_0 * division_scaler);

	}

	

    float old_raw = 0; // old raw val
    float old_filtered = 0; // old filtered val
	float a_0 = 0;
	float b_0 = 0;

    //float ts = 0; // sampling time 1/sampling frequency
    

    // filter function
    // raw signal as input
   float filter(float raw) {
		
		
		float filtered = a_0 * old_filtered + b_0 * raw + b_0 * old_raw;
		old_raw = raw; // save current raw for next calc
		old_filtered = filtered; // save current filtered for next calc
		return filtered; // return the filtered val
    }
};

class HighPassFilterFixed {
	static constexpr int32_t SHIFT = 10;              // scale = 2^10 = 1024
	static constexpr int32_t SCALE = 1 << SHIFT;
  public:

    // default constructor
    HighPassFilterFixed() {}

    // custom constructor
    HighPassFilterFixed(float f_c, float f_timePeriod = 1024) {
		setup(f_c, f_timePeriod);
	}

    int32_t old_raw = 0;      // old raw value
    int32_t old_filtered = 0; // old filtered value
	int32_t a_0_scaled = 0;
	int32_t b_0_scaled = 0;
	
	void setup(float f_cutoff, float f_timePeriod){
		float dt = f_timePeriod * 1e-6;
		float wc = 2 * PI * f_cutoff;
		float a_0 = (1 - (dt / 2) * wc) / (1 + (dt / 2) * wc);
		float b_0 = 1 / (1 + (dt / 2) * wc);
		a_0_scaled = (a_0 * SCALE + 0.5);
		b_0_scaled = (b_0 * SCALE + 0.5);

	}
    // filter function
    int32_t filter(int32_t raw) {

		int32_t raw_scaled = raw << SHIFT;

		
		      // compute difference (still in ADC units)
      int32_t dx = raw - old_raw;

      // HPF equation in fixed point
      	int32_t filtered = ( (a_0_scaled * old_filtered) + (b_0_scaled * dx) ) >> SHIFT;

    	//int32_t filtered = ( (a_0_scaled * old_filtered) + (b_0_scaled * (raw_scaled - old_raw)) ) >> SHIFT;

      	old_raw = raw;
      	old_filtered = filtered;

      	return filtered;
    }
};


class HighPassFilter {

  public:
    float f_cutoff;
    // default constructor
    HighPassFilter(){}

    // custom constructor
    HighPassFilter(float f_c, float f_timePeriod = 1024) {
		setup(f_c, f_timePeriod);
	}

    float old_raw;      // previous raw value
    float old_filtered; // previous filtered value
    float a_0 = 0;
	float b_0 = 0;


	void setup(float f_cutoff, float f_timePeriod){
		float dt = f_timePeriod * 1e-6;
		float wc = 2 * PI * f_cutoff;
		a_0 = (1 - (dt / 2) * wc) / (1 + (dt / 2) * wc);
		b_0 = 1 / (1 + (dt / 2) * wc);
	}

    // filter function
    float filter(float raw) {
    	// difference equation for HPF
    	float filtered = a_0 * old_filtered + b_0 * (raw - old_raw);

    	old_raw = raw;        // store current raw
    	old_filtered = filtered; // store current filtered
    	return filtered; // return in same scale as input (no bias)
    }
};
#endif