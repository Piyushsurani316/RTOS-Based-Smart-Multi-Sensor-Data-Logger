#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

// WiFi
const char* ssid = "Prasantih";
const char* password = "Swaminarayan@1981";

// HiveMQ Cloud
const char* mqtt_server = "322ca06e4c25499ca552090b47f74baa.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "esp32user";
const char* mqtt_password = "p12345678P";

WiFiClientSecure espClient;
PubSubClient client(espClient);

Adafruit_BMP280 bmp;

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

  connectWiFi();

  espClient.setInsecure();  // Skip certificate check
  client.setServer(mqtt_server, mqtt_port);

  if (!bmp.begin(0x76)) {   // try 0x77 if not working
    Serial.println("BMP280 not found!");
    while (1);
  }

  Serial.println("BMP280 Ready");
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

  delay(5000);
}