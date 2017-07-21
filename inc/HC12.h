#ifndef HC12_H
#define HC12_H

#include "stm32f1xx_hal.h"

#define HC12_SET_GPIO_PIN		GPIO_PIN_1
#define HC12_SET_GPIO_PORT		GPIOB

#define HC12_SET_High()		HAL_GPIO_WritePin(HC12_SET_GPIO_PORT,HC12_SET_GPIO_PIN,GPIO_PIN_SET)
#define HC12_SET_Low()		HAL_GPIO_WritePin(HC12_SET_GPIO_PORT,HC12_SET_GPIO_PIN,GPIO_PIN_RESET)

#define rf_rx_buffer	uart3_rx_buffer	

#define DONT_CARE	0x00
#define OK  		0x01
#define	CRC_ERROR   0x03
#define DLE		0x10
#define STX		0x02
#define ETX		0x03


void HC12_InitController(void);
void HC12_SendByte(uint8_t _value);
void HC12_SendString(uint8_t *str);
void HC12_SendStringLength(uint8_t * str,uint16_t length);
void HC12_SendNumber(uint8_t _number);
uint8_t HC12_ReceiverByte();
uint8_t HC12_RxAvailable();
void HC12_ClearRxBuffer();
uint8_t HC12_WaitOK(uint32_t timeout);
uint8_t HC12_WaitString(uint32_t timeout,uint8_t* pointer);
uint8_t HC12_CheckDataBuffer(uint8_t* pointer);

void HC12_Config(uint8_t channel,uint8_t mode , uint8_t power);
void HC12_SetChannel(uint8_t channel);
void HC12_SetMode(uint8_t mode);
void HC12_SetPower(uint8_t power);

#endif
