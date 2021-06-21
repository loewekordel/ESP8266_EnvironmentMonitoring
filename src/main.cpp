#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include "snsr_sht30.h"
#include "types.h"
#include "debug.h"
#include "ThingSpeakWrpr.h"

#define TSLEEP  (600)   // Time to sleep (in seconds):

#define WIFI_SSID "COM8943b"
#define WIFI_PASSWORD "pyCBfVbaabTPA7T2"
#define WIFI_MAXRECONATTEMPTS (5)

#define MQTT_HOST IPAddress(192, 168, 8, 100)
#define MQTT_PORT (1883)

bool enaSendData = false;
const char * mqtt_topic = "EnvData/Living";
static uint8_t cntrWifiRecon = 0u;

envData_t envData = {
  .tCels = 0.0,
  .pHum = 0.0
};

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

void connectToWifi() {
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
        ESP.deepSleep((TSLEEP * 10e5), WAKE_RF_DEFAULT);    // Sleep for TSLEEP seconds
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
  enaSendData = true;
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

void setup() {
  Serial.begin(115200);
  LOGLN();
  LOGLN();

  wifiConnectedHandler = WiFi.onStationModeConnected(onWifiConnected);
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiGotIp);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  ThingSpeakWrpr_init();
  connectToWifi();
}

void loop() {
  char json[150];

  if (enaSendData) {
    sht30_getData(&envData.tCels, &envData.pHum);
    ThingSpeakWrpr_sendData(envData.tCels, envData.pHum);
    sprintf(json,"{\"tCels\": %.1f, \"pHum\": %.1f}", envData.tCels, envData.pHum);
    mqttClient.publish(mqtt_topic, 0, true, json);
    LOGLN("[MQTT] Publishing at QoS 0");
    enaSendData = false;
    delay(5);   // MQTT stack needs some time to process publish
    ESP.deepSleep((TSLEEP * 10e5), WAKE_RF_DEFAULT);    // Sleep for TSLEEP seconds
  }
}