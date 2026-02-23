#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <MAX30100_PulseOximeter.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

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

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2); // change to 0x3F if needed

// ---------------- Variables ----------------
float temperature = 0, pressure = 0;
float heartRate = 0, spo2 = 0;

// MQTT publish timing
unsigned long lastPublish = 0;
const long publishInterval = 2000; // 2 seconds

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
  Wire.begin(21, 22); // I2C pins for ESP32

  // LCD setup
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("System Starting");

  // WiFi
  connectWiFi();
  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);

  // BMP280 init
  if (!bmp.begin(0x76)) {
    lcd.clear();
    lcd.print("BMP280 Error");
    while (1);
  }

  // MAX30100 init
  if (!pox.begin()) {
    lcd.clear();
    lcd.print("MAX30100 Error");
    while(1);
  }
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA); // moderate LED current

  lcd.clear();
  lcd.print("Sensors Ready");
  delay(2000);
}

// ---------------- Loop ----------------
void loop() {
  // MQTT reconnect
  if (!client.connected()) reconnect();
  client.loop();

  // Frequent MAX30100 update
  pox.update();

  // Read BMP280
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure() / 100.0F;

  // Read MAX30100
  heartRate = pox.getHeartRate();
  spo2 = pox.getSpO2();

  // Publish MQTT every 2 seconds
  if (millis() - lastPublish >= publishInterval) {
    lastPublish = millis();

    // Prepare JSON
    StaticJsonDocument<256> doc;
    doc["temperature"] = temperature;
    doc["pressure"] = pressure;
    doc["heartRate"] = heartRate;
    doc["spo2"] = spo2;

    char buffer[256];
    serializeJson(doc, buffer);

    client.publish("esp32/sensor/all", buffer);
    Serial.println(buffer);

    // Update LCD (avoid clear to prevent flicker)
    lcd.setCursor(0,0);
    lcd.print("T:");
    lcd.print(temperature,1);
    lcd.print("C HR:");
    lcd.print((int)heartRate);
    lcd.print("  "); // clear leftover digits

    lcd.setCursor(0,1);
    lcd.print("P:");
    lcd.print((int)pressure);
    lcd.print(" SpO2:");
    lcd.print((int)spo2);
    lcd.print("  "); // clear leftover digits
  }
}