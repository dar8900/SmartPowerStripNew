#include <Wire.h>
#include <stdint.h>
#include "ArduinoNanoI2c.h"
#include "EEPROM_Ard_Nano.h"
#include "Measure.h"
#include "ReleI2C.h"


short    ButtonPress = NO_PRESS;
short    TickSecond = 0;
uint32_t Tick10Min;
short    WichData = NO_DATA;
short    CommandResult = UNDONE;
uint16_t TimeExecEnergy, TimeExec4Calib;

extern float 		EnergyMeasured;
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

short GetCommand(short Command)
{
	bool Result = false;
	short CommandReturn = UNDONE;
	switch(Command)
	{
		case RELE_1_OFF:
		case RELE_2_OFF:
		case RELE_3_OFF:
		case RELE_4_OFF:
		case RELE_5_OFF:
		case RELE_6_OFF:
		case RELE_7_OFF:
		case RELE_8_OFF:
			ReleOff(Command);
			Result = true;
			break;
		case RELE_1_ON:		
		case RELE_2_ON:		
		case RELE_3_ON:		
		case RELE_4_ON:		
		case RELE_5_ON:		
		case RELE_6_ON:		
		case RELE_7_ON:		
		case RELE_8_ON:	
			ReleOn(Command);
			Result = true;
			break;
		case RESET_ENERGY:
			Result = ResetEnergies();
			break;
		default:
			break;
	}
	if(Result)	
		CommandReturn = DONE;
	return CommandReturn;
}


static void WichInfo()
{
	while(Wire.available())
	{
   		WichData = Wire.read();
	}	
	CommandResult = GetCommand(WichData);
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
		case RESET_ENERGY:
			Wire.write(CommandResult);
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
	int32_t TempReadedEnergy = 0;
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
#ifdef DEBUG_SERIAL
	if(!IsMemoryEmpty())
		ClearMemory();
	WriteBigData(ENERGY_VALUE_INIT_ADDR, 3450); // 0.5mA * 230 * 30s
    TempReadedEnergy = ReadBigData(ENERGY_VALUE_INIT_ADDR, NINE_REG);
	Serial.print("Energia registrata: ");
	Serial.println(TempReadedEnergy);
	delay(30000);
#endif
	TurnOffRele();
	// if(TempReadedEnergy > 0)
		// EnergyMeasured = (float) TempReadedEnergy;
	CurrentCalibration();
}

void loop() 
{
	TimeExecEnergy = millis();
	ChekButtons();
	CalcEnergy();
	TickSecond++;
	Tick10Min++;
	TimeExecEnergy = millis() - TimeExecEnergy;
	
	// Viene misurata l'energia in Ws ogni sec e scritta nella stringa
	if(TickSecond == (SECOND(1) / TimeExecEnergy))
	{
		TickSecond = 0;
		MeasureValueSec();
#ifdef DEBUG_SERIAL
		Serial.println(TimeExecEnergy);
#endif
	}	
	// if(Tick10Min == (SECOND(60) / TimeExecEnergy))
	// {
		// Tick10Min = 0;
		// WriteBigData(ENERGY_VALUE_INIT_ADDR, (uint32_t)EnergyMeasured);
	// }
}
