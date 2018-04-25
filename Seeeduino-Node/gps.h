#ifndef GPS_H
#define GPS_H
#ifdef ENABLE_GPS

#include <TimerTCC0.h>

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

#else

class GPS {
 public:
 void init(void) {}
};
GPS gps = GPS();

#endif
#endif
