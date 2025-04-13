#ifndef EMOTION_MODEL_H
#define EMOTION_MODEL_H

#include <Arduino.h>

/*
 * EmotionModel - Analyzes sensor data to determine emotional state
 * 
 * Input Parameters:
 * - Heart Rate (BPM): Normal range 60-100, elevated indicates stress
 * - GSR (microsiemens): Higher values indicate stress
 * - Temperature (C): Normal range 36.5-37.5, elevated may indicate stress
 * - CO2 (ppm): Normal range 400-1000, higher values may affect mood
 * - Breathing Rate (BPM): Normal range 12-20, rapid breathing indicates stress
 * - Motion (accel/gyro): High movement indicates agitation
 * - Sound Level (dB): Elevated levels may indicate distress
 */
class EmotionModel {
private:
    // Thresholds for stress detection
    const float STRESS_HR_THRESHOLD = 90.0;    // BPM
    const float STRESS_GSR_THRESHOLD = 5.0;    // microsiemens
    const float STRESS_TEMP_THRESHOLD = 37.5;  // C
    const float STRESS_CO2_THRESHOLD = 1000.0; // ppm
    const float STRESS_BREATH_THRESHOLD = 20.0; // BPM
    const float STRESS_MOTION_THRESHOLD = 2.0;  // m/s^2
    const float STRESS_SOUND_THRESHOLD = 70.0;  // dB

    // Weights for each parameter in stress calculation
    const float HR_WEIGHT = 0.25;
    const float GSR_WEIGHT = 0.20;
    const float TEMP_WEIGHT = 0.15;
    const float CO2_WEIGHT = 0.10;
    const float BREATH_WEIGHT = 0.15;
    const float MOTION_WEIGHT = 0.10;
    const float SOUND_WEIGHT = 0.05;

public:
    enum EmotionState {
        CALM,
        MILD_STRESS,
        MODERATE_STRESS,
        HIGH_STRESS,
        UNKNOWN
    };

    struct SensorData {
        float heartRate;
        float gsr;
        float temperature;
        float co2;
        float breathingRate;
        float motionX;
        float motionY;
        float motionZ;
        float soundLevel;
    };

    EmotionModel() {}

    EmotionState analyze(const SensorData& data) {
        float stressScore = calculateStressScore(data);
        
        if (stressScore < 0.3) return CALM;
        else if (stressScore < 0.5) return MILD_STRESS;
        else if (stressScore < 0.7) return MODERATE_STRESS;
        else return HIGH_STRESS;
    }

private:
    float calculateStressScore(const SensorData& data) {
        float score = 0.0;
        
        // Heart Rate contribution
        if (data.heartRate > STRESS_HR_THRESHOLD) {
            score += HR_WEIGHT * (data.heartRate - STRESS_HR_THRESHOLD) / 20.0;
        }
        
        // GSR contribution
        if (data.gsr > STRESS_GSR_THRESHOLD) {
            score += GSR_WEIGHT * (data.gsr - STRESS_GSR_THRESHOLD) / 5.0;
        }
        
        // Temperature contribution
        if (data.temperature > STRESS_TEMP_THRESHOLD) {
            score += TEMP_WEIGHT * (data.temperature - STRESS_TEMP_THRESHOLD) / 0.5;
        }
        
        // CO2 contribution
        if (data.co2 > STRESS_CO2_THRESHOLD) {
            score += CO2_WEIGHT * (data.co2 - STRESS_CO2_THRESHOLD) / 500.0;
        }
        
        // Breathing Rate contribution
        if (data.breathingRate > STRESS_BREATH_THRESHOLD) {
            score += BREATH_WEIGHT * (data.breathingRate - STRESS_BREATH_THRESHOLD) / 10.0;
        }
        
        // Motion contribution (using magnitude)
        float motionMagnitude = sqrt(
            data.motionX * data.motionX + 
            data.motionY * data.motionY + 
            data.motionZ * data.motionZ
        );
        if (motionMagnitude > STRESS_MOTION_THRESHOLD) {
            score += MOTION_WEIGHT * (motionMagnitude - STRESS_MOTION_THRESHOLD) / 2.0;
        }
        
        // Sound Level contribution
        if (data.soundLevel > STRESS_SOUND_THRESHOLD) {
            score += SOUND_WEIGHT * (data.soundLevel - STRESS_SOUND_THRESHOLD) / 30.0;
        }
        
        return constrain(score, 0.0, 1.0);
    }
};

#endif 