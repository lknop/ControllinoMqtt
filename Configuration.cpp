#include "Configuration.h"
#include <avr/wdt.h>
#include <EEPROM.h>


   
namespace Configuration {
    
    States state = undefinedState;
    
    char PLC_Topic[CONFIG_MAX_LENGTH] = "controllino";
    char root_Topic[CONFIG_MAX_LENGTH] = "home";
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
        Serial.print("Loading configuration...");
        int currentAddress = 0;
        char aux[CONFIG_SIGNATURE_LENGTH+1];
        isValid = true;

        // signature
        readCharArray(currentAddress, aux, CONFIG_SIGNATURE_LENGTH+1);
        if(strcmp(aux,signature)) {
            Serial.print("signature ");
            Serial.print(aux);
            Serial.print(" invalid!!!");
            isValid = false;
        } else {
            Serial.print("found...");
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
       Serial.println("To enter configuration mode press C");
       state = initialState;
    }
       
    void initial(String readedString) {
        if(readedString.equals(String("C"))) {
            setMenuState();
        } else {
            setInitialState();
        }
    }
    
    void setMenuState() {
        isConfiguring = true;
        Serial.println("*** MAIN CONFIGURATION MENU ***");
        Serial.println();
        
        Serial.print("1: Enter MAC => (");
        char formatedMAC[17]; 
        sprintf(formatedMAC, "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        Serial.print(formatedMAC);  Serial.println(")");
        
        Serial.print("2: Enter IP => (");
        char formatedIP[16]; 
        sprintf(formatedIP, "%03d.%03d.%03d.%03d", ip[0], ip[1], ip[2], ip[3]);
        Serial.print(formatedIP);  Serial.println(")");
        
        Serial.print("3: MQTT Server IP => (");
        sprintf(formatedIP, "%03d.%03d.%03d.%03d", server[0], server[1], server[2], server[3]);
        Serial.print(formatedIP);  Serial.println(")");
                                                 
        Serial.print("4: MQTT Port => (");
        Serial.print(port);  
        Serial.println(")");
                                                 
        Serial.print("5: Root Topic => ("); 
        Serial.print(root_Topic);  
        Serial.println(")");
                                                 
        Serial.print("6: PLC Topic => ("); 
        Serial.print(PLC_Topic);  
        Serial.println(")");
                                                 
        Serial.print("7: Command Topic => ("); 
        Serial.print(command_Topic);  
        Serial.println(")");
                                                 
        Serial.print("8: State Topic => ("); 
        Serial.print(state_Topic);  
        Serial.println(")");
                                                 
        Serial.print("9: Log Topic => ("); 
        Serial.print(log_Topic);  
        Serial.println(")");
                                                 
        Serial.println();
        Serial.println("X: Exit");
        Serial.println();
        Serial.println ("Enter option: ");
        state = menuState;
    }
    
    void menu(String readedString){
        if(readedString == "X") {
            setInitialState();
        } else if(readedString == "1") {   
            setMACState();
        } else if(readedString == "2") {   
            setIPState();
        } else if(readedString == "3") {   
            setServerState();
        } else if(readedString == "4") {   
            setPortState();
        } else if(readedString == "5") {   
            setRootTopicState();
        } else if(readedString == "6") {   
            setPLCTopicState();
        } else if(readedString == "7") {   
            setCommandTopicState();
        } else if(readedString == "8") {   
            setStateTopicState();
        } else if(readedString == "9") {   
            setLogTopicState();
        } else if(readedString == "R") {   
              save();
              wdt_enable(WDTO_60MS);
              while(1) {}
        } else {

        }
    }
    
    void setPLCTopicState() {
        Serial.print ("Enter PLC Topic: ");
        state = PLCTopicState;
    }
    
   void PLCTopic(String readedString){
        readedString.toCharArray(PLC_Topic,readedString.length()+1);
        setMenuState();
    }
    
   void setRootTopicState() {
        Serial.print ("Enter root topic: ");
        state = rootTopicState;
   }
 
    void rootTopic(String readedString){
        readedString.toCharArray(root_Topic,readedString.length()+1);
        setMenuState();
    } 

    void setCommandTopicState() {
        Serial.print ("Enter command topic: ");
        state = commandTopicState;
   } 
  
   void commandTopic(String readedString){
        readedString.toCharArray(command_Topic,readedString.length()+1);
        setMenuState();
   } 
        
   void setStateTopicState() {
        Serial.print ("Enter state topic: ");
        state = stateTopicState;
   } 
    
   void stateTopic(String readedString){
        readedString.toCharArray(state_Topic,readedString.length()+1);
        setMenuState();
   } 
     
   void setLogTopicState() {
        Serial.print ("Enter log topic: ");
        state = logTopicState;
   }  
    
   void logTopic(String readedString){
        readedString.toCharArray(log_Topic,readedString.length()+1);
        setMenuState();
   } 
    
   void setIPState() {
        Serial.print ("Enter IP: (nnn.nnn.nnn.nnn)");
        state = IPState;
   }  
 
   void IP(String readedString) {
       int i;
       for(i=0;i<4;i++){
        ip[i] = readedString.substring(i*4,i*4+3).toInt();
       }
        setMenuState();
    }
    
        
   void setServerState() {
        Serial.print ("Enter server IP: (nnn.nnn.nnn.nnn)");
        state = serverState;
   }  
 
   void server_State(String readedString) {
       int i;
       for(i=0;i<4;i++){
        server[i] = readedString.substring(i*4,i*4+3).toInt();
       }
        setMenuState();
    }

    void setPortState() {
        Serial.print ("Enter server port: ");
        state = portState;
   } 
    
   void port_State(String readedString){
       port = readedString.toInt(); 
       setMenuState();
   } 
    
   void setMACState() {
        Serial.print ("Enter MAC: (HH:HH:HH:HH:HH:HH)");
        state = MACState;
   }  
 
   void MAC(String readedString) {
        char aux[3];
        int i;
        for(i=0;i<6;i++) {
            readedString.substring(i*3,i*3+2).toCharArray(aux,3);
            mac[i] = strtol(aux, 0, 16);
        }

        setMenuState();
    } 
    
    void loop(){
        static String readedString("");
        if (Serial.available() > 0) {
            char readed = Serial.read();
            Serial.print(readed);

            if(readed == 13){
                Serial.println();
                switch (state) {
                    case menuState:
                       menu(readedString);
                       break;
                    case initialState:
                       initial(readedString);
                       break;
                    case PLCTopicState:
                       PLCTopic(readedString);
                       break;
                    case MACState:
                       MAC(readedString);
                       break;
                    case IPState:
                       IP(readedString);
                       break;
                    case serverState:
                       server_State(readedString);
                       break;
                    case portState:
                       port_State(readedString);
                       break;
                    case rootTopicState:
                       rootTopic(readedString);
                       break;
                    case commandTopicState:
                       commandTopic(readedString);
                       break;
                    case stateTopicState:
                       stateTopic(readedString);
                       break;
                    case logTopicState:
                       logTopic(readedString);
                        break;
                   default:
                        setInitialState();
                        break;
                }
                
                readedString="";
                Serial.print(">");
            } else {
                 readedString += readed;
            }
            
        }
    }
   
}