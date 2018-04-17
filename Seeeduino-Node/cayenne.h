#ifndef CAYENNE_H
#define CAYENNE_H

#define LPP_DIGITAL_INPUT       0       // 1 byte
#define LPP_DIGITAL_OUTPUT      1       // 1 byte
#define LPP_ANALOG_INPUT        2       // 2 bytes, 0.01 signed
#define LPP_ANALOG_OUTPUT       3       // 2 bytes, 0.01 signed
#define LPP_LUMINOSITY          101     // 2 bytes, 1 lux unsigned
#define LPP_PRESENCE            102     // 1 byte, 1
#define LPP_TEMPERATURE         103     // 2 bytes, 0.1°C signed
#define LPP_RELATIVE_HUMIDITY   104     // 1 byte, 0.5% unsigned
#define LPP_ACCELEROMETER       113     // 2 bytes per axis, 0.001G
#define LPP_BAROMETRIC_PRESSURE 115     // 2 bytes 0.1 hPa Unsigned
#define LPP_GYROMETER           134     // 2 bytes per axis, 0.01 °/s
#define LPP_GPS                 136     // 3 byte lon/lat 0.0001 °, 3 bytes alt 0.01m
#define LPP_DUST                255     // 3 byte dust float pcs/0.01cf


// Data ID + Data Type + Data Size
#define LPP_DIGITAL_INPUT_SIZE       3
#define LPP_DIGITAL_OUTPUT_SIZE      3
#define LPP_ANALOG_INPUT_SIZE        4
#define LPP_ANALOG_OUTPUT_SIZE       4
#define LPP_LUMINOSITY_SIZE          4
#define LPP_PRESENCE_SIZE            3
#define LPP_TEMPERATURE_SIZE         4
#define LPP_RELATIVE_HUMIDITY_SIZE   3
#define LPP_ACCELEROMETER_SIZE       8
#define LPP_BAROMETRIC_PRESSURE_SIZE 4
#define LPP_GYROMETER_SIZE           8
#define LPP_GPS_SIZE                 11
#define LPP_DUST_SIZE                5

class CayenneLPP {
  private:
    #define CAYENNE_BUFFER_SIZE   32
    uint8_t buffer[CAYENNE_BUFFER_SIZE];
    uint8_t cursor;
    
  public:
    /**
     * Reset the payload, to call before building a frame payload
     */
    void reset(void) { cursor = 0; };
    
    /**
     * Returns the current size of the payload
     */
    uint8_t getSize(void) { return cursor; };
    /**
     * Return the payload buffer
     */
    uint8_t* getBuffer(void) {
      return buffer;
    };
    /**
     * Copy buffer to destination
     */
    uint8_t copy(uint8_t* dst) {
        memcpy(dst, buffer, cursor);
        return cursor;
    };
    
    uint8_t addDigitalInput(uint8_t channel, uint8_t value){
        if ((cursor + LPP_DIGITAL_INPUT_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_DIGITAL_INPUT; 
        buffer[cursor++] = value; 
        return cursor;
    };
    uint8_t addDigitalOutput(uint8_t channel, uint8_t value){
        if ((cursor + LPP_DIGITAL_OUTPUT_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_DIGITAL_OUTPUT; 
        buffer[cursor++] = value; 
    
        return cursor;
    };

    uint8_t addAnalogInput(uint8_t channel, float value) {
        if ((cursor + LPP_ANALOG_INPUT_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        
        int16_t val = value * 100;
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_ANALOG_INPUT; 
        buffer[cursor++] = val >> 8; 
        buffer[cursor++] = val; 
    
        return cursor;
    };
    uint8_t addAnalogOutput(uint8_t channel, float value) {
        if ((cursor + LPP_ANALOG_OUTPUT_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        int16_t val = value * 100;
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_ANALOG_OUTPUT;
        buffer[cursor++] = val >> 8; 
        buffer[cursor++] = val; 
    
        return cursor;
    };
    uint8_t addLuminosity(uint8_t channel, uint16_t lux) {
        if ((cursor + LPP_LUMINOSITY_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_LUMINOSITY; 
        buffer[cursor++] = lux >> 8; 
        buffer[cursor++] = lux; 
    
        return cursor;
    };
    uint8_t addPresence(uint8_t channel, uint8_t value) {
        if ((cursor + LPP_PRESENCE_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_PRESENCE; 
        buffer[cursor++] = value; 
    
        return cursor;
    };
    uint8_t addTemperature(uint8_t channel, float celsius) {
        if ((cursor + LPP_TEMPERATURE_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        int16_t val = celsius * 10;
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_TEMPERATURE; 
        buffer[cursor++] = val >> 8; 
        buffer[cursor++] = val; 
    
        return cursor;
    };
    uint8_t addRelativeHumidity(uint8_t channel, float rh) {
        if ((cursor + LPP_RELATIVE_HUMIDITY_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_RELATIVE_HUMIDITY; 
        buffer[cursor++] = rh * 2; 
    
        return cursor;
    };
    uint8_t addAccelerometer(uint8_t channel, float x, float y, float z) {
        if ((cursor + LPP_ACCELEROMETER_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        int16_t vx = x * 1000;
        int16_t vy = y * 1000;
        int16_t vz = z * 1000;
        
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_ACCELEROMETER; 
        buffer[cursor++] = vx >> 8; 
        buffer[cursor++] = vx; 
        buffer[cursor++] = vy >> 8; 
        buffer[cursor++] = vy; 
        buffer[cursor++] = vz >> 8; 
        buffer[cursor++] = vz; 
    
        return cursor;
    };
    uint8_t addBarometricPressure(uint8_t channel, float hpa) {
        if ((cursor + LPP_BAROMETRIC_PRESSURE_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        int16_t val = hpa * 10;
        
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_BAROMETRIC_PRESSURE; 
        buffer[cursor++] = val >> 8; 
        buffer[cursor++] = val; 
    
        return cursor;
    };
    uint8_t addGyrometer(uint8_t channel, float x, float y, float z) {
        if ((cursor + LPP_GYROMETER_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        int16_t vx = x * 100;
        int16_t vy = y * 100;
        int16_t vz = z * 100;
        
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_GYROMETER; 
        buffer[cursor++] = vx >> 8; 
        buffer[cursor++] = vx; 
        buffer[cursor++] = vy >> 8; 
        buffer[cursor++] = vy; 
        buffer[cursor++] = vz >> 8; 
        buffer[cursor++] = vz; 
    
        return cursor;
    };
    uint8_t addGPS(uint8_t channel, float latitude, float longitude, float meters) {
        if ((cursor + LPP_GPS_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        int32_t lat = latitude * 10000;
        int32_t lon = longitude * 10000;
        int32_t alt = meters * 100;
        
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_GPS; 
    
        buffer[cursor++] = lat >> 16; 
        buffer[cursor++] = lat >> 8; 
        buffer[cursor++] = lat; 
        buffer[cursor++] = lon >> 16; 
        buffer[cursor++] = lon >> 8; 
        buffer[cursor++] = lon; 
        buffer[cursor++] = alt >> 16; 
        buffer[cursor++] = alt >> 8;
        buffer[cursor++] = alt;
    
        return cursor;
    };
    
    uint8_t addDust(uint8_t channel, float dust) {
      /*
        if ((cursor + LPP_DUST_SIZE) > CAYENNE_BUFFER_SIZE) {
            return 0;
        }
        int32_t d = dust* 100;
        
        buffer[cursor++] = channel; 
        buffer[cursor++] = LPP_DUST; 
    
        buffer[cursor++] = d >> 16; 
        buffer[cursor++] = d >> 8; 
        buffer[cursor++] = d;
    */
        return cursor;
    };
};

#endif

