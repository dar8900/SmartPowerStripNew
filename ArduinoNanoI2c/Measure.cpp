#include "ArduinoNanoI2c.h"
#include "Measure.h"

const float TensioneLinea  = 230.0;
const float mVoltPerAmpere = 100.0;

static float 			  CurrentOffset;

float 			  CurrentCalculated;
float 		      EnergyMeasured;
static float      EnergyAcc;
static uint16_t   EnergyAccCnt = 1;
float   	      PowerMeasure;

String		      EnergyStr;
String		      CurrentStr;
String 			  PowerStr;

void CurrentCalibration()
{
	float mVolt = 0.0, Current = 0.0;
	uint32_t CurrentAcc = 0;
	uint16_t ReadedValue = 0, AdcOffset = ZERO_CURRENT_ANALOG_VALUE, NumSampleCal = (N_CAMPIONI_CORRENTE / 4);
	
	for(int cnt = 0; cnt < NumSampleCal; cnt++) // legge per 50 ms
	{	
		ReadedValue = (analogRead(A0) - AdcOffset);
		CurrentAcc += (uint32_t)(ReadedValue * ReadedValue);		
	}
	mVolt = (sqrt((float)(CurrentAcc / NumSampleCal))) * 4.9;
	CurrentOffset = (mVolt / mVoltPerAmpere); // Valore RMS in Ampere
	return;
}


float CalcCurrent()
{
	float mVolt = 0.0, Current = 0.0;
	uint32_t CurrentAcc = 0;
	uint16_t ReadedValue = 0, AdcOffset = ZERO_CURRENT_ANALOG_VALUE;
	
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
	if(CurrentOffset < 0)
		CurrentOffset = -CurrentOffset;
	CurrentCalculated = CalcCurrent() - CurrentOffset;
	Serial.println(CurrentCalculated);
	if(CurrentCalculated > 0.0)
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
	EnergyMeasured += (EnergyAcc / (float)EnergyAccCnt);	
	EnergyAccCnt = 1;
	EnergyAcc = 0.0;
	EnergyStr = String(EnergyMeasured); // Invio la stringa formattata in W/s
	CurrentStr = String(CurrentCalculated);
	PowerStr = String(PowerMeasure);
}
