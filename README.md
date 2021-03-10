
![Controllino](https://www.controllino.com/wp-content/uploads/2017/03/controllino_mega_arduino_sps_plc_speicherprogrammierbare_steuerung_programmable_logic_controller_side_white-300x300.jpg)<img src="https://mqtt.org/assets/img/mqtt-logo.svg" alt="MQTT logo" width="500" height="200">

# ControllinoMqtt
Automation PLC based on Controllino, Arduino and MQTT.

Originally forked from [aitorforero/MyHome](https://github.com/aitorforero/MyHome) but renamed to be more precise and reinitialized to reduce the size of the git repository. 

Project structure was updated to support compilation by [PlatformIO](https://platformio.org/), [Sloeber](https://eclipse.baeyens.it/) and original [Arduino IDE](https://www.arduino.cc/en/Main/Software)

[Controllino](https://www.controllino.biz/product/controllino-mega/) is a PLC based on Arduino and in this project it is used for  both inputs from wall switches as well as relay outputs. At the moment only controllino MEGA is supported. However, the code also runs correctly on Arduino Mega with Ethernet shield - in that case a #define SIMULATED_CONTROLLINO is necessary.


Features: 
- supports digital outputs for external relays
- supports MODBUS for external inputs
- supports MODBUS for external relays and digital outputs
- multiple MODBUS modules
- configurable MODBUS addressing
- mqtt authentication
- mqtt auto discovery topics for home assistant <img src="https://www.home-assistant.io/images/home-assistant-logo.svg" height="20" width="20">


