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

  delay(2500); // Delay between each reading
}
