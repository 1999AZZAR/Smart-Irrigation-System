/********************************************************
*        copyright : github.com/1999azzar               *
*        year      : 2023                               *
*                                                       *
*        Project   : code2.ino(esp8266)                 *
*********************************************************/

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
const int BUTTON_PIN = 5; // Physical button connected to GPIO pin 5

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish temp(&mqtt, AIO_USERNAME "/feeds/irrigation.temperature");
Adafruit_MQTT_Publish humi(&mqtt, AIO_USERNAME "/feeds/irrigation.humidity");
Adafruit_MQTT_Publish soil(&mqtt, AIO_USERNAME "/feeds/irrigation.soilMoisture");
Adafruit_MQTT_Publish rain(&mqtt, AIO_USERNAME "/feeds/irrigation.rainSensor");

Adafruit_MQTT_Subscribe bt1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/irrigation.relay1");
Adafruit_MQTT_Subscribe bt2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/irrigation.relay2");

bool mqttConnected = false;
bool buttonState = false;
bool previousButtonState = false;

void setup() {
  Serial.begin(115200);
  softSerial.begin(9600);

  connectToWiFi();
  connectToMQTT();

  for (int i = 0; i < 2; i++) {
    pinMode(RELAY_PIN[i], OUTPUT);
  }
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
    connectToMQTT();
  }

  if (mqttConnected) {
    mqtt.processPackets(200);
    mqtt.ping();
    onRelayChange();
  }

  if (softSerial.available()) {
    String jsonStr = softSerial.readStringUntil('\n');
    deserializeAndPublish(jsonStr);
    Serial.println(jsonStr);
  }

  // Read the state of the button
  buttonState = digitalRead(BUTTON_PIN);

  // Check if the button state has changed
  if (buttonState != previousButtonState) {
    delay(50); // Debounce delay
    if (buttonState == LOW) {
      toggleRelayValue(0); // Toggle bt1 value
      toggleRelayValue(1); // Toggle bt2 value
    }
  }

  previousButtonState = buttonState;
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
  uint32_t connectDelay = 5000;

  Serial.print("Connecting to MQTT... ");
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    delay(connectDelay);
    connectDelay += 5000; // Increment the delay for each retry
  }
  mqttConnected = true;
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

void onRelayChange() {
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

void relaySwitch(int index, int pinValue) {
  digitalWrite(RELAY_PIN[index], pinValue);
}

void toggleRelayValue(int index) {
  int currentValue = digitalRead(RELAY_PIN[index]);
  int newValue = (currentValue == HIGH) ? LOW : HIGH;
  digitalWrite(RELAY_PIN[index], newValue);
  publishValue(index, newValue);
}

void publishValue(int index, int value) {
  Adafruit_MQTT_Publish* feed;
  if (index == 0) {
    feed = &bt1;
  } else if (index == 1) {
    feed = &bt2;
  } else {
    return;
  }

  if (!feed->publish(value)) {
    Serial.println("Failed to publish data to MQTT topic");
  }
}

void printSensorData(const char* sensorName, float value) {
  Serial.print(sensorName);
  Serial.print(": ");
  Serial.println(value);
}
