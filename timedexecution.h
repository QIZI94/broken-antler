#ifndef STATIC_MODULE
#define STATIC_MODULE

#include "statictimer.h"

template<class TimerBase>
class TimedExecution{
private: // functions
static volatile TimedExecution** getTimedExecutionListBegin(){
	volatile static TimedExecution* listBegin = nullptr;
	return &listBegin;
}
public: // types
	using ExecFunPtr = void (*)(TimedExecution&);
	using StaticTimerBase = TimerBase;

	enum class DebugStage: uint8_t {
		BeforeExecution,
		AfterExecution
	};

	struct List{
		class ConstIterator{
		public:
			ConstIterator(const TimedExecution* timedExec){
				timedExecution = timedExec;
			}
			ConstIterator(const ConstIterator& other){
				timedExecution = other.timedExecution;
			}
			//ConstIterator& operator=(const ConstIterator&);
    		ConstIterator& operator++(){
				*this = next();
				return *this;
			}

			ConstIterator& operator--(){
				*this = previous();
				return *this;
			}

			const TimedExecution* operator->() const {return timedExecution;};
			const TimedExecution& operator*() const {return *timedExecution;};
  			bool operator==(const ConstIterator& rhs) const {return timedExecution==rhs.timedExecution;}
  			bool operator!=(const ConstIterator& rhs) const {return timedExecution!=rhs.timedExecution;}
			
			const ConstIterator next() const {
				return ConstIterator(timedExecution->next);
			}

			const ConstIterator previous() const {
				return ConstIterator(timedExecution->prev);
			}

		//private:
			const TimedExecution* timedExecution;
		};
		
		
		List(ConstIterator begin = ConstIterator(*getTimedExecutionListBegin()), ConstIterator end = ConstIterator(nullptr)) : _begin(begin), _end(end) {}
		
		ConstIterator cbegin() const {
			return _begin;
		}
		ConstIterator cend() const {
			return _end;
		}

		private:
		ConstIterator _begin;
		ConstIterator _end;
	};

	using DebugFuncPtr = void (*)(const List&, DebugStage);
	
public: // functions

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
            volatile TimedExecution** begin = getTimedExecutionListBegin();
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
				*getTimedExecutionListBegin() = next;
			}
			
			if(next != nullptr)
				next->prev = prev;
			
			prev = nullptr;
			next = nullptr;
		}
    }

    bool isEnabled() const{
        return (prev != nullptr || next != nullptr || this == *getTimedExecutionListBegin());
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
        TimedExecution* currentTimedExecution = *getTimedExecutionListBegin();
#ifdef DEBUG_TIMED_EXECUTION
		DebugFuncPtr debugFunc = *GetDebugRoutine();
		debugFunc(List(), DebugStage::BeforeExecution);
#endif
        while(currentTimedExecution != nullptr){
           	currentTimedExecution->exec();
            currentTimedExecution = currentTimedExecution->next;
        }
#ifdef DEBUG_TIMED_EXECUTION
		debugFunc(List(), DebugStage::AfterExecution);
#endif
    }
private: // functions
#ifdef DEBUG_TIMED_EXECUTION
	static void noDebugFunc(const List&, DebugStage){}
	static DebugFuncPtr* GetDebugRoutine(){
		static DebugFuncPtr g_debugFuncPtr = &noDebugFunc;
		return &g_debugFuncPtr;
	}
#endif

public: // functions
	static void SetDebugRoutine(DebugFuncPtr debugFuncPtr){
#ifdef DEBUG_TIMED_EXECUTION
		if(debugFuncPtr == nullptr){
			debugFuncPtr = noDebugFunc;
		}
		*GetDebugRoutine() = debugFuncPtr;
#endif
	}

public: // variables
	StaticTimerBase timer;

private: // variables
    //basically a singelthon which will return pointer to a pointer of the beging of managed linked list

	
    volatile TimedExecution* prev = nullptr;
    volatile TimedExecution* next = nullptr;
	volatile ExecFunPtr execPtr = nullptr;
};










#endif