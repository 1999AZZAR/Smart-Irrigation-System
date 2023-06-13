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
const byte arduinoRxPin = 2;  // ESP8266 Tx pin
const byte arduinoTxPin = 3;  // ESP8266 Rx pin
SoftwareSerial arduinoSerial(arduinoRxPin, arduinoTxPin);

const int RELAY_PIN[] = {16, 5};

void setup() {
  Serial.begin(9600);
  arduinoSerial.begin(9600);  // Start software serial communication with Arduino Uno
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  processSensorData();
}

void processSensorData() {
  if (arduinoSerial.available()) {
    StaticJsonDocument<128> jsonDoc;
    DeserializationError error;

    // Read data from Arduino Uno
    String arduinoData = arduinoSerial.readStringUntil('\n');
    
    error = deserializeJson(jsonDoc, arduinoData);

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
}

void relaySwitch(int index, int pinValue) {
  digitalWrite(RELAY_PIN[index], pinValue);
}

BLYNK_WRITE(V4) { relaySwitch(0, param.asInt()); }
BLYNK_WRITE(V5) { relaySwitch(1, param.asInt()); } 

BLYNK_CONNECTED() {
  Blynk.syncAll();
}

BLYNK_WRITE_DEFAULT() {
  // This function is not used in this code, but you can keep it for future use
  // You can add your own implementation if needed
}
