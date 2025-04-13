#ifndef SENSOR_HEALTH_H
#define SENSOR_HEALTH_H

#include <Arduino.h>
#include "utils/Logger.h"

namespace Emopod {
namespace Sensors {

class SensorHealth {
private:
    struct SensorStatus {
        unsigned long lastUpdateTime;
        float lastValue;
        bool isHealthy;
        const char* name;
    };
    
    static const int MAX_SENSORS = 10;
    SensorStatus sensors[MAX_SENSORS];
    int sensorCount;
    unsigned long healthCheckInterval;
    unsigned long lastHealthCheck;
    
public:
    SensorHealth(unsigned long interval = 5000) 
        : sensorCount(0), healthCheckInterval(interval), lastHealthCheck(0) {}
    
    void registerSensor(const char* name) {
        if (sensorCount < MAX_SENSORS) {
            sensors[sensorCount] = {
                .lastUpdateTime = 0,
                .lastValue = 0.0f,
                .isHealthy = true,
                .name = name
            };
            sensorCount++;
        }
    }
    
    void updateSensor(int index, float value) {
        if (index >= 0 && index < sensorCount) {
            sensors[index].lastUpdateTime = millis();
            sensors[index].lastValue = value;
        }
    }
    
    void checkHealth() {
        unsigned long currentTime = millis();
        if (currentTime - lastHealthCheck >= healthCheckInterval) {
            lastHealthCheck = currentTime;
            
            for (int i = 0; i < sensorCount; i++) {
                bool wasHealthy = sensors[i].isHealthy;
                sensors[i].isHealthy = (currentTime - sensors[i].lastUpdateTime) <= healthCheckInterval * 2;
                
                if (wasHealthy && !sensors[i].isHealthy) {
                    Emopod::Utils::Logger::warn("SENSOR", "Sensor %s is not responding", sensors[i].name);
                } else if (!wasHealthy && sensors[i].isHealthy) {
                    Emopod::Utils::Logger::info("SENSOR", "Sensor %s is back online", sensors[i].name);
                }
            }
        }
    }
    
    bool isSensorHealthy(int index) const {
        return (index >= 0 && index < sensorCount) ? sensors[index].isHealthy : false;
    }
    
    float getLastValue(int index) const {
        return (index >= 0 && index < sensorCount) ? sensors[index].lastValue : 0.0f;
    }
    
    const char* getSensorName(int index) const {
        return (index >= 0 && index < sensorCount) ? sensors[index].name : nullptr;
    }
    
    int getSensorCount() const {
        return sensorCount;
    }
};

} // namespace Sensors
} // namespace Emopod

#endif 