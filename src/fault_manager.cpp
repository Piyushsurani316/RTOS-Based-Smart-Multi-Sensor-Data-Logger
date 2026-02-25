#include "fault_manager.h"
#include <Arduino.h>

static uint8_t buzzerPin;

#define HR_THRESHOLD 120
#define PRESSURE_THRESHOLD 1020

void initBuzzer(uint8_t pin) {
    buzzerPin = pin;
    pinMode(buzzerPin, OUTPUT);
    digitalWrite(buzzerPin, LOW);
}

void setBuzzer(bool state) {
    digitalWrite(buzzerPin, state ? HIGH : LOW);
}

bool checkFault(int heartRate, int pressure) {
    if (heartRate > HR_THRESHOLD || pressure > PRESSURE_THRESHOLD) {
        setBuzzer(true);
        return true;
    }
    setBuzzer(false);
    return false;
}