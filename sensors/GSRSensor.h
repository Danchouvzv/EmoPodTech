#ifndef GSR_SENSOR_H
#define GSR_SENSOR_H

class GSRSensor {
private:
    int sensorPin;
    float gsrValue;
    float gsrAverage;
    const int NUM_SAMPLES = 10;
    float samples[10];
    int sampleIndex = 0;
    
public:
    GSRSensor(int pin) : sensorPin(pin) {}
    
    void begin() {
        pinMode(sensorPin, INPUT);
        for (int i = 0; i < NUM_SAMPLES; i++) {
            samples[i] = 0;
        }
    }
    
    float read() {
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
    
    float getAverage() {
        return gsrAverage;
    }
};

#endif 