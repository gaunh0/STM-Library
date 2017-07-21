#ifndef _GPIO_H
#define _GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define BUTTON1_PIN              GPIO_PIN_13
#define BUTTON1_PORT        	 GPIOB

#define BUTTON2_PIN              GPIO_PIN_14
#define BUTTON2_PORT        	 GPIOB

#define BUTTON1_MASK		0x01
#define BUTTON2_MASK		0x02


#define PRESSED			1
#define RELEASED		0

void ButtonInitController(void);
void ButtonService(void);
uint8_t Button1Pressed(void);
uint8_t Button2Pressed(void);
uint8_t GetButtonValue(void);
uint8_t AllButtonReleased(void);
#endif


