#include <DHT.h>
#include <ArduinoJson.h>

#define SOIL_MOISTURE_PIN A0
#define RAIN_SENSOR_PIN A1
#define BUTTON_PIN 2
#define RELAY_1_PIN 3
#define RELAY_2_PIN 4

#define DHT_TYPE DHT11
#define DHT_PIN 5

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
  checkSensorData();
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
  int soilMoisture, rainSensor;

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
  StaticJsonDocument<128> jsonDoc;
  jsonDoc["temperature"] = temperature;
  jsonDoc["humidity"] = humidity;
  jsonDoc["soil_moisture"] = soilMoisture;
  jsonDoc["rain_sensor"] = rainSensor;

  // Send JSON data over serial
  serializeJson(jsonDoc, Serial);
  Serial.println();
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
