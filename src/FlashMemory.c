/**
  ******************************************************************************
  * @file    FlashMemory.c
  * @author  Combros
  * @version 1.0
  * @date    10/18/2017 4:26:14 PM
  * @brief   $brief$     
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "FlashMemory.h"
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t FlashBufferTemp[100];
uint8_t	Flash_Buffer_Temp[FLASH_PAGE_SIZE+1];

/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void FLASH_Init()
{
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();

  /* Clear all FLASH flags */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
  /* Unlock the Program memory */
  HAL_FLASH_Lock();
}

uint8_t FLASH_WriteBuffer(uint32_t Addr, uint8_t* pBuffer, uint16_t Length)
{
  uint8_t ret_val = 0;
  uint32_t AddressTemp;
  uint32_t DataTemp;
  uint8_t DataBuffer[4];
  uint16_t Index = 0;

  AddressTemp = Addr;
  
  HAL_FLASH_Unlock();
  while (Index < Length)
  {
    for ( uint8_t i = 0; i < 4; i++ )
    {
      if (Index < Length)
      {
        DataBuffer[i] = pBuffer[Index++];
      }
      else
      {
        DataBuffer[i] = 0xFF;
      }
    }

    DataTemp = DataBuffer[3];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[2];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[1];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[0];
    
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, AddressTemp, DataTemp) == HAL_OK)
    {
      AddressTemp = AddressTemp + 4;
    }
    else
    {
      return ret_val;
    }
    DataTemp = 0; 
  }
 
  HAL_FLASH_Lock();
  return 1;
}

uint8_t FLASH_WriteData(uint32_t Addr, uint8_t* pData, uint16_t Length)
{
  uint8_t   ret_val = 0;
  uint32_t  AddressTemp;
  uint32_t  DataTemp;
  uint8_t   DataBuffer[4];
  uint16_t  DataLength;
  uint16_t  Index = 0;

  AddressTemp = Addr;
  DataLength  = strlen(pData);
  
  HAL_FLASH_Unlock();
  while (Index < Length)
  {
    for ( uint8_t i = 0; i < 4; i++ )
    {
      if (Index < Length)
      {
        if (Index < DataLength)
        {
          DataBuffer[i] = pData[Index];
        }
        else
        {
          DataBuffer[i] = 0x00;
        }
      }
      else
      {
        DataBuffer[i] = 0xFF;
      }
      Index++;
    }

    DataTemp = DataBuffer[3];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[2];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[1];
    DataTemp = DataTemp<<8;
    DataTemp |= DataBuffer[0];
    
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, AddressTemp, DataTemp) == HAL_OK)
    {
      AddressTemp = AddressTemp + 4;
    }
    else
    {
      return ret_val;
    }
    DataTemp = 0; 
  }
 
  HAL_FLASH_Lock();
  return 1;

}

uint8_t* FLASH_ReadFromAddr(uint32_t Addr, uint8_t* pData, uint16_t Length)
{ 
	uint32_t Address = 0;	
	uint32_t datatemp = 0;
	Address = Addr;
  uint8_t word[4];
  
	for (uint16_t i = 0; i < Length; i = i + 4)
  {
		datatemp = *(__IO uint32_t*)Address;

    word[0] = datatemp & 0xFF;
    *(pData + i) = word[0];

    word[1] = (datatemp>>8) & 0xFF;
    *(pData + i + 1) = word[1];

    word[2] = (datatemp>>16) & 0xFF;
    *(pData + i + 2) = word[2];
    
		word[3] = (datatemp>>24) & 0xFF;
    *(pData + i + 3) = word[3];

		Address += 4;

    //HAL_UART_Transmit(&UartDebug, word, 4, 10);
	}	
	//printf("\r\n");
  return pData;
}

uint32_t FLASH_EraseFrom(uint32_t start, uint8_t num_page)
{
	uint32_t PageError = 0;
	FLASH_EraseInitTypeDef pEraseInit;
	HAL_StatusTypeDef status = HAL_OK;

	/* Unlock the Flash to enable the flash control register access *************/ 
	HAL_FLASH_Unlock();

	/* Get the sector where start the user flash area */

	pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	pEraseInit.PageAddress = start;
	pEraseInit.Banks = FLASH_BANK_1;
	pEraseInit.NbPages = num_page;
	status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	if (status != HAL_OK){
		/* Error occurred while page erase */
		return 0;
	}

  return 1;

}

uint8_t	FLASH_WriteReplace(uint32_t Addr, uint8_t* pData, uint16_t Length)
{
	uint32_t PageAddr;
	uint16_t startIndex;
	uint16_t dataLength;

	PageAddr = Addr - (Addr % FLASH_PAGE_SIZE);
	startIndex = Addr % FLASH_PAGE_SIZE;
	dataLength = strlen(pData);
	
//	printf("Write replace at PageAddr: 0x0%X | start at index: %d\r\n", PageAddr, startIndex);
	FLASH_ReadFromAddr(PageAddr, Flash_Buffer_Temp, FLASH_PAGE_SIZE);

	for (uint16_t i = 0; i < Length; i++)
	{
		if (i < dataLength)
		{
			Flash_Buffer_Temp[startIndex + i] = *(pData++);
		}
		else
		{
			Flash_Buffer_Temp[startIndex + i] = 0x00;
		}
	}
	FLASH_EraseFrom(PageAddr, 1);
	FLASH_WriteBuffer(PageAddr, Flash_Buffer_Temp, FLASH_PAGE_SIZE);
	
	return 0;
}

void FLASH_WriteBKUP(uint32_t BackupRegister, uint32_t Data)
{
  uint32_t tmp = 0U;

  /* Check the parameters */
  assert_param(IS_RTC_BKP(BackupRegister));
  
  tmp = (uint32_t)BKP_BASE; 
  tmp += (BackupRegister * 4U);

  *(__IO uint32_t *) tmp = (Data & BKP_DR1_D);
}

/**
  * @brief  Reads data from the specified RTC Backup data Register.
  * @param  hrtc: pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC. 
  * @param  BackupRegister: RTC Backup data Register number.
  *          This parameter can be: RTC_BKP_DRx where x can be from 1 to 10 (or 42) to 
  *                                 specify the register (depending devices).
  * @retval Read value
  */
uint32_t FLASH_ReadBKUP(uint32_t BackupRegister)
{
  uint32_t backupregister = 0U;
  uint32_t pvalue = 0U;

  /* Check the parameters */
  assert_param(IS_RTC_BKP(BackupRegister));

  backupregister = (uint32_t)BKP_BASE; 
  backupregister += (BackupRegister * 4U);
  
  pvalue = (*(__IO uint32_t *)(backupregister)) & BKP_DR1_D;

  /* Read the specified register */
  return pvalue;
}


