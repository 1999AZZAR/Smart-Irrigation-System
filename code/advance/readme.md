## Smart Irrigation System (Advance)

This Arduino sketch enables you to monitor sensor data (temperature, humidity, soil moisture, and rain) using Blynk, an IoT platform, and control an actuator (relay) remotely. The sketch receives JSON data over the Blynk server and uploads the sensor values to specific virtual pins. The actuator control is replaced by a virtual pin on the Blynk app.

### Prerequisites

- ESP8266-based board (e.g., NodeMCU)
- AVR-based board (e.g., Arduino UNO)
- Dual relay module
- Capacitive soil moisture sensor
- Analog rain sensor (resistance-based)
- DHT11 temperature and humidity sensor
- Jumper wires
- Breadboard (optional)
- Blynk mobile app (iOS or Android)

### Installation

1. Install the Arduino IDE on your computer.
2. Connect the ESP8266 board to your computer using a USB cable.
3. Install the necessary libraries: Blynk library, ArduinoJson library.
4. Open the Arduino sketch file (`sensor_data_monitoring.ino`) in the Arduino IDE.
5. Customize the sketch by replacing the placeholders with your Blynk project information and Wi-Fi credentials.

### Usage

1. Create a new project in the Blynk mobile app.
2. Set up the necessary widgets in the Blynk app.
3. Obtain the Blynk authorization token for your project.
4. Replace the placeholders in the sketch with your actual information.
5. Upload the sketch to the ESP8266 board.
6. Power on the board and ensure it is connected to the Wi-Fi network.
7. Open the Blynk mobile app and navigate to your project.
8. Sensor data will be displayed on the corresponding virtual pins (V0 to V3).
9. Use virtual pin V4 on the Blynk app to control the actuator (relay).

### Flowchart

```mermaid
graph TD

subgraph Arduino
A[Start] --> B[Setup]
B --> C[Main Loop]
C --> D[Check Button]
C --> E[Check Sensor Data]
E --> F[Read Temperature and Humidity]
F --> H[Check Sensor Reading Errors]
E --> G[Read Soil Moisture and Rain Sensor]
G --> H
H -- Error --> I[Handle Error]
H -- No Error --> J[Convert Analog Readings to 0-100 Scale]
J --> K[Create JSON Data]
K --> L[Send JSON Data over Serial]
L --> op9((ESP8266: Read Data))
end

subgraph ESP8266
op9 --> op10[Check if data available from Arduino Uno]
cond1[Data available?]
op10 --> cond1
cond1 -- Yes --> op11[Read data from Arduino Uno]
op11 --> op12[Parse JSON data]
cond1 -- No --> op13[End]
op12 --> op14[Check parsing error]
cond2[Error occurred?]
op14 --> cond2
cond2 -- Yes --> op15[Print error message]
cond2 -- No --> op16[Retrieve sensor values]
op16 --> op17[Update Blynk virtual pins]
op15 --> op13
op17 --> op13
op13 --> op8((Main Loop))
end

subgraph Relay Control
op19[Relay write event] --> op18[relay status]
op19 --> op17
end

subgraph Button
D -- pressed --> M[Activate Actuator]
D -- released --> N[Deactivate Actuator]
M --> O[Actuator Status]
N --> O
O --> C
end

subgraph Blynk Connection
op20[Blynk connected event] --> op21[Blynk syncAll]
op21 --> op8
end

subgraph Default Write Event
op22[Default write event] --> op8
end

subgraph End
e((End))
end

op18 --> D
op8 --> e
```

### Customization

- Modify virtual pins and widgets in the Blynk app to match your project setup.
- Replace the relay control functions with your own implementation.
- Extend the sketch to include additional sensors or actuators as needed.

For troubleshooting, refer to the Blynk documentation and community resources.
