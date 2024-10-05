/*
  Scan

  This example scans for Bluetooth® Low Energy peripherals and prints out their advertising details:
  address, local name, advertised service UUID's.

  The circuit:
  - Arduino MKR WiFi 1010, Arduino Uno WiFi Rev2 board, Arduino Nano 33 IoT,
    Arduino Nano 33 BLE, or Arduino Nano 33 BLE Sense board.

  This example code is in the public domain.
*/

#include <ArduinoBLE.h>
#include "RaptPillDataV2.h"
#define HEATER_OFF_LED 3

// RAPT Pill's BLE address can be reliably calculated 
// by taking the the MAC address used for registration and adding 2 on the last octet. 
const String broadcasterAddress = "FC:E8:C0:B2:16:8e";  // MAC address for BLE.

void setup() {
  Serial.begin(9600);
  while (!Serial);
  pinMode(HEATER_OFF_LED, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");

    while (1);
  }

  Serial.println("Bluetooth® Low Energy Central scan callback");

  // set the discovered event handle
  BLE.setEventHandler(BLEDiscovered, bleCentralDiscoverHandler);

  // start scanning for peripheral
  BLE.scanForAddress(broadcasterAddress, false);
}

void bleCentralDiscoverHandler(BLEDevice peripheral) {
  RaptPillDataV2 pillData;

  // discovered a peripheral
  Serial.println("Discovered a peripheral");
  Serial.println("-----------------------");
  
  // print address
  Serial.print("Address: ");
  Serial.println(peripheral.address());
  Serial.print("Device name: ");
  Serial.println(peripheral.deviceName());
  Serial.print("Device local name: ");
  Serial.println(peripheral.localName());

  Serial.print("RSSI: ");
  Serial.println(peripheral.rssi());

  if (peripheral.hasManufacturerData()) {
    int len = peripheral.manufacturerDataLength();
    uint8_t manufactureData[len];
    peripheral.manufacturerData(manufactureData, len);

    // for (int i = 0; i < len; i++) {
    //   Serial.println(manufactureData[i], HEX);
    //   Serial.println(" ");
    // }
    if (parseRaptPillDataV2(manufactureData, len, pillData)) {
      blink();
      Serial.print("Gravity Velocity: ");
      Serial.println(pillData.gravityVelocity);
      Serial.print("Temperature (C): ");
      Serial.println(pillData.temperature);
      Serial.print("Specific Gravity: ");
      Serial.println(pillData.specificGravity);
      Serial.print("Accel X: ");
      Serial.println(pillData.accelX);
      Serial.print("Accel Y: ");
      Serial.println(pillData.accelY);
      Serial.print("Accel Z: ");
      Serial.println(pillData.accelZ);
      Serial.print("Battery Percentage: ");
      Serial.println(pillData.batteryPercentage);      
    } else {
      Serial.println("Failed to parse RAPT Pill data");
    }
    Serial.print("Free memory: ");
    Serial.println(freeMemory());
  }
}

void loop() {
  BLE.poll();
}

extern "C" char *sbrk(int incr);

int freeMemory() {
  char stack_pointer;
  return &stack_pointer - sbrk(0);
}
void blink() {
  digitalWrite(HEATER_OFF_LED, HIGH);
  delay(500);
  digitalWrite(HEATER_OFF_LED, LOW);
}
