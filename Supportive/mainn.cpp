#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP280.h>
#include <MAX30100_PulseOximeter.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <MPU6050.h>

// ---------------- WiFi ----------------
const char* ssid = "Prasantih";
const char* password = "Swaminarayan@1981";

// ---------------- MQTT ----------------
const char* mqtt_server = "322ca06e4c25499ca552090b47f74baa.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "esp32user";
const char* mqtt_password = "p12345678P";

WiFiClientSecure espClient;
PubSubClient client(espClient);

// ---------------- Sensors ----------------
Adafruit_BMP280 bmp;
PulseOximeter pox;
MPU6050 mpu;

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- Pins ----------------
#define BUZZER_PIN 25
#define SD_CS 5

// ---------------- Thresholds ----------------
#define HR_THRESHOLD 120
#define PRESSURE_THRESHOLD 1020
uint8_t lcdPage = 0; // Toggle LCD page

// ---------------- Variables ----------------
int temperature = 0, pressure = 0;
int heartRate = 0, spo2 = 0;
int16_t ax, ay, az, gx, gy, gz;

unsigned long lastPublish = 0;
const long interval = 2000; // 2 seconds
int scrollPos = 0;
// -------- LCD Display --------
static unsigned long pageTimer = 0;
const int pageInterval = 2000;   // change page every 2 seconds

// ---------------- WiFi Connect ----------------
void connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
}

// ---------------- MQTT Reconnect ----------------
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

// ---------------- SD Init ----------------
void initSD() {
  SPI.begin(18, 19, 23, SD_CS); // SCK, MISO, MOSI, CS
  if (!SD.begin(SD_CS, SPI, 1000000)) {
    Serial.println("SD Init Failed!");
    lcd.clear(); lcd.print("SD Fail!");
    return;
  }
  Serial.println("SD Ready");

  File file = SD.open("/log.csv", FILE_APPEND);
  if (file) {
    if (file.size() == 0) {
      file.println("Time(ms),Temp,Pressure,HR,SpO2,AX,AY,AZ,GX,GY,GZ,Fault");
      Serial.println("Header Written");
    }
    file.close();
  } else {
    Serial.println("Header Write Failed");
  }
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // I2C pins ESP32

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.begin();
  lcd.backlight();
  lcd.print("Starting...");

  connectWiFi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);

  // BMP280
  if (!bmp.begin(0x76)) { lcd.clear(); lcd.print("BMP Error"); while (1); }

  // MAX30100
  if (!pox.begin()) { lcd.clear(); lcd.print("MAX30100 Err"); while (1); }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // MPU6050
  mpu.initialize();
  if (!mpu.testConnection()) { lcd.clear(); lcd.print("MPU Error"); while (1); }

  // SD init
  initSD();

  lcd.clear();
  lcd.print("System Ready");
  delay(1500);
}

// ---------------- Loop ----------------
void loop() {
  connectWiFi();
  if (!client.connected()) reconnectMQTT();
  client.loop();

  pox.update(); // important

  if (millis() - lastPublish >= interval) {
    lastPublish = millis();

    // -------- Read Sensors --------
    temperature = (int)bmp.readTemperature();
    pressure = (int)(bmp.readPressure() / 100.0F);
    heartRate = (int)pox.getHeartRate();
    spo2 = (int)pox.getSpO2();
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // -------- Fault Detection --------
    bool fault = false;
    if (heartRate > HR_THRESHOLD || pressure > PRESSURE_THRESHOLD) {
      fault = true;
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }

    // -------- MQTT Publish --------
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
    client.publish("esp32/sensor/all", buffer);
    Serial.println(buffer);

    // -------- SD Logging --------
    File file = SD.open("/log.csv", FILE_APPEND);
    if (file) {
      file.print(millis()); file.print(",");
      file.print(temperature); file.print(",");
      file.print(pressure); file.print(",");
      file.print(heartRate); file.print(",");
      file.print(spo2); file.print(",");
      file.print(ax); file.print(",");
      file.print(ay); file.print(",");
      file.print(az); file.print(",");
      file.print(gx); file.print(",");
      file.print(gy); file.print(",");
      file.print(gz); file.print(",");
      file.println(fault ? 1 : 0);
      file.close();
      Serial.println("SD Logged");
    } else {
      Serial.println("SD Write Error");
    }

    // -------- LCD Display --------
    lcd.clear();

if (millis() - pageTimer > pageInterval) {
  pageTimer = millis();
  lcdPage++;
  if (lcdPage > 2) lcdPage = 0;   // 3 pages: 0,1,2
}

lcd.clear();

if (lcdPage == 0) {
  // -------- Page 1 --------
  lcd.setCursor(0,0);
  lcd.print("T:"); lcd.print(temperature);
  lcd.print(" P:"); lcd.print(pressure);

  lcd.setCursor(0,1);
  lcd.print("HR:"); lcd.print(heartRate);
  lcd.print(" S:"); lcd.print(spo2);
}

else if (lcdPage == 1) {
  // -------- Page 2 (Accelerometer) --------
  lcd.setCursor(0,0);
  lcd.print("AX:"); lcd.print(ax);
  lcd.print(" AY:"); lcd.print(ay);

  lcd.setCursor(0,1);
  lcd.print("AZ:"); lcd.print(az);
}

else if (lcdPage == 2) {
  // -------- Page 3 (Gyroscope) --------
  lcd.setCursor(0,0);
  lcd.print("GX:"); lcd.print(gx);
  lcd.print(" GY:"); lcd.print(gy);

  lcd.setCursor(0,1);
  lcd.print("GZ:"); lcd.print(gz);
}
}
}