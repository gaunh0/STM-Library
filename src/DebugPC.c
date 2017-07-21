 #include "DebugPC.h"
#include "VTimer.h"
#include "UART.h"
#include <string.h>

uint8_t VTimerDebugTimeout_ID = 0;
extern UART_HandleTypeDef UartDebug;

void DebugInitController(){
	VTimerDebugTimeout_ID = VTimerGetID();
}

void DEBUG_SendByte(uint8_t _value){
	UART_SendByte(UartDebug,_value);
}
void DEBUG_SendString(uint8_t* str){
	UART_SendString(UartDebug,str);
}
void DEBUG_SendStringLength(uint8_t* str, uint16_t length){
	for (uint16_t i=0;i<length;i++){
		DEBUG_SendByte(str[i]);
	}
}
void DEBUG_SendNumber(uint16_t _number){
	if (_number >= 10000) { 	// 5 char
		DEBUG_SendByte((_number/10000) + '0' );
		DEBUG_SendByte(((_number/1000) % 10 )+ '0' );
		DEBUG_SendByte(((_number/100) % 10 )+ '0' );
		DEBUG_SendByte(((_number/10) % 10 )+ '0' );
		DEBUG_SendByte((_number % 10 )+ '0' );
	}
	else if (_number >= 1000) { 	// 4 char
		DEBUG_SendByte(((_number/1000))+ '0' );
		DEBUG_SendByte(((_number/100) % 10 )+ '0' );
		DEBUG_SendByte(((_number/10) % 10 )+ '0' );
		DEBUG_SendByte((_number % 10 )+ '0' );
	}
	else if (_number >= 100) {	 // 3 char
		DEBUG_SendByte(((_number/100) )+ '0' );
		DEBUG_SendByte(((_number/10) % 10 )+ '0' );
		DEBUG_SendByte((_number % 10 )+ '0' );
	}
	else if (_number >= 10) { 	// 2 char
		DEBUG_SendByte(((_number/10))+ '0' );
		DEBUG_SendByte((_number % 10 )+ '0' );
	}
	else {					// 1 char
		DEBUG_SendByte(_number+ '0' );
	}
}
uint8_t DEBUG_ReceiveByte(){
	return GetReceiveByte(&UartDebug);
}
uint8_t DEBUG_RxAvailable(){
	return RxBufferAvailable(&UartDebug);
}
void DEBUG_ClearRxBuffer(){
	uint16_t i = 0;
	for (i=0;i<UART2_RX_BUFFER_SIZE;i++){
		uart2_rx_buffer[i] =0;
	}
	ClearUart2RxIndex();
}

uint8_t DEBUG_WaitOK(uint32_t timeout){
	VTimerSet(VTimerDebugTimeout_ID, timeout);
	while(VTimerIsFired(VTimerDebugTimeout_ID) == 0){
		if(DEBUG_RxAvailable()){
			if(strstr(uart2_rx_buffer,"OK")){
				return 1;
			}
		}
	}
	return 0;
}

uint8_t DEBUG_WaitResponse(uint32_t timeout){
	DEBUG_ClearRxBuffer();
	VTimerSet(VTimerDebugTimeout_ID, timeout);
	while(VTimerIsFired(VTimerDebugTimeout_ID) == 0){
		if(DEBUG_RxAvailable()){
			if(strstr(uart2_rx_buffer,"\r\n")){
				return 1;
				break;
			}
		}
	}
	return 0;
}

uint8_t DEBUG_WaitString(uint32_t timeout,uint8_t* pointer){	
	VTimerSet(VTimerDebugTimeout_ID, timeout);
	while(VTimerIsFired(VTimerDebugTimeout_ID)== 0){
		if(DEBUG_RxAvailable()){
			if(strstr(uart2_rx_buffer,pointer)){
				return 1;
				break;
			}
		}
	}
	return 0;
}

uint8_t DEBUG_CheckDataBuffer(uint8_t* pointer){	
	if(DEBUG_RxAvailable()){
		if (strstr(uart2_rx_buffer,pointer)){
			return 1;
		}
	}
	return 0;
}

uint8_t DEBUG_GetResponseData(uint32_t timeout,uint8_t* pointer){	
	uint8_t rec_char = 0;
	uint8_t rec_char_index = 0;
	uint8_t ret_length = 0;
	VTimerSet(VTimerDebugTimeout_ID, timeout);
	while(VTimerIsFired(VTimerDebugTimeout_ID)== 0){
		if(DEBUG_RxAvailable()){
			rec_char = DEBUG_ReceiveByte();
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

