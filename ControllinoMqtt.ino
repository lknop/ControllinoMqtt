#include <Arduino.h>
#include <ArduinoSTL.h>
#include <EEPROM.h>

#define TRACE_LEVEL_DEBUG 1

#include "PLC.h"


void setup()
{
    Serial.begin(115200);
    PLC::setup();
}

void loop()
{
    PLC::loop();
}
