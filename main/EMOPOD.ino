/*
 * EMOPOD - Emotional Regulation Smart Capsule
 * Main Arduino Sketch
 * 
 * This sketch controls all sensors and actuators for the EMOPOD project:
 * - Pulse Sensor (MAX30102)
 * - GSR Sensor
 * - Temperature Sensor (DS18B20)
 * - CO2 Sensor (SCD30)
 * - Breathing Sensor
 * - MPU6050 (Motion)
 * - Microphone
 * - RGB LEDs
 * - Sound Module
 */

#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>
#include <MAX30105.h>
#include <heartRate.h>
#include <SparkFun_SCD30_Arduino_Library.h>
#include "sensors/SensorManager.h"
#include "models/EmotionModel.h"
#include "network/NetworkManager.h"

// WiFi credentials
const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASSWORD = "your_wifi_password";
const char* SERVER_URL = "http://your-server.com/api/data";

// Create instances of our managers
SensorManager sensorManager;
EmotionModel emotionModel;
NetworkManager networkManager(WIFI_SSID, WIFI_PASSWORD, SERVER_URL);

// Timing
unsigned long lastSensorReadTime = 0;
const unsigned long SENSOR_READ_INTERVAL = 1000; // 1 second
unsigned long lastDataSendTime = 0;
const unsigned long DATA_SEND_INTERVAL = 5000; // 5 seconds

// Sensor objects
Adafruit_MPU6050 mpu;
MAX30105 particleSensor;
SCD30 airSensor;
OneWire oneWire(4); // DS18B20 on pin 4
DallasTemperature tempSensor(&oneWire);
Adafruit_NeoPixel pixels(8, 5, NEO_GRB + NEO_KHZ800); // 8 LEDs on pin 5

// Sensor data structure
struct SensorData {
  float heartRate;
  float gsr;
  float temperature;
  float co2;
  float breathingRate;
  float motionX;
  float motionY;
  float motionZ;
  float soundLevel;
};

SensorData currentData;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  // Initialize WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  
  // Initialize components
  sensorManager.begin();
  networkManager.begin();
  
  // Initialize sensors
  initializeSensors();
  
  // Initialize LED strip
  pixels.begin();
  pixels.show();
  
  Serial.println("\n[EMOPOD] Initialization complete");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Update network connection
  networkManager.update();
  
  // Read sensors periodically
  if (currentMillis - lastSensorReadTime >= SENSOR_READ_INTERVAL) {
    lastSensorReadTime = currentMillis;
    
    // Update sensor readings
    sensorManager.update();
    SensorManager::SensorData sensorData = sensorManager.readSensors();
    
    // Analyze emotional state
    EmotionModel::EmotionState state = emotionModel.analyze({
      sensorData.heartRate,
      sensorData.gsr,
      sensorData.temperature,
      sensorData.co2,
      sensorData.breathingRate,
      sensorData.motion,
      sensorData.motion,
      sensorData.motion,
      sensorData.soundLevel
    });
    
    // Print emotional state
    switch (state) {
      case EmotionModel::CALM:
        Serial.println("[EMOTION] State: CALM");
        break;
      case EmotionModel::MILD_STRESS:
        Serial.println("[EMOTION] State: MILD_STRESS");
        break;
      case EmotionModel::MODERATE_STRESS:
        Serial.println("[EMOTION] State: MODERATE_STRESS");
        break;
      case EmotionModel::HIGH_STRESS:
        Serial.println("[EMOTION] State: HIGH_STRESS");
        break;
      default:
        Serial.println("[EMOTION] State: UNKNOWN");
        break;
    }
  }
  
  // Send data to server periodically
  if (currentMillis - lastDataSendTime >= DATA_SEND_INTERVAL) {
    lastDataSendTime = currentMillis;
    
    if (networkManager.isWiFiConnected()) {
      // Create JSON document
      StaticJsonDocument<512> doc;
      
      // Add sensor data
      SensorManager::SensorData data = sensorManager.readSensors();
      doc["heartRate"] = data.heartRate;
      doc["spO2"] = data.spO2;
      doc["gsr"] = data.gsr;
      doc["temperature"] = data.temperature;
      doc["co2"] = data.co2;
      doc["motion"] = data.motion;
      doc["breathingRate"] = data.breathingRate;
      doc["soundLevel"] = data.soundLevel;
      
      // Add timestamp
      doc["timestamp"] = millis();
      
      // Send data
      if (!networkManager.sendData(doc)) {
        Serial.println("[ERROR] Failed to send data to server");
      }
    } else {
      Serial.println("[WARNING] Skipping data send - WiFi not connected");
    }
  }
  
  // Small delay to prevent watchdog reset
  delay(10);
}

void initializeSensors() {
  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1);
  }
  
  // Initialize MAX30102
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 was not found. Please check wiring/power.");
    while (1);
  }
  
  // Initialize SCD30
  if (!airSensor.begin()) {
    Serial.println("SCD30 not found. Please check wiring.");
    while (1);
  }
  
  // Initialize temperature sensor
  tempSensor.begin();
  
  // Configure sensors
  configureSensors();
}

void configureSensors() {
  // Configure MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  
  // Configure MAX30102
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
  
  // Configure SCD30
  airSensor.setMeasurementInterval(2);
  airSensor.setAutoSelfCalibration(true);
}

void readAllSensors() {
  // Read heart rate
  readHeartRate();
  
  // Read GSR
  readGSR();
  
  // Read temperature
  readTemperature();
  
  // Read CO2
  readCO2();
  
  // Read breathing
  readBreathing();
  
  // Read motion
  readMotion();
  
  // Read sound
  readSound();
}

void processAndSendData() {
  // Create JSON document
  StaticJsonDocument<512> doc;
  
  // Add sensor data to JSON
  doc["heart_rate"] = currentData.heartRate;
  doc["gsr"] = currentData.gsr;
  doc["temperature"] = currentData.temperature;
  doc["co2"] = currentData.co2;
  doc["breathing_rate"] = currentData.breathingRate;
  doc["motion_x"] = currentData.motionX;
  doc["motion_y"] = currentData.motionY;
  doc["motion_z"] = currentData.motionZ;
  doc["sound_level"] = currentData.soundLevel;
  
  // Send data to server
  sendDataToServer(doc);
}

void sendDataToServer(JsonDocument& doc) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    int httpResponseCode = http.POST(jsonString);
    
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
    
    http.end();
  }
}

void controlActuators() {
  // Control LEDs based on stress level
  controlLEDs();
  
  // Control sound module
  controlSound();
}

// Individual sensor reading functions will be implemented in separate files 