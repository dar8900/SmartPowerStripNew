#include "ArduinoNanoI2c.h"
#include "ReleI2C.h"

extern short WichData;

static short ReleIndx2Pin(short ReleIdx)
{
	short Pin = RELE_1;
	switch(ReleIdx)
	{
		case RELE_1_OFF:
		case RELE_1_ON:
			Pin = RELE_1;
			break;
		case RELE_2_OFF:
		case RELE_2_ON:
			Pin = RELE_2;
			break;
		case RELE_3_OFF:
		case RELE_3_ON:
			Pin = RELE_3;
			break;
		case RELE_4_OFF:
		case RELE_4_ON:
			Pin = RELE_4;
			break;
		case RELE_5_OFF:
		case RELE_5_ON:
			Pin = RELE_5;
			break;
		case RELE_6_OFF:
		case RELE_6_ON:
			Pin = RELE_6;
			break;
		case RELE_7_OFF:
		case RELE_7_ON:
			Pin = RELE_7;
			break;
		case RELE_8_OFF:		
		case RELE_8_ON:
			Pin = RELE_8;
			break;
		default:
			break;
	}
	return Pin;
}

void ReleAction(short WichRele)
{
	if(WichRele >= RELE_1_OFF && WichRele <= RELE_8_ON)
	{
		switch(WichRele)
		{
			case RELE_1_OFF:
			case RELE_2_OFF:
			case RELE_3_OFF:
			case RELE_4_OFF:
			case RELE_5_OFF:
			case RELE_6_OFF:
			case RELE_7_OFF:
			case RELE_8_OFF:
				ReleOff(WichRele);
				break;
			case RELE_1_ON:		
			case RELE_2_ON:		
			case RELE_3_ON:		
			case RELE_4_ON:		
			case RELE_5_ON:		
			case RELE_6_ON:		
			case RELE_7_ON:		
			case RELE_8_ON:	
				ReleOn(WichRele);
				break;		
			default:
				break;
		}
	}
	return;
}

void ReleOn(short ReleIdx)
{
	short Pin = ReleIndx2Pin(ReleIdx);
	digitalWrite(Pin, HIGH);
	return;
}

void ReleOff(short ReleIdx)
{
	short Pin = ReleIndx2Pin(ReleIdx);
	digitalWrite(Pin, LOW);
	return;
}

