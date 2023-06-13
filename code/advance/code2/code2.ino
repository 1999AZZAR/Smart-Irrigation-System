#define BLYNK_TEMPLATE_ID "<your_template_id>"
#define BLYNK_DEVICE_NAME "<your_device_name>"
#define BLYNK_AUTH_TOKEN "<your_auth_token>"

#include <BlynkSimpleEsp8266.h>
#include <ArduinoJson.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "<your_wifi_ssid>";
char pass[] = "<your_wifi_password>";

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
}

BLYNK_WRITE(V4) {
  int buttonState = param.asInt();

  if (buttonState == HIGH) {
    activateRelay();
  } else {
    deactivateRelay();
  }
}

void activateRelay() {
  // Code to activate the relay here
  // Replace this with your own implementation
}

void deactivateRelay() {
  // Code to deactivate the relay here
  // Replace this with your own implementation
}

void processSensorData(const char* sensorData) {
  StaticJsonDocument<128> jsonDoc;
  DeserializationError error = deserializeJson(jsonDoc, sensorData);

  if (error) {
    Serial.print("JSON parsing error: ");
    Serial.println(error.c_str());
    return;
  }

  float temperature = jsonDoc["temperature"];
  float humidity = jsonDoc["humidity"];
  int soilMoisture = jsonDoc["soil_moisture"];
  int rainSensor = jsonDoc["rain_sensor"];

  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, soilMoisture);
  Blynk.virtualWrite(V3, rainSensor);
}

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE_DEFAULT() {
  const char* sensorData = param.asStr();
  processSensorData(sensorData);
}
