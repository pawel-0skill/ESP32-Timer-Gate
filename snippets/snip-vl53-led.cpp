#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
//#include <WiFi.h>
//#include <esp_now.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  // LED
  pinMode(12, OUTPUT);
  if (!lox.begin()) {
    digitalWrite(12, HIGH);
    delay(2000);
    digitalWrite(12, LOW);
    while(1);
  }
  else {
    digitalWrite(12, HIGH);
    delay(500);
    digitalWrite(12, LOW);
    delay(500);
    digitalWrite(12, HIGH);
    delay(500);
    digitalWrite(12, LOW);
    delay(500);
    digitalWrite(12, HIGH);
    delay(500);
    digitalWrite(12, LOW);
  }
  lox.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);
  lox.startRangeContinuous();
}

void loop() {
  if (lox.isRangeComplete()) {
    digitalWrite(12, HIGH);
    delay(2000);
    digitalWrite(12, LOW);
  }
}