#include "EEPROM_Ard.h"
#include <Arduino.h>

#ifndef NEW_EEPROM
#include <EEPROM.h>
#endif

/*
es. value = 6793
numReg = 27
resto = 163

*/

void EepromInit()
{
	EEPROM.begin(MAX_EEPROM_DIM_NODEMCU);
}


int WriteMemory(short address, short value)
{
  short FlagValueBig = 0;
  short numReg;
  short resto = value % MAX_CELL_EEPROM;

  if(address < MAX_EEPROM_DIM_NODEMCU)
  {
    if((value / MAX_CELL_EEPROM) == 0)
    {
      numReg = 1;
      FlagValueBig = 0;
      EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
    }
    else if((value / MAX_CELL_EEPROM) == 1)
    {
      if(value > MAX_CELL_EEPROM)
      {
        numReg = (value / MAX_CELL_EEPROM) + 1;
        FlagValueBig = 1;
        EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
      }
      else
      {
        numReg = 1;
        FlagValueBig = 0;
        EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
      }
    }
    else
    {
      numReg = (value / MAX_CELL_EEPROM) + 1;
      FlagValueBig = 1;
      EepromUpdate(FLAG_VALUE_ADDR, FlagValueBig);
      if(address + (numReg - 1) > MAX_EEPROM_DIM_NODEMCU)
      {
        numReg = 0;
      }
    }

    for(short idx = 0; idx < numReg; idx++)
    {
      if(numReg == 0)
      {
        break;
      }
      else
      {
        if(numReg > 1)
        {
          if(idx < numReg - 1)
          {
            EEPROM.write(address + idx, MAX_CELL_EEPROM);
          }
          else
          {
            EEPROM.write(address + idx, resto);
          }
        }
        else
        {
          EEPROM.write(address, value);
        }
      }
    }
  }
  else
  {
    numReg = 0;
  }

  if(numReg > 1)
  {
	  EepromUpdate(NUMBER_REG_BACKLIGHT, numReg);
  }

  EEPROM.commit();
  return  numReg;
}

bool ReadMemory(short address, short numReg, short *value)
{
  short ValueRead = 0;
  short FlagValueBig = 0;
  bool ReadOk = false;
  short RealNumReg = numReg;
  FlagValueBig = EEPROM.read(FLAG_VALUE_ADDR);

  if(!FlagValueBig)
  {
	 ValueRead = EEPROM.read(address);
	 ReadOk = true;
  }
  else
  {
	RealNumReg = EEPROM.read(NUMBER_REG_BACKLIGHT);
	for(short idx = 0; idx < RealNumReg; idx++)
	{
	  ValueRead += EEPROM.read(address + idx);
	}
	ReadOk = true;
  }


  if(ReadOk)
  {
    *value = ValueRead;
  }
  return ReadOk;
}

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
	if(EEPROM.read(address) != value)
	{
		EEPROM.write(address, value);
	}
}
