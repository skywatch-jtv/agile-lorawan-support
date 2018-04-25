#ifndef DUST_H
#define DUST_H

#ifdef ENABLE_DUST

#define DUST_PIN  A0
#define DUST_MIN_SAMPLE_TIME_MS   5000

class Dust {
  int pin;
public:
  Dust(int pin) : pin { pin } {
    pinMode(pin, INPUT);
  }
  float getConcentration(uint32_t min_sample_time_ms) {
    const uint32_t starttime { millis() };
    uint32_t lowpulseoccupancy { 0.0 };
    uint32_t sampletime_ms { 0 };

    do {
      uint32_t duration { pulseIn(pin, LOW) };
      lowpulseoccupancy = lowpulseoccupancy+duration;
      sampletime_ms = millis()-starttime;
    } while (sampletime_ms < min_sample_time_ms);
    
    const float ratio { lowpulseoccupancy/(sampletime_ms*10.0) };  // Integer percentage 0=>100
    const float concentration { 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62 }; // using spec sheet curve
    return concentration;
  }
};

Dust dust { Dust(DUST_PIN) };

#endif
#endif

