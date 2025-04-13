#include "TemperatureSensor.h"

void TemperatureSensor::begin() {
    sensor.begin();
    sensor.setResolution(12);
    for (int i = 0; i < NUM_SAMPLES; i++) {
        samples[i] = 0;
    }
}

float TemperatureSensor::read() {
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

float TemperatureSensor::getAverage() {
    return temperatureAverage;
} 