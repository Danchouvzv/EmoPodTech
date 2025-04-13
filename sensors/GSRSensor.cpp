#include "GSRSensor.h"

void GSRSensor::begin() {
    pinMode(sensorPin, INPUT);
    for (int i = 0; i < NUM_SAMPLES; i++) {
        samples[i] = 0;
    }
}

float GSRSensor::read() {
    // Read raw GSR value
    int rawValue = analogRead(sensorPin);
    
    // Convert to conductance (microsiemens)
    float conductance = (1023.0 / rawValue - 1.0) * 10000.0;
    
    // Update moving average
    samples[sampleIndex] = conductance;
    sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
    
    // Calculate average
    gsrAverage = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        gsrAverage += samples[i];
    }
    gsrAverage /= NUM_SAMPLES;
    
    return gsrAverage;
}

float GSRSensor::getAverage() {
    return gsrAverage;
} 