#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <Arduino.h>

#define CONFIG_SIGNATURE "PLC02"
#define CONFIG_SIGNATURE_LENGTH 5
#define CONFIG_MAX_LENGTH 20

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
        logTopicState,
		modbusAddressState,
		modbusCountState,
		usernameState,
		passwordState
    };

    extern char PLC_Topic[CONFIG_MAX_LENGTH];
    extern char root_Topic[CONFIG_MAX_LENGTH];
    extern char command_Topic[CONFIG_MAX_LENGTH];
    extern char state_Topic[CONFIG_MAX_LENGTH];
    extern char log_Topic[CONFIG_MAX_LENGTH];
    extern char username[CONFIG_MAX_LENGTH];
    extern char password[CONFIG_MAX_LENGTH];
    extern byte mac[6];
    extern byte ip[4];
    extern byte server[4];
    extern uint16_t port;
    extern bool isValid;
    extern bool isConfiguring;
    extern char signature[CONFIG_SIGNATURE_LENGTH+1];
    extern byte modbus_address;
    extern byte modbus_count;
    
    void initial(String readString);
    void menu_action(String readString);
    void read_mac(String readString);
    void read_ip(String readString);
    void read_server(String readString);
    void show_menu();
    void show_menu_element(const __FlashStringHelper* label, char* value);
    void show_menu_element(const __FlashStringHelper*, uint16_t value);
    void loop();
    
    void save();
    void save_char_array(int &offset, char* charArray);
    void save_byte_array(int &offset, byte* byteArray, int length);
    void load();
    void read_char_array(int &offset, char* charArray, int maxLength);
    void read_byte_array(int &offset, byte* byteArray, int length);
}

#endif
