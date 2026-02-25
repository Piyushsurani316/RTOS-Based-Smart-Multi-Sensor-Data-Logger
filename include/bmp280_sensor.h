#ifndef BMP280_SENSOR_H
#define BMP280_SENSOR_H

#include <Adafruit_BMP280.h>

extern Adafruit_BMP280 bmp;

bool initBMP();
int readTemperature();
int readPressure();

#endif