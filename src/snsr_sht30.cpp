#include "snsr_sht30.h"

SHT3X sht30(0x45);

byte sht30_getData(float *tCels, float *pHum) {
  if (sht30.get() != 0) {
      return 1;
  }
  *tCels = sht30.cTemp;
  *pHum = sht30.humidity;
  return 0;
}
