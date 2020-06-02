#ifndef _EventArgs_h
#define _EventArgs_h

class EventArgs
{
public:
    void* sender;     
    EventArgs(void* sender) : sender(sender){} ;
    void* getSender() {
        return sender;
    };

};

#endif