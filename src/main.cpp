#include "types.h"
#include "debug.h"
#include "config.h"
#include "ThingSpeakWrpr.h"
#include "com_m2m.h"
#include "snsr_sht30.h"

envData_t envData = {
  .tCels = 0.0,
  .pHum = 0.0
};

void setup() {
  Serial.begin(115200);
  LOGLN();
  LOGLN();

  wifiInit();
  ThingSpeakWrpr_init();
  connectToWifi();
}

void loop() {
  if (stMqttConnected) {
    sht30_getData(&envData.tCels, &envData.pHum);
    ThingSpeakWrpr_sendData(envData.tCels, envData.pHum);
    mqttPublish(envData.tCels, envData.pHum);
    LOGLN("[MQTT] Publishing");
    stMqttConnected = false;
    delay(5);   // MQTT stack needs some time to process publish
    ESP.deepSleep((TDEEPSLEEP * 10e5), WAKE_RF_DEFAULT);    // Sleep for TDEEPSLEEP seconds
  }
}