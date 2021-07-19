#ifndef _CONFIG_H_
#define _CONFIG_H_

#define TDEEPSLEEP (600)   // Time to deepsleep (in seconds):

#define WIFI_SSID "COM8943b"
#define WIFI_PASSWORD "pyCBfVbaabTPA7T2"
#define WIFI_MAXRECONATTEMPTS (5)

#define MQTT_HOST IPAddress(192, 168, 8, 100)
#define MQTT_PORT (1883)
#define MQTT_TOPIC "EnvData/Living"

#define THINGSPEAK_CHNR (196416)
#define THINGSPEAK_APIKEY "7PAWGDTXDLEDTPKN"
#define THINGSPEAK_FIELD_TEMP (1)
#define THINGSPEAK_FIELD_HUM  (2)
// #define THINGSPEAK_FIELD_TEMP (3)
// #define THINGSPEAK_FIELD_HUM  (4)

#endif
