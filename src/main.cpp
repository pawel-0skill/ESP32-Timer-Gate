#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
#include <WiFi.h>
#include <esp_now.h>

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Set Comms address
uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0x4D, 0x2B, 0xBC};

// Define data structure - we will just send either PING or EVENT 
typedef struct struct_message {
  char a[32];
} struct_message;
struct_message gateMessage;

// ESP-NOW magic, dunno
esp_now_peer_info_t peerInfo;

// Callback when data is sent - original example sent text to serial, we will just blink a LED
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  digitalWrite(12, HIGH);
  delay(100);
  digitalWrite(12, LOW);
}

void setup() {
  // LED init
  pinMode(12, OUTPUT);

  // WIFI init, set device as a WiFi Station, ESP-NOW init
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    digitalWrite(12, HIGH);
    delay(2000);
    digitalWrite(12, LOW);
    return;
  }
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    // Serial.println("Failed to add peer");
    return;
  }

  // LOX init - if fails, turn led on, if not - blink 3 times, then start HS mode, continuous
  if (!lox.begin()) {
    digitalWrite(12, HIGH);
    delay(1000);
    digitalWrite(12, LOW);
    while(1);
  }
  else {
    digitalWrite(12, HIGH);
    delay(250);
    digitalWrite(12, LOW);
    delay(250);
    digitalWrite(12, HIGH);
    delay(250);
    digitalWrite(12, LOW);
    delay(250);
    digitalWrite(12, HIGH);
    delay(250);
    digitalWrite(12, LOW);
  }
  lox.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);
  lox.startRangeContinuous();
}

void loop() {
  // Set values to send
  strcpy(gateMessage.a, "EVENT");
  
  // If target in range, communicate over ESP-NOW and turn the LED on
  // We will change it later to not keep that damn delay on, but it's fine for now
  if (lox.isRangeComplete() and lox.readRange()<1200) {
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &gateMessage, sizeof(gateMessage));
    digitalWrite(12, HIGH);
    delay(500);
    digitalWrite(12, LOW);
  }
}