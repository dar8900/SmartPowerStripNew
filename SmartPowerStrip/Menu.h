#ifndef MENU_H
#define MENU_H
#define MAIN_SCREEN_TIMER_DEFAULT	6000
#define DELAY_CLIENT_CONNECTION 	  60
#define 	TARIFFA			0.05135
#define 	TENSIONE_LINEA	230.0

typedef enum
{
	MANUAL_RELE = 0,
	CHANGE_TIME_BAND,
	ASSIGN_RELE_TIMER,
	SHOW_MEASURES,
	WIFI_CONNECT,
	HELP_INFO,
	WIFI_INFO,
	SETUP,
	MAX_MENU_ITEM
}MENU_NUMBER;

typedef enum
 {
	 CHANGE_TIME = 0,
	 BACKLIGH_TIMER,
	 TURN_OFF_WIFI,
	 CHANGE_UDM_ENERGY,
	 CHANGE_TARIFF,
	 ENERGY_RESET,
	 RESET_DEFAULT,
	 RESTART_ESP,
	 MAX_SETUP_ITEM
 }SETUP_NUMBER;

typedef struct
{
	bool (*MenuFunc)(void);
	String MenuTitle;

}MENU_VOICES;

typedef enum
{
	YES = 0,
	NO
} ANSWER_TYPE;

typedef enum
{
	ALWAYS_ON = 0,
	TEN_MINUTES,
	FIVE_MINUTES,
	ONE_MINUTE,
	HALF_MINUTE,
	TEN_SECONDS,
	MAX_DELAY_TIMERS
}DELAY_TIMERS;

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

typedef struct
{
	uint16_t DelayValue;
	String DelayStr;
}DELAY_TIMER_S;

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

// bool SetupInterrupt(void);
bool CheckYesNo(void);
void MenuInit(void);
void MainScreen(short EnterSetup);
void MainMenu(void);
bool Setup(void);
bool ManualRele(void);
bool ChangeTimeBand(void);
bool WifiConnect(void);
bool HelpInfo(void);
bool ShowMeasures(void);
bool WiFiInfo(void);
bool AssignReleTimer(void);
bool ChangeTimerDisplay(void);
bool TurnOffWifi(void);
bool ChangeUdmEnergy(void);
bool ChangeTariff(void);
void LoadTariffValue(void);
bool ResetEnergy(void);
void FirstResetEnergy(void);
bool ResetDefault(void);
bool RestartEsp(void);
void ScreenTimerRefresh(void);

#endif