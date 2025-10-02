#ifndef STATIC_TIMER
#define STATIC_TIMER

#include <inttypes.h>

template<uint32_t TICK_VALUE_MS = 1>
class StaticTimer{

public:
	static constexpr uint32_t TICK_VALUE = TICK_VALUE_MS;
public:
    StaticTimer(){
        // register timer upon creating the instance into doubly linked list
        enable();
        
    }
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

    void reset(uint32_t interval){ // in ms
		//enable();
        countdown = interval/TICK_VALUE;
    }

	bool getCurrentCountDown() const {
		return countdown;
	}

    // checks if time set by reset(us) has elapsed since 
    volatile bool isDown() const{
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

private:
    //basically a singelthon which will return pointer to a pointer of the beging of managed linked list
    static volatile StaticTimer** getTimersListBegin(){
        volatile static StaticTimer* begin = nullptr;
        return &begin;
    }

    // one tick -> decrease of countdown, amount is defined by countAmount(and will be specific for hardware timer used)
    void tick(){
		volatile StaticTimer* currentTimer = this;
        if(!currentTimer->isDown()){
            currentTimer->countdown -= 1;
        }
    }
    volatile uint32_t countdown = 0;
    volatile StaticTimer* prev = nullptr;
    volatile StaticTimer* next = nullptr;
};

//example code usage with interrupt
//void TIMER2_ISR(){
//    StaticTimer::tickAllTimers();
//}


#endif