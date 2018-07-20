
#ifndef I2C_NANO_H
#define I2C_NANO_H
#include <Arduino.h>
#define ARDUINO_ADDR	0x08


typedef enum
{
	BUTTON_UP = 0,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_SET,
	NO_PRESS
}BUTTONS;

typedef enum
{
	BUTTON = 0,
	ENERGY,
	CURRENT
}I2C_WRITE_READ;

typedef enum
{
	RELE_1_OFF = 3,
	RELE_2_OFF,
	RELE_3_OFF,
	RELE_4_OFF,
	RELE_5_OFF,
	RELE_6_OFF,
	RELE_7_OFF,
	RELE_8_OFF,
	RELE_1_ON,
	RELE_2_ON,
	RELE_3_ON,
	RELE_4_ON,
	RELE_5_ON,
	RELE_6_ON,
	RELE_7_ON,
	RELE_8_ON	
}SEND_RELE_STATUS;

short CheckButtons(void);
String EnergyValueStr(void);
String CurrentValueStr(void);
String ReadMeasure(short WichInfo);
void ReadButton(short *ButtonVal);

void ReleOn(short ReleIdx);
void ReleOff(short ReleIdx);

// void ReadEnergy(String *EnergyStr);
// void ReadCurrent(String *CurrentStr);
#endif