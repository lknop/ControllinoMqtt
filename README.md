# ControllinoMqtt
Automation PLC based on Controllino, Arduino and MQTT.

Originally forked from [aitorforero/MyHome](https://github.com/aitorforero/MyHome) but renamed to be more precise and reinitialized to reduce the size of the git repository. 

Structure was changed to support compilation both by [Sloeber](https://eclipse.baeyens.it/) and original [Arduino IDE](https://www.arduino.cc/en/Main/Software)

[Controllino](https://www.controllino.biz/product/controllino-mega/) is a PLC based on Arduino and in this project it is used for  both inputs from wall switches as well as relay outputs. At the moment only controllino MEGA is supported.

TODO: 
- [x] support digital outputs for external relays
- [x] support MODBUS for external inputs
- [ ] support MODBUS for external relays
- [x] multiple MODBUS modules
- [x] configurable MODBUS addressing
- [x] mqtt authentication
- [x] mqtt auto discovery topics for home assistant
- [x] drop Button, FastDelegate, Timer and Event classes to reduce RAM usage and simplify code

