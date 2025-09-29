#ifndef STATIC_MODULE
#define STATIC_MODULE

#include "statictimer.h"

template<class TimerBase>
class TimedExecution{
public:
using ExecFunPtr = void (*)(TimedExecution&);
using StaticTimerBase = TimerBase;
public:

	struct List{
		class const_iterator{
		public:
			const_iterator(const TimedExecution* timedExec){
				timedExecution = timedExec;
			}
			const_iterator(const const_iterator& other){
				timedExecution = other.timedExecution;
			}
			//const_iterator& operator=(const const_iterator&);
    		const_iterator& operator++(){
				timedExecution = next();
				return *this;
			}

			const_iterator& operator--(){
				timedExecution = previous();
				return *this;
			}

			const TimedExecution* operator->() const {return timedExecution;};
			const TimedExecution& operator*() const {return *timedExecution;};
  			bool operator==(const const_iterator& rhs) const {return timedExecution==rhs.timedExecution;}
  			bool operator!=(const const_iterator& rhs) const {return timedExecution!=rhs.timedExecution;}
			
			const TimedExecution* next() const {
				return timedExecution->next;
			}

			const TimedExecution* previous() const {
				return timedExecution->prev;
			}

		//private:
			const TimedExecution* timedExecution;
		};
		
		List() = delete;

		static const_iterator begin(){
			return const_iterator(*getTimedExecutionListBegin());
		}
		static const_iterator end(){
			return const_iterator(nullptr);
		}

		static volatile TimedExecution** getTimedExecutionListBegin(){
			volatile static TimedExecution* listBegin = nullptr;
			return &listBegin;
		}
	};

    TimedExecution(){
        // register timer upon creating the instance into doubly linked list
        //enable();
    }
    ~TimedExecution(){
        // unregister timer upon destruction,
        // this more of a memory safety measure if timer is defined on stack and function goes out of scope it will remove it-self
        disable();        
    }
    // register timer instance into doubly linked list
    // enables countdown by tickAllTimers() function
    void enable(){
        if(!isEnabled()){
            volatile TimedExecution** begin = List::getTimedExecutionListBegin();
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
        if(prev != nullptr)
            prev->next = next;
        else{
            *List::getTimedExecutionListBegin() = next;
        }
        
        if(next != nullptr)
            next->prev = prev;
        
        prev = nullptr;
        next = nullptr;
    }

    bool isEnabled() const{
        return (prev != nullptr || next != nullptr || this == *List::getTimedExecutionListBegin());
    }

	void setExecFunction(ExecFunPtr exec){
        execPtr = exec;
    }

	void setup(ExecFunPtr execFunction, uint32_t interval, bool enableTimer = true){
		setExecFunction(execFunction);
		timer.reset(interval);
		if(enableTimer){
			enable();
			timer.enable();
		}
	}

	void restart(uint32_t interval){
		timer.reset(interval);
		enable();
		timer.enable();
	}
    
    void exec(){
        if(execPtr != nullptr){
            if(timer.isDown()){
				disable();
                execPtr(*this);
            }
        }
    }
    
    // this function will be called by timer interrupt and handle registered timers countdowns
    static void executeAllTimedExecutions(){
        TimedExecution* currentTimedExecution = *List::getTimedExecutionListBegin();

        while(currentTimedExecution != nullptr){
           	currentTimedExecution->exec();
            currentTimedExecution = currentTimedExecution->next;
        }
    }
public:
	StaticTimerBase timer;

private:
    //basically a singelthon which will return pointer to a pointer of the beging of managed linked list

	
    volatile TimedExecution* prev = nullptr;
    volatile TimedExecution* next = nullptr;
	volatile ExecFunPtr execPtr = nullptr;
};










#endif