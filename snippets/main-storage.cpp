#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
#include <WiFi.h>
#include <esp_now.h>

// Set sensors and measurements
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

// Set Comms address
uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0x4D, 0x2B, 0xBC};

// Define message structure
typedef struct struct_message {
  char messageType[32]; // P for Pairing, E for Event
  char gateType[32]; // S for Start, F for Finish
} struct_message;
struct_message gateMessage;

// Define gate type variable
char gType[32];

// ESP-NOW magic, dunno
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
// Nothing for now
}

void setup() {
  // Serial init, for debug. Comment out when not needed.
  Serial.begin(9600);

  // LED init
  pinMode(12, OUTPUT);

  // XSHUT init
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);

  // Jumper init - if pin is not grounded, reads HIGH, else reads LOW. If it's LOW, set is as the Start gate, else set it as the Finish gate
  pinMode(19, INPUT_PULLUP);
  if (digitalRead(19) == LOW) {
    strcpy(gType, "S");
  } else {
    strcpy(gType, "F");
  }

  // WIFI init, set device as a WiFi Station, ESP-NOW init
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    return;
  }
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    return;
  }

  // LOX init - if fails, turn led on, if not - blink 3 times, then start HS mode, continuous
  digitalWrite(25, LOW); // XSHUT both sensors
  digitalWrite(26, LOW);
  delay(10);
  digitalWrite(25, HIGH); // XSHUT both sensors
  digitalWrite(26, HIGH);
  delay(10);
  digitalWrite(25, HIGH); // Turn on first sensor
  digitalWrite(26, LOW);
  delay(10);
  if (!lox1.begin(0x30)) {
    while(1);
  }
  else {
  }
  delay(10);
  digitalWrite(26, HIGH); // Turn on second sensor
  delay(10);
  if (!lox2.begin(0x31)) {
    while(1);
  }
  else {
  }
  delay(10);
  //lox1.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);
  lox1.startRangeContinuous();
  //lox2.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);
  lox2.startRangeContinuous();
}

void loop() {
  // Set values to send
  strcpy(gateMessage.messageType, "E");
  strcpy(gateMessage.gateType, gType);
  
  // If target in range, communicate over ESP-NOW and turn the LED on
  // We will change it later to not keep that damn delay on, but it's fine for now
  if (lox1.isRangeComplete() and lox1.readRange()<1200) {
  //  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &gateMessage, sizeof(gateMessage));
    Serial.println(lox1.readRange());
  //  digitalWrite(12, HIGH);
  //  delay(500);
  //  digitalWrite(12, LOW);
  }
  if (lox2.isRangeComplete() and lox2.readRange()<1200) {
  //  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &gateMessage, sizeof(gateMessage));
    Serial.println(lox2.readRange());
  //  digitalWrite(12, HIGH);
  //  delay(500);
  //  digitalWrite(12, LOW);
  }
}