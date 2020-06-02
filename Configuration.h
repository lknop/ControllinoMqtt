#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>

#define CONFIG_SIGNATURE "PLC00"
#define CONFIG_SIGNATURE_LENGTH 5
#define CONFIG_MAX_LENGTH 100

namespace Configuration {
    enum States {
        undefinedState, 
        initialState, 
        menuState, 
        PLCTopicState,
        MACState,
        IPState,
        serverState,
        portState,
        rootTopicState,
        commandTopicState,
        stateTopicState,
        logTopicState
    };

    extern char PLC_Topic[CONFIG_MAX_LENGTH];
    extern char root_Topic[CONFIG_MAX_LENGTH];
    extern char command_Topic[CONFIG_MAX_LENGTH];
    extern char state_Topic[CONFIG_MAX_LENGTH];
    extern char log_Topic[CONFIG_MAX_LENGTH];
    extern byte mac[6];
    extern byte ip[4];
    extern byte server[4];
    extern int port;
    extern bool isValid;
    extern bool isConfiguring;
    extern char signature[CONFIG_SIGNATURE_LENGTH+1];
    
    void initial(String readedString);
    void menu(String readedString);
    void PLCTopic(String readedString);
    void MAC(String readedString);
    void IP(String readedString);
    void server_State(String readedString);
    void port_State(String readedString);
    void rootTopic(String readedString);
    void commandTopic(String readedString);
    void stateTopic(String readedString);
    void logTopic(String readedString);
    void setMenuState();
    void setInitialState();
    void setPLCTopicState();
    void setMACState();
    void setIPState();
    void setServerState();
    void setPortState();
    void setRootTopicState();
    void setCommandTopicState();
    void setStateTopicState();
    void setLogTopicState();
    void loop();
    
    void save();
    void saveCharArray(int &offset, char* charArray);
    void saveByteArray(int &offset, byte* byteArray, int length);
    void load();
    void readCharArray(int &offset, char* charArray, int maxLength);
    void saveByteArray(int &offset, byte* byteArrayv, int length);
}

#endif