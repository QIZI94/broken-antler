#ifndef ADC_SAMPLER_H
#define ADC_SAMPLER_H


extern void initSamplerADC();
extern uint16_t nonBlockingAnalogRead(uint8_t pin);
extern uint16_t averagedAnalogRead(uint8_t pin);

#endif