#include "Button.h"
#include "Event.h"
#include "EventArgs.h"
#include "FastDelegate.h"
#include "DebugUtils.h"

void Button::initialize(byte pin, byte pushedValue, bool usePullUp, unsigned long debounceTime){
    this->_pin = pin;
    this->_pushedValue = pushedValue;
	this->_click = new Event<EventArgs>;
	this->_up = new Event<EventArgs>;
	this->_down = new Event<EventArgs>;

	byte mode =  usePullUp ? INPUT_PULLUP : INPUT;
    
    pinMode(pin, mode);
	
	this->downRaised = false;
    
    unsigned long period = debounceTime >> 3;
    t = Timer::create(period);
    t->tick()->addHandler(MakeDelegate(this, &Button::onTick));
    t->tick()->raise(nullptr);
    t->enable();

}



Button::Button(){};

bool Button::isPushed(){
	return this->downRaised;
};



Button::Button(byte pin, byte pushedValue, bool usePullUp,  Event<EventArgs>::Handler h, unsigned long debounceTime = 50){
    this->initialize(pin, pushedValue, usePullUp, debounceTime);
	this->_click->addHandler(h);    
   
}

Event<EventArgs>* Button::down(){
	return this->_down;
}

Event<EventArgs>* Button::click(){
	return this->_click;
}

Event<EventArgs>* Button::up(){
	return this->_up;
}
 
void Button::onTick(EventArgs* e){
    byte readValue = digitalRead(_pin);
	byte value = !(readValue ^ this->_pushedValue);
    EventArgs eClick(this);
    if(this->debounceValue==0xFF && value==0) {
        this->debounceValue = 0;
        DEBUG_PRINT("CLICK!!!")
        this->_click->raise(&eClick); 
        DEBUG_PRINT("UP!!!")
        this->_up->raise(&eClick); 	
		this->downRaised = false;
	} else {
		this->debounceValue = (this->debounceValue << 1) | value; 
        if(!downRaised && this->debounceValue==0xFF) {
            DEBUG_PRINT("DOWN!!!")
        	this->_down->raise(&eClick); 
			this->downRaised = true;
		};
    } 
}

byte Button::pin(){
    return this->_pin;
}
