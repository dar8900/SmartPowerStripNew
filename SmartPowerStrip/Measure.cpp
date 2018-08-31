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

#define  TIMER_REFRESH_MEASURE	30

extern RELE Rele[];
extern FLAGS Flag;

const UDM_ENERGY_MENU UdmEnergyScale[MAX_UDM_ITEM] = 
{
	{3600.0,    "Wh" },
	{60.0  ,    "Wm" },
	{1.0   ,    "Ws" },	
};

const FORMAT_ENERGY TabFormat[] = 
{
	{0.001,             1000.0, "m"},
	{0.01,              1000.0, "m"},
	{0.1,               1000.0, "m"},	
	{1.0,                    1,  ""},
	{10.0,                   1,  ""},
	{100.0,                  1,  ""},
	{1000.0,             0.001, "k"},
	{10000.0,            0.001, "k"},
	{100000.0,           0.001, "k"},
	{1000000.0,       0.000001, "M"},
	{10000000.0,      0.000001, "M"},
	{100000000.0,     0.000001, "M"},
	{1000000000.0, 0.000000001, "G"},
};

uint16_t TariffaInt;
float TariffaFloat;

static uint8_t SearchFormatRange(float ValueToFormat)
{
	uint8_t TabLenght = MAX_FORMAT_RANGE;
	uint8_t Range = 0;
	for(Range = 0; Range < TabLenght; Range++)
	{
		if(Range == TabLenght - 1)
			break;
		if(ValueToFormat > TabFormat[Range].RangeValue && ValueToFormat < TabFormat[Range + 1].RangeValue)
		{
			break;
		}
		else if(ValueToFormat >= 0.0 && ValueToFormat < TabFormat[0].RangeValue)
		{
			Range = 3;
			break;
		}		
	}
	return Range;
}

bool ChangeUdmEnergy()
{
	bool ExitChangeUdm = false;
	uint8_t ButtonPress = NO_PRESS;
	short UdmEnergyItem, OldUdmEnergyItem;
	ReadMemory(UDM_ENERGY_ADDR, &UdmEnergyItem);
	OldUdmEnergyItem = UdmEnergyItem;
	ClearLCD();
	while(!ExitChangeUdm)
	{
		LCDPrintString(ONE, CENTER_ALIGN, "Visualizzazione");
		LCDPrintString(TWO, CENTER_ALIGN, "misura energia in:");
		LCDPrintString(THREE, CENTER_ALIGN, UdmEnergyScale[UdmEnergyItem].UdmStr);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case BUTTON_UP:
				BlinkLed(BUTTON_LED);
				if(UdmEnergyItem > 0)
					UdmEnergyItem--;
				else
					UdmEnergyItem = MAX_UDM_ITEM - 1;
				ClearLCDLine(THREE);
				break;
			case BUTTON_DOWN:
				BlinkLed(BUTTON_LED);
				if(UdmEnergyItem < MAX_UDM_ITEM - 1)
					UdmEnergyItem++;
				else
					UdmEnergyItem = WATT_ORA;
				ClearLCDLine(THREE);
				break;
			case BUTTON_LEFT:
				BlinkLed(BUTTON_LED);
				ExitChangeUdm = true;
				break;
			case BUTTON_SET:
				BlinkLed(BUTTON_LED);
				if(UdmEnergyItem != OldUdmEnergyItem)
				{
					ClearLCD();
					LCDPrintString(TWO, CENTER_ALIGN, "Settato!");
					WriteMemory(UDM_ENERGY_ADDR, UdmEnergyItem);
					delay(DELAY_INFO_MSG);
				}
				else
				{
					ClearLCD();
					LCDPrintString(TWO, CENTER_ALIGN, "Uguale a prima");
					EepromUpdate(UDM_ENERGY_ADDR, UdmEnergyItem);
					delay(DELAY_INFO_MSG);					
				}
				ExitChangeUdm = true;
				break;
			default:
				break;
		}
		ButtonPress = NO_PRESS;
		delay(WHILE_LOOP_DELAY);
	}
	ClearLCD();
}

bool ShowMeasures()
{
	uint16_t TimerDisplay = 3000; // 60s con delay 10ms
	uint16_t TimerSwitchMeasure = 0, TimeExecShowMeasure = 0; // Cambio ogni 10s
	uint8_t FormatRange = 0;
	uint8_t TimerRefreshMeasure = TIMER_REFRESH_MEASURE; 
	uint8_t ButtonPress = NO_PRESS;
	short UdmEnergy = 0;
	String EnergyStr;
	String CurrentStr;
	String PowerStr;
	float EnergyScaled = 0.0;
	float EuroValue = 0.0;
	float CurrentScaled = 0.0;
	float PowerScaled = 0.0;
	bool ExitShowEnergy = false, FormatEnergy = false, CurrentOrPower = false, EnergyOrEuro = true;
	ReadMemory(UDM_ENERGY_ADDR, &UdmEnergy);
	ClearLCD();
	if(!Flag.IsDisplayOn)
	{
		LCDDisplayOn();
		Flag.IsDisplayOn = true;
	}
	while(!ExitShowEnergy)
	{
		TimeExecShowMeasure = millis();
		if(!CurrentOrPower)
			LCDPrintString(ONE, CENTER_ALIGN, "Corrente Misurata:");
		else
			LCDPrintString(ONE, CENTER_ALIGN, "Potenza Misurata:");
		if(!EnergyOrEuro)
			LCDPrintString(THREE, CENTER_ALIGN, "Euro spesi:");
		else
			LCDPrintString(THREE, CENTER_ALIGN, "Energia Misurata:");
		if(TimerRefreshMeasure == TIMER_REFRESH_MEASURE)
		{
			ClearLCDLine(TWO);
			ClearLCDLine(FOUR);
			CurrentStr = CurrentValueStr();
			EnergyStr = EnergyValueStr();
			if(!CurrentOrPower)
			{	
				CurrentScaled = CurrentStr.toFloat();
				FormatRange = SearchFormatRange(CurrentScaled);
				CurrentScaled *= TabFormat[FormatRange].FormatFactor;
				CurrentStr = String(CurrentScaled);
				CurrentStr += " " + TabFormat[FormatRange].Prefix + "A";
			}
			else
			{
				PowerScaled = CurrentStr.toFloat() * TENSIONE_LINEA;						
				FormatRange = SearchFormatRange(PowerScaled);
				PowerScaled *= TabFormat[FormatRange].FormatFactor;
				PowerStr = String(PowerScaled);
				PowerStr += " " + TabFormat[FormatRange].Prefix + "W";		
			}
			if(!EnergyOrEuro)
			{
				EuroValue = ((EnergyStr.toFloat()) / 3600.0) * TariffaFloat;
				EnergyStr = String(EuroValue) + " E/kWh";
			}
			else
			{
				switch(UdmEnergy)
				{
					case WATT_MINUTO:
						EnergyScaled = ((EnergyStr.toFloat()) / UdmEnergyScale[UdmEnergy].UdmValue);
						FormatRange = SearchFormatRange(EnergyScaled);
						EnergyScaled *= TabFormat[FormatRange].FormatFactor;
						EnergyStr = String(EnergyScaled);
						break;
					case WATT_SECONDO:
						EnergyScaled = ((EnergyStr.toFloat()) / UdmEnergyScale[UdmEnergy].UdmValue);
						FormatRange = SearchFormatRange(EnergyScaled);
						EnergyScaled *= TabFormat[FormatRange].FormatFactor;
						EnergyStr = String(EnergyScaled);
						break;
					case WATT_ORA:
						EnergyScaled = ((EnergyStr.toFloat()) / UdmEnergyScale[UdmEnergy].UdmValue);
						FormatRange = SearchFormatRange(EnergyScaled);
						EnergyScaled *= TabFormat[FormatRange].FormatFactor;
						EnergyStr = String(EnergyScaled);
						break;
					default:
						break;
				}
				EnergyStr +=  " " + TabFormat[FormatRange].Prefix + UdmEnergyScale[UdmEnergy].UdmStr;
			}
			if(!CurrentOrPower)
				LCDPrintString(TWO, CENTER_ALIGN, CurrentStr);
			else
				LCDPrintString(TWO, CENTER_ALIGN, PowerStr);
			LCDPrintString(FOUR, CENTER_ALIGN, EnergyStr);
		}
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case BUTTON_UP:
			case BUTTON_DOWN:
			case BUTTON_SET:
				BlinkLed(BUTTON_LED);
				TimerDisplay = 3000;
				if(!Flag.IsDisplayOn)
				{
					LCDDisplayOn();
					Flag.IsDisplayOn = true;
				}
				break;
			case BUTTON_LEFT:
				BlinkLed(BUTTON_LED);
				ExitShowEnergy = true;
				if(!Flag.IsDisplayOn)
				{
					TimerDisplay = 3000;
					LCDDisplayOn();
					Flag.IsDisplayOn = true;
				}
				break;
		}
		if(TimerDisplay > 0)
			TimerDisplay--;
		if(TimerDisplay == 0 && Flag.IsDisplayOn)
		{
			LCDDisplayOff();
			Flag.IsDisplayOn = false;
		}
		TimerRefreshMeasure--;
		if(TimerRefreshMeasure == 0)
		{
			TimerRefreshMeasure = TIMER_REFRESH_MEASURE;
		}
		delay(10);
		TimerSwitchMeasure++;
		TimeExecShowMeasure = millis() - TimeExecShowMeasure;
		if(TimerSwitchMeasure == (10000 / TimeExecShowMeasure))
		{
			ClearLCDLine(ONE);
			ClearLCDLine(THREE);
			TimerSwitchMeasure = 0;
			CurrentOrPower = !CurrentOrPower;
			EnergyOrEuro = !EnergyOrEuro;
		}
	}
	return true;
}


void LoadTariffValue()
{
	uint8_t TariffMemoryAddr = 0;
	short Tariffa[4];
	for(TariffMemoryAddr = FIRST_TARIFF_NUMBER_ADDR; TariffMemoryAddr < (FOURTH_TARIFF_NUMBER_ADDR + 1); TariffMemoryAddr++)
	{
		ReadMemory(TariffMemoryAddr, &Tariffa[TariffMemoryAddr - FIRST_TARIFF_NUMBER_ADDR]);
	}	
	TariffaInt = (Tariffa[0] * 1000) + (Tariffa[1] * 100) + (Tariffa[2] * 10) + (Tariffa[3]);
	TariffaFloat = ((float)TariffaInt) / 100000.0;
}

bool ChangeTariff()
{
	short ButtonPress = NO_PRESS, TariffStatus = 0;
	uint8_t TariffMemoryAddr = FIRST_TARIFF_NUMBER_ADDR;
	uint16_t CycleTimer = 0, ExitCnt = 0, OldExitCnt = 0;
	int8_t Tariffa[4] = {0,0,0,0}, Cursor = 0;
	String TariffaStr = "0.0";
	bool ExitChangeTariff = false;
	ReadMemory(TARIFF_STATUS_ADDR, &TariffStatus);
	if(TariffStatus == SETTED)
	{
		Tariffa[0] = TariffaInt / 1000;
		Tariffa[1] = (TariffaInt / 100) - ((TariffaInt / 1000) * 10);
		Tariffa[2] = (TariffaInt / 10) - ((TariffaInt / 100) * 10);
		Tariffa[3] = (TariffaInt) - ((TariffaInt / 10) * 10);
	}
	ClearLCD();
	LCDPrintString(TWO, CENTER_ALIGN, "Cambia la tariffa");
	LCDPrintString(FOUR, CENTER_ALIGN, "E/kWh");
	LCDBlink();
	
	while(!ExitChangeTariff)
	{
		CheckEvents();
		TariffaInt = (Tariffa[0] * 1000) + (Tariffa[1] * 100) + (Tariffa[2] * 10) + (Tariffa[3]);
		TariffaStr = "0.0" + String(TariffaInt);
		LCDPrintString(THREE, 6, TariffaStr);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case BUTTON_UP:
				Tariffa[Cursor]--;
				if(Tariffa[Cursor] < 0)
					Tariffa[Cursor] = 9;
				break;
			case BUTTON_DOWN:
				Tariffa[Cursor]++;
				if(Tariffa[Cursor] > 9)
					Tariffa[Cursor] = 0;
				break;
			case BUTTON_LEFT:
				Cursor++;
				if(Cursor > 3)
					Cursor = 0;
				ExitCnt++;
				break;
			case BUTTON_SET:
				LCDNoBlink();
				ClearLCD();
				LCDPrintString(TWO, CENTER_ALIGN, "Tariffa settata");
				delay(DELAY_INFO_MSG);
				ClearLCD();
				for(TariffMemoryAddr = FIRST_TARIFF_NUMBER_ADDR; TariffMemoryAddr < (FOURTH_TARIFF_NUMBER_ADDR + 1); TariffMemoryAddr++)
				{
					EepromUpdate(TariffMemoryAddr, Tariffa[TariffMemoryAddr - FIRST_TARIFF_NUMBER_ADDR]);
				}
				TariffaInt = (Tariffa[0] * 1000) + (Tariffa[1] * 100) + (Tariffa[2] * 10) + (Tariffa[3]);
				TariffaFloat = ((float)TariffaInt) / 100000.0;
				EepromUpdate(TARIFF_STATUS_ADDR, SETTED);
				ExitChangeTariff = true;
				break;
			default:
				break;			
		}
		switch(Cursor)
		{
			case 0:
				LCDMoveCursor(THREE, 9);
				break;
			case 1: 
				LCDMoveCursor(THREE, 10);
				break;
			case 2:
				LCDMoveCursor(THREE, 11);
				break;
			case 3:
				LCDMoveCursor(THREE, 12);
				break;
			default:
				break;
		}
		CycleTimer++;
		if(CycleTimer == (WHILE_LOOP_DELAY * 134))
			CycleTimer = 0;
		if(CycleTimer == (WHILE_LOOP_DELAY * 64) && ExitCnt == OldExitCnt)
			ExitCnt = 0;
		if(ExitCnt == (WHILE_LOOP_DELAY * 134))
		{
			LCDNoBlink();
			ClearLCD();
			LCDPrintString(TWO, CENTER_ALIGN, "Tariffa non settata");
			delay(DELAY_INFO_MSG);
			ClearLCD();	
			TariffaInt = 0;
			TariffaFloat = 0.0;
			for(TariffMemoryAddr = FIRST_TARIFF_NUMBER_ADDR; TariffMemoryAddr < (FOURTH_TARIFF_NUMBER_ADDR + 1); TariffMemoryAddr++)
			{
				Tariffa[TariffMemoryAddr - FIRST_TARIFF_NUMBER_ADDR] = 0;
				EepromUpdate(TariffMemoryAddr, Tariffa[TariffMemoryAddr - FIRST_TARIFF_NUMBER_ADDR]);
			}
			EepromUpdate(TARIFF_STATUS_ADDR, NOT_SETTED);
			ExitChangeTariff = true;
		}			
		OldExitCnt = ExitCnt;
		delay(WHILE_LOOP_DELAY * 4); // 120ms per il blink
	}
	
}
