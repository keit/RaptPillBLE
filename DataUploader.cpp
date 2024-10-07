#include <Arduino.h>
#include <ArduinoJson.h>

#include "DataUploader.h"
#include "arduino_secrets.h"

#include "WiFiS3.h"

const char* dataLoggerServer = DATA_LOGGER_SERVER;
const int port = DATA_LOGGER_SERVER_PORT;

// WiFi client for HTTP
WiFiClient client;

void sendPostRequest(String jsonData) {
  // Create the POST request
  String postRequest = String("POST /pillData HTTP/1.1\r\n") +
                       "Host: " + dataLoggerServer + "\r\n" +
                       "Content-Type: application/json\r\n" +
                       "Content-Length: " + jsonData.length() + "\r\n" +
                       "Connection: close\r\n\r\n" +
                       jsonData;

  // Send the request to the server
  client.print(postRequest);

  // Wait for the response and print it to Serial Monitor
  while (client.connected() || client.available()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
  }

  // Close the connection
  client.stop();
  Serial.println("Disconnected from server");
}

void uploadData(ControllerData& ctrlData) {
  if (client.connect(dataLoggerServer, port)) {
    Serial.println("Connected to server");

    StaticJsonDocument<200> jsonDoc;
    jsonDoc["currentTemp"] = ctrlData.currentTemp;
    jsonDoc["currentGravity"] = ctrlData.currentGravity;
    jsonDoc["heaterStatus"] = ctrlData.heaterStatus;
    jsonDoc["heaterThreshold"] = ctrlData.heaterThreshold;
    jsonDoc["battery"] = ctrlData.battery;
    jsonDoc["memory"] = ctrlData.memory;

    String jsonData;
    serializeJson(jsonDoc, jsonData);

    // Send HTTP POST request
    sendPostRequest(jsonData);
  } else {
    Serial.println("Connection to server failed");
  }
}
