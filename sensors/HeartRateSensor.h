#ifndef HEART_RATE_SENSOR_H
#define HEART_RATE_SENSOR_H

#include <MAX30105.h>
#include <heartRate.h>

class HeartRateSensor {
private:
    MAX30105& sensor;
    long lastBeat = 0;
    float beatsPerMinute;
    int beatAvg;
    const int RATE_SIZE = 4;
    int rates[RATE_SIZE];
    int rateSpot = 0;
    
public:
    HeartRateSensor(MAX30105& sensorRef) : sensor(sensorRef) {}
    
    void begin() {
        sensor.setup();
        sensor.setPulseAmplitudeRed(0x0A);
        sensor.setPulseAmplitudeGreen(0);
    }
    
    float read() {
        long irValue = sensor.getIR();
        
        if (checkForBeat(irValue)) {
            long delta = millis() - lastBeat;
            lastBeat = millis();
            
            beatsPerMinute = 60 / (delta / 1000.0);
            
            if (beatsPerMinute < 255 && beatsPerMinute > 20) {
                rates[rateSpot++] = (int)beatsPerMinute;
                rateSpot %= RATE_SIZE;
                
                beatAvg = 0;
                for (int x = 0; x < RATE_SIZE; x++)
                    beatAvg += rates[x];
                beatAvg /= RATE_SIZE;
            }
        }
        
        return beatsPerMinute;
    }
    
    int getAverage() {
        return beatAvg;
    }
};

#endif 