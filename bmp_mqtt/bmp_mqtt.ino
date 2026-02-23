#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>

// WiFi
const char* ssid = "Prasantih";
const char* password = "Swaminarayan@1981";

// HiveMQ
const char* mqtt_server = "322ca06e4c25499ca552090b47f74baa.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "esp32user";
const char* mqtt_password = "p12345678P";

WiFiClientSecure espClient;
PubSubClient client(espClient);
Adafruit_BMP280 bmp;

// LCD address (change to 0x3F if not working)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
}

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

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);  // Explicit I2C pins for ESP32

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("System Starting");

  connectWiFi();

  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);

  if (!bmp.begin(0x76)) {
    lcd.clear();
    lcd.print("BMP280 Error");
    while (1);
  }

  lcd.clear();
  lcd.print("BMP280 Ready");
  delay(2000);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  float temperature = bmp.readTemperature();

  char msg[50];
  sprintf(msg, "{\"temperature\": %.2f}", temperature);

  client.publish("esp32/bmp280/temp", msg);

  Serial.print("Sent: ");
  Serial.println(msg);

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(temperature);
  lcd.print(" C");

  lcd.setCursor(0,1);
  lcd.print("MQTT Sent");

  delay(5000);
}