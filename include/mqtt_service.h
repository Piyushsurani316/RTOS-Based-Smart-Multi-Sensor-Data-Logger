#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern PubSubClient client;

void initMQTT();
void reconnectMQTT();
void publishData(const char* topic, const char* payload);

#endif