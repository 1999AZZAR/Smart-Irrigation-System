#include <DHT.h>

#define DHTPIN 2 // Pin connected to DHT11 sensor
#define DHTTYPE DHT11 // DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE);

const int soilMoisturePin = A0; // Pin connected to soil moisture sensor
const int rainSensorPin = A1; // Pin connected to rain sensor
const int relayPin = 9; // Pin connected to the relay module
const int buttonPin = 3; // Pin connected to the push button

boolean isWatering = false; // Variable to track watering status
unsigned long lastSensorCheckTime = 0; // Last time the sensor data was checked
const unsigned long sensorCheckInterval = 1000; // Interval between sensor data checks (in milliseconds)

void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  dht.begin();
}

void loop() {
  checkButton();

  // Check if it's time to check the sensor data
  unsigned long currentMillis = millis();
  if (currentMillis - lastSensorCheckTime >= sensorCheckInterval) {
    lastSensorCheckTime = currentMillis;
    checkSensors();
  }
}

void checkSensors() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int RsoilMoisture = analogRead(soilMoisturePin);
  int soilMoisture = map(RsoilMoisture,0,1023,0,100);
  int RrainSensor = analogRead(rainSensorPin);
  int rainSensor = map(RrainSensor,0,1023,0,100);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Error: Failed to read temperature and humidity from DHT11 sensor");
    return;
  }

  if (soilMoisture < 0 || soilMoisture > 1023) {
    Serial.println("Error: Failed to read soil moisture sensor");
    return;
  }

  if (rainSensor < 0 || rainSensor > 1023) {
    Serial.println("Error: Failed to read rain sensor");
    return;
  }

  // Create JSON data
  String jsonData = "{";
  jsonData += "\"temperature\":" + String(temperature) + ",";
  jsonData += "\"humidity\":" + String(humidity) + ",";
  jsonData += "\"soilMoisture\":" + String(soilMoisture) + ",";
  jsonData += "\"rainSensor\":" + String(rainSensor);
  jsonData += "}";

  Serial.println(jsonData); // Send JSON data over serial

  // Check watering conditions and control actuator
  checkWateringConditions(temperature, humidity, soilMoisture, rainSensor);
}

void checkButton() {
  boolean buttonState = digitalRead(buttonPin);

  if (buttonState == LOW && !isWatering) {
    isWatering = true;
    activateActuator();
  } else if (buttonState == HIGH && isWatering) {
    isWatering = false;
    deactivateActuator();
  }
}

void checkWateringConditions(float temperature, float humidity, int soilMoisture, int rainSensor) {
  if (isWatering) {
    // If manual watering is active, bypass the conditions and keep the actuator activated
    return;
  }

  if (soilMoisture < 40 && rainSensor == 0) {
    activateActuator();
  } else if (humidity < 50 && temperature > 30) {
    activateActuator();
  } else if (soilMoisture >= 75 || rainSensor > 0) {
    deactivateActuator();
  }
}

void activateActuator() {
  digitalWrite(relayPin, HIGH); // Turn on the relay
  Serial.println("Actuator activated");
}

void deactivateActuator() {
  digitalWrite(relayPin, LOW); // Turn off the relay
  Serial.println("Actuator deactivated");
}
