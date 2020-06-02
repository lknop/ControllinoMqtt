#include "Timer.h"
#include <Arduino.h>
#include "Event.h"
#include "EventArgs.h"
#include "DebugUtils.h"

Timer* Timer::timers = nullptr;

Timer* Timer::create(unsigned long period) {
    Timer* newTimer = new Timer(period);
    
    newTimer->_period = period;
    
    if (!Timer::timers) {
        Timer::timers = newTimer;
	} else {
        Timer* currentTimer = Timer::timers;
        while(currentTimer->nextTimer) {
            currentTimer = currentTimer->nextTimer;
        }
        
        currentTimer->nextTimer = newTimer;
		newTimer->previousTimer = currentTimer;
    }
    
    
    return newTimer;
}

void Timer::loop(){
	unsigned long now = millis();

	int i = 0;
    Timer* currentTimer = Timer::timers;
    while(currentTimer) {
		i++;
        if(currentTimer->isEnabled && (now - currentTimer->startTime)>=currentTimer->_period) {
			currentTimer->onTick();
		}
        currentTimer = currentTimer->nextTimer;
    }
}

Event<EventArgs>* Timer::tick(){
	return &(this->_tick);
}

 
void Timer::enable(){
    this->startTime = millis();
    this->isEnabled = true;
}


void Timer::disable(){
    this->isEnabled = false;
}

void Timer::destroy(){

    if (Timer::timers == this) {
        Timer::timers = this->nextTimer;
	} else {
    	bool destroy = false;
        Timer* currentTimer = Timer::timers;
        while(!destroy && currentTimer->nextTimer) {
            if(currentTimer == this) {
               this->previousTimer->nextTimer = this->nextTimer;
				destroy = true;
            } else {
                currentTimer = currentTimer->nextTimer;      
            }
        }
    }
    
    delete this;

};


Timer::Timer(unsigned long period) {
    this->isEnabled = false;
    this->_period = period;
	this->nextTimer=nullptr;
}

void Timer::onTick(){ 
	unsigned long i = 0;
	startTime = millis();
    EventArgs e(this);
	this->_tick.raise(&e);
}
