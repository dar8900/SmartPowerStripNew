#include "EEPROM_Ard_Nano.h"
#include <Arduino.h>

#ifndef NEW_EEPROM
#include <EEPROM.h>
#endif



const EEPROM_RANGE EepromRangeTab[] = 
{
	{254,          1},
	{1000,         3},
	{10000,        4},
	{100000,       5},
	{1000000,      6},
	{10000000, 	   7},
	{100000000,    8},
	{1000000000,   9},
};


void WriteMemory(short address, uint8_t value)
{
	if(address < MAX_EEPROM_DIM)
	{
		if(value < MAX_CELL_EEPROM)
			EEPROM.update(address, value);
		else
			EEPROM.update(address, MAX_CELL_EEPROM);
	}
	return;
}

bool ReadMemory(short address, uint8_t *value)
{
	uint8_t ValueRead = 0;
	bool ReadOk = false;
	ValueRead = EEPROM.read(address);
	*value = ValueRead;
	ReadOk = true;
	return ReadOk;
}

void WriteBigData(short InitAddress, uint32_t Value)
{
	uint8_t RangeIndx = 0, RealNumReg, RegIndx = 0;
	uint8_t *CompoundNumber;
	if(Value <= EepromRangeTab[SINGLE_REGISTER].MaxValue)
	{
		WriteMemory(InitAddress, Value);
	}
	else
	{
		for(RangeIndx = THREE_REG; RangeIndx < MAX_RANGE_ITEM; RangeIndx++)
		{
			if(Value < EepromRangeTab[RangeIndx].MaxValue)
			{
				if(RangeIndx == THREE_REG)
				{
					RealNumReg = EepromRangeTab[RangeIndx].NumReg;
				}
				else
				{
					RealNumReg = EepromRangeTab[RangeIndx].NumReg;
					RangeIndx--;
				}
				break;
			}
		}
		CompoundNumber[0] = Value / EepromRangeTab[RangeIndx].MaxValue;
		for(RegIndx = 1; RegIndx < RealNumReg; RegIndx++)
		{
			if(RegIndx < RealNumReg - 1)
			{
				if(RegIndx == 1)
					CompoundNumber[RegIndx] = (Value / (EepromRangeTab[RangeIndx].MaxValue / (10 * RegIndx))) - ((Value / EepromRangeTab[RangeIndx].MaxValue ) * 10);			
				else
				    CompoundNumber[RegIndx] = (Value / (EepromRangeTab[RangeIndx].MaxValue / (10 * RegIndx))) - ((Value / (EepromRangeTab[RangeIndx].MaxValue / (10 * (RegIndx - 1))) ) * 10);			
			}
			else
				CompoundNumber[RegIndx] = Value - ((Value / 10) * 10);
		}
		for(RegIndx = 0; RegIndx < RealNumReg; RegIndx++)
		{
			WriteMemory(InitAddress + RegIndx, CompoundNumber[RegIndx]);
		}
	}
	return;
}

int32_t ReadBigData(short InitAddress, EEPROM_RANGE_ITEM Range)
{
	uint32_t Value = 0;
	uint8_t *CompoundNumber;
	uint8_t RegIndx = 0;
	int8_t RealRange = Range;
	for(RegIndx = 0; RegIndx < EepromRangeTab[Range].NumReg; RegIndx++)
	{
		Value = EEPROM.read(InitAddress + RegIndx);
		if(Value == EMPTY_MEMORY_VALUE)
		{
			RealRange = RegIndx - 1;
			Value = 0;
			break;
		}
	}
	if(RealRange < 0)
		return -1;
	if(RealRange == SINGLE_REGISTER)
		ReadMemory(InitAddress, (uint8_t *)&Value);
	else
	{
		for(RegIndx = 0; RegIndx < EepromRangeTab[RealRange].NumReg; RegIndx++)
		{
			ReadMemory(InitAddress + RegIndx, &CompoundNumber[RegIndx]);
			if(RegIndx == 0)
				Value = CompoundNumber[RegIndx] * (EepromRangeTab[RealRange].MaxValue);
			else if(RegIndx < (EepromRangeTab[RealRange].NumReg - 1))
				Value += CompoundNumber[RegIndx] * (EepromRangeTab[RealRange].MaxValue / (10 * RegIndx));
			else
				Value += CompoundNumber[RegIndx];
		}
	}
	return Value;
}


bool ClearMemory()
{
	// Tempo di cancellazione massimo 3.3 s
	for(short i = 0; i < MAX_EEPROM_DIM; i++)
	{
		EEPROM.update(i, EMPTY_MEMORY_VALUE);
	}
	return true;
}

bool IsMemoryEmpty()
{
	bool Empty = true;
	uint8_t Value;
	for(uint16_t i = 0; i < MAX_EEPROM_DIM; i++)
	{
		Value = EEPROM.read(i);
		if(Value != EMPTY_MEMORY_VALUE)
			Empty = false;

	}
	return Empty;
}

