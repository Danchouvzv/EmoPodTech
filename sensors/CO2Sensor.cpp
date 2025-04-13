#include "CO2Sensor.h"

void CO2Sensor::begin() {
    sensor.begin();
    sensor.setMeasurementInterval(2);
    sensor.setAutoSelfCalibration(true);
    for (int i = 0; i < NUM_SAMPLES; i++) {
        samples[i] = 0;
    }
}

float CO2Sensor::read() {
    if (sensor.dataAvailable()) {
        co2Value = sensor.getCO2();
        
        // Update moving average
        samples[sampleIndex] = co2Value;
        sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
        
        // Calculate average
        co2Average = 0;
        for (int i = 0; i < NUM_SAMPLES; i++) {
            co2Average += samples[i];
        }
        co2Average /= NUM_SAMPLES;
    }
    
    return co2Average;
}

float CO2Sensor::getAverage() {
    return co2Average;
}

float CO2Sensor::getHumidity() {
    return sensor.getHumidity();
}

float CO2Sensor::getTemperature() {
    return sensor.getTemperature();
} 