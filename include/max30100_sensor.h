#ifndef MAX30100_SENSOR_H
#define MAX30100_SENSOR_H

#include <MAX30100_PulseOximeter.h>

extern PulseOximeter pox;

bool initMAX30100();
void updateMAX();
int getHeartRate();
int getSpO2();

#endif