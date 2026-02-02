#ifndef LED_DEFINITION_H
#define LED_DEFINITION_H
#include <Arduino.h>


using BrightnessConversionFunc = uint8_t (*)(uint8_t);

struct LedParams{
	const uint8_t pin;
	const BrightnessConversionFunc convertBrightness;
};

struct LedDef{
	const LedParams blue;
	const LedParams red;
};

struct LedBrightness{
	uint8_t blue;
	uint8_t red;
	static constexpr LedBrightness from(uint8_t blue, uint8_t red){
		return LedBrightness{
			.blue = blue,
			.red = red
		};
	}
	static constexpr LedBrightness from(uint8_t brightness){
		return LedBrightness{
			.blue = brightness,
			.red = brightness
		};
	}
};

inline constexpr LedBrightness PERCENTAGE_TO_BRIGHTNESS(uint8_t percent){
	return LedBrightness::from((((uint16_t)percent * 255) / 100) & 0xFE);
}

inline constexpr LedBrightness PERCENTAGE_TO_BRIGHTNESS(uint8_t blue, uint8_t red){
	return LedBrightness::from(
		(((uint16_t)blue * 255) / 100) & 0xFE,
		(((uint16_t)red * 255) / 100) & 0xFE
	);
}



template <size_t N> 
struct LedTable {
    static constexpr float GAMMA = 2.2f;
    uint8_t data[N]{};
    constexpr LedTable() {
        for (size_t i = 0; i < N; ++i) {
            float norm = static_cast<float>(i) / static_cast<float>(N - 1); 
            data[i] = gamma_correct(norm); 
        }
        
    } 
    static constexpr uint8_t gamma_correct(float x) { return static_cast<uint8_t>( round(pow(x, GAMMA) * static_cast<float>(N)) ); }
}; 
inline uint8_t logarithmicBrightness(uint8_t inputBrightness){
	
	// Instantiate at compile time
	static constexpr const PROGMEM LedTable<255> led_table;
	return pgm_read_byte(&led_table.data[inputBrightness]);
	//return inputBrightness;//inputBrightness == 0 ? inputBrightness : inputBrightness / 2;
}

inline uint8_t linearBrightness(uint8_t inputBrightness){
	
	return inputBrightness;//logarithmicBrightness(inputBrightness);//inputBrightness == 0 ? inputBrightness : inputBrightness / 2;
}
inline const LedDef LED_LeftFront{
	//.blue = {.pin = A0, .convertBrightness = linearBrightness},
	//.red = {.pin = A1, .convertBrightness = linearBrightness}
	.blue = {.pin = 12, .convertBrightness = logarithmicBrightness},
	.red = {.pin = 13, .convertBrightness = linearBrightness}
	
};

inline const LedDef LED_LeftMiddle{
	.blue = {.pin = A2, .convertBrightness = logarithmicBrightness},
	.red = {.pin = A3, .convertBrightness = linearBrightness}
	
	
	
};

inline const LedDef LED_LeftBack{
	//.blue = {.pin = 11, .convertBrightness = linearBrightness},
	//.red = {.pin = 3, .convertBrightness = linearBrightness}
	.blue = {.pin = A0, .convertBrightness = logarithmicBrightness},
	.red = {.pin = A1, .convertBrightness = linearBrightness}
};

inline const LedDef LED_RightFront{
	.blue = {.pin = 8, .convertBrightness = logarithmicBrightness},
	.red = {.pin = 9, .convertBrightness = linearBrightness}
};

inline const LedDef LED_RightMiddle{
	.blue = {.pin = 4, .convertBrightness = logarithmicBrightness},
	.red = {.pin = 7, .convertBrightness = linearBrightness}
};

inline const LedDef LED_RightBack{
	.blue = {.pin = 11, .convertBrightness = logarithmicBrightness},
	.red = {.pin = 3, .convertBrightness = linearBrightness}
};

inline const LedDef LED_Eye{
	.blue = {.pin = 5, .convertBrightness = logarithmicBrightness},
	.red = {.pin = 6, .convertBrightness = linearBrightness}
};


inline const LedDef LED_AllLeds[] = {
	LED_LeftFront,
	LED_LeftMiddle,
	LED_LeftBack,
	LED_RightFront,
	LED_RightMiddle,
	LED_RightBack
};



enum class LedPosition : uint8_t{
	LEFT_FRONT,
	LEFT_MIDDLE,
	LEFT_BACK,
	RIGHT_FRONT,
	RIGHT_MIDDLE,
	RIGHT_BACK,
	NUM_OF_ALL_LEDS
};

inline void initLeds(){
	for(const LedDef& led : LED_AllLeds){
		pinMode(led.blue.pin, OUTPUT);
		pinMode(led.red.pin, OUTPUT);
	}
}

inline void setLed(const LedDef& led, bool blueLed = true, bool redLed = true){

	digitalWrite(led.blue.pin, blueLed ? HIGH : LOW);
	digitalWrite(led.red.pin, redLed ? HIGH : LOW);

}

#endif