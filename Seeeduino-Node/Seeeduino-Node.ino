// see definitions in lora.h
#define DEVICE2

#define ENABLE_LORA
#if defined(DEVICE1) || defined(DEVICE2) || defined(DEVICE3)
#  define ENABLE_SHT_31
#else
#  define ENABLE_TH02
#endif
//#define ENABLE_DUST
//#define ENABLE_GPS


#include <Arduino.h>
#include "lora.h"
#include "cayenne.h"
#include "dust.h"
#include "gps.h"
#include "sht_31.h"
#include "th_02.h"


#define MILLIS_LOOP_DELAY   20000


void setup(void)
{
  SerialUSB.begin(115200);
  delay(1000);
  SerialUSB.println("initialization started.\n");
  setup_sht31();
  setup_th02();
  setup_lora();
  gps.init();
  SerialUSB.println("All initialized.\n");
  delay(2000);
}


void loop(void)
{
  static uint32_t last_millis = 0xffffffff-MILLIS_LOOP_DELAY+2500;
  static uint32_t measurement_counter = 0;
  uint32_t now = millis();
  if ((now >= last_millis && now-last_millis < MILLIS_LOOP_DELAY) ||
      (now < last_millis && (0xffffffff-last_millis+now < MILLIS_LOOP_DELAY))) {
    return;
  }
  last_millis = now;

  SerialUSB.print("Measurement: ");
  SerialUSB.print(++measurement_counter);
  SerialUSB.println();
  
  CayenneLPP cayenne;
  bool result { false };

  cayenne.reset();
  
#ifdef ENABLE_GPS
  if (gps.isValid()) {
    SerialUSB.print("Time: ");
    SerialUSB.print(gps.getHour());
    SerialUSB.print(':');
    SerialUSB.print(gps.getMinute());
    SerialUSB.print(':');
    SerialUSB.println(gps.getSecond());
    
    SerialUSB.print("Lat: ");
    SerialUSB.println(gps.getLattitude());
    SerialUSB.print("Lon: ");
    SerialUSB.println(gps.getLongitude());

    cayenne.addGPS(0, gps.getLattitude(), gps.getLongitude(), 0.0);
  }
#endif

#ifdef ENABLE_DUST
  float concentration { dust.getConcentration(DUST_MIN_SAMPLE_TIME_MS) };
  SerialUSB.print("Dust concentration = ");
  SerialUSB.print(concentration);
  SerialUSB.println(" pcs/0.01cf");

  cayenne.addDust(1, concentration);
#endif

#if defined(ENABLE_SHT_31) || defined(ENABLE_TH02)
  if (readAvgTempHum()) {
    if (avg_temp > 100.0 || avg_temp <  -50.0 || avg_hum > 100.0 || avg_hum < 0.0) {
      SerialUSB.println("Invalid temperature/humidity");
    }
    else {
      SerialUSB.print("Temp *C = "); SerialUSB.println(avg_temp);
      cayenne.addTemperature(2, avg_temp);
      SerialUSB.print("Hum. % = "); SerialUSB.println(avg_hum);
      cayenne.addRelativeHumidity(3, avg_hum);
    }
  }
#endif

#ifdef ENABLE_LORA
  if (cayenne.getSize() > 0) {
    result = lora.transferPacket(cayenne.getBuffer(), cayenne.getSize(), 10);//DEFAULT_TIMEOUT);
    
    if(result)
    {
      short length;
      short rssi;
      
      /*
      memset(lora_buffer, 0, 256);
      length = lora.receivePacket(lora_buffer, 256, &rssi);
      
      if(length)
      {
        SerialUSB.print("Length is: ");
        SerialUSB.println(length);
        SerialUSB.print("RSSI is: ");
        SerialUSB.println(rssi);
        SerialUSB.print("Data is: ");
        for(unsigned char i = 0; i < length; i ++)
        {
          SerialUSB.print("0x");
          SerialUSB.print(lora_buffer[i], HEX);
          SerialUSB.print(" ");
        }
        SerialUSB.println();
      }
      */
    }
    else {
        SerialUSB.println("Unable to send packet");
    }
  }
  else {
    SerialUSB.println("no data to send, Silent LoRaWAN !");
  }
#endif
  SerialUSB.println();
}


