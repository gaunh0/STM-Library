#include "ExternalFlash.h"
#include "SPI.h"

void EXTFlash_Init(){
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.Pin = GPIO_FLASH_CE_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIO_FLASH_CE_PORT, &GPIO_InitStructure);

	SPI_ExternalFlash_Active();
	EXTFlash_Unprotect();	
}

void EXTFlash_Deinit(){
	HAL_GPIO_DeInit(GPIO_FLASH_CE_PORT,GPIO_FLASH_CE_PIN);	
	SPI_ExternalFlash_DeActive();
}

void SPI_ExternalFlash_DeActive(){
	MX_SPI1_Default();
}
void SPI_ExternalFlash_Active(){
	MX_SPI1_Remap();
}
void EXTFlash_CE_High(){
	HAL_GPIO_WritePin(GPIO_FLASH_CE_PORT,GPIO_FLASH_CE_PIN,GPIO_PIN_SET);
}
void EXTFlash_CE_Low(){
	HAL_GPIO_WritePin(GPIO_FLASH_CE_PORT,GPIO_FLASH_CE_PIN,GPIO_PIN_RESET);
}

/**
  * @brief  Enables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void EXTFlash_WriteEnable(void){
	uint8_t flashstatus = 0;
	EXTFlash_CE_Low();
	/*!< Send "Write Enable" instruction */
	SPI_SendByte(EXTFLASH_CMD_WREN);
	EXTFlash_CE_High();
		
	EXTFlash_CE_Low();
	do {
		SPI_SendByte(EXTFLASH_CMD_RDSR);
		flashstatus =SPI_TransceiverByte(EXTFLASH_DUMMY_BYTE);
	} while ((flashstatus & EXTFLASH_WEL_FLAG) != EXTFLASH_WEL_FLAG);
	
	/*!< Deselect the FLASH: Chip Select high */
	EXTFlash_CE_High();
}

/**
  * @brief  Disables the write access to the FLASH.
  * @param  None
  * @retval None
  */
void EXTFlash_WriteDisable(void){
	uint8_t flashstatus = 0;
	EXTFlash_CE_Low();
	/*!< Send "Write Enable" instruction */
	SPI_SendByte(EXTFLASH_CMD_WRDIS);
	EXTFlash_CE_High();
	
	EXTFlash_CE_Low();
	// Check WE Flag = 0
	do {
		SPI_SendByte(EXTFLASH_CMD_RDSR);
		flashstatus =SPI_TransceiverByte(EXTFLASH_DUMMY_BYTE);
	} while ((flashstatus & EXTFLASH_WEL_FLAG)  == EXTFLASH_WEL_FLAG);
	
	/*!< Deselect the FLASH: Chip Select high */
	EXTFlash_CE_High();
}


//------------ unprotectNVM ------------------
// Disable the write protection from the NVM
void EXTFlash_Unprotect(void){
	uint8_t temp[2];
	// Enable Write
	EXTFlash_CE_Low();
	EXTFlash_WriteEnable();
	EXTFlash_CE_High();        

	// Unprotect Memory
	EXTFlash_CE_Low();
	temp[0] = EXTFLASH_CMD_WRSR;
	temp[1] = 0x00;
	SPI_SendData(temp,2);
	EXTFlash_CE_High();
}

void EXTFlash_WaitUtilDone(){
	uint8_t StatusData=0;
	while (1){
		EXTFlash_CE_Low();
	
		SPI_SendByte(EXTFLASH_CMD_RDSR);
		StatusData = SPI_TransceiverByte(EXTFLASH_DUMMY_BYTE);
		EXTFlash_CE_High();
		DelayMs(1);
		if ( (StatusData & EXTFLASH_BUSY_FLAG) == 0x00) break;		
	}

}

void EXTFlash_WriteByte(uint8_t Data, uint32_t WriteAddr){
	/*!< Enable the write access to the FLASH */
	EXTFlash_WriteEnable();

	/*!< Select the FLASH: Chip Select low */
	EXTFlash_CE_Low();
	/*!< Send "Write to Memory " instruction */
	SPI_SendByte(EXTFLASH_CMD_WRITE);
	/*!< Send WriteAddr high nibble address byte to write to */
	SPI_SendByte((WriteAddr & 0xFF0000) >> 16);
	/*!< Send SPI_SendByte medium nibble address byte to write to */
	SPI_SendByte((WriteAddr & 0xFF00) >> 8);
	/*!< Send WriteAddr low nibble address byte to write to */
	SPI_SendByte(WriteAddr & 0xFF);

	/*!< while there is data to be written on the FLASH */
	SPI_SendData(&Data,1);

	/*!< Deselect the FLASH: Chip Select high */
	EXTFlash_CE_High();

	/*!< Wait the end of Flash writing */
	EXTFlash_WaitUtilDone();
	EXTFlash_WriteDisable();
}

void EXTFlash_WriteData(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite){
	/*!< Enable the write access to the FLASH */
	EXTFlash_WriteEnable();

	/*!< Select the FLASH: Chip Select low */
	EXTFlash_CE_Low();
	/*!< Send "Write to Memory " instruction */
	SPI_SendByte(EXTFLASH_CMD_WRITE);
	/*!< Send WriteAddr high nibble address byte to write to */
	SPI_SendByte((WriteAddr & 0xFF0000) >> 16);
	/*!< Send SPI_SendByte medium nibble address byte to write to */
	SPI_SendByte((WriteAddr & 0xFF00) >> 8);
	/*!< Send WriteAddr low nibble address byte to write to */
	SPI_SendByte(WriteAddr & 0xFF);

	/*!< while there is data to be written on the FLASH */
	 while(NumByteToWrite>0){
		SPI_SendByte(*pBuffer);
		pBuffer++;
		NumByteToWrite--;		
	 }
	/*!< Deselect the FLASH: Chip Select high */
	EXTFlash_CE_High();

	/*!< Wait the end of Flash writing */
	EXTFlash_WaitUtilDone();
	EXTFlash_WriteDisable();
}

void EXTFlash_ReadData(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead){
	/*!< Select the FLASH: Chip Select low */
	EXTFlash_CE_Low();
	/*!< Send "Read from Memory 25Mhz " instruction */
	SPI_SendByte(EXTFLASH_CMD_READ);
	/*!< Send ReadAddr high nibble address byte to read from */
	SPI_SendByte((ReadAddr & 0xFF0000) >> 16);
	/*!< Send ReadAddr middle nibble address byte to read from */
	SPI_SendByte((ReadAddr & 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte to read from */
	SPI_SendByte(ReadAddr & 0xFF);

	 while(NumByteToRead>0){
		*pBuffer  = SPI_TransceiverByte(0xFF);
		pBuffer++;
		NumByteToRead--;
        }

	/*!< Deselect the FLASH: Chip Select high */
	EXTFlash_CE_High();
}

/*
The Sector-Erase instruction clears all bits in the
selected 4 KByte sector to 0xFF. 
*/
void EXTFlash_EraseSector(uint32_t Addr){
	uint32_t AddressTemp = 0; 
	AddressTemp = Addr* EXTFLASH_SECTORSIZE;
	EXTFlash_WriteEnable();
	/*!< Select the FLASH: Chip Select low */
	EXTFlash_CE_Low();
	SPI_SendByte(EXTFLASH_CMD_SE);
	/*!< Send ReadAddr high nibble address byte to read from */
	SPI_SendByte((AddressTemp & 0xFF0000) >> 16);
	/*!< Send ReadAddr middle nibble address byte to read from */
	SPI_SendByte((AddressTemp & 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte to read from */
	SPI_SendByte(AddressTemp & 0xFF);

	/*!< Deselect the FLASH: Chip Select high */
	EXTFlash_CE_High();

	EXTFlash_WaitUtilDone();
}

void EXTFlash_Erase32KBlock(uint32_t Addr){
	uint32_t AddressTemp = 0; 
	AddressTemp = Addr* EXTFLASH_32KBLOCKSIZE;
	EXTFlash_WriteEnable();
	/*!< Select the FLASH: Chip Select low */
	EXTFlash_CE_Low();
	SPI_SendByte(EXTFLASH_CMD_BE_32);
	/*!< Send ReadAddr high nibble address byte to read from */
	SPI_SendByte((AddressTemp & 0xFF0000) >> 16);
	/*!< Send ReadAddr middle nibble address byte to read from */
	SPI_SendByte((AddressTemp & 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte to read from */
	SPI_SendByte(AddressTemp & 0xFF);

	/*!< Deselect the FLASH: Chip Select high */
	EXTFlash_CE_High();

	EXTFlash_WaitUtilDone();
}

void EXTFlash_Erase64KBlock(uint32_t Addr){
	uint32_t AddressTemp = 0; 
	AddressTemp = Addr* EXTFLASH_64KBLOCKSIZE;
	EXTFlash_WriteEnable();
	/*!< Select the FLASH: Chip Select low */
	EXTFlash_CE_Low();
	SPI_SendByte(EXTFLASH_CMD_BE_64);
	/*!< Send ReadAddr high nibble address byte to read from */
	SPI_SendByte((AddressTemp & 0xFF0000) >> 16);
	/*!< Send ReadAddr middle nibble address byte to read from */
	SPI_SendByte((AddressTemp & 0xFF00) >> 8);
	/*!< Send ReadAddr low nibble address byte to read from */
	SPI_SendByte(AddressTemp & 0xFF);

	/*!< Deselect the FLASH: Chip Select high */
	EXTFlash_CE_High();
	EXTFlash_WaitUtilDone();
}


void EXTFlash_EraseAll(){ 
	EXTFlash_WriteEnable();
	/*!< Select the FLASH: Chip Select low */
	EXTFlash_CE_Low();
	/*!< Send "Erase All " instruction */
	SPI_SendByte(EXTFLASH_CMD_FE);
	/*!< Deselect the FLASH: Chip Select high */
	EXTFlash_CE_High();
	EXTFlash_WaitUtilDone();
} 
/**
  * @brief  Reads FLASH identification.
  * @param  None
  * @retval FLASH identification
  */
uint32_t EXTFlash_ReadID(void){
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	EXTFlash_CE_Low();
	/*!< Send "RDID " instruction */
	SPI_SendByte(EXTFLASH_CMD_JEDECID);
	/*!< Read a byte from the FLASH */
	Temp0 = SPI_TransceiverByte(EXTFLASH_DUMMY_BYTE);
	/*!< Read a byte from the FLASH */
	Temp1 = SPI_TransceiverByte(EXTFLASH_DUMMY_BYTE);
	/*!< Read a byte from the FLASH */
	Temp2 = SPI_TransceiverByte(EXTFLASH_DUMMY_BYTE);
	/*!< Deselect the FLASH: Chip Select high */
	EXTFlash_CE_High();

	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
	return Temp;
}

uint8_t EXTFlash_SelfTest(){
	uint8_t ret_val = 0;
	if (EXTFlash_ReadID() == EXTFLASH_SST25VF040B_ID){
		printf("Extenal Flash SST25VF040B Init OK\r\n");    
		ret_val =1;		
	}
	else {
		printf("Extenal Flash Init FAIL\r\n");
		ret_val =0;		
	}
	return ret_val;
}

