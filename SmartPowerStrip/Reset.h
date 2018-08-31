#ifndef RESET_H
#define RESET_H
#include <Arduino.h>

typedef enum
{
	NOT_DEFAULT_RESET = 0,
	DEFAULT_RESET
} DEFAULT_RESET_VALUES;

void RestartESP(void);
void Reset2Factory(void);
bool ResetEnergy(void);
bool FirstResetEnergy(void);
bool ResetDefault(void);
bool RestartEsp(void);

#endif