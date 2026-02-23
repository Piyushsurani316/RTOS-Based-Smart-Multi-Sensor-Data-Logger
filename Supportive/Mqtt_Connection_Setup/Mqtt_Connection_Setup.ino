#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// WiFi Credentials
const char* ssid = "Prasantih";
const char* password = "Swaminarayan@1981";

// HiveMQ Cloud Details
const char* mqtt_server = "322ca06e4c25499ca552090b47f74baa.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;

const char* mqtt_user = "esp32user";
const char* mqtt_password = "p12345678P";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT...");

    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  connectWiFi();
  Serial.println(WiFi.localIP());

  espClient.setInsecure();  // Simplified TLS (no certificate verification)

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  float temperature = random(20, 35);  // Example sensor data

  char msg[50];
  sprintf(msg, "{\"temperature\": %.2f}", temperature);

  client.publish("esp32/sensor/temp", msg);

  Serial.println("Data Sent: " + String(msg));

  delay(5000);
}