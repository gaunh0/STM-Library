#include "stm32f1xx_hal.h"

void DEBUG_SendByte(uint8_t _value);
void DEBUG_SendChar(char _value);
void DEBUG_SendString(uint8_t* str);
void DEBUG_SendStringLength(uint8_t* str, uint16_t length);
void DEBUG_SendNumber(uint16_t _number);
uint8_t DEBUG_ReceiveByte(void);
uint8_t DEBUG_RxAvailable(void);
void DEBUG_ClearRxBuffer(void);
uint8_t DEBUG_WaitOK(uint32_t timeout);
uint8_t DEBUG_WaitResponse(uint32_t timeout);
uint8_t DEBUG_WaitString(uint32_t timeout, uint8_t* pointer);
uint8_t DEBUG_CheckDataBuffer(uint8_t* pointer);
uint8_t DEBUG_GetResponseData(uint32_t timeout,uint8_t* pointer);	
