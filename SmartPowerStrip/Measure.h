#ifndef MEASURE_H
#define MEASURE_H
#include <Arduino.h>

typedef struct
{
	float UdmValue;
	String UdmStr;
}UDM_ENERGY_MENU;

typedef struct
{
	float    RangeValue;
	float 	 FormatFactor;
	String   Prefix;
}FORMAT_ENERGY;

typedef struct
{
	int8_t FirstNumer;
	int8_t SecondNumer;
	int8_t ThirdNumer;
	int8_t FourthNumer;
}TARIFF_ITEM;

typedef enum
{
	WATT_ORA = 0,
	WATT_MINUTO,
	WATT_SECONDO,
	MAX_UDM_ITEM
}UDM_ENERGY_ITEM;

typedef enum
{
	NOT_SETTED = 0,
	SETTED
} TARIFF_STATUS;

typedef enum
{
	m_1 = 0,
	m_2,
	m_3,
	VOID_1,
	VOID_2,
	VOID_3,
	k_1,
	k_2,
	k_3,
	M_1,
	M_2,
	M_3,
	G_1,
	MAX_FORMAT_RANGE
	
}FORMAT_RANGES;


bool ChangeUdmEnergy(void);
bool ShowMeasures(void);
bool ChangeTariff(void);
void SaveTariffValue(int8_t Tariffa[]);
void LoadTariffValue(void);

#endif