#ifndef SD_LOGGER_H
#define SD_LOGGER_H

#include <SD.h>

void initSD();
void logToSD(unsigned long time, int temperature, int pressure, int HR, int spo2, 
             int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, bool fault);

#endif