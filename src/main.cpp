#include <Arduino.h>
#include <Wire.h>

#include <ArduinoJson.h>

// -------- Services --------
#include "wifi_config.h"
#include "mqtt_service.h"
#include "sd_logger.h"
#include "lcd_service.h"
#include "fault_manager.h"


// -------- Sensors --------
#include "bmp280_sensor.h"
#include "max30100_sensor.h"
#include "mpu6050_sensor.h"

// -------- Timing --------
unsigned long lastPublish = 0;
const unsigned long publishInterval = 2000;

unsigned long pageTimer = 0;
const unsigned long pageInterval = 2000;

// ---------------- Variables ----------------
uint8_t lcdPage = 0;
int temperature = 0, pressure = 0;
int heartRate = 0, spo2 = 0;
int16_t ax, ay, az, gx, gy, gz;

void setup() {

    Serial.begin(9600);
    Wire.begin(21, 22);   // ESP32 I2C

    Serial.println("\nSystem Booting...");

    // -------- Initialize Services --------
    connectWiFi();
    initMQTT();
    initSD();
    initLCD();
    initBuzzer(25);

    // -------- Initialize Sensors --------
    if (!initBMP()) while (1);
    if (!initMAX30100()) while (1);
    if (!initMPU()) while (1);

    Serial.println("System Ready");
}

void loop() {

    // -------- Maintain Connections --------
    connectWiFi();
    if (!client.connected()) {
        reconnectMQTT();
    }
    client.loop();

    // -------- Update MAX30100 Continuously --------
    updateMAX();

    unsigned long currentMillis = millis();

    // =================================================
    //              SENSOR READ & PROCESS
    // =================================================
    if (currentMillis - lastPublish >= publishInterval) {

        lastPublish = currentMillis;

        // -------- Read Sensors --------
     temperature = readTemperature();
     pressure    = readPressure();
     heartRate   = getHeartRate();
     spo2        = getSpO2();

        
        readMPU(ax, ay, az, gx, gy, gz);

        // -------- Fault Detection --------
        bool fault = checkFault(heartRate, pressure);

        // -------- Serial Debug Output --------
        Serial.print("T:"); Serial.print(temperature);
        Serial.print(" P:"); Serial.print(pressure);
        Serial.print(" HR:"); Serial.print(heartRate);
        Serial.print(" S:"); Serial.print(spo2);
        Serial.print(" AX:"); Serial.print(ax);
        Serial.print(" AY:"); Serial.print(ay);
        Serial.print(" AZ:"); Serial.print(az);
        Serial.print(" GX:"); Serial.print(gx);
        Serial.print(" GY:"); Serial.print(gy);
        Serial.print(" GZ:"); Serial.print(gz);
        Serial.print(" Fault:");
        Serial.println(fault ? 1 : 0);

        // -------- MQTT JSON Publish --------
        JsonDocument doc;

        doc["temperature"] = temperature;
        doc["pressure"] = pressure;
        doc["heartRate"] = heartRate;
        doc["spo2"] = spo2;
        doc["ax"] = ax; doc["ay"] = ay; doc["az"] = az;
        doc["gx"] = gx; doc["gy"] = gy; doc["gz"] = gz;
        doc["fault"] = fault;

        char buffer[512];
        serializeJson(doc, buffer);

        publishData("esp32/sensor/all", buffer);

        // -------- SD Logging --------
        logToSD(millis(), temperature, pressure, heartRate, spo2,
                ax, ay, az, gx, gy, gz, fault);
    }

    // =================================================
    //                 LCD PAGE SWITCH
    // =================================================
    if (currentMillis - pageTimer >= pageInterval) {

        pageTimer = currentMillis;

        lcdPage++;
        if (lcdPage > 2) lcdPage = 0;

        updateLCDPage(lcdPage,
              temperature,
              pressure,
              heartRate,
              spo2,
              ax, ay, az,
              gx, gy, gz);
    }
}