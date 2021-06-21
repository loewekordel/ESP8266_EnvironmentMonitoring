#ifndef _THINGSPEAKWRPR_H_
#define _THINGSPEAKWRPR_H_

#include <ThingSpeak.h>

// #define THINGSPEAKWRPR_FIELD_TEMP (1)
// #define THINGSPEAKWRPR_FIELD_HUM  (2)
#define THINGSPEAKWRPR_FIELD_TEMP (3)
#define THINGSPEAKWRPR_FIELD_HUM  (4)

void ThingSpeakWrpr_init(void);
int ThingSpeakWrpr_sendData(float tCels, float pRelHum);

#endif
