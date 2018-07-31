#include <Wire.h>
#include "SmartPowerStrip.h"
#include "I2CNano.h"
// #include "LCDLib.h"

#define 	CHAR_FROM_NANO	    9


short CheckButtons()
{
	uint8_t ButtonPress = NO_PRESS;
	ReadButton(&ButtonPress);
	return ButtonPress;
}

String EnergyValueStr()
{
	return ReadMeasure(ENERGY);	
}

String CurrentValueStr()
{
	return ReadMeasure(CURRENT);
}

// String PowerValueStr()
// {
	// return ReadMeasure(POWER);
// }

void ReadButton(uint8_t *ButtonVal)
{
	uint8_t ReadInfo;
	Wire.beginTransmission(ARDUINO_ADDR);
	Wire.write(BUTTON);
	Wire.endTransmission();
	delay(1);
	Wire.requestFrom(ARDUINO_ADDR, 1);
	while(Wire.available())
	{
   		ReadInfo = Wire.read();
	}
	switch (ReadInfo)
	{
		case 0:
			*ButtonVal = BUTTON_UP;
			break;
		case 1:
			*ButtonVal = BUTTON_DOWN;
			break;
		case 2:
			*ButtonVal = BUTTON_LEFT;
			break;
		case 3:
			*ButtonVal = BUTTON_SET;
			break;
		case 4:
			*ButtonVal = NO_PRESS;
			break;
		default:
			break;	
	}
}

String ReadMeasure(short WichInfo)
{
	String InfoStr;
	char ReadInfo[15];
	short TotChar = 0;
	Wire.beginTransmission(ARDUINO_ADDR);
	Wire.write(WichInfo);
	Wire.endTransmission();
	delay(1);
	Wire.requestFrom(ARDUINO_ADDR, CHAR_FROM_NANO);
	while(Wire.available())
	{
   		ReadInfo[TotChar] = Wire.read();
		InfoStr += String(ReadInfo[TotChar]);
 		TotChar++;
	}	
	return InfoStr;
}

bool SendCommand(uint8_t Command)
{
	String InfoStr;
	bool Result = false;
	uint8_t ReadInfo;
	short TotChar = 0;
	Wire.beginTransmission(ARDUINO_ADDR);
	Wire.write(Command);
	Wire.endTransmission();
	delay(1);
	Wire.requestFrom(ARDUINO_ADDR, 1);
	while(Wire.available())
	{
   		ReadInfo = Wire.read();
	}	
	if(ReadInfo = DONE)
		Result = true;
	return Result;
}

void ReleOn(short ReleIdx)
{
	short Rele2Send = ReleIdx + RELE_1_ON;
	Wire.beginTransmission(ARDUINO_ADDR);
	Wire.write(Rele2Send);
	Wire.endTransmission();
	delay(1);
}

void ReleOff(short ReleIdx)
{
	short Rele2Send = ReleIdx + RELE_1_OFF;
	Wire.beginTransmission(ARDUINO_ADDR);
	Wire.write(Rele2Send);
	Wire.endTransmission();
	delay(1);
}
