#ifndef MOTION_SENSOR_H
#define MOTION_SENSOR_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class MotionSensor {
private:
    Adafruit_MPU6050& sensor;
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
    float accelAverage[3];
    float gyroAverage[3];
    const int NUM_SAMPLES = 5;
    float accelSamples[3][5];
    float gyroSamples[3][5];
    int sampleIndex = 0;
    
public:
    MotionSensor(Adafruit_MPU6050& sensorRef) : sensor(sensorRef) {}
    
    void begin() {
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
    
    void read() {
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
    
    float getAccelX() { return accelAverage[0]; }
    float getAccelY() { return accelAverage[1]; }
    float getAccelZ() { return accelAverage[2]; }
    
    float getGyroX() { return gyroAverage[0]; }
    float getGyroY() { return gyroAverage[1]; }
    float getGyroZ() { return gyroAverage[2]; }
    
    float getTemperature() { return temp.temperature; }
};

#endif 