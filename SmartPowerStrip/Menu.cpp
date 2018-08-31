#include "LCDLib.h"
#include "TimeLib.h"
#include "SmartPowerStrip.h"
#include "EEPROM_Node.h"
#include "I2CNano.h"
#include "Menu.h"
#include "Web.h"
#include "Band.h"
#include "Rele.h"
#include "Measure.h"
#include "Reset.h"

extern TIME_DATE_FORMAT PresentTime;
extern BAND_FORMAT Band;
extern RELE Rele[];
extern FLAGS Flag;
extern String HostName;
extern String VersionValue;
extern String VersionDate;

uint16_t TimerRefreshMenu;
short TimerClientConnected = DELAY_CLIENT_CONNECTION;
bool ExitFromBand = true;
extern uint16_t TariffaInt;
extern float TariffaFloat;

static const String ONOFF[] = {"Off", "On"};

const MENU_VOICES MainMenuItems[]
{
	{ManualRele		,	"Gestione Manuale"	},
	{ChangeTimeBand	,	"Cambio Banda"		},
	{AssignReleTimer,	"Assegna Timer"		},
	{ShowMeasures	,	"Misure"   		    },
	{WifiConnect	,	"Connetti WiFi"		},
	{HelpInfo		,	"Help e Info"		},
	{WiFiInfo		,	"Wifi Info"			},
	{Setup       	,	"Impostazioni"    	},
};

const MENU_VOICES GeneralSetup[]
{
	{ChangeTime			, "Cambio ora"      },
	{ChangeTimerDisplay , "Timer display on"},
	{TurnOffWifi        , "Spegni Wifi"     },
	{ChangeUdmEnergy    , "UDM Energia"     },
	{ChangeTariff       , "Cambia tariffa"  },
	{ResetEnergy        , "Reset Energia"   },
	{ResetDefault       , "Reset Default"   },
	{RestartEsp         , "Restart Ciabatta"},
};


const DELAY_TIMER_S TimerDalays[]
{
	{1   , "Sempre Acceso"},
	{6000, "10 min"},
	{3000, "5 min"},
	{1000, "1 min"},
	{500,  "30 sec"},
	{166,  "10 sec"},
};



void MenuInit()
{
	short Delay = MAIN_SCREEN_TIMER_DEFAULT;
	short EnergyUdm = 0, TariffStatus = 0;
	ReadMemory(TIMER_BACKLIGHT_ADDR, &Delay);
	ReadMemory(TARIFF_STATUS_ADDR, &TariffStatus);
	if(Delay > MAX_DELAY_TIMERS)
	{
		Delay = TEN_MINUTES;
		WriteMemory(TIMER_BACKLIGHT_ADDR, Delay);
		TimerRefreshMenu = TimerDalays[Delay].DelayValue;
	}
	else
	{
		TimerRefreshMenu = TimerDalays[Delay].DelayValue;
	}
	ClearLCD();
	EepromUpdate(UDM_ENERGY_ADDR, WATT_ORA);
	LCDPrintString(ONE, CENTER_ALIGN, "Retroilluminazione");
	LCDPrintString(TWO, CENTER_ALIGN, "impostata a:");
	LCDPrintString(THREE, CENTER_ALIGN, TimerDalays[Delay].DelayStr);
	delay(DELAY_MENU_MSG);
	ClearLCD();
	if(TariffStatus != SETTED)
	{
		EepromUpdate(TARIFF_STATUS_ADDR, NOT_SETTED);
		ChangeTariff();
	}
	else
		LoadTariffValue();
	return;
}

void MainScreen(short EnterSetup)
{
	short ReleIndx = 0;
	CheckReleStatus();
	CheckEvents();
	short RefreshDelayItem;
	uint8_t GeneralCnt = 0;
	bool Toggle = true;
	Flag.InBand = CheckBand();
	if(Flag.InBand)
	{
		ClearLCD();
		for(GeneralCnt = 0; GeneralCnt < 3; GeneralCnt++)
		{
			if(Toggle)
				LCDPrintString(TWO, CENTER_ALIGN, "Banda in attivazione");
			else
				ClearLCDLine(TWO);
			Toggle = !Toggle;
			delay(400);
		}
		ClearLCD();
		while(CheckBand())
		{
			if(Flag.WifiActive)
				WifiDisconnect();

			if(Flag.IsDisplayOn)
			{
				LCDDisplayOff();
				Flag.IsDisplayOn = false;
			}
			if(!Flag.AllReleDown)
			{
				TurnOffAllRele(NO_SAVE);
			}
			for(ReleIndx = RELE_1; ReleIndx < RELE_MAX; ReleIndx++)
			{
				if(Rele[ReleIndx].HaveTimer == true)
				{
					Rele[ReleIndx].HaveTimer = false;
					Rele[ReleIndx].TimerTime = SetTimeVarRele(0,0,0,0);
				}
			}
			delay(WHILE_LOOP_DELAY);
		}
		EnterSetup = NO_PRESS;
		ExitFromBand = false;
		Flag.InBand = false;
	}
	else
	{
		if(!ExitFromBand)
		{
			LCDDisplayOn();
			Flag.IsDisplayOn = true;
			for(GeneralCnt = 0; GeneralCnt < 3; GeneralCnt++)
			{
				if(Toggle)
					LCDPrintString(TWO, CENTER_ALIGN, "Uscita dalla banda");
				else
					ClearLCDLine(TWO);
				Toggle = !Toggle;
				delay(400);
			}
			ClearLCD();
			BandInit();
			ReleReStart();
			ExitFromBand = true;
		}
		// Scrittura data e ora
		ShowDateTime(ONE);
		// Icona WebPage aperta
		if(Flag.ClientConnected)
		{
			TimerClientConnected--;
			if(TimerClientConnected > 0)
			{
				ShowClientConnected(ONE, 9, true);
			}
			else
			{
				TimerClientConnected = DELAY_CLIENT_CONNECTION;
				Flag.ClientConnected = false;
				ShowClientConnected(ONE, 9, false);
			}
		}
		// Stato relè
		LCDPrintString(TWO, CENTER_ALIGN, "Prese attive:");
		ShowReleIcons(THREE);
		// Stato wifi
		ShowWifiStatus(ONE, 7, Flag.WifiActive);
		ScreenTimerRefresh();
	}
	if(EnterSetup == BUTTON_SET)
	{
		if(!Flag.IsDisplayOn)
		{
			LCDDisplayOn();
			Flag.IsDisplayOn = true;
		}
		MainMenu();
		ClearLCD();
		EnterSetup = NO_PRESS;
	}
	else if(EnterSetup != NO_PRESS)
	{
		if(!Flag.IsDisplayOn)
		{
			Flag.IsDisplayOn = true;
			LCDDisplayOn();
			EnterSetup = NO_PRESS;
		}

	}

}

void MainMenu()
{
	bool ExitMainMenu = false;
	bool ReEnterMenu = false;
	uint8_t ButtonPress = 0, Item = MANUAL_RELE;
	uint16_t TimerBackLightMenu = 0, TimeExecMainMenu = 0;
	ClearLCD();
	LCDPrintString(ONE  , CENTER_ALIGN, "Premere Up o Down");
	LCDPrintString(TWO  , CENTER_ALIGN, "per scegliere");
	LCDPrintString(THREE, CENTER_ALIGN, "Premere Ok/Set");
	LCDPrintString(FOUR , CENTER_ALIGN, "per confermare");
	delay(DELAY_MENU_MSG);
	ClearLCD();
	LCDPrintString(ONE  , CENTER_ALIGN, "Premere Left/Back");
	LCDPrintString(TWO  , CENTER_ALIGN, "per tornare alla");
	LCDPrintString(THREE, CENTER_ALIGN, "schermata iniziale");
	delay(DELAY_MENU_MSG);
	ClearLCD();
	while(!ExitMainMenu)
	{
		TimeExecMainMenu = millis();
		if(ReEnterMenu)
		{
			ClearLCD();
			CheckEvents();
			LCDPrintString(ONE  , CENTER_ALIGN, "Premere Up o Down");
			LCDPrintString(TWO  , CENTER_ALIGN, "per scegliere");
			LCDPrintString(THREE, CENTER_ALIGN, "Premere Ok/Set");
			LCDPrintString(FOUR , CENTER_ALIGN, "per confermare");
			delay(DELAY_MENU_MSG);
			ClearLCD();
			LCDPrintString(ONE  , CENTER_ALIGN, "Premere Left/Back");
			LCDPrintString(TWO  , CENTER_ALIGN, "per tornare alla");
			LCDPrintString(THREE, CENTER_ALIGN, "schermata iniziale");
			delay(DELAY_MENU_MSG);
			ClearLCD();
			CheckEvents();
			ReEnterMenu = false;
		}
		CheckEvents();
		// Scrittura data e ora
		ShowDateTime(ONE);
		// Icona WebPage aperta
		if(Flag.ClientConnected)
		{
			TimerClientConnected--;
			if(TimerClientConnected > 0)
			{
				ShowClientConnected(ONE, 9, true);
			}
			else
			{
				TimerClientConnected = DELAY_CLIENT_CONNECTION;
				Flag.ClientConnected = false;
				ShowClientConnected(ONE, 9, false);
			}
		}
		ShowWifiStatus(ONE, 7, Flag.WifiActive);
		LCDPrintString(THREE, CENTER_ALIGN, MainMenuItems[Item].MenuTitle);

		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case BUTTON_UP:
				BlinkLed(BUTTON_LED);
				if(Item > MANUAL_RELE)
				{
					Item--;
				}
				else
				{
					Item = MAX_MENU_ITEM - 1;
				}
				TimerBackLightMenu = 0;
				if(!Flag.IsDisplayOn)
				{
					Flag.IsDisplayOn = true;
					LCDDisplayOn();
				}
				ClearLCDLine(THREE);
				break;
			case BUTTON_DOWN:
				BlinkLed(BUTTON_LED);
				if(Item < MAX_MENU_ITEM - 1)
				{
					Item++;
				}
				else
				{
					Item = MANUAL_RELE;
				}
				TimerBackLightMenu = 0;
				if(!Flag.IsDisplayOn)
				{
					Flag.IsDisplayOn = true;
					LCDDisplayOn();
				}
				ClearLCDLine(THREE);
				break;
			case BUTTON_SET:
				BlinkLed(BUTTON_LED);
				TimerBackLightMenu = 0;
				if(!Flag.IsDisplayOn)
				{
					Flag.IsDisplayOn = true;
					LCDDisplayOn();
				}
				ReEnterMenu = true;
				break;
			case BUTTON_LEFT:
				BlinkLed(BUTTON_LED);
				TimerBackLightMenu = 0;
				if(!Flag.IsDisplayOn)
				{
					Flag.IsDisplayOn = true;
					LCDDisplayOn();
				}
				ExitMainMenu = true;
				break;
			default:
				break;
		}
		ButtonPress = NO_PRESS;
		delay(WHILE_LOOP_DELAY);
		if(Flag.IsDisplayOn)
			TimerBackLightMenu++;
		TimeExecMainMenu = millis() - TimeExecMainMenu;
		if(TimerBackLightMenu == (15000 / TimeExecMainMenu))
		{
			if(Flag.IsDisplayOn)
			{
				Flag.IsDisplayOn = false;
				LCDDisplayOff();
			}
		}
		if(ReEnterMenu)
		{
			MainMenuItems[Item].MenuFunc();
		}
	}
}

bool Setup()
{
	bool ExitSetup = false;
	uint8_t ButtonPress = NO_PRESS;
	short SetupItem = CHANGE_TIME;
	ClearLCD();
	while(!ExitSetup)
	{
		CheckEvents();
		ShowDateTime(ONE);
		if(Flag.ClientConnected)
		{
			TimerClientConnected--;
			if(TimerClientConnected > 0)
			{
				ShowClientConnected(ONE, 9, true);
			}
			else
			{
				TimerClientConnected = DELAY_CLIENT_CONNECTION;
				Flag.ClientConnected = false;
				ShowClientConnected(ONE, 9, false);
			}
		}
		LCDPrintString(THREE, CENTER_ALIGN, GeneralSetup[SetupItem].MenuTitle);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case BUTTON_UP:
				BlinkLed(BUTTON_LED);
				if(SetupItem > 0)
				{
					SetupItem--;
				}
				else
				{
					SetupItem = MAX_SETUP_ITEM - 1;
				}
				ClearLCDLine(THREE);
				break;
			case BUTTON_DOWN:
				BlinkLed(BUTTON_LED);
				if(SetupItem < MAX_SETUP_ITEM - 1)
				{
					SetupItem++;
				}
				else
				{
					SetupItem = 0;
				}
				ClearLCDLine(THREE);
				break;
			case BUTTON_SET:
				BlinkLed(BUTTON_LED);
				GeneralSetup[SetupItem].MenuFunc();
				ClearLCD();
				break;
			case BUTTON_LEFT:
				BlinkLed(BUTTON_LED);
				ClearLCD();
				ExitSetup = true;
				break;
			default:
				break;
		}
		delay(WHILE_LOOP_DELAY);
	}
}

bool ManualRele()
{
	short ReleIndx = RELE_1;
	bool ReleSetted = false, OnOffAll = false,  ExitManualRele = false, SetTheRele = false;
	bool FirstBackSet = false;
	String SelRele;
	uint8_t ButtonPress = NO_PRESS, Status = 0, OldStatus = 0;
	if(Flag.AllReleUp)
	{
		ClearLCD();
		LCDPrintString(ONE, CENTER_ALIGN, "Vuoi spegnere");
		LCDPrintString(TWO, CENTER_ALIGN, "tutte le prese?");
		delay(DELAY_MENU_MSG);
	}
	else
	{
		ClearLCD();
		LCDPrintString(ONE, CENTER_ALIGN, "Vuoi accendere");
		LCDPrintString(TWO, CENTER_ALIGN, "tutte le prese?");
		delay(DELAY_MENU_MSG);
	}
	OnOffAll = CheckYesNo();
	if(OnOffAll)
	{
		ClearLCD();
		LCDPrintString(TWO, CENTER_ALIGN, "Attendere...");
		delay(DELAY_MENU_MSG);
		if(Flag.AllReleUp)
		{
			TurnOffAllRele(SAVE);
			LCDShowPopUp("Tutte Spente");
		}
		else
		{
			TurnOnAllRele(SAVE);
			LCDShowPopUp("Tutte Accese");
		}
	}
	else
	{
		ClearLCD();
		while(!ExitManualRele)
		{
			SelRele = "Impostare la presa:";
			LCDPrintString(ONE, CENTER_ALIGN, SelRele);
			SelRele = String(ReleIndx + 1);
			LCDPrintString(TWO, CENTER_ALIGN, SelRele);
			CheckEvents();
			ReleSetted = false;
			SetTheRele = false;
			ButtonPress = CheckButtons();
			switch(ButtonPress)
			{
				case BUTTON_UP:
					if(ReleIndx > RELE_1)
						ReleIndx--;
					else
						ReleIndx = RELE_MAX - 1;
					ClearLCDLine(TWO);
					break;
				case BUTTON_DOWN:
					if(ReleIndx < RELE_MAX - 1)
						ReleIndx++;
					else
						ReleIndx = RELE_1;
					ClearLCDLine(TWO);
					break;
				case BUTTON_LEFT:
					SetTheRele = false;
					ExitManualRele = true;
					break;
				case BUTTON_SET:
					SetTheRele = true;
				default:
					break;
			}
			ButtonPress = NO_PRESS;
			if(SetTheRele)
			{
				if(Rele[ReleIndx].IsActive)
				{
					LCDPrintString(THREE, CENTER_ALIGN, ONOFF[STATUS_ON]);
					Status = STATUS_ON;
				}
				else
				{
					LCDPrintString(THREE, CENTER_ALIGN, ONOFF[STATUS_OFF]);
					Status = STATUS_OFF;
				}
				OldStatus = Status;			
				while(!ReleSetted)
				{
					CheckEvents();
					ButtonPress = CheckButtons();	
					LCDPrintString(THREE, CENTER_ALIGN, ONOFF[Status]);
					switch(ButtonPress)
					{
						case BUTTON_UP:
							BlinkLed(BUTTON_LED);
							if(Status == STATUS_ON)
								Status = STATUS_OFF;
							else
								Status = STATUS_ON;
							ClearLCDLine(THREE);
							break;
						case BUTTON_DOWN:
							BlinkLed(BUTTON_LED);
							if(Status == STATUS_ON)
								Status = STATUS_OFF;
							else
								Status = STATUS_ON;
							ClearLCDLine(THREE);
							break;
						case BUTTON_SET:
							BlinkLed(BUTTON_LED);
							ClearLCD();
							if(OldStatus != Status)
								LCDPrintString(TWO, CENTER_ALIGN, "Valore Salvato");
							else
								LCDPrintString(TWO, CENTER_ALIGN, "Valore non cambiato");
							if(Status == STATUS_OFF)
							{
								TurnOffRele(ReleIndx);
								SelRele = "Presa " + String(ReleIndx + 1) + " spenta";
								LCDPrintString(THREE, CENTER_ALIGN, SelRele);
							}
							else
							{
								TurnOnRele(ReleIndx);
								SelRele = "Presa " + String(ReleIndx + 1) + " accesa";
								LCDPrintString(THREE, CENTER_ALIGN, SelRele);
							}
							SaveReleStatus(ReleIndx, Status);
							ReleSetted = true;
							delay(DELAY_MENU_MSG);
							ClearLCD();
							break;
						case BUTTON_LEFT:
							ClearLCD();
							LCDPrintString(THREE, CENTER_ALIGN, "Valore non impostato");
							delay(DELAY_INFO_MSG);
							ClearLCD();
							ReleSetted = true;
							break;
						default:
							break;
					}
					delay(WHILE_LOOP_DELAY);
					ButtonPress = NO_PRESS;
				}
			}
			delay(WHILE_LOOP_DELAY);
		}
		CheckReleStatus();
	}
	return true;
}

bool ChangeTimeBand()
{
	bool ExitChangeBand = false, OkBandSet = true;
	uint8_t ButtonPress;
    ClearLCD();
	CheckEvents();
	LCDPrintString(ONE  , CENTER_ALIGN, "Cambiare la banda");
	LCDPrintString(TWO  , CENTER_ALIGN, "per spegnere la");
	LCDPrintString(THREE, CENTER_ALIGN, "ciabatta all'interno");
	LCDPrintString(FOUR , CENTER_ALIGN, "dell'orario");
	delay(DELAY_INFO_MSG);
	ClearLCD();
	LCDPrintString(ONE  , CENTER_ALIGN, "Premere Ok/Set");
	LCDPrintString(TWO  , CENTER_ALIGN, "per cambiare");
	LCDPrintString(THREE, CENTER_ALIGN, "la banda o");
	LCDPrintString(FOUR , CENTER_ALIGN, "diabilitarla");
	delay(DELAY_INFO_MSG);
	ClearLCD();
	LCDPrintString(ONE  , CENTER_ALIGN, "Premere Left/Back");
	LCDPrintString(TWO  , CENTER_ALIGN, "per tornare al");
	LCDPrintString(THREE, CENTER_ALIGN, "Menu Principale");
	delay(DELAY_INFO_MSG);
	ClearLCD();
	CheckEvents();
	while(!ExitChangeBand)
	{
		if(!OkBandSet)
		{
			ClearLCD();
			CheckEvents();
			LCDPrintString(ONE  , CENTER_ALIGN, "Premere Ok/Set");
			LCDPrintString(TWO  , CENTER_ALIGN, "per cambiare");
			LCDPrintString(THREE, CENTER_ALIGN, "la banda o");
			LCDPrintString(FOUR , CENTER_ALIGN, "diabilitarla");
			delay(DELAY_INFO_MSG);
			ClearLCD();
			LCDPrintString(ONE  , CENTER_ALIGN, "Premere Left/Back");
			LCDPrintString(TWO  , CENTER_ALIGN, "per tornare al");
			LCDPrintString(THREE, CENTER_ALIGN, "Menu Principale");
			delay(DELAY_INFO_MSG);

			ClearLCD();
			OkBandSet = true;
		}

		CheckEvents();
		ButtonPress = CheckButtons();
		LCDPrintString(ONE, CENTER_ALIGN, "Premi Ok/Set");
		LCDPrintString(TWO, CENTER_ALIGN, "per modificare");
		LCDPrintString(THREE, CENTER_ALIGN, "Premi Left/Back");
		LCDPrintString(FOUR, CENTER_ALIGN, "per uscire");
		switch(ButtonPress)
		{
			case BUTTON_UP:
			case BUTTON_DOWN:
			default:
				break;
			case BUTTON_LEFT:
				BlinkLed(BUTTON_LED);
				ClearLCD();
				LCDPrintString(TWO, CENTER_ALIGN, "Uscita...");
				delay(DELAY_INFO_MSG);
				ClearLCD();
				ExitChangeBand = true;
				break;
			case BUTTON_SET:
				BlinkLed(BUTTON_LED);
				if(!Flag.IsBandSetted)
				{
					if(SetTimeBand())
					{
					  if(IsBandCorrect())
						{
							Flag.IsBandSetted = true;
							ExitChangeBand = true;
						}
						else
						{
							ClearLCD();
							LCDPrintString(ONE, CENTER_ALIGN, "Banda");
							LCDPrintString(TWO, CENTER_ALIGN, "non corretta");
							delay(DELAY_INFO_MSG);
							ClearLCD();
							CheckEvents();
							SetBandInvalid();
							ExitChangeBand = false;
							OkBandSet = false;
						}
					}
				}
				else
				{
					SetBandInvalid();
					ExitChangeBand = true;
				}
				break;
		}
		delay(WHILE_LOOP_DELAY);
		ButtonPress = NO_PRESS;
	}
	return true;
}

bool WifiConnect()
{
	bool WifiDisconnectChoice = false, WpsChoice = false, WpsConfStart = false, WpsSuccess = false;
	uint8_t ButtonPress = NO_PRESS;
	short ConnectionSetted = 0;
	if(Flag.WifiActive)
	{
		if(!Flag.WpsConfigSelected)
		{
			ClearLCD();
			LCDPrintString(ONE, CENTER_ALIGN, "Sei ancora connesso");
			LCDPrintString(TWO, CENTER_ALIGN, "ad una rete WiFi!");
			LCDPrintString(THREE, CENTER_ALIGN, "Disconnettere?");
			CheckEvents();
			WifiDisconnectChoice = CheckYesNo();
			ClearLCD();
			if(WifiDisconnectChoice)
				WifiDisconnect();
		}
		else
		{
			ClearLCD();
			LCDPrintString(ONE, CENTER_ALIGN, "Sei ancora connesso");
			LCDPrintString(TWO, CENTER_ALIGN, "ad una rete WiFi!");
			LCDPrintString(THREE, CENTER_ALIGN, "Disconnettere?");
			CheckEvents();
			WifiDisconnectChoice = CheckYesNo();
			ClearLCD();
			if(WifiDisconnectChoice)
				WifiTurnOff();			
		}
	}
	else
	{
		ClearLCD();
		ReadMemory(WIFI_WPS_CONF_ADDR, &ConnectionSetted);
		switch(ConnectionSetted)
		{
			case CONN_NOT_SETTED:
				LCDPrintString(TWO, CENTER_ALIGN, "Connettere tramite");
				LCDPrintString(THREE, CENTER_ALIGN, "WPS?");
				delay(DELAY_INFO_MSG);
				WpsChoice = CheckYesNo();
				if(WpsChoice)
				{
					WpsSuccess = WPSConnection();
					CheckEvents();
					if(WpsSuccess)
						WebServerInit();
				}
				else
				{
					WifiWiredConnections();
					if(Flag.WifiActive)
					{
						WebServerInit();
					}
				}				
				break;
			case WPS:
				WpsSuccess = WPSConnection();
				CheckEvents();
				if(WpsSuccess)
					WebServerInit();				
				break;
			case CABLATA:
				WifiWiredConnections();
				if(Flag.WifiActive)
				{
					WebServerInit();
				}				
				break;
			default:
				break;
		}
	}
	return true;
}

static bool BackPressed()
{
	uint8_t ButtonPress = NO_PRESS;
	uint8_t TimerPressing = 50; // 0.5 sec
	bool Pressed = false;
	while(TimerPressing > 0)
	{
		ButtonPress = CheckButtons();
		TimerPressing--;
		if(ButtonPress == BUTTON_LEFT)
		{
			Pressed = true;
			break;
		}
		delay(10);
	}
	return Pressed;
}


bool HelpInfo()
{
	uint8_t NumTimer = 0, ReleIndx = 0, ReleTimer[2];
	String BandTime1, BandTime2, Hour, Minute, Day, Second, TotalTime;
	bool NoTimer = true;
	CheckEvents();
	ClearLCD();
	LCDPrintString(ONE, CENTER_ALIGN, "Info e Aiuto");
	LCDPrintString(THREE, CENTER_ALIGN, "Premere Left/Back");
	LCDPrintString(FOUR, CENTER_ALIGN, "per uscire");
	delay(DELAY_INFO_MSG);
	CheckEvents();
	ClearLCD();
	while(1)
	{
		if(BackPressed())
			break;
		LCDPrintString(ONE, CENTER_ALIGN, "Stato Prese:");
		ShowReleIcons(TWO);
		LCDPrintString(THREE, CENTER_ALIGN, "Stato Wifi:");
		ShowWifiStatus(FOUR, CENTER_ALIGN, (Flag.WifiActive) ? "Attivo" : "Spento");
		delay(DELAY_INFO_MSG);
		CheckEvents();
		if(BackPressed())
			break;
		ClearLCD();
		for(ReleIndx = RELE_1; ReleIndx < RELE_MAX; ReleIndx++)
		{
			if(Rele[ReleIndx].HaveTimer)
			{
				if(NumTimer > 1)
					break;
				ReleTimer[NumTimer] = ReleIndx + 1;
				NumTimer++;
				NoTimer = false;
			}
			else
			{
				if(ReleIndx == RELE_MAX-1 && NoTimer)
				{
					break;
				}
			}
		}
		CheckEvents();
		if(!NoTimer)
		{
			LCDPrintString(ONE, LEFT_ALIGN, "Timer attivi:");
			LCDPrintValue(ONE, 15, NumTimer);
			LCDPrintString(TWO, LEFT_ALIGN, "Prese associate:");
			LCDPrintValue(THREE, 5, ReleTimer[0]);
			if(ReleTimer[1] <= 8)
				LCDPrintValue(THREE, 15, ReleTimer[1]);
		}
		else
		{
			LCDPrintString(ONE, CENTER_ALIGN, "Timer attivi:");
			LCDPrintString(TWO, CENTER_ALIGN, "Nessuno");
		}
		if(BackPressed())
			break;
		delay(DELAY_INFO_MSG);
		CheckEvents();
		if(BackPressed())
			break;
		ClearLCD();
		LCDPrintString(ONE, CENTER_ALIGN, "Stato Banda:");
		if(Flag.IsBandSetted)
		{
			LCDPrintString(TWO, CENTER_ALIGN, "Settata");
			LCDPrintString(THREE, CENTER_ALIGN, "Orario impostato:");
			BandTime1 = String(Band.InitHour) + ":";
			BandTime2 = String(Band.EndHour) + ":";
			if(Band.InitMinute < 10)
				BandTime1 += "0" + String(Band.InitMinute);
			else
				BandTime1 += String(Band.InitMinute);
			if(Band.EndMinute < 10)
				BandTime2 += "0" + String(Band.EndMinute);
			else
				BandTime2 += String(Band.EndMinute);

			BandTime1 = BandTime1 + "  " + BandTime2;
			LCDPrintString(FOUR, CENTER_ALIGN, BandTime1);
		}
		else
		{
			LCDPrintString(TWO, CENTER_ALIGN, "Non Settata");
		}
		if(BackPressed())
			break;
		delay(DELAY_INFO_MSG);
		CheckEvents();
		if(BackPressed())
			break;
		ClearLCD();
		LCDPrintString(ONE, CENTER_ALIGN, "Tempo accensione");
		for(ReleIndx = RELE_1; ReleIndx < RELE_MAX; ReleIndx++)
		{
			CheckEvents();
			LCDPrintString(TWO, 6, "presa: ");
			LCDPrintValue(TWO, 14, ReleIndx+1);
			if(Rele[ReleIndx].IsActive)
			{
				short TimerShowTotalTime = 30;
				while(TimerShowTotalTime > 0)
				{
					CheckEvents();
					Day = String(Rele[ReleIndx].ActiveTime.day);
					Hour = String(Rele[ReleIndx].ActiveTime.hour);
					Minute = String(Rele[ReleIndx].ActiveTime.minute);
					TotalTime = Day + "g " + Hour + "h " + Minute + "m";
					LCDPrintString(THREE, CENTER_ALIGN, TotalTime);
					TimerShowTotalTime--;
					delay(100);
				}
				ClearLCDLine(THREE);
			}
			else
			{
				LCDPrintString(THREE, CENTER_ALIGN, "Non Attiva");
				delay(DELAY_MENU_MSG);
				ClearLCDLine(THREE);
			}
			if(BackPressed())
				break;
		}
		delay(DELAY_INFO_MSG);
		if(BackPressed())
			break;
		ClearLCD();
		CheckEvents();
		LCDPrintString(ONE, CENTER_ALIGN, "Versione Firmware:");
		LCDPrintString(TWO, CENTER_ALIGN, VersionValue);
		LCDPrintString(THREE, CENTER_ALIGN, "Data rilascio:");
		LCDPrintString(FOUR, CENTER_ALIGN, VersionDate);
		if(BackPressed())
			break;
		delay(DELAY_INFO_MSG);
		CheckEvents();
		ClearLCD();
		if(BackPressed())
			break;
	}
	ClearLCD();
	LCDPrintString(THREE, CENTER_ALIGN, "Uscita...");
	delay(DELAY_INFO_MSG);
	ClearLCD();
    CheckEvents();
	
	return true;
}


bool WiFiInfo()
{
	bool ExitWifiInfo = false;
	uint8_t ButtonPress = 0;
	short WifiItemSsid = 0;
	short RefreshSignal = 150;
	ClearLCD();
	LCDPrintString(ONE  , CENTER_ALIGN, "Wifi Info");
	LCDPrintString(TWO  , CENTER_ALIGN, "Premere Left/Back");
	LCDPrintString(THREE, CENTER_ALIGN, "per tornare al");
	LCDPrintString(FOUR , CENTER_ALIGN, "Menu Principale");
	delay(DELAY_MENU_MSG);
	ReadMemory(WIFI_SSID_ADDR, &WifiItemSsid);
	CheckEvents();
	ClearLCD();
	while(!ExitWifiInfo)
	{
		CheckEvents();
		if(Flag.WifiActive)
		{
			LCDPrintString(ONE, LEFT_ALIGN, "IP:");
			LCDPrintString(TWO, LEFT_ALIGN, "SSID:");
			LCDPrintString(TWO, RIGHT_ALIGN, MyNetworkList[WifiItemSsid].WifiName);
			LCDPrintString(THREE, LEFT_ALIGN, "Hostname:");
			LCDPrintString(THREE, RIGHT_ALIGN, HostName);
			LCDPrintString(ONE, RIGHT_ALIGN, WifiIP());
			LCDPrintString(FOUR, LEFT_ALIGN, "Segnale:");
			// Ogni 4.5s
			if(RefreshSignal == 150)
			{
				ClearLCDLine(FOUR);
				LCDPrintString(FOUR, LEFT_ALIGN, "Segnale:");
				LCDPrintString(FOUR, RIGHT_ALIGN, GetWifiSignalPower());
				if(!Flag.WifiActive)
				{
					delay(DELAY_MENU_MSG);
					ClearLCD();
				}
				RefreshSignal = 0;
			}

		}
		else
		{
			LCDPrintString(TWO, CENTER_ALIGN, "Nessuna rete");
			LCDPrintString(THREE, CENTER_ALIGN, "associata");
		}
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case BUTTON_LEFT:
				BlinkLed(BUTTON_LED);
				ExitWifiInfo = true;
				break;
			case BUTTON_UP:
			case BUTTON_DOWN:
			case BUTTON_SET:
			default:
				break;

		}
		ButtonPress = NO_PRESS;
		RefreshSignal++;
		delay(WHILE_LOOP_DELAY);
	}
	return true;
}

bool AssignReleTimer()
{
	uint8_t ButtonPress, ReleNumber = RELE_1, TimerAssignedCnt = 0;
	bool ExitAssignReleTimer = false;
	ClearLCD();
	CheckEvents();
	if(Flag.AllReleDown)
	{
		LCDPrintString(TWO, CENTER_ALIGN, "Non ci sono");
		LCDPrintString(THREE, CENTER_ALIGN, "prese attive!");
		delay(DELAY_INFO_MSG);
		ClearLCD();
	}
	else
	{
		LCDPrintString(TWO, CENTER_ALIGN, "Assegna un timer");
		LCDPrintString(THREE, CENTER_ALIGN, "a massimo 2 prese");
		delay(DELAY_MENU_MSG);
		ClearLCD();
		LCDPrintString(TWO, CENTER_ALIGN, "Premi Up o Down");
		LCDPrintString(THREE, CENTER_ALIGN, "per scegliere");
		LCDPrintString(FOUR, CENTER_ALIGN, "la presa");
		delay(DELAY_MENU_MSG);
		ClearLCD();
		LCDPrintString(ONE, CENTER_ALIGN, "Premi Ok/Set");
		LCDPrintString(TWO, CENTER_ALIGN, "per confermare");
		LCDPrintString(THREE, CENTER_ALIGN, "o per disabilitare");
		LCDPrintString(FOUR, CENTER_ALIGN, "un timer");
		delay(DELAY_MENU_MSG);
		ClearLCD();
		LCDPrintString(TWO, CENTER_ALIGN, "Premi Left/Back");
		LCDPrintString(THREE, CENTER_ALIGN, "per tornare al");
		LCDPrintString(FOUR, CENTER_ALIGN, "Menu Principale");
		delay(DELAY_MENU_MSG);
		ClearLCD();
		CheckEvents();
		while(!ExitAssignReleTimer)
		{
			CheckEvents();
			if(Rele[ReleNumber].IsActive)
			{
				LCDPrintString(ONE, CENTER_ALIGN, "Assegna il timer");
				LCDPrintString(TWO, CENTER_ALIGN, "alla presa numero:");
				LCDPrintValue(THREE, CENTER_ALIGN, ReleNumber + 1);
			}
			else
			{
				LCDPrintString(ONE, CENTER_ALIGN, "Presa numero:");
				LCDPrintValue(TWO, CENTER_ALIGN, ReleNumber + 1);
				LCDPrintString(THREE, CENTER_ALIGN, "spenta");
			}
			if(Rele[ReleNumber].HaveTimer)
			{
				LCDPrintString(FOUR, CENTER_ALIGN, "Timer abilitato");
			}
			else
			{
				ClearLCDLine(FOUR);
			}
			ButtonPress = CheckButtons();
			switch(ButtonPress)
			{
				case BUTTON_UP:
					BlinkLed(BUTTON_LED);
					if(ReleNumber > RELE_1)
						ReleNumber--;
					else
						ReleNumber = RELE_MAX - 1;
					ClearLCD();
					break;
				case BUTTON_DOWN:
					BlinkLed(BUTTON_LED);
					if(ReleNumber < RELE_MAX - 1)
						ReleNumber++;
					else
						ReleNumber = RELE_1;
					ClearLCD();
					break;
				case BUTTON_SET:
					BlinkLed(BUTTON_LED);
					if(Rele[ReleNumber].HaveTimer)
					{
						ClearLCD();
						LCDPrintString(THREE, CENTER_ALIGN, "Timer disabilitato");
						delay(DELAY_MENU_MSG);
						ClearLCD();
						Rele[ReleNumber].HaveTimer = false;
						Rele[ReleNumber].TimerTime = SetTimeVarRele(0,0,0,0);
						TimerAssignedCnt--;
					}
					else if(TimerAssignedCnt == 2)
					{
						ClearLCD();
						LCDPrintString(ONE, CENTER_ALIGN, "Raggiunto il");
						LCDPrintString(TWO, CENTER_ALIGN, "numero massimo");
						LCDPrintString(THREE, CENTER_ALIGN, "di timer assegnati");
						delay(DELAY_INFO_MSG);
						ClearLCD();
						LCDPrintString(ONE, CENTER_ALIGN, "Disabilitare");
						LCDPrintString(TWO, CENTER_ALIGN, "un timer");
						LCDPrintString(THREE, CENTER_ALIGN, "o uscire");
						delay(DELAY_INFO_MSG);
						ClearLCD();
					}
					else if(!Rele[ReleNumber].HaveTimer && Rele[ReleNumber].IsActive)
					{
						ClearLCD();
						LCDPrintString(TWO, CENTER_ALIGN, "Timer abilitato");
						delay(DELAY_INFO_MSG);
						ClearLCD();
						Rele[ReleNumber].HaveTimer = SetTimerRele(ReleNumber);
						ON(ReleIdx2Pin(ReleNumber));
						ReleOn(ReleNumber);
						Flag.AllReleDown = false;
						CheckEvents();
						TimerAssignedCnt++;
					}

					break;
				case BUTTON_LEFT:
					ExitAssignReleTimer = true;
					break;
				default:
					break;
			}
			ButtonPress = NO_PRESS;
			delay(WHILE_LOOP_DELAY);
		}
	}
	return true;
}

bool ChangeTimerDisplay()
{
	uint8_t ButtonPress = NO_PRESS;
	short DelayItem = ALWAYS_ON;
	bool ExitchangeDelay = false;
	ClearLCD();
	LCDPrintString(ONE, CENTER_ALIGN, "Scegliere il");
	LCDPrintString(TWO, CENTER_ALIGN, "timer per la");
	LCDPrintString(THREE, CENTER_ALIGN, "retroilluminazione: ");
	ReadMemory(TIMER_BACKLIGHT_ADDR,&DelayItem);
	while(!ExitchangeDelay)
	{
		LCDPrintString(FOUR, CENTER_ALIGN, TimerDalays[DelayItem].DelayStr);
		ButtonPress	= CheckButtons();
		switch(ButtonPress)
		{
			case BUTTON_UP:
				BlinkLed(BUTTON_LED);
				if(DelayItem > ALWAYS_ON)
					DelayItem--;
				else
					DelayItem = MAX_DELAY_TIMERS - 1;
				ClearLCDLine(FOUR);
				break;
			case BUTTON_DOWN:
				BlinkLed(BUTTON_LED);
				if(DelayItem < MAX_DELAY_TIMERS - 1)
					DelayItem++;
				else
					DelayItem = ALWAYS_ON;
					ClearLCDLine(FOUR);
				break;
			case BUTTON_SET:
				BlinkLed(BUTTON_LED);
				WriteMemory(TIMER_BACKLIGHT_ADDR, DelayItem);
				TimerRefreshMenu = TimerDalays[DelayItem].DelayValue;
				ExitchangeDelay = true;
				break;
			case BUTTON_LEFT:
				ExitchangeDelay = true;
			default:
				break;
		}
		delay(WHILE_LOOP_DELAY);
	}
	return true;
}

bool TurnOffWifi()
{
	bool TurnOff = false;
	ClearLCD();
	LCDPrintString(TWO, CENTER_ALIGN, "Spegnere il wifi?");
	TurnOff = CheckYesNo();
	if(TurnOff)
	{
		WifiTurnOff();
	}
	return true;
}

bool CheckYesNo()
{
	bool Exit = false, Choice = false;
	String YesNo[] = {"Si", "No"};
	uint8_t ButtonPress = NO_PRESS;
	short YesNoChoice = NO;
	while(!Exit)
	{
		CheckEvents();
		LCDPrintString(FOUR, CENTER_ALIGN, YesNo[YesNoChoice]);
		ButtonPress = CheckButtons();
		switch(ButtonPress)
		{
			case BUTTON_UP:
				BlinkLed(BUTTON_LED);
				if(YesNoChoice == YES)
					YesNoChoice = NO;
				else
					YesNoChoice = YES;
				ClearLCDLine(FOUR);
				break;
			case BUTTON_DOWN:
				BlinkLed(BUTTON_LED);
				if(YesNoChoice == YES)
					YesNoChoice = NO;
				else
					YesNoChoice = YES;
					ClearLCDLine(FOUR);
				break;
			case BUTTON_SET:
				BlinkLed(BUTTON_LED);
				if(YesNoChoice == YES)
				{
					Exit = true;
					Choice = true;
				}
				else
				{
					Exit = true;
					Choice = false;
				}
				break;
			case BUTTON_LEFT:
				Exit = true;
				Choice = false;
			default:
				break;
		}
		ButtonPress = NO_PRESS;
		delay(WHILE_LOOP_DELAY);
	}
	return Choice;
}

void ScreenTimerRefresh()
{
	short DelayItem;
	ReadMemory(TIMER_BACKLIGHT_ADDR,&DelayItem);
	if(DelayItem != ALWAYS_ON)
	{
		TimerRefreshMenu--;
		if(TimerRefreshMenu == 0)
		{
			TimerRefreshMenu = TimerDalays[DelayItem].DelayValue;
			if(Flag.IsDisplayOn)
			{
				LCDDisplayOff();
				Flag.IsDisplayOn = false;
			}
		}
	}
	return;
}