#ifndef ESP8266_H
#define ESP8266_H

#include "stm32f1xx_hal.h"
#include "ESP8266WifiType.h"

#define WIFI_ENABLE_GPIO_PIN		GPIO_PIN_12
#define WIFI_ENABLE_GPIO_PORT		GPIOA

#define WIFI_RST_GPIO_PIN					GPIO_PIN_11
#define WIFI_RST_GPIO_PORT				GPIOA

#define ESP_EN_High()		HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_PORT,WIFI_ENABLE_GPIO_PIN,GPIO_PIN_SET)
#define ESP_EN_Low()			HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_PORT,WIFI_ENABLE_GPIO_PIN,GPIO_PIN_RESET)
#define ESP_RST_High()		HAL_GPIO_WritePin(WIFI_RST_GPIO_PORT,WIFI_RST_GPIO_PIN,GPIO_PIN_SET)
#define ESP_RST_Low()		HAL_GPIO_WritePin(WIFI_RST_GPIO_PORT,WIFI_RST_GPIO_PIN,GPIO_PIN_RESET)

#define wifi_rx_buffer	uart1_rx_buffer	

#define SINGLE_CONNECTION		0
#define MULTIPLE_CONNECTION	1


void ESP8266_InitController(void);
void ESP8266_SendByte(uint8_t _value);
void ESP8266_SendChar(char _value);
void ESP8266_SendString(uint8_t* str);
void ESP8266_SendStringLength(uint8_t* str, uint16_t length);
void ESP8266_SendNumber(uint16_t _number);
uint8_t ESP8266_ReceiveByte(void);
uint8_t ESP8266_RxAvailable(void);
void ESP8266_ClearRxBuffer(void);
uint8_t ESP8266_WaitOK(uint32_t timeout);
uint8_t ESP8266_WaitResponse(uint32_t timeout);
uint8_t ESP8266_WaitString(uint32_t timeout, uint8_t* pointer);
uint8_t ESP8266_CheckDataBuffer(uint8_t* pointer);
uint8_t ESP8266_GetResponseData(uint32_t timeout,uint8_t* pointer);	

void ESP8266_HardReset(void);
void ESP8266_SoftReset(void);

 #endif
 
