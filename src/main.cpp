#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "Device.h"
#include "Room.h"
#include "Unit.h"
#include "Vector.h"
#include "Relay.h"

JsonDocument doc;
const char *ssidAP = "P424-2";
const char *passwordAP = "0947900523";
const char *BASE_URL = "http://157.10.52.61:3000";
HTTPClient http;

void connectToWifi();
void fetchDataDevices();
void updateDevice(Device *device);
void initRelay();
void initDevice(Vector<Device *> devicesData);

void setup()
{
  Serial.begin(115200);
  connectToWifi();
  fetchDataDevices();
  initRelay();
}

void connectToWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssidAP, passwordAP);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to the WiFi network");
}

void loop()
{
  static unsigned long lastFetchTime = 0;
  unsigned long currentTime = millis();

  // Fetch data every 3 seconds
  if (currentTime - lastFetchTime >= 3000)
  {
    fetchDataDevices();
    lastFetchTime = currentTime;
  }
}

void fetchDataDevices()
{
  http.begin(String(BASE_URL) + "/device");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "*/*");
  Serial.println("Fetching data devices..");
  Serial.println(String(BASE_URL) + "/device");
  int httpCode = http.GET();
  Serial.println(httpCode);
  if (httpCode == 200)
  {
    String payload = http.getString();
    deserializeJson(doc, payload);
    for (int i = 0; i < doc.size(); i++)
    {
      JsonObject device = doc[i];
      int id = device["id"];
      String name = device["name"];
      int value = device["value"];
      bool status = device["status"];
      String image = device["image"];
      int pinModec = device["pinMode"];
      bool isSensor = device["isSensor"];
      if (!isSensor)
      {
        pinMode(pinModec, OUTPUT);
        if (status)
        {
          digitalWrite(pinModec, LOW);
        }
        else
        {
          digitalWrite(pinModec, HIGH);
        }
      }
      String description = device["description"];
      JsonObject unit = device["unit"];
      int unitId = unit["id"];
      String unitName = unit["name"];
      String unitAbbreviation = unit["abbreviation"];
      Unit *unitObj = new Unit(unitId, unitName, unitAbbreviation);
      JsonObject room = device["room"];
      int roomId = room["id"];
      String roomName = room["name"];
      String roomDescription = room["description"];
      Room *roomObj = new Room(roomId, roomName, roomDescription);
      String pinModeData = String(pinModec);
      Device *deviceObj = new Device(id, name, description, status, pinModeData, value, image, roomObj, unitObj, isSensor, unitId, roomId);
    }
  }
  else
  {
    Serial.println("Failed to fetch data from server");
  }
  http.end();
}

void updateDevice(Device *device)
{
  Serial.println("Updating device..");
}

void initRelay()
{
}
