#ifndef _Event_h
#define _Event_h

#include <Arduino.h>

#include "DebugUtils.h"

#include "FastDelegate.h"
#include <vector>

using namespace fastdelegate;
using namespace std;

template <class TEventArgs> 
class Event
{
	public:
		typedef FastDelegate1<TEventArgs*, void> Handler;

		void addHandler(Handler h)
		{
			handlers.push_back(h);
		};

		void removeHandler(Handler h)
		{ 
			typename vector<Handler>::iterator Iter;
			for (Iter = handlers.begin() ; Iter != handlers.end() ; Iter++ )  
			{   
				if(*Iter==h) 
				{
					handlers.erase(Iter);
					break;
				}
			}
		};
		
		void raise(TEventArgs* args) { 
			if(handlerCount()==0) 
			{
				DEBUG_PRINT("There are: " << handlerCount() << " handlers")
			}

			typename vector<Handler>::iterator Iter;
			for (Iter = handlers.begin() ; Iter != handlers.end() ; Iter++ )  
			{   
				(*Iter)(args);				
			}
		};

		int handlerCount()
		{
			return handlers.size();
		}
		
		private:
			vector<Handler> handlers;
};

#endif

