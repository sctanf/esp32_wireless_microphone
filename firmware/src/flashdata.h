
#include <Arduino.h>

#define CFG_HOLD 129
#define EPROM_SIZE 512

//
// EEPROM data
//
struct FlashData {
  uint8_t saved;
  char ssid[32];
  char password[32];
};

