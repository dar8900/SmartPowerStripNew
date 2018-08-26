#define  OFF(pin)  digitalWrite(pin, LOW)
#define  ON(pin)   digitalWrite(pin, HIGH)
#define ARDUINO_ADDR	0x08

#define SECOND(sec)		(sec * 1000)

#define NO_DATA	30

#undef DEBUG_SERIAL

typedef enum
{
	BUTTON_UP = 0,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_SET,
	NO_PRESS
}BUTTONS;

enum
{
	UP = 2,
	DOWN,
	LEFT,
	SET,
	RELE_1,
	RELE_2,
	RELE_3,
	RELE_4,
	RELE_5,
	RELE_6,
	RELE_7,
	RELE_8	
};

enum
{
	BUTTON = 0,
	ENERGY,
	CURRENT
};

typedef enum
{
	RESET_ENERGY = 20,
	MAX_COMMAND_LIST
}COMMAND_LIST;

typedef enum
{
	UNDONE = 0,
	DONE
}COMMAND_RESULT;

short GetCommand(short Command);