#include <Arduino.h>
#include <Controllino.h>
#undef SPI_HAS_EXTENDED_CS_PIN_HANDLING

#include <SPI.h>
#include <PubSubClient.h>
#include "Button.h"
#include "Timer.h"
#include "FastDelegate.h"
#include "DebugUtils.h"
#include <vector>
#include "Configuration.h"
#include "PLC.h"


#define INVALID_VALUE -99

using namespace std;



EthernetClient PLC::ethClient;
PubSubClient PLC::mqttClient(ethClient);
vector<Input*> PLC::inputs;
long PLC::millisLastAttempt = 0;

void PLC::setup() {
#ifdef CONTROLLINO_MEGA
	DEBUG_PRINT("CONTROLLINO_MEGA definido");	
#endif


	DEBUG_PRINT("Inicializando PLC");

	Configuration::load();	

	if(Configuration::isValid) {
		PLC::initializeMQTT();
		PLC::initializeEthernet();
		PLC::initializeInputs();
		DEBUG_PRINT("Inicializacion OK");
	} else {
		INFO_PRINT("PLC sin configurar");
	}
	DEBUG_PRINT("PLC inicializado");

}
 
void PLC::loop() {
    DEBUG_PRINT("loop start");
	
	Configuration::loop();
	
	if(Configuration::isValid && !Configuration::isConfiguring) {
		bool connected  = true;

		if (!mqttClient.connected() && (millisLastAttempt ==0 || ((millis() - millisLastAttempt) >= 2000))) {
			millisLastAttempt = millis();
			connected = reconnect();
		}

		if (connected) {
			mqttClient.loop();
		}  else {
			INFO_PRINT("Reintentando dentro de cinco segun");
		}
		Timer::loop();
	}
	
    DEBUG_PRINT("loop end");
}

void PLC::initializeMQTT() {
  INFO_PRINT("Inicializando cliente MQTT...");
  IPAddress server(Configuration::server[0], Configuration::server[1], Configuration::server[2], Configuration::server[3]);
  PLC::mqttClient.setServer(server, Configuration::port);
  PLC::mqttClient.setCallback(PLC::onMQTTMessage);
}

void PLC::initializeEthernet() {
  INFO_PRINT("Inicializando ethernet...");
  IPAddress ip(Configuration::ip[0], Configuration::ip[1], Configuration::ip[2], Configuration::ip[3]);
  Ethernet.begin(Configuration::mac,ip);
  
  DEBUG_PRINT("Inicializado")
  // Allow the hardware to sort itself out
  delay(1500);

    Serial.println(Ethernet.localIP());
	
}

void PLC::initializeInputs() {  
  	INFO_PRINT("Inicializando entradas...");
	const char * names[19] = {"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","A10","A11","A12","A13","A14","A15","I16","I17","I18"};
	byte pins[19] = {
		CONTROLLINO_A0,  CONTROLLINO_A1,  CONTROLLINO_A2,
		CONTROLLINO_A3,	 CONTROLLINO_A4,  CONTROLLINO_A5,
		CONTROLLINO_A6,	 CONTROLLINO_A7,  CONTROLLINO_A8,
		CONTROLLINO_A9,	 CONTROLLINO_A10, CONTROLLINO_A11,
		CONTROLLINO_A12, CONTROLLINO_A13, CONTROLLINO_A14,
		CONTROLLINO_A15, CONTROLLINO_I16, CONTROLLINO_I17,
		CONTROLLINO_I18
	};
	
	for(int i=0;i<=18;i++) {
		DEBUG_PRINT("Entrada  " << i << " pin " << pins[i])
		Button *button = new Button(pins[i] , HIGH, false, &PLC::onButtonClick, 10);
		DEBUG_PRINT("Handlers  " << i)
		button->down()->addHandler(&PLC::onButtonDown);
		button->up()->addHandler(&PLC::onButtonUp);
		DEBUG_PRINT("Creo entrada  " << i)
		Input *input = new Input(names[i], button);
		DEBUG_PRINT("La pongo en la lista  " << i)
		PLC::inputs.push_back(input);
		DEBUG_PRINT("Inicializada  " << i)
 }
	DEBUG_PRINT("Inicializadas")

	INFO_PRINT("Entradas inicializadas: ");
 }

bool PLC::reconnect() {
    bool res;
    // Loop until we're reconnected
    INFO_PRINT("Conectando con servidor MQTT...");
    // Attempt to connect
    if (mqttClient.connect(Configuration::PLC_Topic)) {
      // Once connected, publish an announcement...
        log("Conectado!");
        // ... and resubscribe
		int topicLength = strlen(Configuration::root_Topic) +  strlen(Configuration::PLC_Topic)+4;
		char subscribe_Topic[topicLength]; 
        sprintf(subscribe_Topic, "%s/%s/#", Configuration::root_Topic, Configuration::PLC_Topic);
        mqttClient.subscribe(subscribe_Topic);
        INFO_PRINT("Suscrito a: ");
        INFO_PRINT(subscribe_Topic); 
        res = true;
    } else {
        INFO_PRINT("Error!, rc=");
        INFO_PRINT(mqttClient.state());
        //INFO_PRINT(" reintentando en 5 segundos");
        // Wait 5 seconds before retrying
        //delay(5000);
        res = false;
    }
    return res;
}

void PLC::log(const char* errorMsg)
{
    INFO_PRINT(errorMsg);
    if (mqttClient.connected()) {
		int topicLength = strlen(Configuration::root_Topic) +  strlen(Configuration::log_Topic)+ strlen(Configuration::PLC_Topic)+3;
		char log_Topic[topicLength]; 
        sprintf(log_Topic, "%s/%s/%s", Configuration::root_Topic, Configuration::log_Topic, Configuration::PLC_Topic);
        mqttClient.publish(log_Topic, errorMsg);
    }
}

void PLC::onMQTTMessage(char* topic, byte* payload, unsigned int length) {
    DEBUG_PRINT("Message arrived [" << topic << "] ");
    
    for (unsigned int i=0;i<length;i++) {
        DEBUG_PRINT((char)payload[i]);
    }
    

    char command[10];
    if (getOuput(topic, command)) {
        DEBUG_PRINT("Comando: " << command);
        
        int newState = getValue(payload, length);
        
        if(command[0]=='R') {
            updateRelay(command, newState);
        }

  } else {
      DEBUG_PRINT("Mensaje de estado");
  }
}


bool PLC::getOuput(char* topic,char* ouput) {
    String sTopic(topic);
    if(!sTopic.endsWith(Configuration::command_Topic)) {
        return false;
    }
	int topicLength = strlen(Configuration::root_Topic) +  strlen(Configuration::PLC_Topic)+4;
	char subscribe_Topic[topicLength]; 
	sprintf(subscribe_Topic, "%s/%s/#", Configuration::root_Topic,  Configuration::PLC_Topic);
    String sOutput = sTopic.substring(strlen(subscribe_Topic)-1);
    sOutput = sOutput.substring(0,sOutput.indexOf("/"));
  
    sOutput.toCharArray(ouput, sOutput.length()+1);
    return true;
}

void PLC::updateRelay(char* relayName,int newState) {
    String strRelayName(relayName);
    int relayNumber = strRelayName.substring(1).toInt();
    if(relayNumber>=0 && relayNumber<16) {
        int relayPort = CONTROLLINO_RELAY_00 + relayNumber;
		pinMode(relayPort,OUTPUT);
        digitalWrite(relayPort, newState);

		char value[2];
        itoa(newState,value,10);
		PLC::publish(relayName, Configuration::state_Topic, value);
    } else {
        log("Numero de rele incorrecto");
    }

}

void PLC::publish(const char* portName,const char* messageType, const char* payload ){
	// Create message topic
	String topicString = String(Configuration::root_Topic);
	topicString += String("/");
	topicString += String(Configuration::PLC_Topic);
	topicString += String("/");
	topicString += String(portName);
	topicString += String("/");
	topicString += String(messageType);
	
	int topicLength = topicString.length()+1;

	char topic[topicLength];
	topicString.toCharArray(topic, topicLength); 

	mqttClient.publish(topic, payload);  
}

void PLC::onButtonClick(EventArgs* e){
	INFO_PRINT("Click!!!");
	INFO_PRINT(((Button*)e->sender)->pin());
	publishInput(((Button*)e->sender)->pin(), "click");
	
}

void PLC::onButtonDown(EventArgs* e){
	INFO_PRINT("Down!!!");
	INFO_PRINT(((Button*)e->sender)->pin());
	publishInput(((Button*)e->sender)->pin(), "down");
	
}

void PLC::onButtonUp(EventArgs* e){
	INFO_PRINT("Up!!!");
	INFO_PRINT(((Button*)e->sender)->pin());
	
	publishInput(((Button*)e->sender)->pin(), "up");
	
}

void PLC::publishInput(int pin, const char * event)
{
	typename vector<Input*>::iterator Iter;
	
	for (Iter = inputs.begin() ; Iter != inputs.end() ; Iter++ )  
			{   
				if((*Iter)->button->pin()==pin) 
				{
					PLC::publish((*Iter)->topic, "command", event);
					break;
				}
			}
	
}





int PLC::getValue(byte* payload, unsigned int length) {
    int value = INVALID_VALUE;
    if (length==1) {
        switch (payload[0]-'0') {
            case 0:
                value = LOW;
                break;
            case 1:
                value = HIGH;
                break;
            default:
                log("Error valor mensaje");
                break;
        }
    }  else  {
       DEBUG_PRINT("Comando ignorado");
    }
    
    return value;
}
