# EmoPodTech - AI-Powered Emotional State Monitoring Device

EmoPodTech is an advanced IoT device that monitors and analyzes emotional states using multiple biometric sensors. It provides real-time feedback and data visualization for stress management and emotional well-being.

## 🚀 Features

- **Multi-Sensor Integration**
  - Heart Rate & SpO2 (MAX30100)
  - Galvanic Skin Response (GSR)
  - Body Temperature (DHT22)
  - CO2 Levels (MQ135)
  - Motion Detection (MPU6050)
  - Sound Level Analysis
  - Breathing Rate Monitoring

- **Advanced Analytics**
  - Real-time emotional state classification
  - Dynamic baseline calibration
  - Trend analysis and pattern recognition
  - Stress score calculation

- **Robust Connectivity**
  - WiFi with automatic reconnection
  - Data buffering for offline operation
  - Secure data transmission
  - Cloud integration

- **Power Management**
  - Deep sleep optimization
  - Battery life monitoring
  - Power-efficient sensor sampling

## 📁 Project Structure

```
EmoPodTech/
├── src/
│   ├── models/           # Emotion analysis models
│   ├── sensors/          # Sensor drivers and management
│   ├── network/          # Network and cloud connectivity
│   └── utils/            # Utility classes and helpers
├── main/                 # Main application code
├── lib/                  # External libraries
└── docs/                 # Documentation
```

## 🛠️ Hardware Requirements

- ESP32 Development Board
- MAX30100 Pulse Oximeter
- GSR Sensor
- DHT22 Temperature Sensor
- MQ135 Gas Sensor
- MPU6050 Motion Sensor
- Microphone Module
- Breathing Sensor
- RGB LED Strip
- Power Management Circuit

## 🔧 Software Requirements

- Arduino IDE 2.0+
- ESP32 Board Support Package
- Required Libraries:
  - ArduinoJson
  - Adafruit_MPU6050
  - MAX30100_PulseOximeter
  - DHT sensor library
  - MQ135
  - FastLED

## 🚀 Getting Started

1. **Clone the Repository**
   ```bash
   git clone https://github.com/Danchouvzv/EmoPodTech.git
   ```

2. **Install Dependencies**
   - Install Arduino IDE
   - Install ESP32 Board Support
   - Install required libraries through Arduino Library Manager

3. **Configure WiFi**
   - Open `main/EMOPOD.ino`
   - Update WiFi credentials:
     ```cpp
     const char* WIFI_SSID = "your_wifi_ssid";
     const char* WIFI_PASSWORD = "your_wifi_password";
     ```

4. **Upload the Code**
   - Connect your ESP32
   - Select the correct board and port
   - Upload the sketch

## 📊 Data Flow

```
Sensors → Data Collection → Preprocessing → Emotion Analysis → Feedback/Cloud
```

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Samsung Innovation Campus
- Arduino Community
- Open Source Contributors

## 📞 Contact

Danil Chuvilin - [@Danchouvzv](https://github.com/Danchouvzv)

Project Link: [https://github.com/Danchouvzv/EmoPodTech](https://github.com/Danchouvzv/EmoPodTech)

SensorManager sensorManager;
sensorManager.begin();

// In your main loop:
sensorManager.update();
SensorManager::SensorData data = sensorManager.readSensors(); 