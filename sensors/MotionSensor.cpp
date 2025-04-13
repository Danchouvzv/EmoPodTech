#include "MotionSensor.h"

void MotionSensor::begin() {
    sensor.begin();
    sensor.setAccelerometerRange(MPU6050_RANGE_8_G);
    sensor.setGyroRange(MPU6050_RANGE_500_DEG);
    sensor.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < NUM_SAMPLES; j++) {
            accelSamples[i][j] = 0;
            gyroSamples[i][j] = 0;
        }
    }
}

void MotionSensor::read() {
    sensors_event_t a, g, temp;
    sensor.getEvent(&a, &g, &temp);
    
    // Update accelerometer values
    accelX = a.acceleration.x;
    accelY = a.acceleration.y;
    accelZ = a.acceleration.z;
    
    // Update gyroscope values
    gyroX = g.gyro.x;
    gyroY = g.gyro.y;
    gyroZ = g.gyro.z;
    
    // Update moving averages for accelerometer
    accelSamples[0][sampleIndex] = accelX;
    accelSamples[1][sampleIndex] = accelY;
    accelSamples[2][sampleIndex] = accelZ;
    
    // Update moving averages for gyroscope
    gyroSamples[0][sampleIndex] = gyroX;
    gyroSamples[1][sampleIndex] = gyroY;
    gyroSamples[2][sampleIndex] = gyroZ;
    
    sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
    
    // Calculate averages
    for (int i = 0; i < 3; i++) {
        accelAverage[i] = 0;
        gyroAverage[i] = 0;
        
        for (int j = 0; j < NUM_SAMPLES; j++) {
            accelAverage[i] += accelSamples[i][j];
            gyroAverage[i] += gyroSamples[i][j];
        }
        
        accelAverage[i] /= NUM_SAMPLES;
        gyroAverage[i] /= NUM_SAMPLES;
    }
}

float MotionSensor::getAccelX() { return accelAverage[0]; }
float MotionSensor::getAccelY() { return accelAverage[1]; }
float MotionSensor::getAccelZ() { return accelAverage[2]; }

float MotionSensor::getGyroX() { return gyroAverage[0]; }
float MotionSensor::getGyroY() { return gyroAverage[1]; }
float MotionSensor::getGyroZ() { return gyroAverage[2]; }

float MotionSensor::getTemperature() { return temp.temperature; } 