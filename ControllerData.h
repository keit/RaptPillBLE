#ifndef CONTROLLERDATA_H
#define CONTROLLERDATA_H

#define HEATER_OFF false
#define HEATER_ON true
#define TEMP_THRESHOLD_DEFAULT 18.0

struct ControllerData {
  float currentTemp;
  float currentGravity;
  bool heaterStatus;
  float heaterThreshold;
  float battery;
  int memory;
};

void initCtrlData(ControllerData &ctrlData);
void updateHeaterThreshold(ControllerData &ctrlData, float heaterThreashold);
void updateMemorySize(ControllerData &ctrlData);
bool parseRaptPillDataV2(const uint8_t* data, size_t length, ControllerData& parsedData);
int freeMemory();

// void printMemorySize(String message);
#endif
