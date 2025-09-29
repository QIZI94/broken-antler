#ifndef ANIMATIONS_HANDLER_H
#define ANIMATIONS_HANDLER_H
#include "leddefinition.h"


#define LENGTH_OF_CONST_ARRAY(arr) (sizeof(arr)/sizeof(arr[0]))
#define MAKE_BEGIN(arr) (&arr[0])
#define MAKE_END(arr) (&arr[LENGTH_OF_CONST_ARRAY(arr)])
#define MAKE_SPAN(arr) (SequentialAnimationStepSpan(MAKE_BEGIN(arr), MAKE_END(arr)))


enum class AnimationDirection: uint8_t{
    FORWARD,
    BACKWARD,
    BIDIRECTIONAL_FORWARD,
    BIDIRECTIONAL_BACKWARD
};

struct AnimationStep{
    const uint8_t brightness;
    const uint16_t duration;
    
    bool isDelay() const {
        return brightness == 0xFF && duration != 0xFFFF;
    }
};

#define STEP_DELAY(delay_ms) (AnimationStep{.brightness = 0xFF, .duration = (delay_ms)})

class SequentialAnimationStepSpan{
public:
	constexpr SequentialAnimationStepSpan(const AnimationStep* begin, const AnimationStep* end) : m_begin(begin), m_end(end){}

	const AnimationStep* begin() const{
        return m_begin;
    }

	const AnimationStep* end() const{
        return m_end;
    }

    const AnimationStep* rbegin() const {
        return m_end - 1;
    }
	
    const AnimationStep* rend() const {
        return m_begin - 1;
    }
private:
	const AnimationStep* m_begin;
    const AnimationStep* m_end;
};


struct AnimationDef{
	constexpr AnimationDef(LedPosition ledPosition, AnimationDirection direction, const SequentialAnimationStepSpan& stepSpan, uint16_t initialDelay = 0) : ledPosition(ledPosition), direction(direction), stepSpan(stepSpan), initialDelay(initialDelay) {}
	const LedPosition ledPosition;
    const AnimationDirection direction;
	const uint16_t initialDelay;
	const SequentialAnimationStepSpan stepSpan;
	bool isValid() const {
        return !(stepSpan.begin() == nullptr || stepSpan.end() == nullptr || ledPosition == LedPosition::NUM_OF_ALL_LEDS);
    }
};




inline const AnimationDef ANIM_END(LedPosition::NUM_OF_ALL_LEDS, AnimationDirection::FORWARD, SequentialAnimationStepSpan(nullptr, nullptr));


extern void initAnimations();
extern void handleAnimations();
extern void setAnimation(const AnimationDef* newAnimation);
#endif