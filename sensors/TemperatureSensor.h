#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <OneWire.h>
#include <DallasTemperature.h>

class TemperatureSensor {
private:
    DallasTemperature& sensor;
    float temperature;
    float temperatureAverage;
    const int NUM_SAMPLES = 5;
    float samples[5];
    int sampleIndex = 0;
    
public:
    TemperatureSensor(DallasTemperature& sensorRef) : sensor(sensorRef) {}
    
    void begin() {
        sensor.begin();
        sensor.setResolution(12);
        for (int i = 0; i < NUM_SAMPLES; i++) {
            samples[i] = 0;
        }
    }
    
    float read() {
        sensor.requestTemperatures();
        temperature = sensor.getTempCByIndex(0);
        
        // Update moving average
        samples[sampleIndex] = temperature;
        sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
        
        // Calculate average
        temperatureAverage = 0;
        for (int i = 0; i < NUM_SAMPLES; i++) {
            temperatureAverage += samples[i];
        }
        temperatureAverage /= NUM_SAMPLES;
        
        return temperatureAverage;
    }
    
    float getAverage() {
        return temperatureAverage;
    }
};

#endif 