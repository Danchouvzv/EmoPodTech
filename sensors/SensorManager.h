#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <MAX30100_PulseOximeter.h>
#include <DHT.h>
#include <MQ135.h>

class SensorManager {
private:
    // Sensor objects
    Adafruit_MPU6050 mpu;
    PulseOximeter pox;
    DHT dht;
    MQ135 gasSensor;
    
    // Calibration data
    struct CalibrationData {
        float baselineGSR;
        float baselineTemp;
        float baselineCO2;
        float baselineHR;
        float baselineSpO2;
    } calibration;
    
    // Moving averages for smoothing
    struct MovingAverage {
        float values[10];
        int index;
        float sum;
        
        MovingAverage() : index(0), sum(0) {
            for (int i = 0; i < 10; i++) values[i] = 0;
        }
        
        float addValue(float value) {
            sum -= values[index];
            values[index] = value;
            sum += value;
            index = (index + 1) % 10;
            return sum / 10;
        }
    };
    
    MovingAverage hrAvg;
    MovingAverage gsrAvg;
    MovingAverage tempAvg;
    MovingAverage co2Avg;
    MovingAverage motionAvg;
    
    // Sensor pins
    const int GSR_PIN = 34;
    const int DHT_PIN = 35;
    const int MQ135_PIN = 36;
    
    // Timing
    unsigned long lastReadTime;
    const unsigned long READ_INTERVAL = 100; // 100ms
    
    bool isCalibrated;
    
public:
    struct SensorData {
        float heartRate;
        float spO2;
        float gsr;
        float temperature;
        float co2;
        float motion;
        float breathingRate;
        float soundLevel;
    };
    
    SensorManager() 
        : dht(DHT_PIN, DHT22), gasSensor(MQ135_PIN),
          lastReadTime(0), isCalibrated(false) {}
    
    void begin() {
        Wire.begin();
        
        // Initialize MPU6050
        if (!mpu.begin()) {
            Serial.println("Failed to find MPU6050 chip");
            while (1);
        }
        mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
        
        // Initialize Pulse Oximeter
        if (!pox.begin()) {
            Serial.println("Failed to find MAX30100 chip");
            while (1);
        }
        pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
        
        // Initialize DHT
        dht.begin();
        
        // Calibrate sensors
        calibrate();
    }
    
    void update() {
        unsigned long currentMillis = millis();
        if (currentMillis - lastReadTime >= READ_INTERVAL) {
            lastReadTime = currentMillis;
            pox.update();
        }
    }
    
    SensorData readSensors() {
        SensorData data;
        
        // Read heart rate and SpO2
        data.heartRate = hrAvg.addValue(pox.getHeartRate());
        data.spO2 = pox.getSpO2();
        
        // Read GSR
        int gsrRaw = analogRead(GSR_PIN);
        float gsrVoltage = (gsrRaw * 3.3) / 4095.0;
        data.gsr = gsrAvg.addValue(gsrVoltage);
        
        // Read temperature
        data.temperature = tempAvg.addValue(dht.readTemperature());
        
        // Read CO2
        float co2Raw = gasSensor.getPPM();
        data.co2 = co2Avg.addValue(co2Raw);
        
        // Read motion
        sensors_event_t a, g, temp;
        mpu.getEvent(&a, &g, &temp);
        float motion = sqrt(a.acceleration.x * a.acceleration.x +
                           a.acceleration.y * a.acceleration.y +
                           a.acceleration.z * a.acceleration.z);
        data.motion = motionAvg.addValue(motion);
        
        // Calculate breathing rate (placeholder - implement actual algorithm)
        data.breathingRate = 15.0; // Default value
        
        // Read sound level (placeholder - implement actual microphone reading)
        data.soundLevel = 0.0;
        
        return data;
    }
    
    void calibrate() {
        Serial.println("Calibrating sensors...");
        
        // Take multiple readings and average them
        const int CALIBRATION_SAMPLES = 100;
        float sumGSR = 0, sumTemp = 0, sumCO2 = 0, sumHR = 0, sumSpO2 = 0;
        
        for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
            update();
            SensorData data = readSensors();
            
            sumGSR += data.gsr;
            sumTemp += data.temperature;
            sumCO2 += data.co2;
            sumHR += data.heartRate;
            sumSpO2 += data.spO2;
            
            delay(10);
        }
        
        calibration.baselineGSR = sumGSR / CALIBRATION_SAMPLES;
        calibration.baselineTemp = sumTemp / CALIBRATION_SAMPLES;
        calibration.baselineCO2 = sumCO2 / CALIBRATION_SAMPLES;
        calibration.baselineHR = sumHR / CALIBRATION_SAMPLES;
        calibration.baselineSpO2 = sumSpO2 / CALIBRATION_SAMPLES;
        
        isCalibrated = true;
        Serial.println("Calibration complete");
    }
    
    bool isSensorCalibrated() const {
        return isCalibrated;
    }
    
    const CalibrationData& getCalibrationData() const {
        return calibration;
    }
};

#endif 