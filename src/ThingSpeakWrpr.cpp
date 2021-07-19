#include <ESP8266WiFi.h>
#include "ThingSpeakWrpr.h"
#include "debug.h"
#include "config.h"

WiFiClient client;

const long ThingSpeakWrpr_chNr = THINGSPEAK_CHNR;
const char*  ThingSpeakWrpr_apiKey = THINGSPEAK_APIKEY;

void ThingSpeakWrpr_init(void) {
  ThingSpeak.begin(client);
}

int ThingSpeakWrpr_sendData(float tCels, float pRelHum) {
  int rtn = 0;

  // ThingSpeak.begin(client);
  rtn = ThingSpeak.setField(THINGSPEAK_FIELD_TEMP, tCels);  // write temp in celsius
  if (rtn != TS_OK_SUCCESS) {
    LOG("Error: ThingSpeak error number: ");
    LOGLN(rtn);    
    return rtn;
  }
  rtn = ThingSpeak.setField(THINGSPEAK_FIELD_HUM, pRelHum);    // write rel humidity
  if (rtn != TS_OK_SUCCESS) {
    LOG("Error: ThingSpeak error number: ");
    LOGLN(rtn);
    return rtn;
  }
  rtn = ThingSpeak.writeFields(ThingSpeakWrpr_chNr, ThingSpeakWrpr_apiKey);
  if (rtn != TS_OK_SUCCESS) {
    LOG("Error: ThingSpeak error number: ");
    LOGLN(rtn);
    return rtn;
  }

  LOGLN("[TS] Data sent");
  return 0;
}