#ifndef RELE_H
#define RELE_H
#define MAX_TIME_TIMER		14401

#define NO_SAVE		false
#define SAVE		true

#define RELE_1_STR	"Presa 1"
#define RELE_2_STR	"Presa 2"
#define RELE_3_STR	"Presa 3"
#define RELE_4_STR	"Presa 4"
#define RELE_5_STR	"Presa 5"
#define RELE_6_STR	"Presa 6"
#define RELE_7_STR	"Presa 7"
#define RELE_8_STR	"Presa 8"

typedef enum
{
	RELE_1 = 0,
	RELE_2,
	RELE_3,
	RELE_4,
	RELE_5,
	RELE_6,
	RELE_7,
	RELE_8,
	RELE_MAX
}RELE_NUMB;

typedef struct
{
	short hour;
	short minute;
	short second;
	short day;
}RELE_TIME_FORMAT;

enum
{
	TURN_ON_TIME = 0,
	TIMER_TIME
};

typedef enum
{
	STATUS_OFF = 0,
	STATUS_ON
} RELE_STATUS;

typedef struct
{
	bool IsActive;
	RELE_TIME_FORMAT TurnOnTime;
	RELE_TIME_FORMAT ActiveTime;
	bool HaveTimer;
	RELE_TIME_FORMAT TimerTime;
	short EepromAddr;
}RELE;

void SaveReleStatus(short ReleIndx, short Status);

bool ReleInit(bool FirstGo);
void ReleReStart(void);
void TakeReleTime(void);
void ShowReleIcons(short Row);
void TurnOnRele(short ReleIndx);
void TurnOffRele(short ReleIndx);
void TurnOffAllRele(bool SaveSatus);
void TurnOnAllRele(bool SaveSatus);
void CheckReleStatus(void);
bool SetTimerRele(short ReleNbr);
RELE_TIME_FORMAT SetTimeVarRele(short Hour, short Minute, short Second, short Day);
short ReleIdx2Pin(short ReleIndx);
void SaveReleStatus(short ReleIndx, short Status);
#endif