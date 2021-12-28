/*
 * ModbusClient.h
 *
 *  Created on: Feb 25, 2021
 *      Author: lknop
 */

#ifndef MODBUSCLIENT_H_
#define MODBUSCLIENT_H_

#include <Arduino.h>
#include <Controllino.h>
#include <cppQueue.h>
#include "ModbusRtu.h"
#include "DebugUtils.h"
#include "Configuration.h"

#define RS485Serial     3
#define MasterModbusAddress  0
#define MODBUS_INTERVAL 80
#define MODBUS_SIZE 16

#ifndef INPUT_CALLBACK_SIGNATURE
#define INPUT_CALLBACK_SIGNATURE void (*publish_callback)(const char*, uint8_t)
#endif


using ModbusWrite = struct ModbusWriteStruct {
	uint8_t		slave;
	uint8_t		function;
	uint16_t	coil_register;
	uint16_t	value;
};


class ModbusClient {
public:
	ModbusClient();
	bool parseCommand(char* command, uint16_t new_state);
	void begin(INPUT_CALLBACK_SIGNATURE);
	void loop();
private:
    Modbus modbus_master;
    modbus_t modbus_data = { .u8id = 0, .u8fct = MB_FC_READ_DISCRETE_INPUT, .u16RegAdd = 0, .u16CoilsNo = MODBUS_SIZE, .au16reg = &modbus_reg };
    uint8_t modbus_state;
    uint8_t modbus_unit;
    uint16_t modbus_reg;
    uint16_t modbus_values[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    uint32_t modbus_millis;
    cppQueue queue;
    INPUT_CALLBACK_SIGNATURE;
};

#endif /* MODBUSCLIENT_H_ */
