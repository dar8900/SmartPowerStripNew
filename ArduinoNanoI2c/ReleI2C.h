#ifndef RELE_I2C_H
#define RELE_I2C_H
#include <Arduino.h>

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
	
}RECEIVE_RELE_STATUS;

void ReleOn(short ReleIdx);
void ReleOff(short ReleIdx);
#endif