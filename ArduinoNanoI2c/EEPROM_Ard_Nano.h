#ifndef EEPROM_ARD_H
#define EEPROM_ARD_H

#include <Arduino.h>

#define MAX_EEPROM_DIM	      EEPROM.length()



#define MAX_CELL_EEPROM	                  254
#define EMPTY_MEMORY_VALUE				  255


typedef struct
{
	uint32_t MaxValue;
	uint8_t  NumReg;
}EEPROM_RANGE;

typedef enum
{	
	SINGLE_REGISTER = 0,
	THREE_REG,			
	FOUR_REG,			
	FIVE_REG,			
	SIX_REG,			
	SEVEN_REG,			
	EIGHT_REG,			
	NINE_REG,						
	MAX_RANGE_ITEM
}EEPROM_RANGE_ITEM;

#define ENERGY_VALUE_INIT_ADDR		0

void WriteMemory(short address, uint8_t value);
bool ReadMemory(short address, uint8_t *value);
bool ClearMemory(void);
bool IsMemoryEmpty(void);

void WriteBigData(short InitAddress, uint32_t Value);
int32_t ReadBigData(short InitAddress, EEPROM_RANGE_ITEM Range);


#endif
