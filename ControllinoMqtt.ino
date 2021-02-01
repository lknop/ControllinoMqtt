#include <Arduino.h>
#include <ArduinoSTL.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include <Controllino.h>

#define TRACE_LEVEL_DEBUG 1
#define RXD2 17
#include "PLC.h"

static uint8_t mode = 0;

void setup()
{
    pinMode(RXD2, INPUT_PULLUP);
    Serial.begin(115200);
    mode = digitalRead(RXD2);

    if (mode == LOW) {
    	PLC::setup();
    } else {
    	Serial.println(F("Fallback logic"));
    }
}

void loop()
{
	if (digitalRead(RXD2) != mode) {
		wdt_enable(WDTO_60MS);
		while(1) {}
	}
	if (mode) {
		fallbackLogic();
	} else {
		PLC::loop();
	}
}

void fallbackLogic() {
	digitalWrite(CONTROLLINO_R6, digitalRead(CONTROLLINO_A12));
	digitalWrite(CONTROLLINO_D0, digitalRead(CONTROLLINO_A2));
	digitalWrite(CONTROLLINO_D1, digitalRead(CONTROLLINO_A2));
	digitalWrite(CONTROLLINO_D2, digitalRead(CONTROLLINO_A2));
	digitalWrite(CONTROLLINO_R1, digitalRead(CONTROLLINO_A0));
	digitalWrite(CONTROLLINO_D12, digitalRead(CONTROLLINO_A0));
	digitalWrite(CONTROLLINO_R0, digitalRead(CONTROLLINO_A10));
	digitalWrite(CONTROLLINO_R14, digitalRead(CONTROLLINO_A10));
	digitalWrite(CONTROLLINO_R15, digitalRead(CONTROLLINO_A10));
	digitalWrite(CONTROLLINO_R10, digitalRead(CONTROLLINO_A13));
	digitalWrite(CONTROLLINO_R9, digitalRead(CONTROLLINO_A6));
	digitalWrite(CONTROLLINO_R13, digitalRead(CONTROLLINO_A1));
	digitalWrite(CONTROLLINO_R3, digitalRead(CONTROLLINO_A7));
	digitalWrite(CONTROLLINO_R4, digitalRead(CONTROLLINO_A7));
	digitalWrite(CONTROLLINO_R7, digitalRead(CONTROLLINO_A8));
	digitalWrite(CONTROLLINO_D7, digitalRead(CONTROLLINO_A4));
	digitalWrite(CONTROLLINO_R8, digitalRead(CONTROLLINO_A7));
	digitalWrite(CONTROLLINO_D8, digitalRead(CONTROLLINO_A4));
	digitalWrite(CONTROLLINO_D9, digitalRead(CONTROLLINO_A4));
	digitalWrite(CONTROLLINO_D10, digitalRead(CONTROLLINO_A4));
}
