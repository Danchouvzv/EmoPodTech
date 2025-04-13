#include "MicrophoneSensor.h"

void MicrophoneSensor::begin() {
    pinMode(sensorPin, INPUT);
    for (int i = 0; i < NUM_SAMPLES; i++) {
        samples[i] = 0;
    }
}

float MicrophoneSensor::read() {
    // Read raw microphone value
    int rawValue = analogRead(sensorPin);
    
    // Update min/max values for calibration
    if (rawValue > maxAmplitude) maxAmplitude = rawValue;
    if (rawValue < minAmplitude) minAmplitude = rawValue;
    
    // Normalize the value between 0 and 1
    float normalizedValue = (float)(rawValue - minAmplitude) / (maxAmplitude - minAmplitude);
    
    // Convert to decibels (simplified)
    soundLevel = 20 * log10(normalizedValue * 100);
    
    // Update moving average
    samples[sampleIndex] = soundLevel;
    sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
    
    // Calculate average
    soundAverage = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        soundAverage += samples[i];
    }
    soundAverage /= NUM_SAMPLES;
    
    return soundAverage;
}

float MicrophoneSensor::getAverage() {
    return soundAverage;
}

float MicrophoneSensor::getMaxAmplitude() {
    return maxAmplitude;
}

float MicrophoneSensor::getMinAmplitude() {
    return minAmplitude;
} 