#include "mpu6050_sensor.h"
#include <Arduino.h>

MPU6050 mpu;

bool initMPU() {
    mpu.initialize();
    if (!mpu.testConnection()) {
        Serial.println("MPU6050 Error");
        return false;
    }
    return true;
}

void readMPU(int16_t &ax, int16_t &ay, int16_t &az,
             int16_t &gx, int16_t &gy, int16_t &gz) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
}