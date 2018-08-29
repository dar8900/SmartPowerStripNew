#include "EEPROM_Node.h"
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

void EepromInit()
{
	EEPROM.begin(MAX_EEPROM_DIM_NODEMCU);
}

void WriteMemory(short address, short value)
{
	if(address < MAX_EEPROM_DIM_NODEMCU)
	{
		if(value < MAX_CELL_EEPROM)
			EEPROM.write(address, value);
		else
			EEPROM.write(address, MAX_CELL_EEPROM);
		EEPROM.commit();
	}
	return;
}

bool ReadMemory(short address, short *value)
{
	short ValueRead = 0;
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
		return;
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
			WriteMemory(InitAddress + RegIndx, (short)CompoundNumber[RegIndx]);
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
	for(RegIndx = 0; RegIndx < Range; RegIndx++)
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
		ReadMemory(InitAddress, (short *)&Value);
	else
	{
		for(RegIndx = 0; RegIndx < EepromRangeTab[RealRange].NumReg; RegIndx++)
		{
			ReadMemory(InitAddress + RegIndx, (short)CompoundNumber[RegIndx]);
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


// int WriteMemory(short address, short value)
// {
  // short FlagValueBig = 0;
  // short numReg;
  // short resto = value % MAX_CELL_EEPROM;

  // if(address < MAX_EEPROM_DIM_NODEMCU)
  // {
    // if((value / MAX_CELL_EEPROM) == 0)
    // {
      // numReg = 1;
      // FlagValueBig = 0;
      // EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
    // }
    // else if((value / MAX_CELL_EEPROM) == 1)
    // {
      // if(value > MAX_CELL_EEPROM)
      // {
        // numReg = (value / MAX_CELL_EEPROM) + 1;
        // FlagValueBig = 1;
        // EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
      // }
      // else
      // {
        // numReg = 1;
        // FlagValueBig = 0;
        // EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
      // }
    // }
    // else
    // {
      // numReg = (value / MAX_CELL_EEPROM) + 1;
      // FlagValueBig = 1;
      // EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
      // if(address + (numReg - 1) > MAX_EEPROM_DIM_NODEMCU)
      // {
        // numReg = 0;
      // }
    // }

    // for(short idx = 0; idx < numReg; idx++)
    // {
      // if(numReg == 0)
      // {
        // break;
      // }
      // else
      // {
        // if(numReg > 1)
        // {
          // if(idx < numReg - 1)
          // {
            // EEPROM.write(address + idx, MAX_CELL_EEPROM);
          // }
          // else
          // {
            // EEPROM.write(address + idx, resto);
          // }
        // }
        // else
        // {
          // EEPROM.write(address, value);
        // }
      // }
    // }
  // }
  // else
  // {
    // numReg = 0;
  // }

  // if(numReg > 1)
  // {
	  // EepromUpdate(NUMBER_REG_BACKLIGHT, numReg);
  // }

  // EEPROM.commit();
  // return  numReg;
// }

// bool ReadMemory(short address, short numReg, short *value)
// {
  // short ValueRead = 0;
  // short FlagValueBig = 0;
  // bool ReadOk = false;
  // short RealNumReg = numReg;
  // FlagValueBig = EEPROM.read(FLAG_VALUE_ADDR);

  // if(!FlagValueBig)
  // {
	 // ValueRead = EEPROM.read(address);
	 // ReadOk = true;
  // }
  // else
  // {
	// RealNumReg = EEPROM.read(NUMBER_REG_BACKLIGHT);
	// for(short idx = 0; idx < RealNumReg; idx++)
	// {
	  // ValueRead += EEPROM.read(address + idx);
	// }
	// ReadOk = true;
  // }


  // if(ReadOk)
  // {
    // *value = ValueRead;
  // }
  // return ReadOk;
// }

bool ClearMemory()
{
	// Tempo di cancellazione massimo 3.3 s
	for(short i = 0; i < MAX_EEPROM_DIM_NODEMCU; i++)
	{
		EepromUpdate(i, EMPTY_MEMORY_VALUE);
	}
	return true;
}

bool IsMemoryEmpty()
{
	bool Empty = true;
	short Value;
	for(short i = 0; i < MAX_EEPROM_DIM_NODEMCU; i++)
	{
		Value = EEPROM.read(i);
		if(Value != EMPTY_MEMORY_VALUE)
			Empty = false;

	}
	return Empty;
}

void EepromUpdate(short address, short value)
{
	short ValueRead = EEPROM.read(address);
	if(ValueRead != value)
	{
		EEPROM.write(address, value);
	}
}
