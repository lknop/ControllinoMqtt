#include "Configuration.h"
#include <avr/wdt.h>
#include <EEPROM.h>

   
namespace Configuration {
    
    States state = undefinedState;
    
    char PLC_Topic[CONFIG_MAX_LENGTH] = "controllino";
    char root_Topic[CONFIG_MAX_LENGTH] = "iot";
    char command_Topic[CONFIG_MAX_LENGTH] = "command";
    char state_Topic[CONFIG_MAX_LENGTH] = "state";
    char log_Topic[CONFIG_MAX_LENGTH]= "log";
    char username[CONFIG_MAX_LENGTH]= "mqttuser";
    char password[CONFIG_MAX_LENGTH]= "mqttpass";
    byte mac[6]  = {0x1A, 0x6B, 0xE7, 0x45, 0xB9, 0x26}; 
    byte ip[4] = {192, 168, 1, 20}; 
    byte server[4] = {192, 168, 1, 10}; 
    uint16_t port = 1883;
    bool isValid = false;
    bool isConfiguring = false;
    char signature[CONFIG_SIGNATURE_LENGTH+1] = CONFIG_SIGNATURE;
    byte modbus_address = 1;
    byte modbus_count = 0;

    
    void save() {
        int currentAddress = 0;
        save_char_array(currentAddress, signature);
        save_char_array(currentAddress, PLC_Topic);
        save_char_array(currentAddress, root_Topic);
        save_char_array(currentAddress, command_Topic);
        save_char_array(currentAddress, log_Topic);
        save_char_array(currentAddress, username);
        save_char_array(currentAddress, password);
        save_byte_array(currentAddress, mac, 6);
        save_byte_array(currentAddress, ip, 4);
        save_byte_array(currentAddress, server, 4);
        EEPROM.update(currentAddress++, modbus_address);
        EEPROM.update(currentAddress++, modbus_count);
        EEPROM.update(currentAddress++, port & 0xff);
        EEPROM.update(currentAddress++, port >> 8);
    }
   
    void save_char_array(int &offset, char* charArray){
        int i = 0;
        bool finish = false;
        while(!finish) {
            EEPROM.update(offset+i,charArray[i]);
            if(!charArray[i]) {
                finish = true;
            }
            i++;
        }
        offset +=i;
    }
    
    
    void save_byte_array(int &offset,byte* byteArray, int length){
        int i = 0;
        for(i=0;i<length;i++){
             EEPROM.update(offset+i,byteArray[i]);
        }
        offset +=i;
    }

    void read_char_array(int &offset, char* charArray, int maxLength){
        String aux("");
        int i = 0;
        bool finish = false;
        while(!finish) {
            char signatureChar = 0;
            EEPROM.get(offset+i, signatureChar);
            if(!signatureChar || i >= (maxLength-1)) {
                aux.toCharArray(charArray,i+1);
                finish = true;
            } else {
                aux += signatureChar;
            }
            i++;
        }
        
        offset +=i;
    }
    
    void read_byte_array(int &offset,byte* byteArray, int length){
        int i = 0;
        for(i=0;i<length;i++){
             EEPROM.get(offset+i,byteArray[i]);
        }
        offset +=i;
    }
    
    void load() {
        Serial.print(F("Loading configuration..."));
        int currentAddress = 0;
        char aux[CONFIG_SIGNATURE_LENGTH+1];
        isValid = true;

        // signature
        read_char_array(currentAddress, aux, CONFIG_SIGNATURE_LENGTH+1);
        if(strcmp(aux,signature)) {
            Serial.print(F("signature "));
            Serial.print(aux);
            Serial.print(F(" invalid!!!"));
            isValid = false;
        } else {
            Serial.print(F("found..."));
            read_char_array(currentAddress, PLC_Topic, CONFIG_MAX_LENGTH);
            read_char_array(currentAddress, root_Topic, CONFIG_MAX_LENGTH);
            read_char_array(currentAddress, command_Topic, CONFIG_MAX_LENGTH);
            read_char_array(currentAddress, log_Topic, CONFIG_MAX_LENGTH);
            read_char_array(currentAddress, username, CONFIG_MAX_LENGTH);
            read_char_array(currentAddress, password, CONFIG_MAX_LENGTH);
            read_byte_array(currentAddress, mac, 6);
            read_byte_array(currentAddress, ip, 4);
            read_byte_array(currentAddress, server, 4);
            EEPROM.get(currentAddress++, modbus_address);
            EEPROM.get(currentAddress++, modbus_count);
            EEPROM.get(currentAddress++, port);
            Serial.println("OK");
        }
   }

    void initial_state() {
       isConfiguring = false;
       Serial.println(F("To enter configuration mode press C"));
       state = initialState;
    }
       
    void initial(String readString) {
        if(readString.equals(String("C"))) {
            show_menu();
        } else {
            initial_state();
        }
    }

    void show_menu_element(const __FlashStringHelper* label, char* value) {
        Serial.print(label);
        Serial.print(value);
        Serial.println(")");
    }

    void show_menu_element(const __FlashStringHelper* label, uint16_t value) {
        Serial.print(label);
        Serial.print(value);
        Serial.println(")");
    }

    void show_menu() {
        isConfiguring = true;
        Serial.println(F("*** MAIN CONFIGURATION MENU ***"));
        Serial.println();
        
        char formattedMAC[17]; 
        sprintf(formattedMAC, "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        show_menu_element(F("1: Enter MAC => ("), formattedMAC);
        
        char formattedIP[16]; 
        sprintf(formattedIP, "%03d.%03d.%03d.%03d", ip[0], ip[1], ip[2], ip[3]);
        show_menu_element(F("2: Enter IP => ("), formattedIP);
        sprintf(formattedIP, "%03d.%03d.%03d.%03d", server[0], server[1], server[2], server[3]);
        show_menu_element(F("3: MQTT Server IP => ("), formattedIP);
        show_menu_element(F("4: MQTT Port => ("), port);
        show_menu_element(F("5: Root Topic => ("), root_Topic);
        show_menu_element(F("6: PLC Topic => ("), PLC_Topic);
        show_menu_element(F("7: Command Topic => ("), command_Topic);
        show_menu_element(F("8: State Topic => ("), state_Topic);
        show_menu_element(F("9: Log Topic => ("), log_Topic);
        show_menu_element(F("10: Modbus address => ("), modbus_address);
        show_menu_element(F("11: Modbus unit count => ("), modbus_count);
        show_menu_element(F("12: MQTT username => ("), username);
        show_menu_element(F("13: MQTT password => ("), password);
        Serial.println();
        Serial.println(F("X: Discard and Exit"));
        Serial.println();
        Serial.println(F("R: Save and Reboot"));
        Serial.println();
        Serial.println (F("Enter option: "));
        state = menuState;
    }
    
    void menu_action(String readString){
        if(readString == "X") {
            initial_state();
        } else if(readString == "1") {
            Serial.print (F("Enter MAC: (HH:HH:HH:HH:HH:HH)"));
            state = MACState;
        } else if(readString == "2") {
            Serial.print (F("Enter IP: (nnn.nnn.nnn.nnn)\n(or 000.000.000.000 for DHCP)"));
            state = IPState;
        } else if(readString == "3") {
            Serial.print (F("Enter server IP: (nnn.nnn.nnn.nnn)"));
            state = serverState;
        } else if(readString == "4") {
            Serial.print (F("Enter server port: "));
            state = portState;
        } else if(readString == "5") {
            Serial.print (F("Enter root topic: "));
            state = rootTopicState;
        } else if(readString == "6") {
            Serial.print (F("Enter PLC Topic: "));
            state = PLCTopicState;
        } else if(readString == "7") {
            Serial.print (F("Enter command topic: "));
            state = commandTopicState;
        } else if(readString == "8") {
            Serial.print (F("Enter state topic: "));
            state = stateTopicState;
        } else if(readString == "9") {
            Serial.print (F("Enter log topic: "));
            state = logTopicState;
        } else if(readString == "10") {
            Serial.print (F("Enter starting modbus address: "));
            state = modbusAddressState;
        } else if(readString == "11") {
            Serial.print (F("Enter modbus unit count: "));
            state = modbusCountState;
        } else if(readString == "12") {
            Serial.print (F("Enter mqtt username (empty for no auth): "));
            state = usernameState;
        } else if(readString == "13") {
            Serial.print (F("Enter mqtt password: "));
            state = passwordState;
        } else if(readString == "R") {
            save();
            Serial.println(F("Saved"));
            wdt_enable(WDTO_60MS);
            while(1) {}
        }
    }

     
   void read_ip(String readString) {
       int i;
       for (i = 0; i < 4; i++) {
           ip[i] = readString.substring(i * 4, i * 4 + 3).toInt();
       }
       show_menu();
   }
 
   void read_server(String readString) {
       int i;
       for(i=0;i<4;i++){
        server[i] = readString.substring(i*4,i*4+3).toInt();
       }
       show_menu();
    }
 
   void read_mac(String readString) {
        char aux[3];
        int i;
        for(i=0;i<6;i++) {
            readString.substring(i*3,i*3+2).toCharArray(aux,3);
            mac[i] = strtol(aux, 0, 16);
        }
        show_menu();
    } 
    
    void loop() {
        static String readString("");
        if (Serial.available() > 0) {
            char read = Serial.read();
            Serial.print(read);

            if (read == 13) {
                Serial.println();
                switch (state) {
                case menuState:
                    menu_action(readString);
                    break;
                case initialState:
                    initial(readString);
                    break;
                case PLCTopicState:
                    readString.toCharArray(PLC_Topic, readString.length() + 1);
                    show_menu();
                    break;
                case MACState:
                    read_mac(readString);
                    break;
                case IPState:
                    read_ip(readString);
                    break;
                case serverState:
                    read_server(readString);
                    break;
                case portState:
                    port = readString.toInt();
                    show_menu();
                    break;
                case rootTopicState:
                    readString.toCharArray(root_Topic, readString.length() + 1);
                    show_menu();
                    break;
                case commandTopicState:
                    readString.toCharArray(command_Topic, readString.length() + 1);
                    show_menu();
                    break;
                case stateTopicState:
                    readString.toCharArray(state_Topic, readString.length() + 1);
                    show_menu();
                    break;
                case logTopicState:
                    readString.toCharArray(log_Topic, readString.length() + 1);
                    show_menu();
                    break;
                case modbusAddressState:
                    modbus_address = readString.toInt();
                    show_menu();
                    break;
                case modbusCountState:
                    modbus_count = readString.toInt();
                    show_menu();
                    break;
                case usernameState:
                    readString.toCharArray(username, readString.length() + 1);
                    show_menu();
                    break;
                case passwordState:
                    readString.toCharArray(password, readString.length() + 1);
                    show_menu();
                    break;
                default:
                    initial_state();
                    break;
                }

                readString = "";
                Serial.print(">");
            } else {
                readString += read;
            }

        }
    }
}
