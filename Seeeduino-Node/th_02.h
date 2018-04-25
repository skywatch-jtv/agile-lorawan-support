#ifndef TH_02_H
#define TH_02_H
#ifdef ENABLE_TH_02

#include <TH02_dev.h>

void setup_th02(void) {
  TH02.begin();
}

float avg_temp, avg_hum;

bool readAvgTempHum()
{
  const int NUM = 5;
  float t=0.0, h=0.0;
  for (int num=0; num<NUM; num++) {
    t += TH02.ReadTemperature();
    h += TH02.ReadHumidity();
    delay(50);
  }
  avg_temp = t/NUM;
  avg_hum = h/NUM;
}

#else

void setup_th02(void) {}

#endif
#endif

