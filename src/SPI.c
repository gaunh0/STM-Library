#include "SPI.h"

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_rx;

extern void Error_Handler(void);
void MX_SPI1_Init(){
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	if (HAL_SPI_Init(&hspi1) != HAL_OK){		
		Error_Handler();
	}
}

void MX_SPI1_DeInit(){
	if (HAL_SPI_DeInit(&hspi1)  != HAL_OK){
		Error_Handler();
	}
}
void MX_SPI1_Default(){
	__HAL_AFIO_REMAP_SPI1_DISABLE();
}

void MX_SPI1_Remap(){
	__HAL_AFIO_REMAP_SPI1_ENABLE();
}
uint8_t SPI_SendData(uint8_t* _data,uint8_t length){
	uint8_t ret_val = 1;
	if (HAL_SPI_Transmit(&hspi1,_data,length,SPI_TIMEOUT) != HAL_OK){
		ret_val = 0;
	}
	return ret_val;
	
}
	
uint8_t  SPI_SendByte(uint8_t _value){
	uint8_t ret_val = 1;
	if (HAL_SPI_Transmit(&hspi1,&_value,1,SPI_TIMEOUT) != HAL_OK){
		ret_val = 0;
	}
	return ret_val;	
}

uint8_t  SPI_TransceiverByte(uint8_t _value){
	uint8_t ret_val =1;
	if (HAL_SPI_TransmitReceive(&hspi1,&_value,&ret_val,1,SPI_TIMEOUT)!=HAL_OK){
		ret_val = 0;
	}
	return ret_val;	
	
}

