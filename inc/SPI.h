#include "stm32f1xx_hal.h"

#define SPI_TIMEOUT			100

void MX_SPI1_Init(void);
void MX_SPI1_DeInit(void);
void MX_SPI1_Default(void);
void MX_SPI1_Remap(void);
uint8_t SPI_SendData(uint8_t* _data,uint8_t length);
uint8_t  SPI_SendByte(uint8_t _value);
uint8_t  SPI_TransceiverByte(uint8_t _value);

