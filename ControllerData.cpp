#include <Arduino.h>
#include "ControllerData.h"

extern "C" char *sbrk(int incr);

int freeMemory() {
  char stack_pointer;
  return &stack_pointer - sbrk(0);
}

void print(ControllerData &ctrlData) {
  char buffer[128];
  snprintf(buffer, sizeof(buffer),
    "CurrentTemp: %.2f Heater Threshold: %.2f Current Gravity: %.2f Heater Status: %i Battery: %.2f  Memory: %i",
    ctrlData.currentTemp,
    ctrlData.heaterThreshold,
    ctrlData.currentGravity,
    ctrlData.heaterStatus,
    ctrlData.battery,
    ctrlData.memory);
  Serial.println(buffer);
}

void initCtrlData(ControllerData &ctrlData) {
  ctrlData.currentGravity = -1;
  ctrlData.currentTemp = -1;
  ctrlData.heaterStatus = HEATER_OFF;
  ctrlData.heaterThreshold = TEMP_THRESHOLD_DEFAULT;
  ctrlData.battery = -1;
  ctrlData.memory = -1;
}

void updateHeaterThreshold(ControllerData &ctrlData, float heaterThreashold) {
  Serial.println("Before updateHeaterThreshold");
  print(ctrlData);
  ctrlData.heaterThreshold = heaterThreashold;
  ctrlData.heaterStatus = ctrlData.currentTemp < ctrlData.heaterThreshold;
  Serial.println("After updateHeaterThreshold");
  print(ctrlData);
}

void updateMemorySize(ControllerData &ctrlData) {
  ctrlData.memory = freeMemory();
  print(ctrlData);
}

// void printMemorySize(String message) {
//   int memorySize = freeMemory();
//   Serial.println(message + " " + String(memorySize));
// }

// Function to convert 4 bytes (big-endian) to a float (IEEE 754)
float parseFloatBigEndian(const uint8_t* data) {
  uint32_t temp = (uint32_t)data[0] << 24 | (uint32_t)data[1] << 16 | (uint32_t)data[2] << 8 | (uint32_t)data[3];
  return *((float*)&temp);
}

// Function to parse 16-bit unsigned big-endian integer
uint16_t parseUint16BigEndian(const uint8_t* data) {
  return (uint16_t)data[0] << 8 | data[1];
}

// Function to parse 16-bit signed big-endian integer
int16_t parseInt16BigEndian(const uint8_t* data) {
  return (int16_t)((data[0] << 8) | data[1]);
}

// The main parser function
bool parseRaptPillDataV2(const uint8_t* data, size_t length, ControllerData& parsedData) {
  if (length < 24) {
    return false;  // Ensure we have enough data
  }

  // Parse gravity velocity valid flag
  // parsedData.gravityVelocityValid = data[6] == 0x01;

  // Parse gravity velocity (4 bytes, IEEE 754 float, only if valid)
  // if (parsedData.gravityVelocityValid) {
  //   parsedData.gravityVelocity = parseFloatBigEndian(data + 7);
  // } else {
  //   parsedData.gravityVelocity = 0.0f;  // Set to 0 if invalid
  // }

  // Parse temperature in Kelvin (2 bytes, divide by 128 to convert to Kelvin)
  uint16_t tempRaw = parseUint16BigEndian(data + 11);
  parsedData.currentTemp = tempRaw / 128.0f - 273.15f;

  // Parse specific gravity (4 bytes, IEEE 754 float)
  parsedData.currentGravity = parseFloatBigEndian(data + 13) / 1000.0f;

  // Parse raw accelerometer data (X, Y, Z)
  // parsedData.accelX = parseInt16BigEndian(data + 17);
  // parsedData.accelY = parseInt16BigEndian(data + 19);
  // parsedData.accelZ = parseInt16BigEndian(data + 21);

  // Parse battery state of charge (2 bytes, divide by 256 to convert to percentage)
  uint16_t batteryRaw = parseUint16BigEndian(data + 23);
  parsedData.battery = batteryRaw / 256.0f;

  return true;  // Parsing successful
}
