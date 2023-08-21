#include <Arduino.h>
#include <Adafruit_VL53L0X.h>
#include <WiFi.h>
#include <esp_now.h>

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31

// set the pins to shutdown
#define SHT_LOX1 3 //25
#define SHT_LOX2 4 //26

// Set pins for LEDs
#define LED_RED 32
#define LED_GREEN 33

// Set pin for jumper selector
#define GATE_SELECT 15

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

// Define state variables
int objectDetected = 0;

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
  Serial.begin(115200);
  
  // LED pin test
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // wait until serial port opens for native USB devices
  while (! Serial) { delay(1); }

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);
  Serial.println(F("Shutdown pins inited..."));

  // all reset
  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  Serial.println(F("Both in reset mode...(pins are low)"));
    
  Serial.println(F("Starting..."));

  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and resetting LOX2
  digitalWrite(SHT_LOX2, LOW);

  // initing LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while(1);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }

  // Set high speed measurement
  delay(10);
  lox1.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);
  lox2.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_SPEED);

  // Jumper init - if pin is not grounded, reads HIGH, else reads LOW. If it's LOW, set is as the Start gate, else set it as the Finish gate
  pinMode(GATE_SELECT, INPUT_PULLUP);
  if (digitalRead(GATE_SELECT) == LOW) {
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
}

void loop() {
  // Set values to send
  strcpy(gateMessage.messageType, "E");
  strcpy(gateMessage.gateType, gType);

  // Perform the measurement
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!

  // Set state after lox1 measurement
  objectDetected = 0;
  if(measure1.RangeStatus != 4 && measure1.RangeMilliMeter < 1100) { 
    // Serial.print(F("lox1 in range, "));
    objectDetected++;
  } else {
    // Serial.print(F("lox1 out of range, "));
  }
  // Set state after lox2 measurement
  if(measure2.RangeStatus != 4 && measure2.RangeMilliMeter < 1100) {
    // Serial.print(F("lox2 in range."));
    objectDetected++;
  } else {
    // Serial.print(F("lox2 out of range."));
  }
  
  if(objectDetected > 0) {
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &gateMessage, sizeof(gateMessage));
  }

  Serial.println(objectDetected);
  // delay(10);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
}