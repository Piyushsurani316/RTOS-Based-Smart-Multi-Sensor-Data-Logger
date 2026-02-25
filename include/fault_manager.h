#include <Arduino.h>
#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

void initBuzzer(uint8_t pin);
bool checkFault(int heartRate, int pressure);
void setBuzzer(bool state);

#endif