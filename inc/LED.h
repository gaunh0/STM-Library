#ifndef LED_H
#define LED_H

#include "stm32f1xx_hal.h"

#define LED_OFF		0
#define LED_ON		1

enum LedID{
	LEDRED = 0,
	LEDGREEN = 1,
};

#define LEDRED_PIN			GPIO_PIN_1
#define LEDRED_PORT		GPIOB

#define LEDGREEN_PIN		GPIO_PIN_0
#define LEDGREEN_PORT		GPIOB


void LedInitController(void);
void LedOff(uint8_t LedIndex);
void LedOn(uint8_t LedIndex);
void LedToggle(uint8_t LedIndex);

void SetLedDisplayFlag(void);
void ClearLedDisplayFlag(void);
void LedScanDisplay(void);
#endif

