#include <ESP8266WiFi.h>
#include "ThingSpeakWrpr.h"
#include "debug.h"

WiFiClient client;

const long ThingSpeakWrpr_chNr = 196416;
const char*  ThingSpeakWrpr_apiKey = "7PAWGDTXDLEDTPKN";

void ThingSpeakWrpr_init(void) {
  ThingSpeak.begin(client);
}

int ThingSpeakWrpr_sendData(float tCels, float pRelHum) {
  int rtn = 0;

  // ThingSpeak.begin(client);
  rtn = ThingSpeak.setField(THINGSPEAKWRPR_FIELD_TEMP, tCels);  // write temp in celsius
  if (rtn != TS_OK_SUCCESS) {
    LOG("Error: ThingSpeak error number: ");
    LOGLN(rtn);    
    return rtn;
  }
  rtn = ThingSpeak.setField(THINGSPEAKWRPR_FIELD_HUM, pRelHum);    // write rel humidity
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