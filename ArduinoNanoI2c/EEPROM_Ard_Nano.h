#ifndef EEPROM_ARD_H
#define EEPROM_ARD_H

#include <Arduino.h>

#define MAX_EEPROM_DIM	      EEPROM.length()

#define ENERGY_MEMORY_INIT_ADDR				0
#define ENERGY_MEMORY_SIZE  				4

#define MAX_CELL_EEPROM	                  254
#define EMPTY_MEMORY_VALUE				  255


typedef struct
{
	uint16_t ItemInitAddress;
	uint8_t  ItemBlockSize;
	uint8_t NumberElements[4];
	uint32_t SavedValue;
}EEPROM_DATA;

typedef enum
{
	ENERGY_VALUE = 0,
	MAX_EEPROM_ITEM
	
}EEPROM_DATA_ITEM;

#define ENERGY_VALUE_INIT_ADDR		0

void WriteMemory(short address, uint8_t value);
bool ReadMemory(short address, uint8_t *value);
bool ClearMemory(void);
bool IsMemoryEmpty(void);


void NumberDecomposition(EEPROM_DATA EepromItem[], uint8_t EepromTabIndex);
void NumberComposition(EEPROM_DATA EepromItem[], uint8_t EepromTabIndex);
void WriteBigData(EEPROM_DATA EepromItem[], uint8_t EepromTabIndex);
void ReadBigData(EEPROM_DATA EepromItem[], uint8_t EepromTabIndex);


#endif
