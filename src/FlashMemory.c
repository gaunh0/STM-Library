#include "FlashMemory.h"

#define PAGE_START			44
#define CARD_DATA_SIZE		8
uint8_t BufferTemp[FLASH_PAGE_SIZE];

FLASHStatus SaveCardDataToFlash(CombrosCardData Data){
	/*Variable used for Erase procedure*/
	FLASH_EraseInitTypeDef EraseInitStruct;
	
	uint32_t Address = 0, PAGEError = 0;
	uint32_t DataTemp = 0;	
	uint16_t PageIndex = 0;
	uint32_t PageDataAddress = 0;	
	uint8_t *ptBufferTemp;
	FLASHStatus MemoryProgramStatus = PASSED;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();
	PageIndex = PAGE_START + (Data.UserIndex /CARD_DATA_SIZE) ;
	PageDataAddress = (uint32_t)ADDR_FLASH_PAGE_0 +(FLASH_PAGE_SIZE*(PageIndex-1));
	// Read data from FLASH to buffer Temp
	
//	printf("\r\n PageIndex: %x ",PageIndex);
	Address = PageDataAddress;
//	printf("\r\n Address: %x ",Address);

	ptBufferTemp = BufferTemp;
	while (Address < (PageDataAddress + FLASH_PAGE_SIZE)){
		DataTemp = *(__IO uint32_t*)Address;		
		*ptBufferTemp =  (DataTemp>>24) &0xFF;
		*ptBufferTemp++ =  (DataTemp>>16) &0xFF;
		*ptBufferTemp++ =  (DataTemp>>8) &0xFF;
		*ptBufferTemp++ =  DataTemp &0xFF;
		Address +=4;
		*ptBufferTemp +=4;
	}
	/*for (i = 0;i<1024;i++){
		printf("%x",BufferTemp[i]);		
	}*/
	
	// Add new data to Buffer
	uint16_t index_temp;
	
		// CARD_DATA_SIZE = 8
		// PAGE_SIZE = 1024
		// =>  have 128 blocks data in one page
	index_temp = (Data.UserIndex%128)*CARD_DATA_SIZE;
	printf("\r\n index_temp: %x ",index_temp);
	BufferTemp[index_temp++] = Data.TagID[0];
	BufferTemp[index_temp++] = Data.TagID[1];
	BufferTemp[index_temp++] = Data.TagID[2];
	BufferTemp[index_temp++] = Data.TagID[3];

	BufferTemp[index_temp++] = Data.UserActiveFlag;
	BufferTemp[index_temp++] = Data.UserPriority;	
	BufferTemp[index_temp++] = Data.UserIndex>>8;
	BufferTemp[index_temp++] = Data.UserIndex;

//	printf("\r\n PageIndex: %d ",PageIndex);
//	printf("\r\n PageAdd: %x ",Address);


	/* Erase the FLASH pages */	
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase    	= FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = PageDataAddress;
	EraseInitStruct.NbPages     	= 1;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK){
		printf("Flash Erase Error Code = %d",HAL_FLASH_GetError());
		MemoryProgramStatus = FAILED;
		return MemoryProgramStatus;
	}
	
	/* Program Flash Page */
	Address = PageDataAddress;	
	ptBufferTemp = BufferTemp;
	uint8_t ProgramRetry = 0;
	while (Address < (PageDataAddress + FLASH_PAGE_SIZE)){
		DataTemp = *ptBufferTemp<<24;
		DataTemp = DataTemp | ((*ptBufferTemp+1)<<16);
		DataTemp = DataTemp | ((*ptBufferTemp+2)<<8);
		DataTemp = DataTemp | (*ptBufferTemp+3);
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, DataTemp) == HAL_OK){
			Address +=4;	
			ptBufferTemp +=4;
			ProgramRetry = 0;
		}
		else {
			ProgramRetry ++;
			if (ProgramRetry>2){
				MemoryProgramStatus = FAILED;
				return MemoryProgramStatus;
			}
		}
		DataTemp = 0;			
	}	
	return MemoryProgramStatus;
}

FLASHStatus ReadCardDataFromFlash(CombrosCardData* cData, uint16_t CardIndex){	
	uint8_t PageIndex = 0;
	uint32_t Address = 0;	
	uint32_t datatemp = 0;

	PageIndex = (CardIndex /CARD_DATA_SIZE) + PAGE_START;
//	printf("\r\n PageIndex: %x ",PageIndex);
	Address = (uint32_t)ADDR_FLASH_PAGE_0 + (FLASH_PAGE_SIZE*(PageIndex-1)) + (CardIndex%128)*CARD_DATA_SIZE;
//	printf("\r\n Address: %x ",Address);
	// Read TAG_ID from FLASH
	
	datatemp = *(__IO uint32_t*)Address;
	cData->TagID[0] =  (datatemp>>24) &0xFF;
	cData->TagID[1] =  (datatemp>>16) &0xFF;
	cData->TagID[2] =  (datatemp>>8) &0xFF;
	cData->TagID[3] =  datatemp & 0xFF;
	datatemp = *(__IO uint32_t*)(Address+4);
	cData->UserActiveFlag =  (datatemp>>24) &0xFF;
	cData->UserPriority =  (datatemp>>16) &0xFF;	
	cData->UserIndex =  datatemp & 0xFFFF;
}


uint8_t CheckTagIDFromFlash(CombrosCardData Data){	
	uint32_t Address = 0;	
	uint32_t datatemp = 0;
	uint8_t PageIndex = 0;
	uint8_t CardIDTemp[4];
	if (Data.UserIndex > 2000){
		printf("Do not exist User \r\n");
		return 0;
	}
	else {

		// CARD_DATA_SIZE = 8
		// PAGE_SIZE = 1024
		// =>  have 128 blocks data in one page
		PageIndex = (Data.UserIndex /128) + PAGE_START;
		Address = (uint32_t)ADDR_FLASH_PAGE_0 + (FLASH_PAGE_SIZE*(PageIndex-1)) + (Data.UserIndex%128)*CARD_DATA_SIZE;	
		printf("\r\n Address: %x ",Address);
		// Read TAG_ID from FLASH
		datatemp = *(__IO uint32_t*)Address;
		CardIDTemp[0] =  (datatemp>>24) &0xFF;
		CardIDTemp[1] =  (datatemp>>16) &0xFF;
		CardIDTemp[2] =  (datatemp>>8) &0xFF;
		CardIDTemp[3] =  datatemp &0xFF;

		if (memcmp(Data.TagID,CardIDTemp,4) == 0) {
			return 1;
		}
		else return 0;
	}	
}

