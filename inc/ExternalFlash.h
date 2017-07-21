#ifndef _EXT_FLASH
#define _EXT_FLASH

#include "stm32f1xx_hal.h"
#include "CombrosLib.h"

#define GPIO_FLASH_CE_PIN			GPIO_PIN_6	
#define GPIO_FLASH_CE_PORT		GPIOB

#define EXTFLASH_CMD_WRITE          	0x02  /*!< Write to Memory instruction */
#define EXTFLASH_CMD_EWSR             0x50  /*!< Enable Write Status Register instruction */
#define EXTFLASH_CMD_WRSR           	0x01  /*!< Write Status Register instruction */
#define EXTFLASH_CMD_WREN           	0x06  /*!< Write enable instruction */
#define EXTFLASH_CMD_WRDIS         	0x04  /*!< Write disable instruction */

#define EXTFLASH_CMD_READ           	0x03  /*!< Read from Memory instruction 25Mhz*/
#define EXTFLASH_CMD_READHS		0x0B  /*!< High Speed Read from Memory instruction 80Mhz*/
#define EXTFLASH_CMD_RDSR            	0x05  /*!< Read Status Register instruction  */
#define EXTFLASH_CMD_RDID             	0x90  /*!< Read identification */
#define EXTFLASH_CMD_JEDECID        	0x9F  /*!< JEDEC ID read instruction */

#define EXTFLASH_CMD_SE                  	0x20  /*!< Sector Erase 4KByte instruction */
#define EXTFLASH_CMD_BE_32			0x52  /*!< Erase 32KByte block of memory array instruction */				
#define EXTFLASH_CMD_BE_64			0xD8  /*!< Erase 32KByte block of memory array instruction */				
#define EXTFLASH_CMD_FE                  	0x60  /*!< Erase Full Memory Array instruction */
#define EXTFLASH_CMD_AAI                	0xAD  /*!< Auto Address Increment instruction */

#define EXTFLASH_BUSY_FLAG           	0x01  	/*!< Write In Progress (BUSY) flag */  /*1: BUSY ; 0: FREE*/
#define EXTFLASH_WEL_FLAG			0x02	 /*!< Write Enable Latch (WEL) flag */   	/*1: Write Enable ; 0: Not accept any memory Write*/
#define EXTFLASH_BP_0_FLAG			0x04	//Indicate current level of block write protection
#define EXTFLASH_BP_1_FLAG			0x08	//Indicate current level of block write protection
#define EXTFLASH_BP_2_FLAG			0x10	//Indicate current level of block write protection
#define EXTFLASH_BP_3_FLAG			0x20	//Indicate current level of block write protection
#define EXTFLASH_AAI_FLAG			0x40	/*!< Auto Address Increment (AAI) flag */   /*1: AAI program mode ; 0: Byte-program mode, default = 0*/
#define EXTFLASH_BPL_FLAG			0x80	//  1 = BP3, BP2, BP1, BP0 are read-only bits , 0 =  BP3, BP2, BP1, BP0 are read/writable

#define EXTFLASH_DUMMY_BYTE         	0xFF

#define EXTFLASH_SECTORSIZE		0x1000	//user define
#define EXTFLASH_32KBLOCKSIZE		0x8000	//user define
#define EXTFLASH_64KBLOCKSIZE		0x10000	//user define

#define EXTFLASH_SST25VF040B_ID         	0xBF258D
#define EXTFLASH_SST25VF016B_ID         	0xBF2541

#define   EXTFLASH_OK         0x00
#define   EXTFLASH_ERROR      0x01

void EXTFlash_Init(void);
void EXTFlash_Deinit(void);
void SPI_ExternalFlash_Active(void);
void SPI_ExternalFlash_DeActive(void);
void EXTFlash_WriteEnable(void);
void EXTFlash_WriteDisable(void);
void EXTFlash_Unprotect(void);
void EXTFlash_WriteByte(uint8_t Data, uint32_t WriteAddr);
void EXTFlash_WriteData(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void EXTFlash_ReadData(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
void EXTFlash_EraseSector(uint32_t Addr);
void EXTFlash_Erase32KBlock(uint32_t Addr);
void EXTFlash_Erase64KBlock(uint32_t Addr);
void EXTFlash_EraseAll(void);
uint32_t EXTFlash_ReadID(void);
uint8_t EXTFlash_SelfTest(void);

#endif

