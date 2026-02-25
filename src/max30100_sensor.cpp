#include "max30100_sensor.h"
#include <Arduino.h>

PulseOximeter pox;

bool initMAX30100() {
    if (!pox.begin()) {
        Serial.println("MAX30100 Error");
        return false;
    }
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    return true;
}

void updateMAX() {
    pox.update();
}

int getHeartRate() {
    return (int)pox.getHeartRate();
}

int getSpO2() {
    return (int)pox.getSpO2();
}