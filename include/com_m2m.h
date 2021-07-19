#ifndef _COM_M2M_H_
#define _COM_M2M_H_

extern bool stMqttConnected;

extern void wifiInit(void);
extern void connectToWifi(void);
uint16_t mqttPublish(float tCels, float pHum);

#endif
