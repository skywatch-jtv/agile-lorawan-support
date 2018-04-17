
#define ENABLE_LORA
#define ENABLE_SHT_31
//#define ENABLE_DUST
#define ENABLE_GPS

#include <Arduino.h>
#include <TimerTCC0.h>
#include "cayenne.h"

#ifdef ENABLE_LORA
#include <LoRaWan.h>

#define LORA_BUFFER_SIZE          256
#define LORA_DEVICE_ADDR          "0111b223"
#define LORA_DEVICE_EUID          "4799b26900370056"
#define LORA_APP_EUID             "my-app"
#define LORA_NETWORK_SESSION_KEY  "2B7E151628AED2A6ABF7158809CF4F3C"
#define LORA_APP_SESSION_KEY      "2B7E151628AED2A6ABF7158809CF4F3C"
#define LORA_APPLICATION_KEY      "00000000000000000000000000100203"

char lora_buffer[LORA_BUFFER_SIZE];

void setup_lora(void) {
    lora.init();
    
    lora.setId(LORA_DEVICE_ADDR, LORA_DEVICE_EUID, LORA_APP_EUID);
    lora.setKey(LORA_NETWORK_SESSION_KEY, LORA_APP_SESSION_KEY, LORA_APPLICATION_KEY);
    
    lora.setDeciveMode(LWABP);
    lora.setDataRate(DR0, EU868);
    
    lora.setChannel(0, 868.1);
    lora.setChannel(1, 868.3);
    lora.setChannel(2, 868.5);
    
    lora.setReceiceWindowFirst(0, 868.1);
    lora.setReceiceWindowSecond(869.5, DR3);
    
    lora.setDutyCycle(false);
    lora.setJoinDutyCycle(false);
    
    lora.setPower(14);
}
#else
void setup_lora(void) {}
#endif


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


#ifdef ENABLE_SHT_31
#include <Wire.h>
#include "Adafruit_SHT31.h"

Adafruit_SHT31 sht31 { Adafruit_SHT31() };

void setup_sht31(void) {
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    SerialUSB.println("Couldn't find SHT31");
    while (1) delay(1);
  }
}
#else
void setup_sht31(void) {}
#endif


#ifdef ENABLE_GPS
class GPS {
private:
  #define BUFFER_LEN  256
  char buffer[BUFFER_LEN];
  int buffer_index = 0;

  bool valid;
  uint8_t hour, minute, second;
  float lat, lon;
  
  static int strnchr_index(const char *str, char ch, size_t n) {
    for (int i=0; i<n; i++) {
      if (str[i] == ch) {
        return i;
      }
    }
    return -1;
  }

  static int hex2dec(char ch) {
    if (ch >= '0' && ch <= '9') return ch-'0';
    if (ch >= 'A' && ch <= 'F') return ch-'A'+10;
    if (ch >= 'a' && ch <= 'f') return ch-'a'+10;
    return 0;
  }
  
  static bool checkChecksum(const char *str, size_t n) {
    if (*str == '$') {
      char checksum = 0;
      ++str;
      while (*str != '*') {
        checksum ^= *str;
        ++str;
      }
      ++str; // skip '*', if we're over the end, it's a good thing we only read :-)
      int expected = (hex2dec(*str++) << 4) | hex2dec(*str);

      return expected == checksum;
    }
    return false;
  }

  static void split(char *str, size_t str_n, char split_chr, char **split_arr, size_t arr_n) {
    size_t str_i = 0;
    size_t arr_i = 0;
    while (arr_i < arr_n) {
      split_arr[arr_i++] = &str[str_i];
      for (; str_i < str_n; str_i++) {
        if (str[str_i] == split_chr) {
          str[str_i] = 0;
          ++str_i;
          break;
        }
      }
    }
  }

  static float convert_lat(const char *str, bool isN) {
    float d = (str[0]-'0')*10 + (str[1]-'0')*1;
    float m = atof(&str[2])/60.0;
    return (isN)? d + m : 360 - (d + m);
  }

  static float_t convert_lon(const char *str, bool isE) {
    float d = (str[0]-'0')*100 + (str[1]-'0')*10 + (str[2]-'0')*1;
    float m = atof(&str[3])/60.0;
    return (isE)? d + m : 360 - (d + m);
  }
  
  void parseRMC(char str[]) {
    char *split_str[12];
    split(str, strlen(str), ',', split_str, 12);

    if (0 == strcmp(split_str[2], "A")) {
      valid = true;
      const char *time_str = split_str[1];
      hour = (time_str[0]-'0')*10 + (time_str[1]-'0');
      minute = (time_str[2]-'0')*10 + (time_str[3]-'0');
      second = (time_str[4]-'0')*10 + (time_str[5]-'0');
      
      const char *lat_str = split_str[3];
      const char *lat_dir = split_str[4];
      lat = convert_lat(lat_str, *lat_dir == 'N');
      
      const char *lon_str = split_str[5];
      const char *lon_dir = split_str[6];
      lon = convert_lon(lon_str, *lon_dir == 'E');
    }
    else {
      valid = false;
    }
    return;
  }
  
  void parse_line(char *str, size_t n) {
    // last char might be '\r' - remove it
    if (str[n-1] == '\r') {
      --n;
      str[n] = '\0';
    }
    
    if (str[n-3] == '*' && checkChecksum(str, n)) {
      if ('$' == str[0] && strncmp(&str[3], "RMC", 3) == 0) {
        parseRMC(str);
      }
    }
  }

public:
  bool isValid() { return valid; }
  uint8_t getHour() { return hour; }
  uint8_t getMinute() { return minute; }
  uint8_t getSecond() { return second; }
  float getLattitude() { return lat; }
  float getLongitude() { return lon; }
  
  void init(void) {
    Serial.begin(9600);
    TimerTcc0.initialize(1000);
    TimerTcc0.attachInterrupt(timerIsr);
  }

private:
  friend void timerIsr(void);
  void read(void) {
    
    // fill buffer with newest content
    int ch;
    bool r = false;
    while(buffer_index < (BUFFER_LEN - 1) && (ch = Serial.read()) >= 0) {
      buffer[buffer_index++] = (char)ch;
      r = true;
    }
    if (!r) return;
    
    int cmd_end_index = strnchr_index(buffer, '\n', BUFFER_LEN);
    while (cmd_end_index >= 0) {
      // terminate command in buffer
      buffer[cmd_end_index] = '\0'; 

      parse_line(buffer, cmd_end_index);
      
      // move end-of-buffer to start-of-buffer
      if (buffer[cmd_end_index+1] == '\r') ++cmd_end_index;
      ++cmd_end_index;
      buffer_index -= cmd_end_index;
      for (int i=0; cmd_end_index < BUFFER_LEN; cmd_end_index++, i++) {
        buffer[i] = buffer[cmd_end_index];
      }
      
      // check for more lines in the buffer
      cmd_end_index = strnchr_index(buffer, '\n', BUFFER_LEN);
    }
  }
};

GPS gps = GPS();

void timerIsr(void)
{
  gps.read();
}
#endif


void setup(void)
{
  SerialUSB.begin(115200);
  while(!SerialUSB);

  setup_sht31();
  setup_lora();
  gps.init();
}


void loop(void)
{
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

#ifdef ENABLE_SHT_31
  float t { sht31.readTemperature() };
  float h { sht31.readHumidity() };

  if (! isnan(t)) {  // check if 'is not a number'
    SerialUSB.print("Temp *C = "); SerialUSB.println(t);
    cayenne.addTemperature(2, t);
  } else { 
    SerialUSB.println("Failed to read temperature");
  }
  
  if (! isnan(h)) {  // check if 'is not a number'
    SerialUSB.print("Hum. % = "); SerialUSB.println(h);
    cayenne.addRelativeHumidity(3, h);
  } else {
    SerialUSB.println("Failed to read humidity");
  }
#endif

#ifdef ENABLE_LORA
  result = lora.transferPacket(cayenne.getBuffer(), cayenne.getSize(), 10);//DEFAULT_TIMEOUT);
  
  if(result)
  {
    short length;
    short rssi;
    
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
  }
#endif

  delay(10*1000);
}


