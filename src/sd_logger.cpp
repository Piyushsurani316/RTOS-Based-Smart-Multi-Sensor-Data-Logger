#include "sd_logger.h"
#include <SPI.h>
#include <Arduino.h>

#define SD_CS 5

void initSD() {
    SPI.begin(18, 19, 23, SD_CS);
    if (!SD.begin(SD_CS, SPI, 1000000)) {
        Serial.println("SD Init Failed!");
        return;
    }
    Serial.println("SD Ready");

    File file = SD.open("/log.csv", FILE_APPEND);
    if (file && file.size() == 0) {
        file.println("Time(ms),Temp,Pressure,HR,SpO2,AX,AY,AZ,GX,GY,GZ,Fault");
        file.close();
    }
}

void logToSD(unsigned long time, int temperature, int pressure, int HR, int spo2, 
             int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, bool fault) {
    File file = SD.open("/log.csv", FILE_APPEND);
    if (file) {
        file.print(time); file.print(",");
        file.print(temperature); file.print(",");
        file.print(pressure); file.print(",");
        file.print(HR); file.print(",");
        file.print(spo2); file.print(",");
        file.print(ax); file.print(",");
        file.print(ay); file.print(",");
        file.print(az); file.print(",");
        file.print(gx); file.print(",");
        file.print(gy); file.print(",");
        file.print(gz); file.print(",");
        file.println(fault ? 1 : 0);
        file.close();
    } else {
        Serial.println("SD Write Error");
    }
}