# ESP32-Timer-Gate

Project idea - use a set of two ESP32 boards and VL53L0 ToF sensors as timing gates for agility courses, communicating with a third ESP32 board over ESP-NOW, itself communication with a PC over USB.

Related repo - [ESP32-Timer-Comms](https://github.com/pawel-0skill/ESP32-Timer-Comms).

Related repo - ESP32-Timer-GUI, to be done (or not, BT serial monitor might do the trick for most of my needs).

Steps:

0. 100% - Learn how to code Arduino in VSC with Platform.io on ESP32.
1. 80% - Re-learn VL53L0:
   - Good enough for now at least - LED goes on when sensor reports something closer than 1200mm.
   - 100% - Take a step back, try to understand why the sensor does not sense up to 1200mm...  
     No issue, it works fine. Serial timing is shit - let's try to set the diode ON after reading for a full second.
     Works like a charm - buuuut small objects at high speed might fail to register. Two sensors per gate might be preferable - to be checked.
     Oh yeah, two sensors working in parallel are the way to go!
   ~~- 0% - Take another step back, learn how to make it work with a cover and not generating false positives. Cross-talk? Offset? Time to read the API documentation and find some examples.~~
   - 0% - Yeah, that's not it. Adafruit example code for dual sensor setup works fine. Seems I have an issue with my code... Lovely. So - fix the damn code, go line by line vs example.
2. 100% - Learn ESP-NOW - this is going to be fun, as in poke my eyes out fun.
   - Rui Santos is king, go check <https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/>
3. 100% - Write code for the PC communications board.
   - Takes incoming messages, prints mac addresses, nice.
   - Differentiates between Start and Finish messages, calculates run time.
4. 80% - Rework the gates:
   - Add a second ToF sensor per gate - parts just arrived! With additional housing for the sensor itself.
   - Consider minimal measuring time allocation - no need to be precise.
5. 0% - Add a RESET button on Comms.
6. ~~0% - Add BT communication to send serial messages out.~~
7. ~~0% - Add state LEDs to gates and comms.~~
   ~~- For comms, that's quite easy in single thread - basically most of the stuff is handled by OnReceive, so the loop can read blink the lighties... _Assuming_ that this will not block OnReceive from working at that time. Otherwise - multithreads, here we come!~~
   ~~- For gates, it's multihtreads anyway.~~
   ~~- _Or_ if I actually think this through and stop delaying my code like an animal, and check for millis to operate those LEDs, nothing will be blocked from executing by no damn delay, and I can leave threads out of it until I actually want them.~~
8. ~~0% - Write code for a time keeping program for the PC.~~
9. 10% - Add an OLED display to Comms, display whatever is needed.

Future updates:

- ~~Make it work on interrupts, if it makes any sense. I'm starting to think it will make no sense at all, but we will see.~~
- Split the work between two cores - 1st to constantly monitor I2C for sensor measurements, 2nd to commumicate and set LED states.

## How this is going to work

Typical flow of events should be:

1. __Gates__ are _ready_.
   - [X] Check jumper setting, set internal identification to START or FINISH.
   - [X] Initialize ESP-NOW, connect to comms.
   - [X] Initialize ToF sensor.
   - [ ] If pinged, respond accordingly.
2. __Comms__ is _ready_.
   - [X] Initialize serial port communications.
   - [ ] Initialize ESP-NOW, connect to start gate and finish gate - for pinging.
   - [ ] Ping gates.
   - [X] Set detection flag variables to False.
   - [X] Set event time variables to 0.
3. Dog enters course.
4. __Comms__ _expects start_ event.
   - [X] Detection flags are False.
5. Dog starts run, crosses start gate.
6. __Start gate__ _registers change_ in ToF sensor reading.
   - [X] When reading lower than threshold, send message to comms.
   - [ ] Turn LED on.
   - [ ] When reading higher than threshold, turn LED off.
7. __Start gate__ _communicates event_ over EPS-NOW to comms.
   - ~~[ ] When reading lower than threshold for the first time, set control variable true - to avoid sending multiple messages.~~
   - ~~[ ] When reading higher than threshold for the rist time, set control variable false.~~
   - In fact NO! - let the gates send a ton of messages when reading an object, just in case of communications issues - allow comms to work it out.
   - [X] Send gate identifier and detection event message. Keep sending as long as sensor registers distance lower than threshold.
   - Damn... If by _identifier_ I mean char "start" or "finish", that means Gates have different code - and I don't want that. Either hardcode Gate mac addresses in Comms, or make a hardware switch for Start or Finish. This second option seems more elegant... On the other hand, I'm hardcoding Comms address in Gates anyway, so what. For now - __hardcoding__.
   - I _could_ try broadcasting events to anyone who listens over 0:0:0:0:0:0, but _theoreticaly_ some asshole could come, power up a fake Gate and post trigger events just for kicks (and yeah, this is quite possible, you wouldn't believe how salty the community around agility is here).
   - This could be easily (as in for someone who knows what they are doing) done with <https://randomnerdtutorials.com/esp-now-auto-pairing-esp32-esp8266/> - Gate at startup attempts to pair with Comms, and Comms has a hardware and/or software switch to go into pairing mode, then we can ask Comms to list all peers and ping them, and see the LEDs on the Gates to verify, or even send an error message over serial if there are more than two peers connected. _This will be a problem for future me, big FU to that guy_.
8. __Comms__ _registers the time_ of start gate event.
   - [X] Set detection flag for start gate to true.
   - [X] Register the time in millis of start event.
9. __Comms__ _gives feedback_ about start event over serial port to PC GUI and/or change in LED state, and/or HTTP server.
   - [X] Send event information through serial to PC app.
10. __Comms__ _expects finish_ event.
    - [X] Start detection flag is true, finish detection flag is false.
11. Dog crosses finish gate.
12. __Finish gate__ _registers change_ in ToF sensor reading.
    - [X] When reading lower than threshold, send message to comms.
    - [ ] Turn LED on.
    - [ ] When reading higher than threshold, turn LED off.
13. __Finish gate__ _communicates event_ over ESP-NOW to comms.
    - [X] Send gate identifier and detection event message. Keep sending as long as sensor registers distance lower than threshold.
14. __Comms__ _registers the time_ of finish gate event.
    - [X] Set detection flag for finish gate to true.
    - [X] Register the time in millis of finish event.
15. __Comms__ _gives feedback_ about finish event over serial port to PC GUI and/or change in LED state, and/or HTTP server.
    - [X] Send event information through serial to PC app.
16. __Comms__ _calculates_ the difference between finish and start gate events.
    - [X] Calculate difference between finish event and start event time.
17. __Comms__ _sends result_ time over serial port, and/or HTTP server.
    - [X] Send result through serial.
18. __Comms__ _resets_, expects start event.
    - [X] Reset flags.

Additionally, the comms board should be able to:

1. Ping the gate boards to verify pairing.
2. Reset back to expecting start event in case of DNF.
3. Show last N run time values over HTTP.

On top of that, it would be nice if the comms board would be able to:

1. Constantly report connection status of gate boards.
2. Force remotely reset of gate boards.

## Hardware design

Housing for a gate will contain:

1. NodeMCU-32 board.
2. VL53L0X sensor.
3. Status LEDs:
   - Green for OK, could blink differently depending on init stage, to confirm what's OK or what's wrong, until it goes contant when connected,
   - Red for ERROR, could define different blinks for different situations, and for sensor read feedback.
4. USB power cable - power supplied from a powerbank, 5000mAh should be sufficient for two days.
5. Jumper to switch between Start and Finish gate identification - P19 to GND or to air.
