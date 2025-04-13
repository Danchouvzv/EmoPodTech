#include "SensorManager.h"

void SensorManager::begin() {
    Wire.begin();
    
    // Initialize MPU6050
    if (!mpu.begin()) {
        Serial.println("[ERROR] Failed to find MPU6050 chip");
        while (1);
    }
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.println("[MPU6050] Initialized successfully");
    
    // Initialize Pulse Oximeter
    if (!pox.begin()) {
        Serial.println("[ERROR] Failed to find MAX30100 chip");
        while (1);
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    Serial.println("[MAX30100] Initialized successfully");
    
    // Initialize DHT
    dht.begin();
    Serial.println("[DHT22] Initialized successfully");
    
    // Initialize MQ135
    Serial.println("[MQ135] Initialized successfully");
    
    // Calibrate sensors
    calibrate();
}

void SensorManager::update() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastReadTime >= READ_INTERVAL) {
        lastReadTime = currentMillis;
        pox.update();
    }
}

SensorManager::SensorData SensorManager::readSensors() {
    SensorData data;
    
    // Read heart rate and SpO2
    float hr = pox.getHeartRate();
    if (!isnan(hr) && hr > 0 && hr < 200) {
        data.heartRate = hrAvg.addValue(hr);
        Serial.printf("[HR] Heart rate: %.1f BPM\n", data.heartRate);
    }
    
    float spo2 = pox.getSpO2();
    if (!isnan(spo2) && spo2 > 0 && spo2 <= 100) {
        data.spO2 = spo2;
        Serial.printf("[SpO2] Oxygen saturation: %.1f%%\n", data.spO2);
    }
    
    // Read GSR
    int gsrRaw = analogRead(GSR_PIN);
    if (gsrRaw > 0) {
        float gsrVoltage = (gsrRaw * 3.3) / 4095.0;
        data.gsr = gsrAvg.addValue(gsrVoltage);
        Serial.printf("[GSR] Skin conductance: %.2f uS\n", data.gsr);
    }
    
    // Read temperature
    float temp = dht.readTemperature();
    if (!isnan(temp) && temp > 0 && temp < 50) {
        data.temperature = tempAvg.addValue(temp);
        Serial.printf("[TEMP] Temperature: %.1f°C\n", data.temperature);
    }
    
    // Read CO2
    float co2Raw = gasSensor.getPPM();
    if (!isnan(co2Raw) && co2Raw > 0) {
        data.co2 = co2Avg.addValue(co2Raw);
        Serial.printf("[CO2] Concentration: %.1f ppm\n", data.co2);
    }
    
    // Read motion
    sensors_event_t a, g, temp;
    if (mpu.getEvent(&a, &g, &temp)) {
        float motion = sqrt(a.acceleration.x * a.acceleration.x +
                           a.acceleration.y * a.acceleration.y +
                           a.acceleration.z * a.acceleration.z);
        data.motion = motionAvg.addValue(motion);
        Serial.printf("[MOTION] Magnitude: %.2f m/s²\n", data.motion);
    }
    
    // Calculate breathing rate (placeholder - implement actual algorithm)
    data.breathingRate = 15.0; // Default value
    Serial.printf("[BREATH] Rate: %.1f BPM\n", data.breathingRate);
    
    // Read sound level (placeholder - implement actual microphone reading)
    data.soundLevel = 0.0;
    Serial.printf("[SOUND] Level: %.1f dB\n", data.soundLevel);
    
    return data;
}

void SensorManager::calibrate() {
    Serial.println("[CALIBRATION] Starting sensor calibration...");
    
    const int CALIBRATION_SAMPLES = 100;
    float sumGSR = 0, sumTemp = 0, sumCO2 = 0, sumHR = 0, sumSpO2 = 0;
    int validSamples = 0;
    
    for (int i = 0; i < CALIBRATION_SAMPLES; i++) {
        update();
        SensorData data = readSensors();
        
        if (!isnan(data.gsr) && !isnan(data.temperature) && 
            !isnan(data.co2) && !isnan(data.heartRate) && 
            !isnan(data.spO2)) {
            sumGSR += data.gsr;
            sumTemp += data.temperature;
            sumCO2 += data.co2;
            sumHR += data.heartRate;
            sumSpO2 += data.spO2;
            validSamples++;
        }
        
        delay(10);
    }
    
    if (validSamples > 0) {
        calibration.baselineGSR = sumGSR / validSamples;
        calibration.baselineTemp = sumTemp / validSamples;
        calibration.baselineCO2 = sumCO2 / validSamples;
        calibration.baselineHR = sumHR / validSamples;
        calibration.baselineSpO2 = sumSpO2 / validSamples;
        
        Serial.println("[CALIBRATION] Complete");
        Serial.printf("[CALIBRATION] GSR baseline: %.2f uS\n", calibration.baselineGSR);
        Serial.printf("[CALIBRATION] Temperature baseline: %.1f°C\n", calibration.baselineTemp);
        Serial.printf("[CALIBRATION] CO2 baseline: %.1f ppm\n", calibration.baselineCO2);
        Serial.printf("[CALIBRATION] HR baseline: %.1f BPM\n", calibration.baselineHR);
        Serial.printf("[CALIBRATION] SpO2 baseline: %.1f%%\n", calibration.baselineSpO2);
        
        isCalibrated = true;
    } else {
        Serial.println("[ERROR] Calibration failed - no valid samples");
        isCalibrated = false;
    }
} 