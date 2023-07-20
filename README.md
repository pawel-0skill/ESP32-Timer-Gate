# ESP32-Timer-Gate

Project idea - use a set of two ESP32 boards and VL53L0 ToF sensors as timing gates for agility courses, communicating with a third ESP32 board over ESP-NOW, itself communication with a PC over USB.

Related repo - [ESP32-Timer-Comms](https://github.com/pawel-0skill/ESP32-Timer-Comms).

Related repo - ESP32-Timer-GUI, to be done.

Steps:

0. 100% - learn how to code Arduino in VSC with Platform.io on ESP32
1. 100% - re-learn VL53L0 (good enough for now at least - LED goes on when sensor reports something closer than 1200mm)
2. 0% - learn ESP-NOW
3. 0% - write code for the PC interface board
4. 0% - write code for a time keeping program for the PC

Future updates:

1. Make it work on interrupts
