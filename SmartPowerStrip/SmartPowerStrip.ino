#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "EEPROM_Node.h"
#include "LCDLib.h"
#include "TimeLib.h"
#include "SmartPowerStrip.h"
#include "Web.h"
#include "Menu.h"
#include "Band.h"
#include "Rele.h"
#include "I2CNano.h"
#include "Reset.h"
#include "Measure.h"


#undef FIRST_GO

extern RELE Rele[];
extern WiFiClient client;
extern ESP8266WebServer server;

uint8_t WifiConnectionOn[]
{
    0x00,
    0x00,
    0x00,
    0x1F,
    0x01,
    0x1D,
    0x05,
    0x15
};

uint8_t WifiConnectionOff[]
{
    0x00,
    0x00,
    0x00,
    0x0E,
    0x19,
    0x15,
    0x13,
    0x0E
};

uint8_t OffRele[]
{
	0x0E,
	0x11,
	0x11,
	0x11,
	0x11,
	0x0E,
	0x04,
	0x04
};

uint8_t OnRele[]
{
	0x0E,
	0x1F,
	0x1F,
	0x1F,
	0x1F,
	0x0E,
	0x04,
	0x04
};

uint8_t ClientConnection[]
{
	0x02,
	0x1F,
	0x02,
	0x00,
	0x00,
	0x08,
	0x1F,
	0x08
};


uint8_t EmptyIcon[]
{
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00
};

FLAGS Flag;
short EnterSetup;

String VersionValue = "3.0";
String VersionDate  = "3/9/18";
String ModelNumber  = "001";

void BlinkLed(short pin)
{
	ON(pin);
	delay(5);
	OFF(pin);
}

void delayms(short ms)
{
	delay(ms);
}

void CheckEvents()
{
	TakePresentTime();
	TakeReleTime();
	WebClient();
	if(Flag.WifiActive && !Flag.WpsConfigSelected)
		WifiScanForSignal();
}


bool IsBackToDefault()
{
	bool BackToDefault = false;
	short ResetValue = 0;
	ReadMemory(FACTORY_RESET_ADDR, &ResetValue);
	if(ResetValue == DEFAULT_RESET)
		BackToDefault = true;
	else
		BackToDefault = false;
	EepromUpdate(FACTORY_RESET_ADDR, NOT_DEFAULT_RESET);
	return BackToDefault;
}

void setup()
{
#ifndef FIRST_GO
	Serial.begin(115200);
	pinMode(RELE1, OUTPUT);
	pinMode(RELE2, OUTPUT);
	pinMode(RELE3, OUTPUT);
	pinMode(RELE4, OUTPUT);
	pinMode(RELE5, OUTPUT);
	pinMode(RELE6, OUTPUT);
	pinMode(RELE7, OUTPUT);
	pinMode(RELE8, OUTPUT);
	pinMode(BUTTON_LED, OUTPUT);
	short FirstStart = 0;
	// bool FirstResetEnergyConfirm = false;
	Wire.begin(SDA_PIN, SCL_PIN); // Inizializza I2C per NodeMCU
	EepromInit();
	LCDInit();
	RTCInit();
	// while(!FirstResetEnergyConfirm)
	// {
		// delay(200);
		// FirstResetEnergyConfirm = FirstResetEnergy();
	// }
	LCDShowSplashScreen("Smart Power Strip", "HomeMicroTech", String("Modello: " + ModelNumber));
	WifiConfInit();
	WifiInit();
	if(Flag.WifiActive)
	{
		WebServerInit();
	}
	LCDCreateIcon(WifiConnectionOn, WIFI_OK);
	LCDCreateIcon(WifiConnectionOff, WIFI_NO);
	LCDCreateIcon(OffRele, RELE_OFF);
	LCDCreateIcon(OnRele, RELE_ON);
	LCDCreateIcon(ClientConnection, CLIENT_CONN);
	LCDCreateIcon(EmptyIcon, EMPTY);
	TakePresentTime();

	ReadMemory(FIRST_START_CHECK_ADDR, &FirstStart);
	if(FirstStart == EMPTY_MEMORY_VALUE || IsBackToDefault())
	{
		WriteMemory(FIRST_START_CHECK_ADDR, 0);
		LCDNoBlink();
		LCDDisplayOn();
		SetBandInvalid();
		LCDDisplayOff();
		TurnOffAllRele(SAVE);
		Flag.IsBandSetted = false;
		Flag.IsDisplayOn = false;
		Flag.ReleRS = true;
		LCDDisplayOn();
		Flag.IsDisplayOn = true;
		ReleInit(true);
		EepromUpdate(TARIFF_STATUS_ADDR, NOT_SETTED);
	}
	else
	{
		LCDNoBlink();
		LCDDisplayOn();
		Flag.IsDisplayOn = true;
		ReleInit(false);
		BandInit();
		MenuInit();
	}
	ClearLCD();
	LCDPrintString(ONE, CENTER_ALIGN, "Premere Ok/Set");
	LCDPrintString(TWO, CENTER_ALIGN, "per entrare nel");
	LCDPrintString(THREE, CENTER_ALIGN, "Menu Principale");
	delay(1500);
	ClearLCD();
#else
	Wire.begin(SDA_PIN, SCL_PIN);
	EepromInit();
	LCDInit();
	ClearMemory();
	if(IsMemoryEmpty())
		LCDPrintString(TWO, CENTER_ALIGN, "Memoria Vuota");
	RestartESP();
#endif // FIRST_GO
}

void loop()
{
#ifndef FIRST_GO
	EnterSetup = NO_PRESS;
	EnterSetup = CheckButtons();
	if(Flag.WifiActive)
	{
		server.handleClient();
	}
	MainScreen(EnterSetup);
#endif
}
