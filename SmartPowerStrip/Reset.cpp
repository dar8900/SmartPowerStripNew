#include "LCDLib.h"
#include "TimeLib.h"
#include "SmartPowerStrip.h"
#include "EEPROM_Node.h"
#include "I2CNano.h"
#include "Menu.h"
#include "Web.h"
#include "Band.h"
#include "Rele.h"
#include "Measure.h"
#include "Reset.h"


bool FirstResetEnergy()
{
	bool Confirm = false;
	Confirm = SendCommand(RESET_ENERGY);
	return Confirm;
}

void RestartESP()
{
	ClearLCD();
	LCDPrintString(TWO, CENTER_ALIGN, "Restart in corso");
	LCDPrintString(THREE, CENTER_ALIGN, "attendere...");
	delay(DELAY_MENU_MSG);
	ClearLCD();
	EepromUpdate(FACTORY_RESET_ADDR, NOT_DEFAULT_RESET);
	ESP.restart();
}


void Reset2Factory()
{
	ClearLCD();
	LCDPrintString(ONE, CENTER_ALIGN, "Reset a default");
	LCDPrintString(TWO, CENTER_ALIGN, "in corso");
	LCDPrintString(TWO, CENTER_ALIGN, "attendere...");
	delay(DELAY_MENU_MSG);
	ClearLCD();
	WriteMemory(FACTORY_RESET_ADDR, DEFAULT_RESET);
	ESP.restart();
}


bool RestartEsp()
{
	bool Reset = false;
	ClearLCD();
	LCDPrintString(TWO, CENTER_ALIGN, "Eseguire il restart");
	LCDPrintString(THREE, CENTER_ALIGN, "della ciabatta?");
	Reset = CheckYesNo();
	if(Reset)
		RestartESP();
	return true;
}

bool ResetEnergy()
{
	bool Reset = false, ConfirmReset = false;
	ClearLCD();
	LCDPrintString(TWO, CENTER_ALIGN, "Resettare il valore");
	LCDPrintString(THREE, CENTER_ALIGN, "dell'energia?");
	Reset = CheckYesNo();
	if(Reset)
		ConfirmReset = SendCommand(RESET_ENERGY);
	if(ConfirmReset)
	{
		ClearLCD();
		LCDPrintString(TWO, CENTER_ALIGN, "Resettata!");
		delay(DELAY_INFO_MSG);
		ClearLCD();
	}
	else
	{
		ClearLCD();
		LCDPrintString(TWO, CENTER_ALIGN, "Non riuscito");
		delay(DELAY_INFO_MSG);
		ClearLCD();		
	}
	return true;
}

bool ResetDefault()
{
	bool Reset = false;
	ClearLCD();
	LCDPrintString(TWO, CENTER_ALIGN, "Impostare i valori");
	LCDPrintString(THREE, CENTER_ALIGN, "di default?");
	Reset = CheckYesNo();
	if(Reset)
		Reset2Factory();
	return true;
}