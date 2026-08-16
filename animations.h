#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <inttypes.h>
#include "utils/variant.h"

struct Event{
	enum class Type : uint8_t{
		LED_ANIMATION,
		LED_AUDIOLINK,
	};

	union{
		struct {
			Type type			: 2;
			//uint8_t value	: 5;
			uint8_t firstValue	: 4;
			uint8_t secondValue	: 2;
		};
		uint8_t event;
	};
};


enum class AnimationPreset : uint8_t {
	ALL_LEDS_ON,
	LEFT_RIGHT,
	SLOW_BREATHING,
	LEFT_RIGHT_FLOW,
	DMB_BEAT,
	FLOW,
	SEGMENTED_FLOW,
	IDLE_FLOW,
	IDLE_FLOW_COLOR_ROTATION,
	VIU_VIU_POLICE,

	TURN_OFF_LEDS,
	
	NUM_OF_ANIMATION_RESETS
};

enum class AudioLinkIdlePreset : uint8_t {
	IDLE_FLOW_COLOR_ROTATION
};

enum class AudioLinkBassPreset : uint8_t {
	FAST_FLOW,
	FAST_FLOW_RED,
	FAST_FLOW_BLUE,

	REPEAT_FAST_FLOW,
	REPEAT_FAST_FLOW_RED,
	REPEAT_FAST_FLOW_BLUE
};

using AnimationPresetVariant = Variant<AnimationPreset, AudioLinkIdlePreset, AudioLinkBassPreset>;

extern void initAnimationsSwitcher();
extern void handleAnimationsPersistentStorage();

extern void startAnimationPreset(AnimationPreset animPreset, uint16_t delayMs);

extern void startAudioLink(AudioLinkIdlePreset idlePreset, uint16_t delayMs);
extern void triggerAudioLinkBass(AudioLinkBassPreset bassPreset, uint16_t delayMs);

extern const AnimationPresetVariant& getLastAnimationPreset();
extern AudioLinkIdlePreset getLastAudioLinkIdlePreset();



#endif