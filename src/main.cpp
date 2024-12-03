#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>

// WiFi Credentials
const char *ssid = "P424-2";
const char *password = "1234567899";

// Cập nhật thông tin cấu hình MQTT
const char *mqttServer = "192.168.1.11";   // Địa chỉ của MQTT broker
const int mqttPort = 1883;                 // Cổng MQTT
const char *mqttUser = "smarthome";        // Username của MQTT
const char *mqttPassword = "smarthome123"; // Password của MQTT broker

// Cập nhật MQTT Client ID, có thể sử dụng MAC address
String macAddress = WiFi.macAddress();
const char *mqttClientId = macAddress.c_str();

// DHT Sensor Setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
sensors_event_t event;

// SensorData cấu trúc để chứa thông tin cảm biến
struct SensorData
{
  float temperature;
  float humidity;
  int gasLevel;
  bool flameDetected;
};

// Gas and Flame sensor pins
const int gasSensorPin = 34;
const int flameSensorPin = 16;

// Khởi tạo đối tượng WiFiClient và PubSubClient (MQTT)
WiFiClient espClient;
PubSubClient client(espClient);

// Hàm kết nối WiFi
void connectToWifi()
{
  WiFi.mode(WIFI_STA);        // Đặt ESP32 ở chế độ STA (Station)
  WiFi.begin(ssid, password); // Kết nối WiFi với SSID và mật khẩu đã định nghĩa
  Serial.println("Connecting to WiFi...");

  unsigned long startMillis = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000); // Chờ 1 giây
    Serial.print(".");
    if (millis() - startMillis > 30000) // Timeout sau 30 giây
    {
      Serial.println("\nFailed to connect to WiFi!");
      return;
    }
  }

  // Kết nối thành công
  Serial.println("\nConnected to WiFi network");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // In địa chỉ IP của ESP32
}

// Callback nhận dữ liệu từ MQTT broker
// Callback nhận dữ liệu từ MQTT broker
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  String payloadStr = "";
  for (unsigned int i = 0; i < length; i++)
  {
    payloadStr += (char)payload[i]; // Xây dựng chuỗi payload từ mảng byte
  }

  Serial.print("Payload: ");
  Serial.println(payloadStr);

  // Điều khiển thiết bị theo lệnh nhận được
  if (String(topic).endsWith("/control"))
  {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payloadStr);

    String command = doc["command"];
    float value = doc["value"];

    if (command == "set_status")
    {
      // Xử lý lệnh bật/tắt thiết bị
      bool status = value > 0;
      Serial.print("Setting device status: ");
      Serial.println(status ? "ON" : "OFF");
      // Thực thi hành động (ví dụ: bật đèn)
    }
    else if (command == "set_brightness")
    {
      // Xử lý lệnh điều chỉnh độ sáng
      int brightness = (int)value;
      Serial.print("Setting brightness: ");
      Serial.println(brightness);
      // Thực thi hành động
    }
    else if (command == "set_temperature")
    {
      // Xử lý lệnh điều chỉnh nhiệt độ
      float temperature = value;
      Serial.print("Setting temperature: ");
      Serial.println(temperature);
      // Thực thi hành động
    }
    // Xử lý các lệnh khác nếu có
  }
}

// Kết nối đến MQTT Broker
void connectToMqtt()
{
  // Kiểm tra xem client đã kết nối chưa
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT...");

    // Cố gắng kết nối tới MQTT broker
    if (client.connect(mqttClientId, mqttUser, mqttPassword))
    {
      Serial.println("Connected to MQTT broker");

      // Đăng ký vào các topic cần theo dõi (theo mẫu roomId, deviceId)
      client.subscribe("home/+/+/control");
    }
    else
    {
      // Nếu kết nối thất bại, hiển thị mã lỗi và thử lại sau 5 giây
      Serial.print("Failed to connect to MQTT. Error code: ");
      Serial.println(client.state());
      delay(5000); // Chờ 5 giây trước khi thử lại
    }
  }
}

// Hàm gửi dữ liệu lên MQTT
void sendDataToMqtt(const String &roomId, const String &deviceId, const String &command, float value)
{
  // Xây dựng thông điệp theo cấu trúc của IStatusMessage và ICommandMessage
  String topic = "home/" + roomId + "/" + deviceId + "/control";                // Topic control
  String payload = "{\"command\":\"" + command + "\",\"value\":" + value + "}"; // JSON payload

  if (client.publish(topic.c_str(), payload.c_str()))
  {
    Serial.print("Published to topic: ");
    Serial.println(topic);
    Serial.print("Data: ");
    Serial.println(payload);
  }
  else
  {
    Serial.println("Failed to publish message");
  }
}

// Ví dụ sử dụng
// sendDataToMqtt("livingRoom", "device1", "set_status", 1.0); // Giả sử set_status với giá trị bật (1.0)

// Hàm gửi dữ liệu lên MQTT
void sendStatusToMqtt(const String &roomId, const String &deviceId, const String &status)
{
  String topic = "home/" + roomId + "/" + deviceId + "/status";
  String payload = "{\"status\":\"" + status + "\"}"; // JSON payload

  if (client.publish(topic.c_str(), payload.c_str()))
  {
    Serial.print("Published to topic: ");
    Serial.println(topic);
    Serial.print("Data: ");
    Serial.println(payload);
  }
  else
  {
    Serial.println("Failed to publish message");
  }
}

// Hàm đọc dữ liệu từ các cảm biến
SensorData readSensorData()
{
  SensorData data;

  // Đọc nhiệt độ
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

  // Đọc độ ẩm
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

  // Đọc mức khí gas
  int rawGasLevel = analogRead(gasSensorPin);
  data.gasLevel = rawGasLevel; // Giả sử giá trị raw là mức khí
  Serial.print("Gas level (raw): ");
  Serial.println(rawGasLevel);

  // Kiểm tra cảm biến lửa
  data.flameDetected = digitalRead(flameSensorPin) == LOW;
  Serial.print("Flame detected: ");
  Serial.println(data.flameDetected ? "true" : "false");

  return data;
}

// Thiết lập ban đầu
void setup()
{
  Serial.begin(115200);                   // Khởi động Serial Monitor với tốc độ 115200
  connectToWifi();                        // Kết nối WiFi
  client.setServer(mqttServer, mqttPort); // Cấu hình MQTT broker và port
  client.setCallback(mqttCallback);       // Thiết lập callback cho MQTT

  connectToMqtt(); // Kết nối đến MQTT broker

  dht.begin();                    // Khởi tạo DHT sensor
  pinMode(gasSensorPin, INPUT);   // Cảm biến khí
  pinMode(flameSensorPin, INPUT); // Cảm biến lửa
}

// Vòng lặp chính
void loop()
{
  // Kiểm tra kết nối MQTT và kết nối lại nếu cần
  if (!client.connected())
  {
    connectToMqtt();
  }

  // Gọi client.loop() để xử lý các sự kiện MQTT (nhận tin nhắn)
  client.loop();

  // Đọc dữ liệu cảm biến và gửi lên MQTT mỗi 5 giây
  static unsigned long lastPublishTime = 0;
  unsigned long currentTime = millis();

  if (currentTime - lastPublishTime >= 5000)
  {
    SensorData data = readSensorData();

    // Gửi dữ liệu lên các topic tương ứng cho từng phòng và thiết bị
    sendDataToMqtt("0af68ecd-a059-4ee3-89a1-7f459639c4b4", "5c4e075f-d4d6-47a5-a7e0-5913f8f5232a", "set_value", data.temperature);

    // Cập nhật thời gian gửi dữ liệu
    lastPublishTime = currentTime;
  }
}
