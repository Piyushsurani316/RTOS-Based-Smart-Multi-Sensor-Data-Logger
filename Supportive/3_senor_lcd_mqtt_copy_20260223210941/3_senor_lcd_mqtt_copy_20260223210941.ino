#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
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

// ---------------- Buzzer ----------------
#define BUZZER_PIN 25

// ---------------- Thresholds ----------------
#define HR_THRESHOLD 120
#define PRESSURE_THRESHOLD 1020

// ---------------- Variables ----------------
float temperature = 0, pressure = 0;
float heartRate = 0, spo2 = 0;
int16_t ax, ay, az, gx, gy, gz;

unsigned long lastPublish = 0;
const long publishInterval = 2000;

// ---------------- WiFi Connect ----------------
void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
}

// ---------------- MQTT Reconnect ----------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);   // ESP32 I2C pins

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("System Starting");

  connectWiFi();

  espClient.setInsecure();   // skip certificate validation
  client.setServer(mqtt_server, mqtt_port);

  // BMP280
  if (!bmp.begin(0x76)) {
    lcd.clear();
    lcd.print("BMP280 Error");
    while (1);
  }

  // MAX30100
  if (!pox.begin()) {
    lcd.clear();
    lcd.print("MAX30100 Error");
    while (1);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // MPU6050
  mpu.initialize();
  if (!mpu.testConnection()) {
    lcd.clear();
    lcd.print("MPU6050 Error");
    while (1);
  }

  lcd.clear();
  lcd.print("Sensors Ready");
  delay(2000);
}

// ---------------- Loop ----------------
void loop() {

  if (!client.connected()) reconnect();
  client.loop();

  pox.update();   // very important for MAX30100

  // Read BMP280
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure() / 100.0F;

  // Read MAX30100
  heartRate = pox.getHeartRate();
  spo2 = pox.getSpO2();

  // Read MPU6050
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // -------- Fault Detection --------
  bool fault = false;

  if (heartRate > HR_THRESHOLD || pressure > PRESSURE_THRESHOLD) {
    fault = true;
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // -------- Publish MQTT --------
  if (millis() - lastPublish >= publishInterval) {

    lastPublish = millis();

    StaticJsonDocument<512> doc;

    doc["temperature"] = temperature;
    doc["pressure"] = pressure;
    doc["heartRate"] = heartRate;
    doc["spo2"] = spo2;

    doc["ax"] = ax;
    doc["ay"] = ay;
    doc["az"] = az;
    doc["gx"] = gx;
    doc["gy"] = gy;
    doc["gz"] = gz;

    doc["fault"] = fault;

    char buffer[512];
    serializeJson(doc, buffer);

    client.publish("esp32/sensor/all", buffer);
    Serial.println(buffer);

    // -------- LCD Update --------
    lcd.setCursor(0,0);
    lcd.print("T:");
    lcd.print(temperature,1);
    lcd.print("C HR:");
    lcd.print((int)heartRate);
    lcd.print("   ");

    lcd.setCursor(0,1);
    lcd.print("P:");
    lcd.print((int)pressure);
    lcd.print(" S:");
    lcd.print((int)spo2);

    if (fault) {
      lcd.setCursor(12,1);
      lcd.print("ALRT");
    } else {
      lcd.setCursor(12,1);
      lcd.print("    ");
    }
  }
}