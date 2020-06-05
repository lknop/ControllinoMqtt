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
    byte mac[6]  = {0x1A, 0x6B, 0xE7, 0x45, 0xB9, 0x26}; 
    byte ip[4] = {192, 168, 1, 20}; 
    byte server[4] = {192, 168, 1, 10}; 
    int port = 1883;
    bool isValid = false;
    bool isConfiguring = false;
    char signature[CONFIG_SIGNATURE_LENGTH+1] = CONFIG_SIGNATURE;

    
    void save() {
        int currentAddress = 0;
        saveCharArray(currentAddress, signature);
        saveCharArray(currentAddress, PLC_Topic);
        saveCharArray(currentAddress, root_Topic);
        saveCharArray(currentAddress, command_Topic);
        saveCharArray(currentAddress, log_Topic);
        saveByteArray(currentAddress, mac, 6);
        saveByteArray(currentAddress, ip, 4);
        saveByteArray(currentAddress, server, 4);
        EEPROM.put(currentAddress, port);
    }
   
    void saveCharArray(int &offset, char* charArray){
        int i = 0;
        bool finish = false;
        while(!finish) {
            Serial.println(charArray[i],DEC);
            EEPROM.update(offset+i,charArray[i]);
            if(!charArray[i]) {
                finish = true;
            }
            i++;
        }
        
        offset +=i;
    }
    
    
    void saveByteArray(int &offset,byte* byteArray, int length){
        int i = 0;
        for(i=0;i<length;i++){
             EEPROM.update(offset+i,byteArray[i]);
        }
        offset +=i;
    }
    
    void readCharArray(int &offset, char* charArray, int maxLength){
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
    
    void readByteArray(int &offset,byte* byteArray, int length){
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
        readCharArray(currentAddress, aux, CONFIG_SIGNATURE_LENGTH+1);
        if(strcmp(aux,signature)) {
            Serial.print(F("signature "));
            Serial.print(aux);
            Serial.print(F(" invalid!!!"));
            isValid = false;
        } else {
            Serial.print(F("found..."));
            readCharArray(currentAddress, PLC_Topic, CONFIG_MAX_LENGTH);
            readCharArray(currentAddress, root_Topic, CONFIG_MAX_LENGTH);
            readCharArray(currentAddress, command_Topic, CONFIG_MAX_LENGTH);
            readCharArray(currentAddress, log_Topic, CONFIG_MAX_LENGTH);
            readByteArray(currentAddress, mac, 6);
            readByteArray(currentAddress, ip, 4);
            readByteArray(currentAddress, server, 4);
            EEPROM.get(currentAddress, port);
            Serial.println("OK");
        }
   }

    void setInitialState() {
       isConfiguring = false;
       Serial.println(F("To enter configuration mode press C"));
       state = initialState;
    }
       
    void initial(String readString) {
        if(readString.equals(String("C"))) {
            setMenuState();
        } else {
            setInitialState();
        }
    }
    
    void setMenuState() {
        isConfiguring = true;
        Serial.println(F("*** MAIN CONFIGURATION MENU ***"));
        Serial.println();
        
        Serial.print(F("1: Enter MAC => ("));
        char formattedMAC[17]; 
        sprintf(formattedMAC, "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        Serial.print(formattedMAC);  Serial.println(")");
        
        Serial.print(F("2: Enter IP => ("));
        char formattedIP[16]; 
        sprintf(formattedIP, "%03d.%03d.%03d.%03d", ip[0], ip[1], ip[2], ip[3]);
        Serial.print(formattedIP);  Serial.println(")");
        
        Serial.print(F("3: MQTT Server IP => ("));
        sprintf(formattedIP, "%03d.%03d.%03d.%03d", server[0], server[1], server[2], server[3]);
        Serial.print(formattedIP);  Serial.println(F(")"));
                                                 
        Serial.print(F("4: MQTT Port => ("));
        Serial.print(port);  
        Serial.println(F(")"));
                                                 
        Serial.print(F("5: Root Topic => ("));
        Serial.print(root_Topic);  
        Serial.println(F(")"));
                                                 
        Serial.print(F("6: PLC Topic => ("));
        Serial.print(PLC_Topic);  
        Serial.println(F(")"));
                                                 
        Serial.print(F("7: Command Topic => ("));
        Serial.print(command_Topic);  
        Serial.println(F(")"));
                                                 
        Serial.print(F("8: State Topic => ("));
        Serial.print(state_Topic);  
        Serial.println(F(")"));
                                                 
        Serial.print(F("9: Log Topic => ("));
        Serial.print(log_Topic);  
        Serial.println(F(")"));
                                                 
        Serial.println();
        Serial.println(F("X: Exit"));
        Serial.println();
        Serial.println (F("Enter option: "));
        state = menuState;
    }
    
    void menu(String readString){
        if(readString == "X") {
            setInitialState();
        } else if(readString == "1") {
            setMACState();
        } else if(readString == "2") {
            setIPState();
        } else if(readString == "3") {
            setServerState();
        } else if(readString == "4") {
            setPortState();
        } else if(readString == "5") {
            setRootTopicState();
        } else if(readString == "6") {
            setPLCTopicState();
        } else if(readString == "7") {
            setCommandTopicState();
        } else if(readString == "8") {
            setStateTopicState();
        } else if(readString == "9") {
            setLogTopicState();
        } else if(readString == "R") {
              save();
              wdt_enable(WDTO_60MS);
              while(1) {}
        } else {

        }
    }
    
    void setPLCTopicState() {
        Serial.print (F("Enter PLC Topic: "));
        state = PLCTopicState;
    }
    
   void PLCTopic(String readString){
        readString.toCharArray(PLC_Topic,readString.length()+1);
        setMenuState();
    }
    
   void setRootTopicState() {
        Serial.print (F("Enter root topic: "));
        state = rootTopicState;
   }
 
    void rootTopic(String readedString){
        readedString.toCharArray(root_Topic,readedString.length()+1);
        setMenuState();
    } 

    void setCommandTopicState() {
        Serial.print (F("Enter command topic: "));
        state = commandTopicState;
   } 
  
   void commandTopic(String readString){
        readString.toCharArray(command_Topic,readString.length()+1);
        setMenuState();
   } 
        
   void setStateTopicState() {
        Serial.print (F("Enter state topic: "));
        state = stateTopicState;
   } 
    
   void stateTopic(String readedString){
        readedString.toCharArray(state_Topic,readedString.length()+1);
        setMenuState();
   } 
     
   void setLogTopicState() {
        Serial.print (F("Enter log topic: "));
        state = logTopicState;
   }  
    
   void logTopic(String readString){
        readString.toCharArray(log_Topic,readString.length()+1);
        setMenuState();
   } 
    
   void setIPState() {
        Serial.print (F("Enter IP: (nnn.nnn.nnn.nnn)"));
        state = IPState;
   }  
 
   void IP(String readString) {
       int i;
       for(i=0;i<4;i++){
        ip[i] = readString.substring(i*4,i*4+3).toInt();
       }
        setMenuState();
    }
    
        
   void setServerState() {
        Serial.print (F("Enter server IP: (nnn.nnn.nnn.nnn)"));
        state = serverState;
   }  
 
   void server_State(String readString) {
       int i;
       for(i=0;i<4;i++){
        server[i] = readString.substring(i*4,i*4+3).toInt();
       }
        setMenuState();
    }

    void setPortState() {
        Serial.print (F("Enter server port: "));
        state = portState;
   } 
    
   void port_State(String readString){
       port = readString.toInt();
       setMenuState();
   } 
    
   void setMACState() {
        Serial.print (F("Enter MAC: (HH:HH:HH:HH:HH:HH)"));
        state = MACState;
   }  
 
   void MAC(String readString) {
        char aux[3];
        int i;
        for(i=0;i<6;i++) {
            readString.substring(i*3,i*3+2).toCharArray(aux,3);
            mac[i] = strtol(aux, 0, 16);
        }

        setMenuState();
    } 
    
    void loop(){
        static String readString("");
        if (Serial.available() > 0) {
            char read = Serial.read();
            Serial.print(read);

            if(read == 13){
                Serial.println();
                switch (state) {
                    case menuState:
                       menu(readString);
                       break;
                    case initialState:
                       initial(readString);
                       break;
                    case PLCTopicState:
                       PLCTopic(readString);
                       break;
                    case MACState:
                       MAC(readString);
                       break;
                    case IPState:
                       IP(readString);
                       break;
                    case serverState:
                       server_State(readString);
                       break;
                    case portState:
                       port_State(readString);
                       break;
                    case rootTopicState:
                       rootTopic(readString);
                       break;
                    case commandTopicState:
                       commandTopic(readString);
                       break;
                    case stateTopicState:
                       stateTopic(readString);
                       break;
                    case logTopicState:
                       logTopic(readString);
                        break;
                   default:
                        setInitialState();
                        break;
                }
                
                readString="";
                Serial.print(">");
            } else {
                 readString += read;
            }
            
        }
    }
   
}
