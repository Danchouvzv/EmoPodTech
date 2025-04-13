#include "EmotionModel.h"
#include "utils/Logger.h"

namespace Emopod {
namespace Models {

EmotionModel::EmotionModel() 
    : lastBaselineUpdate(0), baselineUpdateInterval(3600000) // 1 hour
{
    // Initialize baselines with default values
    baselines.heartRate = 75.0f;
    baselines.gsr = 2.0f;
    baselines.temperature = 36.5f;
    baselines.co2 = 400.0f;
    baselines.breathingRate = 15.0f;
    baselines.motion = 1.0f;
    baselines.soundLevel = 30.0f;
}

EmotionModel::EmotionState EmotionModel::analyze(const SensorData& data) {
    updateBaselines(data);
    float stressScore = calculateStressScore(data);
    
    if (stressScore < 0.3) return CALM;
    else if (stressScore < 0.5) return MILD_STRESS;
    else if (stressScore < 0.7) return MODERATE_STRESS;
    else return HIGH_STRESS;
}

float EmotionModel::calculateStressScore(const SensorData& data) {
    float score = 0.0;
    
    // Normalize and calculate contributions
    if (!isnan(data.heartRate)) {
        float normalizedHR = (data.heartRate - baselines.heartRate) / 20.0f;
        score += HR_WEIGHT * max(0.0f, normalizedHR);
    }
    
    if (!isnan(data.gsr)) {
        float normalizedGSR = (data.gsr - baselines.gsr) / 5.0f;
        score += GSR_WEIGHT * max(0.0f, normalizedGSR);
    }
    
    if (!isnan(data.temperature)) {
        float normalizedTemp = (data.temperature - baselines.temperature) / 0.5f;
        score += TEMP_WEIGHT * max(0.0f, normalizedTemp);
    }
    
    if (!isnan(data.co2)) {
        float normalizedCO2 = (data.co2 - baselines.co2) / 500.0f;
        score += CO2_WEIGHT * max(0.0f, normalizedCO2);
    }
    
    if (!isnan(data.breathingRate)) {
        float normalizedBreath = (data.breathingRate - baselines.breathingRate) / 10.0f;
        score += BREATH_WEIGHT * max(0.0f, normalizedBreath);
    }
    
    float motionMagnitude = sqrt(
        data.motionX * data.motionX + 
        data.motionY * data.motionY + 
        data.motionZ * data.motionZ
    );
    if (!isnan(motionMagnitude)) {
        float normalizedMotion = (motionMagnitude - baselines.motion) / 2.0f;
        score += MOTION_WEIGHT * max(0.0f, normalizedMotion);
    }
    
    if (!isnan(data.soundLevel)) {
        float normalizedSound = (data.soundLevel - baselines.soundLevel) / 30.0f;
        score += SOUND_WEIGHT * max(0.0f, normalizedSound);
    }
    
    return constrain(score, 0.0, 1.0);
}

void EmotionModel::updateBaselines(const SensorData& data) {
    unsigned long currentTime = millis();
    if (currentTime - lastBaselineUpdate >= baselineUpdateInterval) {
        lastBaselineUpdate = currentTime;
        
        // Update baselines using exponential moving average
        const float alpha = 0.1f; // Smoothing factor
        
        if (!isnan(data.heartRate)) {
            baselines.heartRate = alpha * data.heartRate + (1 - alpha) * baselines.heartRate;
        }
        
        if (!isnan(data.gsr)) {
            baselines.gsr = alpha * data.gsr + (1 - alpha) * baselines.gsr;
        }
        
        if (!isnan(data.temperature)) {
            baselines.temperature = alpha * data.temperature + (1 - alpha) * baselines.temperature;
        }
        
        if (!isnan(data.co2)) {
            baselines.co2 = alpha * data.co2 + (1 - alpha) * baselines.co2;
        }
        
        if (!isnan(data.breathingRate)) {
            baselines.breathingRate = alpha * data.breathingRate + (1 - alpha) * baselines.breathingRate;
        }
        
        float motionMagnitude = sqrt(
            data.motionX * data.motionX + 
            data.motionY * data.motionY + 
            data.motionZ * data.motionZ
        );
        if (!isnan(motionMagnitude)) {
            baselines.motion = alpha * motionMagnitude + (1 - alpha) * baselines.motion;
        }
        
        if (!isnan(data.soundLevel)) {
            baselines.soundLevel = alpha * data.soundLevel + (1 - alpha) * baselines.soundLevel;
        }
        
        Logger::info("EMOTION", "Updated baselines:");
        Logger::info("EMOTION", "HR: %.1f, GSR: %.2f, Temp: %.1f", 
                    baselines.heartRate, baselines.gsr, baselines.temperature);
        Logger::info("EMOTION", "CO2: %.1f, Breath: %.1f, Motion: %.2f, Sound: %.1f",
                    baselines.co2, baselines.breathingRate, baselines.motion, baselines.soundLevel);
    }
}

} // namespace Models
} // namespace Emopod 