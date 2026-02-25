#include "mqtt_service.h"
#include <Arduino.h>

const char* mqtt_server = "322ca06e4c25499ca552090b47f74baa.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "esp32user";
const char* mqtt_password = "p12345678P";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void initMQTT() {
    espClient.setInsecure();
    client.setServer(mqtt_server, mqtt_port);
}

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

void publishData(const char* topic, const char* payload) {
    client.publish(topic, payload);
}