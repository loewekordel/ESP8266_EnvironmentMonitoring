#include <Arduino.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <AsyncMqttClient.h>
#include "com_m2m.h"
#include "config.h"

bool stMqttConnected = false;
const char *mqtt_topic = MQTT_TOPIC;
static uint8_t cntrWifiRecon = 0u;

Ticker mqttReconnectTimer;
AsyncMqttClient mqttClient;

Ticker wifiReconnectTimer;
WiFiEventHandler wifiConnectedHandler;
WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
void connectToWifi();
static void onWifiConnected(const WiFiEventStationModeConnected& event);
void onWifiGotIp(const WiFiEventStationModeGotIP& event);
void onWifiDisconnect(const WiFiEventStationModeDisconnected& event);
void connectToMqtt();
void onMqttConnect(bool sessionPresent);
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
void onMqttSubscribe(uint16_t packetId, uint8_t qos);
void onMqttUnsubscribe(uint16_t packetId);
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void onMqttPublish(uint16_t packetId);

void wifiInit(void) {
  wifiConnectedHandler = WiFi.onStationModeConnected(onWifiConnected);
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiGotIp);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
}

void connectToWifi(void) {
  LOG("[WiFi] Connecting to Wi-Fi ");
  LOGLN(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

static void onWifiConnected(const WiFiEventStationModeConnected& event) {
  LOGLN("[WiFi] Wifi connected");
}

void onWifiGotIp(const WiFiEventStationModeGotIP& event) {
  LOG("[WiFi] Got IP: ");
  LOGLN(WiFi.localIP());
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  LOG("[WiFi] Wifi disconnected. Reason: ");
  LOGLN(event.reason);

  switch (event.reason) {
    case WIFI_DISCONNECT_REASON_ASSOC_LEAVE:
      break;
    default:
      if (cntrWifiRecon < WIFI_MAXRECONATTEMPTS) {
        LOGLN("[WiFi] Try to reconnect");
        mqttReconnectTimer.detach();      // ensure not attempting reconnecting to MQTT while reconnecting to Wi-Fi
        wifiReconnectTimer.once(2, connectToWifi);
        cntrWifiRecon++;
      } else {
        LOGLN("[WiFi] Max. number of reconnect attempts reached. Shutdown and try later");
        ESP.deepSleep((TDEEPSLEEP * 10e5), WAKE_RF_DEFAULT);    // Sleep for TDEEPSLEEP seconds
      }
      break;
  };
}

void connectToMqtt() {
  LOGLN("[MQTT] Connecting to MQTT");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {

  LOGLN("[MQTT] Connected to MQTT");
  LOG("[MQTT] Session present: ");
  LOGLN(sessionPresent);
  stMqttConnected = true;
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  LOGLN("[MQTT] Disconnected from MQTT");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttPublish(uint16_t packetId) {
  LOGLN("[MQTT] Publish acknowledged");
  LOG("  packetId: ");
  LOGLN(packetId);
}

uint16_t mqttPublish(float tCels, float pHum) {
  char json[150];

  sprintf(json,"{\"tCels\": %.1f, \"pHum\": %.1f}", tCels, pHum);
  return mqttClient.publish(MQTT_TOPIC, 0, true, json);
}
