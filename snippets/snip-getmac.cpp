// Code snippet - get MAC of the board
// Thank you, Random Nerd Tutorials

#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
#include <WiFi.h>
// #include <esp_now.h>
 
void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_MODE_STA);
}
 
void loop() {
  Serial.println(WiFi.macAddress());
}
