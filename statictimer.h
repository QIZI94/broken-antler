#ifndef STATIC_TIMER
#define STATIC_TIMER

#include <inttypes.h>

template<uint32_t TICK_VALUE_AMOUNT = 1>
class StaticTimer{

public:
	static constexpr uint32_t TICK_VALUE = TICK_VALUE_AMOUNT;
public:
    ~StaticTimer(){
        // unregister timer upon destruction,
        // this more of a memory safety measure if timer is defined on stack and function goes out of scope it will remove it-self
        disable();        
    }
    // register timer instance into doubly linked list
    // enables countdown by tickAllTimers() function
    void enable(){
        if(!isEnabled()){
            volatile StaticTimer** begin = getTimersListBegin();
            if(*begin == nullptr){
                *begin = this;
            }
            else{
                (*begin)->prev = this;
                this->next = *begin;
                *begin = this;
            }
        }
    }

    void disable(){
		if(isEnabled()){
			if(prev != nullptr)
				prev->next = next;
			else{
				*getTimersListBegin() = next;
			}
			
			if(next != nullptr)
				next->prev = prev;
			
			prev = nullptr;
			next = nullptr;
		}
    }

    void reset(uint32_t interval){
        countdown = interval/TICK_VALUE;
    }

	void restart(uint32_t interval){
		reset();
		enable();
	}

	uint32_t getCurrentCountDown() const {
		return countdown;
	}
	
	uint32_t getRemainingTime() const {
		return countdown*TICK_VALUE;
	}

    // checks if time set by reset(us) has elapsed since 
    bool isDown() const{
        return (countdown == 0);
    }

    bool isEnabled() const{
        return (prev != nullptr || next != nullptr || this == *getTimersListBegin());
    }
    
    // this function will be called by timer interrupt and handle registered timers countdowns
    static void tickAllTimers(){
        volatile StaticTimer* currentTimer = *getTimersListBegin();

        while(currentTimer != nullptr){
            currentTimer->tick();
            currentTimer = currentTimer->next;
        }
    }

	static uint32_t TimersCount(){
		uint32_t count = 0;

		volatile StaticTimer* currentTimer = *getTimersListBegin();

        while(currentTimer != nullptr){
            ++count;
            currentTimer = currentTimer->next;
        }
		return count;
	}

private: // static functions
    //basically a singelthon which will return pointer to a pointer of the beging of managed linked list
    static volatile StaticTimer** getTimersListBegin(){
        volatile static StaticTimer* begin = nullptr;
        return &begin;
    }

    // one tick -> decrease of countdown, amount is defined by countAmount(and will be specific for hardware timer used)
    void tick(){
        if(!isDown()){
            countdown -= 1;
        }
		else{
			disable();
		}
    }
private: // variables
    volatile uint32_t countdown = 0;
    volatile StaticTimer* prev = nullptr;
    volatile StaticTimer* next = nullptr;
};

//example code usage with interrupt
//void TIMER2_ISR(){
//    StaticTimer::tickAllTimers();
//}


#endif