#include "HeartRateSensor.h"

// Implementation of HeartRateSensor class methods
void HeartRateSensor::begin() {
    sensor.setup();
    sensor.setPulseAmplitudeRed(0x0A);
    sensor.setPulseAmplitudeGreen(0);
}

float HeartRateSensor::read() {
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

int HeartRateSensor::getAverage() {
    return beatAvg;
} 