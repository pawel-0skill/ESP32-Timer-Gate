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

1. __Gates__ are _ready_.
  - [ ] Initialize ToF sensor.
  - [ ] When reading lower than threshold, turn LED on.
  - [ ] When reading lower than threshold for the first time, set control variable true - to avoid sending multiple messages.
  - [ ] When reading higher than threshold for the rist time, set control variable false.
  - [ ] Initialize ESP-NOW, connect to Comms.
  - [ ] If pinged, respond accordingly.
2. __Comms__ is _ready_.
  - [ ] Initialize serial port communications.
3. Dog enters course.
2. __Comms__ _expects start_ event.
3. Dog starts run, crosses start gate.
4. __Start gate__ _registers change_ in ToF sensor reading.
5. __Start gate__ _communicates event_ over EPS-NOW to comms.
6. __Comms__ _registers the time_ of start gate event.
7. __Comms__ _gives feedback_ about start event over serial port to PC GUI and/or change in LED state, and/or HTTP server.
8. __Comms__ _expects finish_ event. 
9. Dog crosses finish gate.
10. __Finish gate__ _registers change_ in ToF sensor reading.
11. __Finish gate__ _communicates event_ over ESP-NOW to comms.
12. __Comms__ _registers the time_ of finish gate event.
13. __Comms__ _gives feedback_ about finish event over serial port to PC GUI and/or change in LED state, and/or HTTP server.
14. __Comms__ _calculates_ the difference between finish and start gate events.
16. __Comms__ _sends result_ time over serial port, and/or HTTP server.
17. __Comms__ _resets_, expects start event.

Additionally, the comms board should be able to:

1. Ping the gate boards to verify pairing.
2. Reset back to expecting start event in case of DNF.
3. Show last N run time values over HTTP.

On top of that, it would be nice if the comms board would be able to:

1. Constantly report connection status of gate boards.
2. Force remotely reset of gate boards.
