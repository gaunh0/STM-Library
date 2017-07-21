#include "UART.h"
#include <stdio.h>
#include <string.h>

UART_HandleTypeDef UartDebug;
UART_HandleTypeDef UartWiFiHandle;
UART_HandleTypeDef huart3;

DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart3_rx;

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART and Loop until the end of transmission */
  HAL_UART_Transmit(&UartDebug, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

uint8_t uart1_rx_buffer[UART1_RX_BUFFER_SIZE];
uint8_t uart1_rx_index;

uint8_t uart2_rx_buffer[UART2_RX_BUFFER_SIZE];
uint8_t uart2_rx_index;

uint8_t uart3_rx_buffer[UART3_RX_BUFFER_SIZE];
uint8_t uart3_rx_index;

uint8_t RxDebugGetByteDMA;
uint8_t RxWifiGetByteDMA;
uint8_t RxRFGetByteDMA;

uint8_t VTimer_UARTInternalTimeout = 0;

void UARTInitController(){

	  /* DMA1_Channel3_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
	/* DMA1_Channel5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
	
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();

	if  (HAL_UART_Receive_DMA(&UartDebug,&RxDebugGetByteDMA, 1) != HAL_OK){
		Error_Handler();
	}
	if  (HAL_UART_Receive_DMA(&UartWiFiHandle, &RxWifiGetByteDMA, 1) != HAL_OK){
		Error_Handler();
	}
	if  (HAL_UART_Receive_DMA(&huart3, &RxRFGetByteDMA, 1) != HAL_OK){
		Error_Handler();
	}
	uart1_rx_index = 0;
	uart2_rx_index = 0;	
	uart3_rx_index = 0;	
	
	VTimer_UARTInternalTimeout = VTimerGetID();
}

void MX_USART1_UART_Init(void){
	UartWiFiHandle.Instance = USART1;
	UartWiFiHandle.Init.BaudRate = 115200;
	UartWiFiHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartWiFiHandle.Init.StopBits = UART_STOPBITS_1;
	UartWiFiHandle.Init.Parity = UART_PARITY_NONE;
	UartWiFiHandle.Init.Mode = UART_MODE_TX_RX;
	UartWiFiHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartWiFiHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&UartWiFiHandle) != HAL_OK){
		Error_Handler();
	}

}
void MX_USART2_UART_Init(void){
	UartDebug.Instance = USART2;
	UartDebug.Init.BaudRate = 115200;
	UartDebug.Init.WordLength = UART_WORDLENGTH_8B;
	UartDebug.Init.StopBits = UART_STOPBITS_1;
	UartDebug.Init.Parity = UART_PARITY_NONE;
	UartDebug.Init.Mode = UART_MODE_TX_RX;
	UartDebug.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartDebug.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&UartDebug) != HAL_OK){
		Error_Handler();
	}
}
void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }

}
void UART_SendByte(UART_HandleTypeDef UARTx,uint8_t _value){
	VTimerSet(VTimer_UARTInternalTimeout,10);
	while (HAL_UART_GetState(&UARTx) == HAL_UART_STATE_BUSY_TX){
		if (VTimerIsFired(VTimer_UARTInternalTimeout)){
			break;
		}
	}
	HAL_UART_Transmit(&UARTx,&_value,1,0xFFFF);
}
void UART_SendString(UART_HandleTypeDef UARTx,uint8_t* str){
	VTimerSet(VTimer_UARTInternalTimeout,10);
	while (HAL_UART_GetState(&UARTx) == HAL_UART_STATE_BUSY_TX){
		if (VTimerIsFired(VTimer_UARTInternalTimeout)){
			break;
		}
	}
	HAL_UART_Transmit(&UARTx,str,strlen(str),0xFFFF);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == UART_WIFI){		//DEBUG
		uart1_rx_buffer[uart1_rx_index++] = RxWifiGetByteDMA;		
		if(uart1_rx_index >= UART1_RX_BUFFER_SIZE){
			uart1_rx_index = 0;
		}
		
	} 
	if(huart->Instance == UART_DEBUG){		//ESP8266
		uart2_rx_buffer[uart2_rx_index++] = RxDebugGetByteDMA;		
		if(uart2_rx_index >= UART2_RX_BUFFER_SIZE){
			uart2_rx_index = 0;
		}
	}
	if(huart->Instance == UART_RF){		//RF433
		uart3_rx_buffer[uart3_rx_index++] = RxRFGetByteDMA;		
		if(uart3_rx_index >= UART3_RX_BUFFER_SIZE){
			uart3_rx_index = 0;
		}
	}

}

void ClearUart2RxIndex(){
	uart2_rx_index = 0;
}
uint8_t* GetBufferPointer(UART_HandleTypeDef* UARTx){
	if (UARTx->Instance == USART1 ){
		return uart1_rx_buffer;
	}
	else if (UARTx->Instance == USART2){
		return uart2_rx_buffer;
	}
	else if (UARTx->Instance == USART3){
		return uart3_rx_buffer;
	}

}

uint8_t GetRxBufferIndex(UART_HandleTypeDef* UARTx){
	if (UARTx->Instance == USART1 ){
		return uart1_rx_index;
	}
	else if (UARTx->Instance == USART2){
		return uart2_rx_index;
	}	
	else if (UARTx->Instance == USART3){
		return uart3_rx_index;
	}

}

uint8_t RxBufferAvailable(UART_HandleTypeDef* UARTx){
	if (GetRxBufferIndex(UARTx) > 0){
		return 1;
	}
	else return 0;
}

uint8_t GetReceiveByte(UART_HandleTypeDef* UARTx){
	uint8_t data = 0;
	if (UARTx->Instance == USART1 ){		
		if  (uart1_rx_index > 0){
			data = uart1_rx_buffer[uart1_rx_index];
			uart1_rx_index--;
		}
	}
	else if (UARTx->Instance == USART2){
		if  (uart2_rx_index > 0){
			data = uart2_rx_buffer[uart2_rx_index];
			uart2_rx_index--;
		}
	}
	else if (UARTx->Instance == USART3){
		if	(uart3_rx_index > 0){
			data = uart3_rx_buffer[uart3_rx_index];
			uart3_rx_index--;
			}

	}
	return data;
}
void ClearUart1RxIndex()
{
	uart1_rx_index = 0;
}

void ClearUart3RxIndex()
{
	uart3_rx_index = 0;
}


void UART_Test(void)
{
	
		 printf("UART2");
		 UART_SendString(UartWiFiHandle,"UART1");
		 UART_SendString(huart3,"UART3");
}
