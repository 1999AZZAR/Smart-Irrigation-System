#define BLYNK_TEMPLATE_ID "<your_template_id>"
#define BLYNK_DEVICE_NAME "<your_device_name>"
#define BLYNK_AUTH_TOKEN "<your_auth_token>"

#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "<your_wifi_ssid>";
char pass[] = "<your_wifi_password>";

// SoftwareSerial pins for communication with Arduino Uno
#define SOFT_SERIAL_RX_PIN D2
#define SOFT_SERIAL_TX_PIN D3

SoftwareSerial softSerial(SOFT_SERIAL_RX_PIN, SOFT_SERIAL_TX_PIN);  // RX, TX

const int RELAY_PIN[] = {14, 12};

BlynkTimer timer;

void setup() {
  for (int i = 0; i < 2; i++) {
    pinMode(RELAY_PIN[i], OUTPUT);
  }
  timer.setInterval(1000L, sensorDataSend); //timer will run every sec
  Serial.begin(115200);
  softSerial.begin(9600);  // Start software serial communication with Arduino Uno
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  timer.run();
}

void sensorDataSend() {
  if (softSerial.available()) {
    String jsonStr = softSerial.readStringUntil('\n'); // Read JSON data from Arduino
    deserializeAndPublish(jsonStr);
    Serial.println(jsonStr);
  }
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
  int soilMoisture = jsonDoc["soil_moisture"].as<int>(); // Changed to int
  int rainSensor = jsonDoc["rain_sensor"].as<int>(); // Changed to int

  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, soilMoisture);
  Blynk.virtualWrite(V3, rainSensor);
}

void relaySwitch(int index, int pinValue) {
  digitalWrite(RELAY_PIN[index], pinValue);
}

BLYNK_WRITE(V4) {
  relaySwitch(0, param.asInt());
}

BLYNK_WRITE(V5) {
  relaySwitch(1, param.asInt());
}
