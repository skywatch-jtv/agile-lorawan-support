#ifndef LORA_H
#define LORA_H

#ifdef ENABLE_LORA
#include <LoRaWan.h>

#define LORA_BUFFER_SIZE          256
#define LORA_APP_EUID             "my-app"
#define LORA_APPLICATION_KEY      "00000000000000000000000000100203"

#ifdef DEVICE1
// HARDWARE
#define LORA_DEVICE_NAME          "Hardware"
#define LORA_DEVICE_EUID          "4799b26900370056"
#define LORA_DEVICE_ADDR          "076e8bb8"
#define LORA_NETWORK_SESSION_KEY  "aef22e2f9add468adbeb7f7e5d6059b0"
#define LORA_APP_SESSION_KEY      "dc7d6106a3318c9dc0558827ae194b44"
#endif
#ifdef DEVICE2
// SOFTWARE
#define LORA_DEVICE_NAME          "Software"
#define LORA_DEVICE_EUID          "4799b26900370057"
#define LORA_DEVICE_ADDR          "06de3d9b"
#define LORA_NETWORK_SESSION_KEY  "c5d5dfbdef9f42779587d65312e4367a"
#define LORA_APP_SESSION_KEY      "2461fce0304f8c2a596030d2d04e4b32"
#endif
#ifdef DEVICE3
// BASEMENT
#define LORA_DEVICE_NAME          "Basement"
#define LORA_DEVICE_EUID          "4799b26900370058"
#define LORA_DEVICE_ADDR          "07a471c3"
#define LORA_NETWORK_SESSION_KEY  "e012370615e8769473a527287f146af5"
#define LORA_APP_SESSION_KEY      "296b0a118e81c6e16beb47198fa81c6c"
#endif
#ifdef DEVICE4
#define LORA_DEVICE_NAME          "Topfloor"
#define LORA_DEVICE_EUID          "4799b26900370059"
#define LORA_DEVICE_ADDR          "068d1fae"
#define LORA_NETWORK_SESSION_KEY  "43c29fd8b4cfcf452d8ace17f4f05308"
#define LORA_APP_SESSION_KEY      "da0bd58c7c28e0dadae9b52651a231ea"
#endif

char lora_buffer[LORA_BUFFER_SIZE];

void setup_lora(void) {
  SerialUSB.print("LORA_DEVICE_NAME: ");
  SerialUSB.println(LORA_DEVICE_NAME);
  SerialUSB.print("LORA_DEVICE_EUID: ");
  SerialUSB.println(LORA_DEVICE_EUID);
  SerialUSB.print("LORA_DEVICE_ADDR: ");
  SerialUSB.println(LORA_DEVICE_ADDR);
  SerialUSB.print("LORA_NETWORK_SESSION_KEY: ");
  SerialUSB.println(LORA_NETWORK_SESSION_KEY);
  SerialUSB.print("LORA_APP_SESSION_KEY: ");
  SerialUSB.println(LORA_APP_SESSION_KEY);
  SerialUSB.println();
  
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

#endif
