#include "Adafruit_VL53L0X.h"

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31

// set the pins to shutdown
#define SHT_LOX1 25
#define SHT_LOX2 26

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;

// Define state variables
int objectDetected = 0;

void setup() {
  Serial.begin(115200);

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
}

void loop() {
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
  
  Serial.println(objectDetected);
  // delay(10);
}