#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
// #include <WiFi.h>
// #include <esp_now.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(9600);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  Serial.println("Adafruit VL53L0X test.");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power
  Serial.println(F("VL53L0X API Continuous Ranging example\n\n"));

  lox.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);
  // start continuous ranging
  lox.startRangeContinuous();

}

void loop() {
  if (lox.isRangeComplete()) {
    Serial.print("Distance in mm: ");
    Serial.println(lox.readRange());
  }
}