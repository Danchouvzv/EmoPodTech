#ifndef BREATHING_SENSOR_H
#define BREATHING_SENSOR_H

class BreathingSensor {
private:
    int sensorPin;
    float breathingRate;
    float breathingAverage;
    const int NUM_SAMPLES = 10;
    float samples[10];
    int sampleIndex = 0;
    unsigned long lastPeakTime = 0;
    int peakCount = 0;
    float lastValue = 0;
    bool rising = false;
    
public:
    BreathingSensor(int pin) : sensorPin(pin) {}
    
    void begin() {
        pinMode(sensorPin, INPUT);
        for (int i = 0; i < NUM_SAMPLES; i++) {
            samples[i] = 0;
        }
    }
    
    float read() {
        float currentValue = analogRead(sensorPin);
        
        // Detect peaks in breathing pattern
        if (currentValue > lastValue && !rising) {
            rising = true;
        } else if (currentValue < lastValue && rising) {
            rising = false;
            peakCount++;
            
            // Calculate breathing rate (breaths per minute)
            if (peakCount >= 2) {
                unsigned long currentTime = millis();
                unsigned long timeDiff = currentTime - lastPeakTime;
                breathingRate = 60000.0 / timeDiff; // Convert to breaths per minute
                lastPeakTime = currentTime;
                peakCount = 0;
                
                // Update moving average
                samples[sampleIndex] = breathingRate;
                sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
                
                // Calculate average
                breathingAverage = 0;
                for (int i = 0; i < NUM_SAMPLES; i++) {
                    breathingAverage += samples[i];
                }
                breathingAverage /= NUM_SAMPLES;
            }
        }
        
        lastValue = currentValue;
        return breathingAverage;
    }
    
    float getAverage() {
        return breathingAverage;
    }
};

#endif 