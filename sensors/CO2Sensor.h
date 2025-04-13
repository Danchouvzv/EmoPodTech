#ifndef CO2_SENSOR_H
#define CO2_SENSOR_H

#include <SparkFun_SCD30_Arduino_Library.h>

class CO2Sensor {
private:
    SCD30& sensor;
    float co2Value;
    float co2Average;
    const int NUM_SAMPLES = 5;
    float samples[5];
    int sampleIndex = 0;
    
public:
    CO2Sensor(SCD30& sensorRef) : sensor(sensorRef) {}
    
    void begin() {
        sensor.begin();
        sensor.setMeasurementInterval(2);
        sensor.setAutoSelfCalibration(true);
        for (int i = 0; i < NUM_SAMPLES; i++) {
            samples[i] = 0;
        }
    }
    
    float read() {
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
    
    float getAverage() {
        return co2Average;
    }
    
    float getHumidity() {
        return sensor.getHumidity();
    }
    
    float getTemperature() {
        return sensor.getTemperature();
    }
};

#endif 