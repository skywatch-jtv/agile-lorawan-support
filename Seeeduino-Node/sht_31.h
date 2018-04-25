#ifndef SHT31_H
#define SHT31_H

#ifdef ENABLE_SHT_31

#include <Wire.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 { Adafruit_SHT31() };

void setup_sht31(void) {
  while (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    SerialUSB.println("Couldn't find SHT31");
    delay(1000);
  }
}

float avg_temp, avg_hum;

bool readAvgTempHum()
{
  if (sht31.readStatus()!= 0xffff) {
    const int NUM = 5;
    float t=0.0, h=0.0;
    for (int num=0; num<NUM; num++) {
      sht31.readTempHum();
      t += sht31.temp;
      h += sht31.humidity;
      delay(50);
    }
    avg_temp = t/NUM;
    avg_hum = h/NUM;
  } else {
    SerialUSB.println("Sensor missing!");
    sht31.reset();
  }
}
#else

void setup_sht31(void) {}

#endif

#endif
