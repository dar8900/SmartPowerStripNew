#include <Wire.h>
#include "ArduinoNanoI2c.h"
#include "Measure.h"
#include "ReleI2C.h"


short ButtonPress = NO_PRESS;
short TickSecond = 0;
short WichData = NO_DATA;
// uint32_t TimeExec;

extern String		EnergyStr;
extern String		CurrentStr;

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
	switch(WichData)
	{
		case RELE_1_OFF:
		case RELE_2_OFF:
		case RELE_3_OFF:
		case RELE_4_OFF:
		case RELE_5_OFF:
		case RELE_6_OFF:
		case RELE_7_OFF:
		case RELE_8_OFF:
			ReleOff(WichData);
			break;
		case RELE_1_ON:		
		case RELE_2_ON:		
		case RELE_3_ON:		
		case RELE_4_ON:		
		case RELE_5_ON:		
		case RELE_6_ON:		
		case RELE_7_ON:		
		case RELE_8_ON:	
			ReleOn(WichData);
			break;		
		default:
			break;
	}
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
}

void loop() 
{
	ChekButtons();
	CalcEnergy();
	TickSecond++;
	if(TickSecond == (1000 / MAIN_DELAY))
	{
		TickSecond = 0;
		EnergyValueSec();	
	}	
	
}
