# MyHome
Home Automation based on Controllino, Arduino and MQTT.

This repository constains code for Actuator (Controllino) and Controls (Arduino). 
In my case I use OpenHab as Automation Server and Mosquitto as MQTT server

Controllino is a PLC based on Arduino and in this project is used as actuator and to recive wall switch signals.

```
Wallshitches  ==>  Controllino <=> Mosquitto <=> OpenHAB
                       ||             ^^
                       ||             ||
Roller Shutters <========             ||  
                       ||             ||
Lights <=================             ||
                                      ||
Controllers   <=========================
