#ifndef MBI5026_H
#define MBI5026_H

#include "stm32f1xx_hal.h"

#define MBI5026_SDI_PIN			GPIO_PIN_9
#define MBI5026_SDI_PORT		GPIOB

#define MBI5026_CLK_PIN			GPIO_PIN_8
#define MBI5026_CLK_PORT		GPIOB

#define MBI5026_LE_PIN			GPIO_PIN_12
#define MBI5026_LE_PORT		GPIOB

#define MBI5026_OE_PIN			GPIO_PIN_15
#define MBI5026_OE_PORT		GPIOB

void MBI5026_InitController(void);
void MBI5026_SendData(uint16_t data);

#endif
