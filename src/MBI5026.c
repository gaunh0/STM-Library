#include "MBI5026.h"

void MBI5026_InitController(){
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.Pin = MBI5026_SDI_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(MBI5026_SDI_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = MBI5026_CLK_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(MBI5026_CLK_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin = MBI5026_LE_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(MBI5026_LE_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = MBI5026_OE_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(MBI5026_OE_PORT, &GPIO_InitStructure);

	MBI5026_SendData(0x0000);
	
}

void MBI5026_SendData(uint16_t data){
	uint8_t bit_index;
	HAL_GPIO_WritePin(MBI5026_OE_PORT,MBI5026_OE_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(MBI5026_LE_PORT,MBI5026_LE_PIN,GPIO_PIN_RESET);
	for (bit_index = 0;bit_index<16;bit_index++){
		HAL_GPIO_WritePin(MBI5026_CLK_PORT,MBI5026_CLK_PIN,GPIO_PIN_RESET);		
		if (((data>>bit_index)&0x0001) == 0){
			HAL_GPIO_WritePin(MBI5026_SDI_PORT,MBI5026_SDI_PIN,GPIO_PIN_RESET);
		}
		else {
			HAL_GPIO_WritePin(MBI5026_SDI_PORT,MBI5026_SDI_PIN,GPIO_PIN_SET);
		}
		//__nop();
		HAL_GPIO_WritePin(MBI5026_CLK_PORT,MBI5026_CLK_PIN,GPIO_PIN_SET);
	}
	HAL_GPIO_WritePin(MBI5026_LE_PORT,MBI5026_LE_PIN,GPIO_PIN_SET);
	HAL_GPIO_WritePin(MBI5026_OE_PORT,MBI5026_OE_PIN,GPIO_PIN_RESET);	
}

void MBI5026_Display(uint8_t _index){
	if (_index == 8){
		MBI5026_SendData(0x0001<<9);
	}
	else if (_index == 9){
		MBI5026_SendData(0x0001<<8);
	}
	else {
		MBI5026_SendData(0x0001<<_index);
	}
}

void MBI5026_Off(uint8_t _index){
	MBI5026_SendData(0xFFFF & (~(1<<_index)));
}


