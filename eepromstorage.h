#ifndef EEPROM_STORAGE_H
#define EEPROM_STORAGE_H
#include <inttypes.h>

extern void initEEPROM();
extern bool storeToEEPROM(uint16_t value);
extern bool loadFromEEPROM(volatile uint16_t& outValue);
#endif