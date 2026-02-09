#include <Arduino.h>
#include <EEPROM.h>

#include "eepromstorage.h"

using InstanceSequence_t = uint8_t;
struct SequentialValue{
	uint16_t value;
	InstanceSequence_t instanceSequenceNumber;
};

struct EPEntry{
	SequentialValue seqVal;
	uint16_t crc;
};
// constants
static constexpr size_t EEPROM_SIZE  = ((E2END + 1) / sizeof(EPEntry)) * sizeof(EPEntry);
static constexpr size_t SLOT_SIZE  = ((E2END + 1) / sizeof(EPEntry));

static constexpr const EPEntry* eepromBegin = (const EPEntry*)0;
static const EPEntry* const eepromEnd = (const EPEntry*)EEPROM_SIZE;
static const EPEntry* const eepromInvalidAddress = (const EPEntry*)E2END; 



// variables
static const EPEntry* lastLoadedInstancePtr = eepromInvalidAddress;
static EPEntry lastLoadedInstance = {.seqVal = {.value = 0, .instanceSequenceNumber = 0}, .crc = 0};

// static functions declarations
static uint16_t crc16(const uint8_t* data, size_t len);
static const EPEntry* findMostRecent(const EPEntry* begin, const EPEntry* end, InstanceSequence_t& outputSequentialNumber);
static uint16_t prepareWrite(EPEntry& writeTo, uint16_t value, InstanceSequence_t previousSequenceInstance);
static bool writeToEEPROM(const EPEntry* newEntry, const EPEntry& content);


// external functions definitions

void initEEPROM(){
	InstanceSequence_t tmp;
	lastLoadedInstancePtr = findMostRecent(eepromBegin, eepromEnd, tmp);
	if(lastLoadedInstancePtr != eepromInvalidAddress){
		eeprom_read_block(&lastLoadedInstance, lastLoadedInstancePtr, sizeof(EPEntry));
	}

	//storeToEEPROM(85);
	
/*
	Serial.print('(');
	Serial.print((int)lastLoadedInstancePtr);
	Serial.print(')');
	Serial.print(F("Seq: "));
	Serial.print(lastLoadedInstance.seqVal.instanceSequenceNumber);
	Serial.print(F(" value: "));
	Serial.print(lastLoadedInstance.seqVal.value);
	Serial.print(F(" CRC: "));
	Serial.println(lastLoadedInstance.crc);
	Serial.println((int)lastLoadedInstancePtr);

	for(const EPEntry* it = eepromBegin; it != eepromEnd; ++it){
		EPEntry readEntry;
		eeprom_read_block(&readEntry, it, sizeof(EPEntry));
		Serial.print('(');
		Serial.print((int)it);
		Serial.print(')');
		Serial.print(F("Seq: "));
		Serial.print(readEntry.seqVal.instanceSequenceNumber);
		Serial.print(F(" value: "));
		Serial.print(readEntry.seqVal.value);
		Serial.print(F(" CRC: "));
		Serial.println(readEntry.crc);
	}*/

}


bool storeToEEPROM(uint16_t value){
	InstanceSequence_t lastSequentialNumber = lastLoadedInstance.seqVal.instanceSequenceNumber;
	const EPEntry* foundPrevious = lastLoadedInstancePtr;
	
	
	if(foundPrevious == eepromInvalidAddress){
		foundPrevious = eepromBegin;
	}
	else if(eepromEnd == ++foundPrevious){
		foundPrevious = eepromBegin;
	}

	EPEntry newEntry;
	for(size_t attempts = 0; attempts < SLOT_SIZE; ++attempts){
		lastSequentialNumber = prepareWrite(newEntry, value, lastSequentialNumber);
		if(writeToEEPROM(foundPrevious, newEntry)){
			return true;
		}
		if(eepromEnd == ++foundPrevious){
			foundPrevious = eepromBegin;
		}
	}
	return false;
}

bool loadFromEEPROM(volatile uint16_t& outValue){
	if(lastLoadedInstancePtr != eepromInvalidAddress){
		outValue = lastLoadedInstance.seqVal.value;
		return true;
	}
	return false;
}



// static functions definitions
static uint16_t crc16(const uint8_t* data, size_t len) {
  uint16_t crc = 0xFFFF;

  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 1)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc >>= 1;
    }
  }
  return crc;
}


static const EPEntry* findMostRecent(const EPEntry* begin, const EPEntry* end, InstanceSequence_t& outputSequentialNumber){
	const EPEntry* foundPtr = eepromInvalidAddress;
	const EPEntry* actualBegin = eepromInvalidAddress;
	InstanceSequence_t lastSequentialNumber = 0xFFFF;
	InstanceSequence_t advanceBy = 1; // in case some of the records are wore out or corrupted by unfinished write
	for(const EPEntry* foundIt = begin; foundIt != end; ++foundIt){
		EPEntry found;
		eeprom_read_block(&found, foundIt, sizeof(EPEntry));
		if(found.crc == crc16((const uint8_t*) &found.seqVal, sizeof(SequentialValue))){		
			if(actualBegin == eepromInvalidAddress){
				actualBegin = foundIt;
			}
			else if(found.seqVal.instanceSequenceNumber != InstanceSequence_t(lastSequentialNumber + advanceBy)){
				break;				
			};
			
			advanceBy = 1;
			foundPtr = foundIt;
			lastSequentialNumber = found.seqVal.instanceSequenceNumber;
		}
		else {
			++advanceBy;
		}
		
	}
	outputSequentialNumber = lastSequentialNumber;
	return foundPtr;
}

static uint16_t prepareWrite(EPEntry& writeTo, uint16_t value, InstanceSequence_t previousSequenceInstance){
	writeTo.seqVal.instanceSequenceNumber = ++previousSequenceInstance;
	writeTo.seqVal.value = value;
	writeTo.crc = crc16((const uint8_t*)&writeTo.seqVal, sizeof(SequentialValue));

	return previousSequenceInstance;
}


static bool writeToEEPROM(const EPEntry* newEntry, const EPEntry& content){
	eeprom_write_block(&content, (void*)newEntry, sizeof(EPEntry));
	EPEntry writtenConent;
	eeprom_read_block(&writtenConent, newEntry, sizeof(EPEntry));
	if(
		content.crc == writtenConent.crc &&
		content.seqVal.value == writtenConent.seqVal.value &&
		content.seqVal.instanceSequenceNumber == writtenConent.seqVal.instanceSequenceNumber
	){
		lastLoadedInstancePtr = newEntry;
		lastLoadedInstance = content;
		return true;
	}
	return false;
}
