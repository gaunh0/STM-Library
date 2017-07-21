#include "HC12.h"
#include "VTimer.h"
#include "UART.h"
#include <string.h>

extern UART_HandleTypeDef huart3;
uint8_t VTimerRFTimeout_ID = 0;

void HC12_InitController(){
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.Pin = HC12_SET_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(HC12_SET_GPIO_PORT, &GPIO_InitStructure);
		
	VTimerRFTimeout_ID = VTimerGetID();
	
	HC12_SET_High();
}
void HC12_SendByte(uint8_t _value)
{
	UART_SendByte(huart3,_value);
}
void HC12_SendString(uint8_t *str)
{
	UART_SendString(huart3,str);
}
void HC12_SendStringLength(uint8_t *str,uint16_t length)
{
	for(uint16_t i = 0; i<length;i++)
	{
		HC12_SendByte(str[i]);
	}
}
void HC12_SendNumber(uint8_t _number)
{
	if (_number >= 10000) { 	// 5 char
		HC12_SendByte((_number/10000) + '0' );
		HC12_SendByte(((_number/1000) % 10 )+ '0' );
		HC12_SendByte(((_number/100) % 10 )+ '0' );
		HC12_SendByte(((_number/10) % 10 )+ '0' );
		HC12_SendByte((_number % 10 )+ '0' );
	}
	else if (_number >= 1000) { 	// 4 char
		HC12_SendByte(((_number/1000))+ '0' );
		HC12_SendByte(((_number/100) % 10 )+ '0' );
		HC12_SendByte(((_number/10) % 10 )+ '0' );
		HC12_SendByte((_number % 10 )+ '0' );
	}
	else if (_number >= 100) {	 // 3 char
		HC12_SendByte(((_number/100) )+ '0' );
		HC12_SendByte(((_number/10) % 10 )+ '0' );
		HC12_SendByte((_number % 10 )+ '0' );
	}
	else if (_number >= 10) { 	// 2 char
		HC12_SendByte(((_number/10))+ '0' );
		HC12_SendByte((_number % 10 )+ '0' );
	}
	else {					// 1 char
		HC12_SendByte(_number+ '0' );
	}	
}
uint8_t HC12_ReceiverByte(){
	return GetReceiveByte(&huart3);
}
uint8_t HC12_RxAvailable(){
	return RxBufferAvailable(&huart3);
}
void HC12_ClearRxBuffer(){
	uint16_t i = 0;
	for (i=0;i<UART3_RX_BUFFER_SIZE;i++){
		uart3_rx_buffer[i] =0;
	}
	ClearUart3RxIndex();
}
uint8_t HC12_WaitOK(uint32_t timeout){
	VTimerSet(VTimerRFTimeout_ID, timeout);
	while(VTimerIsFired(VTimerRFTimeout_ID) == 0){
		if(HC12_RxAvailable()){
			if(strstr(uart3_rx_buffer,"OK")){
				return 1;
			}
		}
	}
	return 0;
}
uint8_t HC12_WaitString(uint32_t timeout,uint8_t* pointer){	
	VTimerSet(VTimerRFTimeout_ID, timeout);
	while(VTimerIsFired(VTimerRFTimeout_ID)== 0){
		if(HC12_RxAvailable()){
			if(strstr(uart3_rx_buffer,pointer)){
				return 1;
				break;
			}
		}
	}
	return 0;
}
uint8_t HC12_CheckDataBuffer(uint8_t* pointer){	
	if(HC12_RxAvailable()){
		if (strstr(uart3_rx_buffer,pointer)){
			return 1;
		}
	}
	return 0;
}
void HC12_Config(uint8_t channel,uint8_t mode , uint8_t power){
	HC12_SET_Low();
	do {
		HC12_ClearRxBuffer();	
		HC12_SendString("AT\r\n");
	}while (HC12_WaitOK(1000) == 0);
	HC12_SetChannel(channel);
	HC12_SetMode(mode);
	HC12_SetPower(power);
	HC12_ClearRxBuffer();
	HC12_SET_High();
}
void HC12_SetChannel(uint8_t channel)
{
	HC12_ClearRxBuffer();
	HC12_SendString("AT+C");
	HC12_SendNumber(channel);
	HC12_SendString("\r\n");
	HC12_WaitString(200,"OK+C");
}
void HC12_SetMode(uint8_t mode)
{
	HC12_ClearRxBuffer();
	HC12_SendString("AT+FU");
	HC12_SendNumber(mode);
	HC12_SendString("\r\n");
	HC12_WaitString(200,"OK+FU");
}
void HC12_SetPower(uint8_t power)
{
	HC12_ClearRxBuffer();
	HC12_SendString("AT+P");
	HC12_SendNumber(power);
	HC12_SendString("\r\n");
	HC12_WaitString(200,"OK+P");
}