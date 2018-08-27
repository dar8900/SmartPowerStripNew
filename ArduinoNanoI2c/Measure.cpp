#include "ArduinoNanoI2c.h"
#include "Measure.h"

const float TensioneLinea  = 230.0;
const float mVoltPerAmpere = 100.0;

static float      CurrentOffset;
uint16_t	      AdcOffset;

float 			  CurrentCalculated;
float 		      EnergyMeasured;
static float      EnergyAcc;
static uint16_t   EnergyAccCnt;
float   	      PowerMeasure;

String		      EnergyStr;
String		      CurrentStr;
String 			  PowerStr;


void CurrentCalibration()
{
	float mVolt = 0.0, Current = 0.0;
	uint32_t CurrentAcc = 0, AdcOffsetAcc = 0;
	uint16_t ReadedValue = 0, NumSampleCal = (N_CAMPIONI_CORRENTE / 2);
	for(int cnt = 0; cnt < NumSampleCal; cnt++) // legge per 100 ms
	{	
		AdcOffsetAcc += analogRead(A0);	
	}	
	AdcOffset = AdcOffsetAcc / NumSampleCal;
	for(int cnt = 0; cnt < NumSampleCal; cnt++) // legge per 100 ms
	{	
		ReadedValue = (analogRead(A0) - AdcOffset);
		CurrentAcc += (uint32_t)(ReadedValue * ReadedValue);		
	}
	mVolt = (sqrt((float)(CurrentAcc / NumSampleCal))) * 4.9;
	CurrentOffset = (mVolt / mVoltPerAmpere); // Valore RMS in Ampere
	if(CurrentOffset < 0)
		CurrentOffset = -CurrentOffset;
	return;
}


float CalcCurrent()
{
	float mVolt = 0.0, Current = 0.0;
	uint32_t CurrentAcc = 0;
	uint16_t ReadedValue = 0;
	
	for(int cnt = 0; cnt < N_CAMPIONI_CORRENTE; cnt++) // legge per 200 ms (10 periodi di rete a 50 Hz)
	{	
		ReadedValue = (analogRead(A0) - AdcOffset);
		CurrentAcc += (uint32_t)(ReadedValue * ReadedValue);		
	}
	mVolt = (sqrt((float)(CurrentAcc / N_CAMPIONI_CORRENTE))) * 4.9;
	Current = (mVolt / mVoltPerAmpere); // Valore RMS in Ampere
	return Current;
}

void CalcEnergy() // 200ms ca
{
	CurrentCalculated = CalcCurrent() - CurrentOffset;
	if(CurrentCalculated >= -MIN_CURRENT && CurrentCalculated <= MIN_CURRENT)
	{
		PowerMeasure = CurrentCalculated * (float)TENSIONE_LINEA;
		EnergyAcc += PowerMeasure;
		EnergyAccCnt++;
	}
	else
	{
		PowerMeasure = 0.0;		
	}
}

void MeasureValueSec()
{
	if(EnergyAccCnt > 0)
		EnergyMeasured += (EnergyAcc / (float)EnergyAccCnt);	
	EnergyAccCnt = 0;
	EnergyAcc = 0.0;
	EnergyStr = String(EnergyMeasured); // Invio la stringa formattata in W/s
	CurrentStr = String(CurrentCalculated);
	PowerStr = String(PowerMeasure);
	
#ifdef DEBUG_SERIAL
	Serial.print("AdcOffset = ");
	Serial.print(AdcOffset);
	Serial.println();
	Serial.print(CurrentCalculated, 3);
	Serial.print("A");
	Serial.println();
	Serial.print(CurrentOffset, 3);
	Serial.print("A Offset");
	Serial.println();
	Serial.print(PowerMeasure, 3);
	Serial.print("W");
	Serial.println();
	Serial.print(EnergyMeasured, 3);
	Serial.print("Ws");
	Serial.println();
#endif
}


bool ResetEnergies()
{
	EnergyMeasured = 0.0;	
	EnergyAccCnt = 0;
	EnergyAcc = 0.0;
	return true;
}
