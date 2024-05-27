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
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 4
#define DHTTYPE DHT11
const int gasSensorPin = 34;
const int flameSensorPin = 16;
DHT_Unified dht(DHTPIN, DHTTYPE);
sensors_event_t event;

JsonDocument doc;
const char *ssidAP = "P424-2";
const char *passwordAP = "0947900523";
const char *BASE_URL = "http://157.10.52.61:3000";
const char *serverName = "http://157.10.52.61:3000/device/";
HTTPClient http;

void ensureWifiConnected();
void connectToWifi();
void fetchDataDevices();
void updateDevice(Device *device);
void initDevice(Vector<Device *> devicesData);
void sendPatchRequest(int id, float value);
void sendStatus(int id, bool status);
struct SensorData
{
  float temperature;
  float humidity;
  int gasLevel;
  bool flameDetected;
};
SensorData senSorData();
int processGasValue(int rawValue);
void setup()
{
  Serial.begin(115200);
  connectToWifi();
  fetchDataDevices();
  dht.begin();
  pinMode(gasSensorPin, INPUT);
  pinMode(flameSensorPin, INPUT);
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

void ensureWifiConnected()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected, trying to reconnect...");
    connectToWifi();
  }
}

void loop()
{
  static unsigned long lastFetchTime = 0;
  unsigned long currentTime = millis();

  ensureWifiConnected();

  // Fetch data every 3 seconds
  if (currentTime - lastFetchTime >= 2000)
  {
    fetchDataDevices();
    lastFetchTime = currentTime;
    int idTemp = 11;
    int idHum = 21;
    int idGas = 12;
    int idFlame = 25;

    SensorData data = senSorData();
    sendPatchRequest(idTemp, data.temperature);
    sendPatchRequest(idHum, data.humidity);
    sendPatchRequest(idGas, data.gasLevel);
     sendStatus(idGas, data.gasLevel > 400); 
    sendStatus(idFlame, data.flameDetected);
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
  http.setTimeout(1000);
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

SensorData senSorData()
{
  SensorData data;
  sensors_event_t event;

  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println("Error reading temperature!");
    data.temperature = NAN;
  }
  else
  {
    data.temperature = event.temperature;
    Serial.print("Temperature: ");
    Serial.print(data.temperature);
    Serial.println("°C");
  }

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println("Error reading humidity!");
    data.humidity = NAN;
  }
  else
  {
    data.humidity = event.relative_humidity;
    Serial.print("Humidity: ");
    Serial.print(data.humidity);
    Serial.println("%");
  }
  int rawGasLevel = analogRead(gasSensorPin);
    data.gasLevel = processGasValue(rawGasLevel);
    Serial.print("Gas level (raw): ");
    Serial.println(rawGasLevel);
    Serial.print("Gas level (processed): ");
    Serial.println(data.gasLevel);

  data.flameDetected = digitalRead(flameSensorPin) == LOW;
  Serial.print("Flame detected: ");
  Serial.println(data.flameDetected ? "true" : "false");

  return data;
}

int processGasValue(int rawValue) {
    const int BASELINE = 651;
    int processedValue = rawValue - BASELINE;
    return processedValue > 0 ? processedValue : 0;
}

void sendPatchRequest(int id, float value)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    String serverPath = String(serverName) + String(id);
    http.begin(serverPath.c_str());
    http.setTimeout(2000); 
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"value\": " + String(value) + "}";

    int httpResponseCode = http.PATCH(jsonPayload);

    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    }
    else
    {
      Serial.println("Error on sending PATCH request: " + String(httpResponseCode));
    }

    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
}

void sendStatus(int id, bool status)
{
  if (WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;

    String serverPath = String(serverName) + String(id);
    http.begin(serverPath.c_str());
    http.addHeader("Content-Type", "application/json");
    String statusString = status ? "true" : "false";
    String jsonPayload = "{\"status\":" + statusString + "}";

    int httpResponseCode = http.PATCH(jsonPayload);

    if (httpResponseCode > 0)
    {
      String response = http.getString();
      Serial.println("HTTP Response code: " + String(httpResponseCode));
      Serial.println("Response: " + response);
    }
    else
    {
      Serial.println("Error on sending PATCH request: " + String(httpResponseCode));
    }
    http.end();
  }
  else
  {
    Serial.println("WiFi Disconnected");
  }
}
