# ESP32-Timer-Gate

Project idea - use a set of two ESP32 boards and VL53L0 ToF sensors as timing gates for agility courses, communicating with a third ESP32 board over ESP-NOW, itself communication with a PC over USB.

Related repo - [ESP32-Timer-Comms](https://github.com/pawel-0skill/ESP32-Timer-Comms).

Related repo - ESP32-Timer-GUI, to be done.

Steps:

0. 100% - learn how to code Arduino in VSC with Platform.io on ESP32
1. 100% - re-learn VL53L0 (good enough for now at least - LED goes on when sensor reports something closer than 1200mm)
2. 1% - learn ESP-NOW - this is going to be fun, as in poke my eyes out fun
3. 0% - write code for the PC communications board
4. 0% - write code for a time keeping program for the PC

Future updates:

1. Make it work on interrupts

# How this is going to work:

Typical flow of events should be:

1. Dog enters course. Devices are paired and waiting.
2. Comms expects start event.
3. Dog starts run, crosses start gate.
4. Start gate registers change in ToF sensor reading.
5. Start gate communicates event over EPS-NOW to comms.
6. Comms registers the time of start gate event.
7. Comms gives feedback about start event over serial port to PC GUI and/or change in LED state, and/or HTTP server.
8. Comms expects finish event. 
9. Dog crosses finish gate.
10. Finish gate registers change in ToF sensor reading.
11. Finish gate communicates event over ESP-NOW to comms.
12. Comms registers the time of finish gate event.
13. Comms gives feedback about finish event over serial port to PC GUI and/or change in LED state, and/or HTTP server.
14. Comms calculates the difference between finish and start gate events.
16. Comms sends result time over serial port, and/or HTTP server.
17. Comms resets, expects start event.

Additionally, the comms board should be able to:

1. Ping the gate boards to verify pairing.
2. Reset back to expecting start event in case of DNF.
3. Show last N run time values over HTTP.

On top of that, it would be nice if the comms board would be able to:

1. Constantly report connection status of gate boards.
2. Force remotely reset of gate boards.
