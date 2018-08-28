#ifndef EEPROM_ARD_H
#define EEPROM_ARD_H

#include <Arduino.h>

#undef  NEW_EEPROM

#ifndef NEW_EEPROM
#define MAX_EEPROM_DIM	      EEPROM.length()
#else
#define MAX_EEPROM_DIM_STM			    32766
#endif

#define MAX_EEPROM_DIM_NODEMCU			  512

#define MAX_CELL_EEPROM	                  254
#define EMPTY_MEMORY_VALUE				  255

typedef enum
{
	RELE_1_ADDR = 0,
	RELE_2_ADDR,
	RELE_3_ADDR,
	RELE_4_ADDR,
	RELE_5_ADDR,
	RELE_6_ADDR,
	RELE_7_ADDR,
	RELE_8_ADDR
}RELE_EEPROM_ADDR;

#define BAND_INIT_HOUR_ADDR					   10
#define BAND_INIT_MINUTE_ADDR				   11
#define BAND_END_HOUR_ADDR					   12
#define BAND_END_MINUTE_ADDR				   13
#define BAND_INVALIDATION_VALUE_ADDR		   14

#define WIFI_SSID_ADDR						   20

#define TIMER_BACKLIGHT_ADDR				   21
#define UDM_ENERGY_ADDR				           22

#define WIFI_WPS_CONF_ADDR					   23

#define FIRST_TARIFF_NUMBER_ADDR               24
#define SECOND_TARIFF_NUMBER_ADDR              25
#define THIRD_TARIFF_NUMBER_ADDR               26
#define FOURTH_TARIFF_NUMBER_ADDR              27
#define TARIFF_STATUS_ADDR					   28

#define FLAG_VALUE_ADDR					 	   50
#define NUMBER_REG_BACKLIGHT				   51	

#define FIRST_START_CHECK_ADDR			      100

#define FACTORY_RESET_ADDR					  200

typedef struct
{
	uint32_t MaxValue;
	uint8_t NumReg;
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


void EepromUpdate(short address, short value);
void EepromInit(void);
void WriteMemory(short address, short value);
// int WriteMemory(short address, short value);
bool ReadMemory(short address, short *value);
// bool ReadMemory(short address, short numReg, short *value);
bool ClearMemory(void);
bool IsMemoryEmpty(void);

void WriteBigData(short InitAddress, uint32_t Value);
uint32_t ReadBigData(short InitAddress, EEPROM_RANGE_ITEM Range);


#endif
