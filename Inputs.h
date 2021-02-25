/*
 * Inputs.h
 *
 *  Created on: Feb 25, 2021
 *      Author: lknop
 */

#ifndef INPUTS_H_
#define INPUTS_H_

#include <Arduino.h>
#include <Controllino.h>
#include "DebugUtils.h"

#define INPUT_INTERVAL 15
#define INPUT_COUNT 19

#ifndef INPUT_CALLBACK_SIGNATURE
#define INPUT_CALLBACK_SIGNATURE void (*publish_callback)(char*, uint8_t)
#endif

class Inputs {
public:
	static const constexpr char* const PIN_NAMES[] = {"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","A10",
			"A11","A12","A13","A14","A15","I16","I17","I18"};
	Inputs();
	void loop();
	void begin(INPUT_CALLBACK_SIGNATURE);

private:
	INPUT_CALLBACK_SIGNATURE;
    uint8_t pins[INPUT_COUNT] = {
    		CONTROLLINO_A0,  CONTROLLINO_A1,  CONTROLLINO_A2,
    		CONTROLLINO_A3,	 CONTROLLINO_A4,  CONTROLLINO_A5,
    		CONTROLLINO_A6,	 CONTROLLINO_A7,  CONTROLLINO_A8,
    		CONTROLLINO_A9,	 CONTROLLINO_A10, CONTROLLINO_A11,
    		CONTROLLINO_A12, CONTROLLINO_A13, CONTROLLINO_A14,
    		CONTROLLINO_A15, CONTROLLINO_I16, CONTROLLINO_I17,
    		CONTROLLINO_I18
    	};

#ifdef SIMULATED_CONTROLLINO
	uint8_t pin_values[INPUT_COUNT] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
#else
	uint8_t pin_values[INPUT_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif
	uint8_t pin_debounce[INPUT_COUNT]  = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	uint32_t pin_millis;
};

#endif /* INPUTS_H_ */
