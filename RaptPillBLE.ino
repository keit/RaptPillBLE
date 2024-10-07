#include "WiFiS3.h"
#include "IPAddress.h"
#include <ArduinoJson.h>
#include <ArduinoBLE.h>

#include "arduino_secrets.h"
#include "HttpServerUtils.h"
#include "ControllerData.h"
#include "DataUploader.h"

#define POWER 7
#define HEATER_ON_LED 0
#define HEATER_OFF_LED 3

// WiFi credentials
const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

// Server settings
WiFiServer server(80);  // HTTP server on port 80

// RAPT Pill's BLE address can be reliably calculated
// by taking the the MAC address used for registration and adding 2 on the last octet.
const String broadcasterAddress = RAPT_PILL_BLE_MAC_ADDRESS;  // MAC address for BLE.

ControllerData ctrlData;

void setup() {
  delay(1000); // 1 second delay to give time initialization after upload.
  Serial.begin(9600);

  pinMode(HEATER_ON_LED, OUTPUT);
  pinMode(HEATER_OFF_LED, OUTPUT);
  pinMode(POWER, OUTPUT);

  initCtrlData(ctrlData);

  connectToWiFi();

  // Start the HTTP server
  server.begin();

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

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.print("Connected to WiFi! ");
  Serial.println(WiFi.localIP());
}

void checkWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.print(".");
        }
        Serial.println("Reconnected to WiFi.");
    }
}


void switchPower(bool heaterStatus) {
  if (heaterStatus) {
    heaterOn();
  }
  else {
    heaterOff();
  }
}

void heaterOn() {
  Serial.println("tuon ON heater");
  digitalWrite(POWER, HIGH);
  digitalWrite(HEATER_ON_LED, HIGH);
  digitalWrite(HEATER_OFF_LED, LOW);
}

void heaterOff() {
  Serial.println("tuon OFF heater");
  digitalWrite(POWER, LOW);
  digitalWrite(HEATER_ON_LED, LOW);
  digitalWrite(HEATER_OFF_LED, HIGH);
}

// Function to handle incoming HTTP requests
void handleClient(WiFiClient& client) {
  char requestLine[40];
  size_t lengthToRead1 = sizeof(requestLine) - 1;
  client.readBytesUntil('\r', requestLine, lengthToRead1);

  char skipRest[10];
  size_t lengthToRead2 = sizeof(skipRest) - 1;
  client.readBytesUntil('\r', skipRest, lengthToRead2);


  // Wait until the client sends some data
  while(client.available()) {
    if (strstr(requestLine, "GET /data") != NULL) {
      // Serve the JSON data
      sendJSONData(client, ctrlData);
     } else if (strstr(requestLine, "POST /updateThreshold") != NULL) {
      // Receive and update the heaterThreshold
      updateThreshold(client, ctrlData);
     } else {
      client.print(getHttpRespHeader());
      client.print(htmlPage);
    }
    client.flush();

    break;
  }

  // Close the connection
  client.stop();
}

void bleCentralDiscoverHandler(BLEDevice peripheral) {
  if (peripheral.hasManufacturerData()) {
    int len = peripheral.manufacturerDataLength();
    uint8_t manufactureData[len];
    peripheral.manufacturerData(manufactureData, len);

    if (parseRaptPillDataV2(manufactureData, len, ctrlData)) {
      uploadData(ctrlData);
      switchPower(ctrlData.heaterStatus);
      updateMemorySize(ctrlData);
    } else {
      Serial.println("Failed to parse RAPT Pill data");
    }
  }
}

void loop() {
  checkWiFi();

  // Check for incoming HTTP clients
  WiFiClient client = server.available();
  if (client) {
    handleClient(client);
    switchPower(ctrlData.heaterStatus);
    updateMemorySize(ctrlData);
  }

  BLE.poll();
}
