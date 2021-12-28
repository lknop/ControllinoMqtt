/*
 * ModbusClient.cpp
 *
 *  Created on: Feb 25, 2021
 *      Author: lknop
 */

#include "ModbusClient.h"

ModbusClient::ModbusClient()
	: modbus_master(MasterModbusAddress, RS485Serial, 0),
	  queue(sizeof(ModbusWriteStruct), 10, FIFO)  {
	modbus_reg = 0;
	modbus_state = 0;
	modbus_unit = 0;
	modbus_millis = 0;
	publish_callback = NULL;
}

void ModbusClient::begin(INPUT_CALLBACK_SIGNATURE) {
    modbus_master.begin( 19200 ); // baud-rate at 19200
    modbus_master.setTimeOut( 5000 ); // if there is no answer in 5000 ms, roll over
    modbus_millis = millis() + 1000;
	this->publish_callback = publish_callback;
}

bool ModbusClient::parseCommand(char* command, uint16_t new_state) {
    ModbusWrite mw;
    char *end;
    mw.slave = strtoul(command+1, &end, 10);
    mw.function = (*end == 'C') ? MB_FC_WRITE_COIL : MB_FC_WRITE_REGISTER;
    mw.coil_register = strtoul(end+1, NULL, 10);
    mw.value = new_state;
    queue.push(&mw);
    return mw.function == MB_FC_WRITE_COIL;
}

void ModbusClient::loop() {
	  if (Configuration::modbus_count == 0 && queue.isEmpty()) {
		  return;
	  }
	  char subscribe_topic[8];
	  switch(modbus_state) {
		  case 0:
			  if (millis() - modbus_millis > MODBUS_INTERVAL) {
				modbus_state++;
			  }
			  break;
		  case 1:
			  if (queue.isEmpty()) {
				  modbus_data.u8id = Configuration::modbus_address + modbus_unit;
				  modbus_master.query(modbus_data); // send query (only once)
				  modbus_state++;
			  } else {
				  ModbusWrite mw;
				  queue.pop(&mw);
				  DEBUG_PRINT_PARAM("sending to ", mw.slave);
				  modbus_t write_query = { .u8id = mw.slave, .u8fct = mw.function, .u16RegAdd = mw.coil_register, .u16CoilsNo = 0, .au16reg = &mw.value };
				  modbus_master.query(write_query);
				  modbus_state = 3;
			  }
			  break;
		  case 2:
			  modbus_master.poll(); // check incoming messages
			  if (modbus_master.getState() == COM_IDLE)
			  {
				 uint16_t current_modbus = modbus_reg;
				 uint16_t mask = current_modbus ^ modbus_values[modbus_unit];
				 if (mask) {
					 for (uint8_t i = 0; i < MODBUS_SIZE; i++) {
						 if (bitRead(mask, i)) {
							 sprintf_P(subscribe_topic, PSTR("M%dI%d"), modbus_unit + Configuration::modbus_address, i + 1);
							 INFO_PRINT_PARAM("Publishing ", subscribe_topic);
							 publish_callback(subscribe_topic, bitRead(current_modbus, i));
						 }
					 }
					 modbus_values[modbus_unit] = current_modbus;
				 }
				 modbus_state = 0;
				 modbus_unit = (modbus_unit + 1) % Configuration::modbus_count;
				 modbus_millis = millis();
			  }
			  break;
		  case 3:
			  modbus_master.poll(); // check incoming messages
	          if (modbus_master.getState() == COM_IDLE)
	          {
	              modbus_state = 0;
	          }
	          break;
		}

}
