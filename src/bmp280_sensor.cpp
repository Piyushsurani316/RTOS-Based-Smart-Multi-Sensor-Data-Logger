#include "bmp280_sensor.h"
#include <Arduino.h>

Adafruit_BMP280 bmp;

bool initBMP() {
    if (!bmp.begin(0x76)) {
        Serial.println("BMP Error");
        return false;
    }
    return true;
}

int readTemperature() { return (int) bmp.readTemperature(); }
int readPressure() { return (int) (bmp.readPressure() / 100.0F); }