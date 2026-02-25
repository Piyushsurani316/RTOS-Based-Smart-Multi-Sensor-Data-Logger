
#include "wifi_config.h"
#include <Arduino.h>

const char* ssid = "Prasantih";
const char* password = "Swaminarayan@1981";

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