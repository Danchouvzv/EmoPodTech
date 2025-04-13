#ifndef MICROPHONE_SENSOR_H
#define MICROPHONE_SENSOR_H

class MicrophoneSensor {
private:
    int sensorPin;
    float soundLevel;
    float soundAverage;
    const int NUM_SAMPLES = 10;
    float samples[10];
    int sampleIndex = 0;
    float maxAmplitude = 0;
    float minAmplitude = 1023;
    
public:
    MicrophoneSensor(int pin) : sensorPin(pin) {}
    
    void begin() {
        pinMode(sensorPin, INPUT);
        for (int i = 0; i < NUM_SAMPLES; i++) {
            samples[i] = 0;
        }
    }
    
    float read() {
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
    
    float getAverage() {
        return soundAverage;
    }
    
    float getMaxAmplitude() {
        return maxAmplitude;
    }
    
    float getMinAmplitude() {
        return minAmplitude;
    }
};

#endif 