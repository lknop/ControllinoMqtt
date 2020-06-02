#ifndef Timer_h
#define Timer_h

#include <Arduino.h>
#include "Event.h"
#include "EventArgs.h"
 
class Timer {
    public:
        static Timer* create(unsigned long period);
        static void loop();
        Event<EventArgs>* tick();
        void enable();
        void disable();
        void destroy();

    
    private:
        Timer(unsigned long period);
        bool isEnabled;
        unsigned long _period;
        unsigned long startTime;
        void onTick();
        Event<EventArgs> _tick;
        Timer* nextTimer;
        Timer* previousTimer;
        static Timer* timers;
};

#endif
