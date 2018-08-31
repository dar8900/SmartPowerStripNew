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


typedef struct
{
	uint16_t DelayValue;
	String DelayStr;
}DELAY_TIMER_S;



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
bool WiFiInfo(void);
bool AssignReleTimer(void);
bool ChangeTimerDisplay(void);
bool TurnOffWifi(void);
void ScreenTimerRefresh(void);

#endif