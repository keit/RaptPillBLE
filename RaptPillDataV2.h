#ifndef RAPTPILLDATAV2_H
#define RAPTPILLDATAV2_H

struct RaptPillDataV2 {
  bool gravityVelocityValid;   // True if gravity velocity is valid
  float gravityVelocity;       // Gravity velocity in points per day (float)
  float temperature;           // Temperature in Kelvin (float)
  float specificGravity;       // Specific gravity (float)
  int16_t accelX;              // Raw accelerometer data X (signed)
  int16_t accelY;              // Raw accelerometer data Y (signed)
  int16_t accelZ;              // Raw accelerometer data Z (signed)
  float batteryPercentage;     // Battery state of charge in percentage (float)
};


bool parseRaptPillDataV2(const uint8_t* data, size_t length, RaptPillDataV2& parsedData);

#endif
