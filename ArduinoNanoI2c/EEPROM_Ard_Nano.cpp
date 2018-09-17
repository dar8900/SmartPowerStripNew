#include "EEPROM_Ard_Nano.h"
#include <Arduino.h>
#include <math.h>
#ifndef NEW_EEPROM
#include <EEPROM.h>
#endif


EEPROM_DATA EepromTab[MAX_EEPROM_ITEM] = 
{
	{ENERGY_MEMORY_INIT_ADDR, ENERGY_MEMORY_SIZE, 0, 0, 0, 0, 0},	
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

void WriteBigData(EEPROM_DATA EepromItem[], uint8_t EepromTabIndex)
{
	uint8_t AddressIndx = 0;
	NumberDecomposition(EepromItem, EepromTabIndex);
	for(AddressIndx = EepromItem[EepromTabIndex].ItemInitAddress; AddressIndx < EepromItem[EepromTabIndex].ItemBlockSize; AddressIndx++)
	{
		EEPROM.update(AddressIndx, EepromItem[EepromTabIndex].NumberElements[AddressIndx]);
	}
	return;
}

void ReadBigData(EEPROM_DATA EepromItem[], uint8_t EepromTabIndex)
{
	uint8_t AddressIndx = 0;
	for(AddressIndx = EepromItem[EepromTabIndex].ItemInitAddress; AddressIndx < EepromItem[EepromTabIndex].ItemBlockSize; AddressIndx++)
	{
		EepromItem[EepromTabIndex].NumberElements[AddressIndx] = EEPROM.read(AddressIndx);
	}
	NumberComposition(EepromItem, EepromTabIndex);
	return;
}

void NumberComposition(EEPROM_DATA EepromItem[], uint8_t EepromTabIndex)
{
        uint8_t Resto[4][8] = {0}, RestoCnt = 0, powexp = 31;
        uint8_t Bin[4][8] = {0};
        uint16_t Dividendo = 0;
        for(uint8_t i = 0; i < 4; i++)
        {
                Dividendo = EepromItem[EepromTabIndex].NumberElements[i];
                RestoCnt = 0;
                while(Dividendo > 0)
                {
                        Resto[i][RestoCnt] = (Dividendo % 2);
                        RestoCnt++;
                        Dividendo /= 2;
                }
                RestoCnt--;
                for(uint8_t j = 0; j < ( 7 - RestoCnt); j++)
                        Resto[i][RestoCnt + j + 1] = 0;
        }
        for(uint8_t i = 0; i < 4; i++)
        {
                for(uint8_t j = 0; j < 8; j++)
                {
                        Bin[i][j] = Resto[i][7-j];
                }
        }
        for(uint8_t i = 0; i < 4; i++)
        {
                for(uint8_t j = 0; j < 8; j++)
                {
                        EepromItem[EepromTabIndex].SavedValue += ((Bin[i][j]) * (uint32_t)pow(2, powexp));
                        powexp--;
                }
        }

}

void NumberDecomposition(EEPROM_DATA EepromItem[], uint8_t EepromTabIndex)
{
        uint32_t Dividendo = 0;
        uint8_t RestoCnt = 0, cnt = 0;
        uint16_t NumDecomposto[8][4];
        uint16_t Resto[31], trueResto[31];
        Dividendo = EepromItem[EepromTabIndex].SavedValue;
        while(Dividendo > 0)
        {
                Resto[RestoCnt] = (Dividendo % 2);
                RestoCnt++;
                Dividendo /= 2;
        }
        RestoCnt--;
        for(int8_t i = 0; i < ( 31 - RestoCnt); i++)
                Resto[RestoCnt + i + 1] = 0;
        for(int8_t i = 31; i >= 0; i--)
        {
                trueResto[31 - i] = Resto[i];
        }
        for(uint8_t i = 0; i < 4; i++)
        {
                for(uint8_t j = 0; j < 8; j++)
                {
                        NumDecomposto[j][i] = trueResto[cnt];
                        cnt++;

                }
        }
        for(uint8_t i = 0; i < 4; i++)
        {
                for(uint8_t j = 0; j < 8; j++)
                {
                        EepromItem[EepromTabIndex].NumberElements[i] += (NumDecomposto[j][i] * (uint8_t)pow(2,(7-j)));

                }
        }
}


// void WriteBigData(short InitAddress, uint32_t Value)
// {
	// uint8_t RangeIndx = 0, RealNumReg, RegIndx = 0;
	// uint8_t *CompoundNumber;
	// if(Value <= EepromRangeTab[SINGLE_REGISTER].MaxValue)
	// {
		// WriteMemory(InitAddress, Value);
	// }
	// else
	// {
		// for(RangeIndx = THREE_REG; RangeIndx < MAX_RANGE_ITEM; RangeIndx++)
		// {
			// if(Value < EepromRangeTab[RangeIndx].MaxValue)
			// {
				// if(RangeIndx == THREE_REG)
				// {
					// RealNumReg = EepromRangeTab[RangeIndx].NumReg;
				// }
				// else
				// {
					// RealNumReg = EepromRangeTab[RangeIndx].NumReg;
					// RangeIndx--;
				// }
				// break;
			// }
		// }
		// CompoundNumber[0] = Value / EepromRangeTab[RangeIndx].MaxValue;
		// for(RegIndx = 1; RegIndx < RealNumReg; RegIndx++)
		// {
			// if(RegIndx < RealNumReg - 1)
			// {
				// if(RegIndx == 1)
					// CompoundNumber[RegIndx] = (Value / (EepromRangeTab[RangeIndx].MaxValue / (10 * RegIndx))) - ((Value / EepromRangeTab[RangeIndx].MaxValue ) * 10);			
				// else
				    // CompoundNumber[RegIndx] = (Value / (EepromRangeTab[RangeIndx].MaxValue / (10 * RegIndx))) - ((Value / (EepromRangeTab[RangeIndx].MaxValue / (10 * (RegIndx - 1))) ) * 10);			
			// }
			// else
				// CompoundNumber[RegIndx] = Value - ((Value / 10) * 10);
		// }
		// for(RegIndx = 0; RegIndx < RealNumReg; RegIndx++)
		// {
			// WriteMemory(InitAddress + RegIndx, CompoundNumber[RegIndx]);
		// }
	// }
	// return;
// }

// int32_t ReadBigData(short InitAddress, EEPROM_RANGE_ITEM Range)
// {
	// uint32_t Value = 0;
	// uint8_t *CompoundNumber;
	// uint8_t RegIndx = 0;
	// int8_t RealRange = Range;
	// for(RegIndx = 0; RegIndx < EepromRangeTab[Range].NumReg; RegIndx++)
	// {
		// Value = EEPROM.read(InitAddress + RegIndx);
		// if(Value == EMPTY_MEMORY_VALUE)
		// {
			// RealRange = RegIndx - 1;
			// Value = 0;
			// break;
		// }
	// }
	// if(RealRange < 0)
		// return -1;
	// if(RealRange == SINGLE_REGISTER)
		// ReadMemory(InitAddress, (uint8_t *)&Value);
	// else
	// {
		// for(RegIndx = 0; RegIndx < EepromRangeTab[RealRange].NumReg; RegIndx++)
		// {
			// ReadMemory(InitAddress + RegIndx, &CompoundNumber[RegIndx]);
			// if(RegIndx == 0)
				// Value = CompoundNumber[RegIndx] * (EepromRangeTab[RealRange].MaxValue);
			// else if(RegIndx < (EepromRangeTab[RealRange].NumReg - 1))
				// Value += CompoundNumber[RegIndx] * (EepromRangeTab[RealRange].MaxValue / (10 * RegIndx));
			// else
				// Value += CompoundNumber[RegIndx];
		// }
	// }
	// return Value;
// }


bool ClearMemory()
{
	// Tempo di cancellazione massimo 3.3 s
	for(short i = 0; i < MAX_EEPROM_DIM; i++)
	{
		EEPROM.write(i, EMPTY_MEMORY_VALUE);
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

