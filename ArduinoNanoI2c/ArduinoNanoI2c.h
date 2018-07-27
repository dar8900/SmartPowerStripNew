#define  OFF(pin)  digitalWrite(pin, LOW)
#define  ON(pin)   digitalWrite(pin, HIGH)
#define ARDUINO_ADDR	0x08

#define SECOND(sec)		(sec * 1000)


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
	CURRENT,
	POWER,
	NO_DATA = 20
};