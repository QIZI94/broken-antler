#ifndef AUDIO_SAMPLER_H
#define AUDIO_SAMPLER_H

#include <inttypes.h>

using AudioInputHandler = void (*)(uint16_t rawSample, uint16_t avgSample, uint16_t avgOverTime, uint16_t baseline);
extern void initAudioSampler(uint8_t analogPin, uint8_t avgAndBaselineRate);
extern void setAudioSampleHandler(AudioInputHandler audioHandler);
extern uint16_t getLastRawAudioSample();
extern void handleAudioSampling();

extern void averageRawSignal();

extern void debugAudioSampler();


class lowpass_filter_fixed {

  public:
	static constexpr int32_t SHIFT = 10;              // scale = 2^10 = 1024
	static constexpr int32_t SCALE = 1 << SHIFT;

    // default constructor
    lowpass_filter_fixed() {}

    // custom constructor
    lowpass_filter_fixed(float f_cutoff, float f_timePeriod) {
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

class lowpass_filter_fixed_2 {

  public:

    // default constructor
    lowpass_filter_fixed_2() {}

    // custom constructor
    lowpass_filter_fixed_2(float f_cutoff) {
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



#endif