/*
 * Inputs.cpp
 *
 *  Created on: Feb 25, 2021
 *      Author: lknop
 */

#include <Controllino.h>
#include "Inputs.h"

constexpr const char* const Inputs::PIN_NAMES[];

Inputs::Inputs() {
	pin_millis = 0;
	publish_callback = NULL;
}

void Inputs::begin(INPUT_CALLBACK_SIGNATURE) {
	this->publish_callback = publish_callback;

	INFO_PRINT("Initializing inputs...");

	for(int i=0;i<19;i++) {
		DEBUG_PRINT_PARAM("Input  ", i);
		DEBUG_PRINT_PARAM(" pin ", PIN_NAMES[i]);

#ifdef SIMULATED_CONTROLLINO
		pinMode(pins[i], INPUT_PULLUP);
#else
		pinMode(pins[i], INPUT);
#endif
		DEBUG_PRINT("Initialized")
	}
	INFO_PRINT("Inputs initialized: ");

}

void Inputs::loop() {
	if (millis() - pin_millis < INPUT_INTERVAL) {
		return;
	}
	pin_millis = millis();
	for (int i = 0; i < 19; i++) {
		byte pinValue = digitalRead(pins[i]);
		if (pinValue != pin_values[i]) {
			pin_debounce[i] = (pin_debounce[i] << 1) | pinValue;
			if (pin_values[i] && pin_debounce[i] == 0) {
				pin_values[i] = 0;
				INFO_PRINT_PARAM("Publishing OFF for ", PIN_NAMES[i]);
				publish_callback(PIN_NAMES[i], pin_values[i]);
			} else if (!pin_values[i] && pin_debounce[i] == 0xFF) {
				pin_values[i] = 1;
				INFO_PRINT_PARAM("Publishing ON for ", PIN_NAMES[i]);
				publish_callback(PIN_NAMES[i], pin_values[i]);
			}
		}
	}

}

