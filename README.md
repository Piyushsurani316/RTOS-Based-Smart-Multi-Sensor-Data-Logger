# Multi-Sensor Data Logger

[![VSCode](https://img.shields.io/badge/IDE-VSCode-blue)](https://code.visualstudio.com/)  
[![PlatformIO](https://img.shields.io/badge/Platform-PlatformIO-blue?logo=platformio&logoColor=white)](https://platformio.org/)    
[![HiveMQ](https://img.shields.io/badge/MQTT-HiveMQ-blue?logo=hivemq&logoColor=white)](https://www.hivemq.com/)   
[![ESP32](https://img.shields.io/badge/Board-ESP32-brightgreen)](https://www.espressif.com/en/products/socs/esp32)  

---

## Overview
This project is an **ESP32-based smart multi-sensor data logger** built in **VSCode/PlatformIO**. It continuously monitors environmental and physiological parameters and:

- Displays them on an **LCD**
- Logs data to an **SD card (SPI interface)**
- Publishes data to an **MQTT broker** in JSON format
- Detects faults in sensor readings

**Sensors used:**
- **BMP280**: Temperature & Pressure  
- **MAX30100**: Heart Rate & SpO₂  
- **MPU6050**: 3-axis accelerometer & gyroscope  

**Services/Modules:**
- WiFi connectivity (`wifi_config.h`)  
- MQTT publishing (`mqtt_service.h`)  
- SD card logging (`sd_logger.h`)  
- LCD display (`lcd_service.h`)  
- Fault detection (`fault_manager.h`)  

---

## Features

- Multi-sensor data acquisition  
- LCD page switching every 2 seconds  
- MQTT JSON publish for real-time monitoring  
- Fault detection with buzzer alert  
- Modular and extendable architecture  

---

## Hardware Setup

| Component      | Connection to ESP32             |
|----------------|--------------------------------|
| BMP280         | I2C (SDA=21, SCL=22)           |
| MAX30100       | I2C (SDA=21, SCL=22)           |
| MPU6050        | I2C (SDA=21, SCL=22)           |
| LCD            | GPIO (as defined in `lcd_service.h`) |
| Buzzer         | GPIO 25                        |
| SD Card Adapter| SPI (MOSI=23, MISO=19, SCK=18, CS=5) |

> Ensure that your I2C addresses do not conflict. SPI pins can be adjusted based on your ESP32 board.

---

## Software Requirements

- **VSCode** with **PlatformIO extension**  
- ESP32 board support installed in PlatformIO  
- Required Libraries (install via PlatformIO Library Manager or `platformio.ini`):
  - `ArduinoJson` — JSON serialization  
  - `Wire` — I2C communication  
  - `SPI` — SPI communication for SD card  
  - `SD` — SD card interface  
  - `Adafruit BMP280` — BMP280 sensor driver  
  - `MAX30100` — Heart Rate & SpO₂ sensor driver  
  - `MPU6050` — Accelerometer & gyroscope driver  
  - `LiquidCrystal_I2C` or equivalent — LCD display  

---

## How It Works

1. **Setup**:
   - Initializes all sensors, LCD, MQTT, SD card, and buzzer  

2. **Loop**:
   - Maintains WiFi and MQTT connection
   - Every 2 seconds (`publishInterval`):
     - Reads all sensors  
     - Detects faults  
     - Logs to SD card  
     - Publishes JSON to MQTT  
   - Every 2 seconds (`pageInterval`): switches LCD page  

---

## MQTT HiveMQ Connection

This project uses **HiveMQ** to publish sensor data in real-time. You can use **HiveMQ public broker** for testing or create your own HiveMQ cloud broker.  

**Broker details (public testing broker):**

| Parameter      | Value                         |
|----------------|-------------------------------|
| Broker URL     | broker.hivemq.com             |
| Port           | 1883 (TCP) or 8883 (SSL)     |
| Username       | Optional (leave blank for public broker) |
| Password       | Optional (leave blank for public broker) |
| Topic          | esp32/sensor/all              |

**Create your HiveMQ Cloud account:**  
[Sign up here](https://auth.hivemq.cloud/u/login/identifier?state=hKFo2SBhQ0ZBNW13ZkZKRWdYTjlla2hoekF0bXdqMU4zS2VHdKFur3VuaXZlcnNhbC1sb2dpbqN0aWTZIHZCUG9zOFRodkhDNTFtZjNrazViVWJoNW5SQjUwS05Io2NpZNkgSWFqbzRlMzJqeHdVczhBZEZ4Z3hRbjJWUDNZd0laVEs)  

**Example PlatformIO MQTT configuration in `mqtt_service.h`:**

```cpp
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/sensor/all";

// If using authentication for HiveMQ cloud broker
const char* mqtt_user = "<your-username>";
const char* mqtt_password = "<your-password>";
   ```    


---


## Real-World Applications

This **ESP32 RTOS-Based Smart Multi-Sensor Data Logger** can be used in multiple industrial and personal applications, including:

1. **Industrial Environmental Monitoring**  
   - Monitor temperature, pressure, and vibrations in factories or warehouses.  
   - Detect abnormal conditions early to prevent equipment failure.  

2. **Health Monitoring Systems**  
   - Heart rate and SpO₂ monitoring for personal fitness or patient tracking.  
   - Can be integrated into wearable devices or remote health monitoring stations.  

3. **IoT Data Logging Platforms**  
   - Publish real-time sensor data to cloud platforms using MQTT (HiveMQ).  
   - Integrate with dashboards for smart homes, offices, or labs.  

4. **Smart Agriculture / Farming**  
   - Monitor environmental conditions (temperature, pressure, vibrations) in greenhouses or farms.  
   - Collect data for automated irrigation or climate control systems.  

5. **Robotics & Motion Tracking**  
   - MPU6050 accelerometer & gyroscope data can be used for robotics motion analysis, drone stabilization, or vehicle monitoring.  

> The architecture is not rtos based ,in future we will be making it suitable for **custom IoT or industrial projects** using RTOS.
