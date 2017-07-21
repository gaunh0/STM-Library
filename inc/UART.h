#include "stm32f1xx_hal.h"

#define UART1_RX_BUFFER_SIZE 10
#define UART2_RX_BUFFER_SIZE 250
#define UART3_RX_BUFFER_SIZE 250


#define UART_DEBUG			USART2
#define UART_WIFI			USART1
#define UART_RF				USART3

void UARTInitController(void);
void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void UART_SendByte(UART_HandleTypeDef UARTx,uint8_t _value);
void UART_SendString(UART_HandleTypeDef UARTx,uint8_t* str);


uint8_t GetWifiRxBufferIndex(void);
void ClearUart2RxIndex(void);
void ClearUart1RxIndex(void);
void ClearUart3RxIndex(void);


uint8_t RxBufferAvailable(UART_HandleTypeDef* UARTx);
uint8_t GetReceiveByte(UART_HandleTypeDef* UARTx);
void UART_Test(void);

extern uint8_t uart1_rx_buffer[];
extern uint8_t uart2_rx_buffer[];
extern uint8_t uart3_rx_buffer[];
