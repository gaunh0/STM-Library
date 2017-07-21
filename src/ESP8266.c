#include "ESP8266.h"
#include "VTimer.h"
#include "UART.h"
#include <string.h>

extern UART_HandleTypeDef UartWiFiHandle;
uint8_t VTimerWifiTimeout_ID = 0;

void ESP8266_InitController(){
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.Pin = WIFI_ENABLE_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(WIFI_ENABLE_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = WIFI_RST_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(WIFI_RST_GPIO_PORT, &GPIO_InitStructure);
	

	VTimerWifiTimeout_ID = VTimerGetID();
	ESP_EN_High();
	ESP_RST_High();
	ESP8266_HardReset();
}

void ESP8266_SendByte(uint8_t _value){
	UART_SendByte(UartWiFiHandle,_value);
}
void ESP8266_SendChar(char _value){
	UART_SendByte(UartWiFiHandle,_value);
}
void ESP8266_SendString(uint8_t* str){
	UART_SendString(UartWiFiHandle,str);
}
void ESP8266_SendStringLength(uint8_t* str, uint16_t length){
	for (uint16_t i=0;i<length;i++){
		ESP8266_SendChar(str[i]);
	}
}
void ESP8266_SendNumber(uint16_t _number){
	if (_number >= 10000) { 	// 5 char
		ESP8266_SendChar((_number/10000) + '0' );
		ESP8266_SendChar(((_number/1000) % 10 )+ '0' );
		ESP8266_SendChar(((_number/100) % 10 )+ '0' );
		ESP8266_SendChar(((_number/10) % 10 )+ '0' );
		ESP8266_SendChar((_number % 10 )+ '0' );
	}
	else if (_number >= 1000) { 	// 4 char
		ESP8266_SendChar(((_number/1000))+ '0' );
		ESP8266_SendChar(((_number/100) % 10 )+ '0' );
		ESP8266_SendChar(((_number/10) % 10 )+ '0' );
		ESP8266_SendChar((_number % 10 )+ '0' );
	}
	else if (_number >= 100) {	 // 3 char
		ESP8266_SendChar(((_number/100) )+ '0' );
		ESP8266_SendChar(((_number/10) % 10 )+ '0' );
		ESP8266_SendChar((_number % 10 )+ '0' );
	}
	else if (_number >= 10) { 	// 2 char
		ESP8266_SendChar(((_number/10))+ '0' );
		ESP8266_SendChar((_number % 10 )+ '0' );
	}
	else {					// 1 char
		ESP8266_SendChar(_number+ '0' );
	}
}
uint8_t ESP8266_ReceiveByte(){
	return GetReceiveByte(&UartWiFiHandle);
}
uint8_t ESP8266_RxAvailable(){
	return RxBufferAvailable(&UartWiFiHandle);
}
void ESP8266_ClearRxBuffer(){
	uint16_t i = 0;
	for (i=0;i<UART1_RX_BUFFER_SIZE;i++){
		uart1_rx_buffer[i] =0;
	}
	ClearUart1RxIndex();
}

uint8_t ESP8266_WaitOK(uint32_t timeout){
	VTimerSet(VTimerWifiTimeout_ID, timeout);
	while(VTimerIsFired(VTimerWifiTimeout_ID) == 0){
		if(ESP8266_RxAvailable()){
			if(strstr(uart1_rx_buffer,"OK")){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t ESP8266_WaitResponse(uint32_t timeout){
	ESP8266_ClearRxBuffer();
	VTimerSet(VTimerWifiTimeout_ID, timeout);
	while(VTimerIsFired(VTimerWifiTimeout_ID) == 0){
		if(ESP8266_RxAvailable()){
			if(strstr(uart1_rx_buffer,"\r\n")){
				return 1;
				break;
			}
		}
	}
	return 0;
}

uint8_t ESP8266_WaitString(uint32_t timeout,uint8_t* pointer){	
	VTimerSet(VTimerWifiTimeout_ID, timeout);
	while(VTimerIsFired(VTimerWifiTimeout_ID)== 0){
		if(ESP8266_RxAvailable()){
			if(strstr(uart1_rx_buffer,pointer)){
				return 1;
				break;
			}
		}
	}
	return 0;
}

uint8_t ESP8266_CheckDataBuffer(uint8_t* pointer){	
	if(ESP8266_RxAvailable()){
		if (strstr(uart1_rx_buffer,pointer)){
			return 1;
		}
	}
	return 0;
}

uint8_t ESP8266_GetResponseData(uint32_t timeout,uint8_t* pointer){	
	uint8_t rec_char = 0;
	uint8_t rec_char_index = 0;
	uint8_t ret_length = 0;
	VTimerSet(VTimerWifiTimeout_ID, timeout);
	while(VTimerIsFired(VTimerWifiTimeout_ID)== 0){
		if(ESP8266_RxAvailable()){
			rec_char = ESP8266_ReceiveByte();
			//pointer[rec_char_index] = rec_char;
			rec_char_index ++;
		}
		if (strstr(pointer,"OK")) {
			break;
		}
	}
	ret_length = rec_char_index;
	return ret_length;
}

void ESP8266_HardReset(){
	ESP_RST_Low();
	DelayMs(100);
	ESP_RST_High();
	DelayMs(2000);
}
void ESP8266_SoftReset(){
	ESP8266_ClearRxBuffer();
	ESP8266_SendString("AT+RST\r\n");
	ESP8266_WaitOK(2000);
}



