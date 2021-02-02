#include <Arduino.h>
#include <Controllino.h>
#undef SPI_HAS_EXTENDED_CS_PIN_HANDLING

#include <SPI.h>
#include <PubSubClient.h>
#include "DebugUtils.h"
#include "Configuration.h"
#include "PLC.h"
#include "MemoryFree.h"


#define INVALID_VALUE -99

using namespace std;


EthernetClient PLC::ethClient;
PubSubClient PLC::mqttClient(ethClient);
Modbus PLC::modbus_master(MasterModbusAddress, RS485Serial, 0);
uint16_t PLC::modbus_reg = 0;
uint16_t PLC::modbus_values[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
modbus_t PLC::modbus_data = { .u8id = 0, .u8fct = 2, .u16RegAdd = 0, .u16CoilsNo = MODBUS_SIZE, .au16reg = &modbus_reg };
uint8_t PLC::modbus_state = 0;
uint8_t PLC::modbus_unit = 0;
uint32_t PLC::modbus_millis = 0;
const char * names[19] = {"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","A10",
		"A11","A12","A13","A14","A15","I16","I17","I18"};
uint8_t PLC::pins[19] = {
	CONTROLLINO_A0,  CONTROLLINO_A1,  CONTROLLINO_A2,
	CONTROLLINO_A3,	 CONTROLLINO_A4,  CONTROLLINO_A5,
	CONTROLLINO_A6,	 CONTROLLINO_A7,  CONTROLLINO_A8,
	CONTROLLINO_A9,	 CONTROLLINO_A10, CONTROLLINO_A11,
	CONTROLLINO_A12, CONTROLLINO_A13, CONTROLLINO_A14,
	CONTROLLINO_A15, CONTROLLINO_I16, CONTROLLINO_I17,
	CONTROLLINO_I18
};
#ifdef SIMULATED_CONTROLLINO
uint8_t PLC::pin_values[19] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
#else
uint8_t PLC::pin_values[19] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif
uint8_t PLC::pin_debounce[19]  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint32_t PLC::pin_millis = 0;

long PLC::mqtt_millis = 0;

void PLC::setup() {
#ifdef CONTROLLINO_MEGA
	DEBUG_PRINT("CONTROLLINO_MEGA defined");
#endif


	DEBUG_PRINT("Initializing PLC");

	Configuration::load();	

	if(Configuration::isValid) {
		PLC::initializeMQTT();
		PLC::initializeEthernet();
		PLC::initializeInputs();
		PLC::initializeModbus();
		DEBUG_PRINT("Initialization OK");
	} else {
		INFO_PRINT("PLC not configured");
	}
	DEBUG_PRINT("PLC initialized");

}
 
void PLC::loop() {
    DEBUG_PRINT("loop start");
	
	Configuration::loop();
	
	if(Configuration::isValid && !Configuration::isConfiguring) {
		bool connected  = true;

		if (!mqttClient.connected() && (mqtt_millis ==0 || ((millis() - mqtt_millis) >= 2000))) {
			mqtt_millis = millis();
			connected = reconnect();
		}

		if (connected) {
			mqttClient.loop();
		}  else {
			INFO_PRINT("Retrying in five seconds");
		}
		loopModbus();
		loopInputs();
	}
	
    DEBUG_PRINT("loop end");
}

void PLC::initializeModbus() {
	  modbus_master.begin( 19200 ); // baud-rate at 19200
	  modbus_master.setTimeOut( 5000 ); // if there is no answer in 5000 ms, roll over
	  modbus_millis = millis() + 1000;
}

void PLC::loopInputs() {
	if (millis() - pin_millis < INPUT_INTERVAL) {
		return;
	}
	pin_millis = millis();
	for (int i = 0; i < 19; i++) {
		// Serial.println("reading " + i);
		byte pinValue = digitalRead(pins[i]);
		if (pinValue != pin_values[i]) {
			pin_debounce[i] = (pin_debounce[i] << 1) | pinValue;
			if (pin_values[i] && pin_debounce[i] == 0) {
				pin_values[i] = 0;
				Serial.print(names[i]);
				Serial.println(" OFF");
				PLC::publish(names[i], Configuration::state_Topic, OFFSTATE);
			} else if (!pin_values[i] && pin_debounce[i] == 0xFF) {
				pin_values[i] = 1;
				Serial.print(names[i]);
				Serial.println(" ON");
				PLC::publish(names[i], Configuration::state_Topic, ONSTATE);
			}
		}
	}
}

void PLC::loopModbus() {
  if (Configuration::modbus_count == 0) {
	  return;
  }
  char subscribe_topic[8];
  switch(PLC::modbus_state) {
	  case 0:
		  if (millis() - modbus_millis > MODBUS_INTERVAL) {
			modbus_state++;
		  }
		  break;
	  case 1:
		  modbus_data.u8id = Configuration::modbus_address + modbus_unit;
		  modbus_master.query(modbus_data); // send query (only once)
		  modbus_state++;
		  break;
	  case 2:
		  modbus_master.poll(); // check incoming messages
		  if (modbus_master.getState() == COM_IDLE)
		  {
			 uint16_t current_modbus = modbus_reg;
			 uint16_t mask = current_modbus ^ modbus_values[modbus_unit];
			 Serial.println(current_modbus);
			 if (mask) {
				 for (uint8_t i = 0; i < MODBUS_SIZE; i++) {
					 if (bitRead(mask, i)) {
						 sprintf_P(subscribe_topic, PSTR("M%dI%d"), modbus_unit + Configuration::modbus_address, i + 1);
						 INFO_PRINT_PARAM("Publishing ", subscribe_topic);
						 PLC::publish(subscribe_topic, Configuration::state_Topic, bitRead(current_modbus, i) ? ONSTATE : OFFSTATE);
					 }
				 }
				 modbus_values[modbus_unit] = current_modbus;
			 }
			 modbus_state = 0;
			 modbus_unit = (modbus_unit + 1) % Configuration::modbus_count;
			 modbus_millis = millis();
		  }
		  break;
	}
}

void PLC::initializeMQTT() {
  INFO_PRINT("Initializing MQTT client...");
  IPAddress server(Configuration::server[0], Configuration::server[1], Configuration::server[2], Configuration::server[3]);
  PLC::mqttClient.setServer(server, Configuration::port);
  PLC::mqttClient.setCallback(PLC::onMQTTMessage);
}

void PLC::initializeEthernet() {
  INFO_PRINT("Initializing Ethernet...");
  IPAddress ip(Configuration::ip[0], Configuration::ip[1], Configuration::ip[2], Configuration::ip[3]);

#ifdef SIMULATED_CONTROLLINO
  Ethernet.init(10);
#endif
  int validAddress;
  if (Configuration::ip[0] == 0) { // use DHCP
      validAddress = Ethernet.begin(Configuration::mac);
  } else {
      Ethernet.begin(Configuration::mac, ip);
      validAddress = 1;
  }

  if (validAddress == 0) {
    INFO_PRINT("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      INFO_PRINT("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      INFO_PRINT("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forever:
    while (true) {
      delay(1);
    }
  }
  
  DEBUG_PRINT("Initializing")
  // Allow the hardware to sort itself out
  delay(1500);

  INFO_PRINT_PARAM("Local IP", Ethernet.localIP());
}

void PLC::initializeInputs() {  
	INFO_PRINT("Initializing inputs...");
	
	for(int i=0;i<19;i++) {
		DEBUG_PRINT_PARAM("Input  ", i);
		DEBUG_PRINT_PARAM(" pin ", names[i]);

#ifdef SIMULATED_CONTROLLINO
		pinMode(pins[i], INPUT_PULLUP);
#else
		pinMode(pins[i], INPUT);
#endif
		DEBUG_PRINT("Initialized")
	}
	INFO_PRINT("Inputs initialized: ");
 }

bool PLC::reconnect() {
    bool res;
    // Loop until we're reconnected
    INFO_PRINT("Connecting MQTT server...");
    // Attempt to connect

    bool connected = (strlen(Configuration::username) == 0) ?
        mqttClient.connect(Configuration::PLC_Topic) :
        mqttClient.connect(Configuration::PLC_Topic, Configuration::username, Configuration::password);

    if (connected) {
      // Once connected, publish an announcement...
        log("Connected!");
        runDiscovery();
        // ... and resubscribe
		int topicLength = strlen(Configuration::root_Topic) +  strlen(Configuration::PLC_Topic)+4;
		char subscribe_Topic[topicLength]; 
        sprintf_P(subscribe_Topic, PSTR("%s/%s/#"), Configuration::root_Topic, Configuration::PLC_Topic);
        mqttClient.subscribe(subscribe_Topic);
        INFO_PRINT_PARAM("Subscribed to: ", subscribe_Topic);
        res = true;
    } else {
        INFO_PRINT_PARAM("Error!, rc=", mqttClient.state());
        // Wait 5 seconds before retrying
        //delay(5000);
        res = false;
    }
    return res;
}

void PLC::runDiscovery()
{
	Serial.println(freeMemory());
	char config_msg[250];
	char name[8];
	char mac[15];
	char config_topic[50];
	sprintf_P(mac, PSTR("%x%x%x%x%x%x"), Configuration::mac[0], Configuration::mac[1], Configuration::mac[2],
			Configuration::mac[3], Configuration::mac[4], Configuration::mac[5]);
	Serial.println(mac);

	for (int m = 0; m < Configuration::modbus_count; m++) {
		for (int i = 0; i < MODBUS_SIZE; i++) {
			sprintf_P(name, PSTR("M%dI%d"), m + Configuration::modbus_address, i + 1);
			sprintf_P(config_msg, PSTR(HASS_DISCOVERY), name, mac, name, Configuration::root_Topic,
		            Configuration::PLC_Topic, name, Configuration::state_Topic, mac);
			sprintf_P(config_topic, PSTR("homeassistant/binary_sensor/%s/config"), name);
			Serial.println(config_msg);
			mqttClient.publish(config_topic, config_msg, true);
		}
	}
}

void PLC::log(const char* errorMsg)
{
    INFO_PRINT_PARAM("LOG", errorMsg);
    if (mqttClient.connected()) {
      int topicLength = strlen(Configuration::root_Topic) +  strlen(Configuration::log_Topic)+ strlen(Configuration::PLC_Topic)+3;
      char log_Topic[topicLength];
      sprintf_P(log_Topic, PSTR("%s/%s/%s"), Configuration::root_Topic, Configuration::PLC_Topic, Configuration::log_Topic);
      mqttClient.publish(log_Topic, errorMsg);
      INFO_PRINT_PARAM("Published ", errorMsg);
      INFO_PRINT_PARAM("to topic ", log_Topic);
    }
}

void PLC::onMQTTMessage(char* topic, byte* payload, unsigned int length) {
    DEBUG_PRINT_PARAM("Message arrived to topic", topic);
    
    for (unsigned int i=0;i<length;i++) {
        DEBUG_PRINT_PARAM("Payload", (char)payload[i]);
    }
    

    char command[10];
    if (getOuput(topic, command)) {
        DEBUG_PRINT_PARAM("Command: ", command);
        
        int newState = getValue(payload, length);
        
        if(command[0]=='R' || command[0]=='D') {
            updateOutput(command, newState);
        }

  } else {
      DEBUG_PRINT("Status message");
  }
}


bool PLC::getOuput(char* topic,char* ouput) {
    String sTopic(topic);
    if(!sTopic.endsWith(Configuration::command_Topic)) {
        return false;
    }
	int topicLength = strlen(Configuration::root_Topic) +  strlen(Configuration::PLC_Topic)+4;
	char subscribe_Topic[topicLength]; 
	sprintf_P(subscribe_Topic, PSTR("%s/%s/#"), Configuration::root_Topic,  Configuration::PLC_Topic);
    String sOutput = sTopic.substring(strlen(subscribe_Topic)-1);
    sOutput = sOutput.substring(0,sOutput.indexOf("/"));
  
    sOutput.toCharArray(ouput, sOutput.length()+1);
    return true;
}

void PLC::updateOutput(char* outputName,int newState) {
    String strOutputName(outputName);
    int outputNumber = strOutputName.substring(1).toInt();

    int pin;

    if (outputName[0] == 'R' && outputNumber>=0 && outputNumber<16) {
      pin = CONTROLLINO_R0 + outputNumber;
    } else if (outputName[0] == 'D' && outputNumber >=0 && outputNumber<12) {
      pin = CONTROLLINO_D0 + outputNumber;
    } else if (outputName[0] == 'D' && outputNumber >=12 && outputNumber<20) {
      pin = CONTROLLINO_D12 + outputNumber;
    } else if (outputName[0] == 'D' && outputNumber >=20 && outputNumber<23) {
      pin = CONTROLLINO_D20 + outputNumber;
    } else {
      log("Invalid output");
      return;
    }
    pinMode(pin,OUTPUT);
    digitalWrite(pin, newState);

    PLC::publish(outputName, Configuration::state_Topic, newState ? ONSTATE : OFFSTATE);
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

	mqttClient.publish(topic, payload, true);
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
                log("Invalid value");
                break;
        }
    }  else  {
       DEBUG_PRINT("Command ignored");
    }
    
    return value;
}
