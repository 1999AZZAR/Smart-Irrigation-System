#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Adafruit_MQTT_Client.h>
#include <ArduinoJson.h>

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "YOUR_AIO_USERNAME"
#define AIO_KEY "YOUR_AIO_KEY"

const char* ssid = "YOUR_SSID";
const char* password = "SSID_PASSWORD";

SoftwareSerial softSerial(D2, D3); // RX, TX
const int RELAY_PIN[] = {14, 12};

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish temp(&mqtt, AIO_USERNAME "/feeds/irrigation.temperature");
Adafruit_MQTT_Publish humi(&mqtt, AIO_USERNAME "/feeds/irrigation.humidity");
Adafruit_MQTT_Publish soil(&mqtt, AIO_USERNAME "/feeds/irrigation.soilMoisture");
Adafruit_MQTT_Publish rain(&mqtt, AIO_USERNAME "/feeds/irrigation.rainSensor");

Adafruit_MQTT_Subscribe bt1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/irrigation.relay1");
Adafruit_MQTT_Subscribe bt2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/irrigation.relay2");

void setup() {
  for (int i = 0; i < 2; i++) {
    pinMode(RELAY_PIN[i], OUTPUT);
  }
  Serial.begin(115200);
  softSerial.begin(9600);

  connectToWiFi();

  connectToMQTT();
}

void loop() {
  if (softSerial.available()) {
    String jsonStr = softSerial.readStringUntil('\n');
    deserializeAndPublish(jsonStr);
    Serial.println(jsonStr);
  }

  mqtt.processPackets(100);
  mqtt.ping();
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.println("Connected to WiFi");
}

void connectToMQTT() {
  int8_t ret;

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
  }

  Serial.println("MQTT connected!");
}

void deserializeAndPublish(const String& jsonStr) {
  DynamicJsonDocument jsonDoc(256);
  DeserializationError error = deserializeJson(jsonDoc, jsonStr);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  float temperature = jsonDoc["temperature"].as<float>();
  float humidity = jsonDoc["humidity"].as<float>();
  float soilMoisture = jsonDoc["soil_moisture"].as<float>();
  float rainSensor = jsonDoc["rain_sensor"].as<float>();

  publishValue(temp, temperature);
  publishValue(humi, humidity);
  publishValue(soil, soilMoisture);
  publishValue(rain, rainSensor);

  printSensorData("Temp", temperature);
  printSensorData("Humi", humidity);
  printSensorData("Soil", soilMoisture);
  printSensorData("Rain", rainSensor);
}

void relaySwitch(int index, int pinValue) {
  digitalWrite(RELAY_PIN[index], pinValue);
}

void onRelay2Change() {
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &bt1) {
      Serial.print(F("Got r1 : "));
      Serial.println((char *)bt1.lastread);
      int bt1_State = atoi((char *)bt1.lastread);
      relaySwitch(0, bt1_State);
    }
    if (subscription == &bt2) {
      Serial.print(F("Got r2 : "));
      Serial.println((char *)bt2.lastread);
      int bt2_State = atoi((char *)bt2.lastread);
      relaySwitch(1, bt2_State);
    }
  }
}

void publishValue(Adafruit_MQTT_Publish & feed, float value) {
  if (!feed.publish(value)) {
    Serial.println("Failed to publish data to MQTT topic");
  }
}

void printSensorData(const char* sensorName, float value) {
  Serial.print(sensorName);
  Serial.print(": ");
  Serial.println(value);
}
