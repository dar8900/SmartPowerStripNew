#include <Wire.h>
#include "ArduinoNanoI2c.h"
#include "Measure.h"
#include "ReleI2C.h"


short ButtonPress = NO_PRESS;
short TickSecond = 0, Tick5Second = 0;
short WichData = NO_DATA;
uint16_t TimeExecEnergy, TimeExec4Calib;

extern String		EnergyStr;
extern String		CurrentStr;
extern String       PowerStr;

static bool ChekButtons()
{
	short ButtonUp = LOW, ButtonDown = LOW, ButtonLeft = LOW, Buttonset = LOW, Press = false;
	ButtonUp = digitalRead(UP);
	ButtonDown = digitalRead(DOWN);
	ButtonLeft = digitalRead(LEFT);
	Buttonset = digitalRead(SET);
	if(ButtonUp == HIGH)
	{
		ButtonPress = BUTTON_UP;	
		Press = true;
	}
	else if(ButtonDown == HIGH)
	{
		ButtonPress = BUTTON_DOWN;
		Press = true;
	}
	else if(ButtonLeft == HIGH)
	{
		ButtonPress = BUTTON_LEFT;
		Press = true;
	}
	else if(Buttonset == HIGH)
	{
		ButtonPress = BUTTON_SET;
		Press = true;
	}
	return Press;
}

static void WichInfo()
{
	while(Wire.available())
	{
   		WichData = Wire.read();
	}
	ReleAction(WichData);
}

static void SendInfo()
{
	switch(WichData)
	{
		case BUTTON:
			Wire.write(ButtonPress);
			ButtonPress = NO_PRESS;
			break;
		case ENERGY:
			Wire.write(EnergyStr.c_str());
			break;
		case CURRENT:
			Wire.write(CurrentStr.c_str());
			break;
		case POWER:
			Wire.write(PowerStr.c_str());
			break;
		default:
			break;
	}
	WichData = NO_DATA;
}

static void BlinkLed(short pin)
{
	ON(pin);
	delay(5);
	OFF(pin);	
}

void setup() 
{
	Serial.begin(9600);
	Wire.begin(ARDUINO_ADDR); 
	pinMode(UP, INPUT);
	pinMode(DOWN, INPUT);
	pinMode(LEFT, INPUT);
	pinMode(SET, INPUT);
	pinMode(RELE_1, OUTPUT);
	pinMode(RELE_2, OUTPUT);
	pinMode(RELE_3, OUTPUT);
	pinMode(RELE_4, OUTPUT);
	pinMode(RELE_5, OUTPUT);
	pinMode(RELE_6, OUTPUT);
	pinMode(RELE_7, OUTPUT);
	pinMode(RELE_8, OUTPUT);
	Wire.onReceive(WichInfo);
	Wire.onRequest(SendInfo);
	CurrentCalibration();
}

void loop() 
{
	TimeExec4Calib = millis();
	TimeExecEnergy = millis();
	ChekButtons();
	CalcEnergy();
	TickSecond++;
	TimeExecEnergy = millis() - TimeExecEnergy;
	
	// Viene misurata l'energia in Ws ogni sec e scritta nella stringa
	if(TickSecond == (SECOND(1) / TimeExecEnergy))
	{
		TickSecond = 0;
		MeasureValueSec();	
	}	
	Tick5Second++;
	TimeExec4Calib = millis() - TimeExec4Calib;
	
	// Viene eseguita una calibrazione della corrente ogni 5s
	if(Tick5Second == (SECOND(5) / TimeExec4Calib))
	{
		CurrentCalibration();
	}
}
