#include <Arduino.h>
#include "RaptPillDataV2.h"

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
bool parseRaptPillDataV2(const uint8_t* data, size_t length, RaptPillDataV2& parsedData) {
  if (length < 24) {
    return false;  // Ensure we have enough data
  }

  // Parse gravity velocity valid flag
  parsedData.gravityVelocityValid = data[6] == 0x01;

  // Parse gravity velocity (4 bytes, IEEE 754 float, only if valid)
  if (parsedData.gravityVelocityValid) {
    parsedData.gravityVelocity = parseFloatBigEndian(data + 7);
  } else {
    parsedData.gravityVelocity = 0.0f;  // Set to 0 if invalid
  }

  // Parse temperature in Kelvin (2 bytes, divide by 128 to convert to Kelvin)
  uint16_t tempRaw = parseUint16BigEndian(data + 11);
  parsedData.temperature = tempRaw / 128.0f - 273.15f;

  // Parse specific gravity (4 bytes, IEEE 754 float)
  parsedData.specificGravity = parseFloatBigEndian(data + 13);

  // Parse raw accelerometer data (X, Y, Z)
  parsedData.accelX = parseInt16BigEndian(data + 17);
  parsedData.accelY = parseInt16BigEndian(data + 19);
  parsedData.accelZ = parseInt16BigEndian(data + 21);

  // Parse battery state of charge (2 bytes, divide by 256 to convert to percentage)
  uint16_t batteryRaw = parseUint16BigEndian(data + 23);
  parsedData.batteryPercentage = batteryRaw / 256.0f;

  return true;  // Parsing successful
}
