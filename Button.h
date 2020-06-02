#ifndef Button_h
#define Button_h

#include <Arduino.h>
#include "Event.h"
#include "EventArgs.h"
#include "Timer.h"

class Button {
    private:
        byte _pin;
        byte _pushedValue;
        bool downRaised;
        Event<EventArgs>* _click;  
        Event<EventArgs>* _down;  
        Event<EventArgs>* _up;  
        Timer* t;
        void onTick(EventArgs* e);
        byte debounceValue;
        void initialize(byte pin, byte pushedValue, bool usePullUp, unsigned long debounceTime);
    
    public:
        Button();
        Button(byte pin, byte pushedValue, bool usePullUp, Event<EventArgs>::Handler h, unsigned long debounceTime = 50);
        Event<EventArgs>* click();
        Event<EventArgs>* down();
        Event<EventArgs>* up();
        bool isPushed();
        byte pin();
};

#endif
