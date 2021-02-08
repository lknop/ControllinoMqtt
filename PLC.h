#ifndef _PLC_h
#define _PLC_h

#include <Arduino.h>
#include <Controllino.h>
#include <SPI.h>
#include <PubSubClient.h>
#include "DebugUtils.h"
#include "Configuration.h"
#include <Ethernet.h>
#include "ModbusRtu.h"

#define MasterModbusAddress  0
#define MODBUS_INTERVAL 80
#define MODBUS_SIZE 16
#define INPUT_INTERVAL 15
#define INPUT_COUNT 19
#define RELAY_COUNT 16
#define OUTPUT_COUNT 24
#define OUTPUT_PIN_SECTION_1 12
#define OUTPUT_PIN_SECTION_2 20

// This MACRO defines number of the com port that is used for RS 485 interface.
// For MAXI and MEGA RS485 is reserved UART Serial3.
#define RS485Serial     3
#define ONSTATE "ON"
#define OFFSTATE "OFF"

#define HASS_DISCOVERY_INPUT "{\"name\":\"%s\",\"uniq_id\":\"%s_%s\", \
\"stat_t\":\"%s/%s/%s/%s\",\"dev\":{\"mf\":\"lino\",\"ids\":[\"lino\",\"%s\"],\"name\":\"lino\"}}"

#define HASS_DISCOVERY_OUTPUT "{ \"~\": \"%s/%s/%s\", \"name\":\"%s\",\"uniq_id\":\"%s_%s\", \
\"cmd_t\":\"~/command\",\"ret\":true,\"stat_t\":\"~/%s\",\"dev\":{\"mf\":\"lino\",\"ids\":[\"lino\",\"%s\"],\"name\":\"lino\"}}"


using namespace std;

class PLC {
    public:
        static void setup();
        static void loop();
   
    private:
        static EthernetClient ethClient;
        static PubSubClient mqttClient;
        static Modbus modbus_master;
        static modbus_t modbus_data;
        static uint8_t modbus_state;
        static uint8_t modbus_unit;
        static uint16_t modbus_reg;
        static uint16_t modbus_values[8];
        static uint32_t modbus_millis;
        static uint8_t pins[19];
        static uint8_t pin_values[19];
        static uint8_t pin_debounce[19];
        static uint32_t pin_millis;
        static void initializeModbus();
        static void loopModbus();
        static void loopInputs();
        static long mqtt_millis;
        static void initializeMQTT();
        static void initializeEthernet();
        static void initializeInputs();
        static bool reconnect();
        static void runDiscovery();
        static void log(const char* errorMsg);
        static void onMQTTMessage(char* topic, byte* payload, unsigned int length) ;
        static bool getOuput(char* topic,char* ouput);
        static void updateOutput(char* outputName,int newState);
        static int getValue(byte* payload, unsigned int length);
        static void publish(const char* portName,const char* messageType, const char* payload);
        static void publishInput(int pin, const char * event);
};
#endif
