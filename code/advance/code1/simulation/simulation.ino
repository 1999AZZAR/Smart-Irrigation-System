#include <ArduinoJson.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  float temperature = random(0, 101);
  float humidity = random(0, 101);
  int soilMoisture = random(0, 101);
  int rainSensor = random(0, 101);

  // Create JSON data
  StaticJsonDocument<128> jsonDoc;
  jsonDoc["temperature"] = temperature;
  jsonDoc["humidity"] = humidity;
  jsonDoc["soil_moisture"] = soilMoisture;
  jsonDoc["rain_sensor"] = rainSensor;

  // Send JSON data over serial
  serializeJson(jsonDoc, Serial);
  Serial.println();

  delay(1000); // Delay between each reading
}
