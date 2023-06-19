#include <DHT.h>
#include <ArduinoJson.h>

#define SOIL_MOISTURE_PIN A0
#define RAIN_SENSOR_PIN A1
#define BUTTON_PIN 2
#define RELAY_1_PIN 3
#define RELAY_2_PIN 4

#define DHT_TYPE DHT11
#define DHT_PIN 5

unsigned long previousMillis = 0;
const long interval = 1000;

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RELAY_1_PIN, OUTPUT);
  pinMode(RELAY_2_PIN, OUTPUT);
  dht.begin();
}

void loop() {
  checkButton();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    checkSensorData();
  }
}

void checkButton() {
  static bool buttonState = HIGH;
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (currentButtonState != buttonState) {
    buttonState = currentButtonState;
    if (buttonState == LOW) {
      activateRelay();
    } else {
      deactivateRelay();
    }
  }
}

void checkSensorData() {
  float temperature, humidity;
  float soilMoisture, rainSensor;

  // Read temperature and humidity
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Check for sensor reading errors
  if (isnan(temperature) || isnan(humidity)) {
    handleSensorError();
    return;
  }

  // Read soil moisture and rain sensor
  soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  rainSensor = analogRead(RAIN_SENSOR_PIN);

  // Convert analog readings to 0-100 scale
  soilMoisture = map(soilMoisture, 0, 1023, 0, 100);
  rainSensor = map(rainSensor, 0, 1023, 0, 100);

  // Create JSON data
  DynamicJsonDocument jsonDoc(256);

  // Populate the JSON document
  jsonDoc["temperature"] = String(temperature);
  jsonDoc["humidity"] = String(humidity);
  jsonDoc["soil_moisture"] = String(soilMoisture);
  jsonDoc["rain_sensor"] = String(rainSensor);

  // Serialize the JSON document to a string
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Send the JSON data over serial
  Serial.println(jsonString);
}

void handleSensorError() {
  // Handle sensor reading errors here
  // You can add your own error handling mechanism
}

void activateRelay() {
  digitalWrite(RELAY_1_PIN, HIGH);
  digitalWrite(RELAY_2_PIN, HIGH);
}

void deactivateRelay() {
  digitalWrite(RELAY_1_PIN, LOW);
  digitalWrite(RELAY_2_PIN, LOW);
}
