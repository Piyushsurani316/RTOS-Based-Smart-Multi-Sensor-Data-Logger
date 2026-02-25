#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include <MPU6050.h>

extern MPU6050 mpu;

bool initMPU();
void readMPU(int16_t &ax, int16_t &ay, int16_t &az,
             int16_t &gx, int16_t &gy, int16_t &gz);

#endif