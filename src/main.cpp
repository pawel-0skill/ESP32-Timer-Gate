#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

// TOF reading
int distance = 0;

void setup() {
  // LED
  pinMode(12, OUTPUT);
  // TOF
  Serial.begin(9600);
  Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.startContinuous();
}

void loop() {
  distance = sensor.readRangeContinuousMillimeters();
  if (distance < 1200) {
    digitalWrite(12, HIGH);
  }
  else {
    digitalWrite(12, LOW);
  }
}
