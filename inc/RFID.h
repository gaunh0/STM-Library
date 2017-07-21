#ifndef _RFID_READER
#define _RFID_READER

#include "stm32f1xx_hal.h"
#include "main.h"


#define	MAX_LEN               16        // Maximum length of an array.

#define 	RFID_NSS_PIN                    	GPIO_PIN_4
#define 	RFID_NSS_PORT              		GPIOA 

#define 	RFID_RESET_PIN                    	GPIO_PIN_0
#define 	RFID_RESET_PORT              	GPIOA 

#define 	RFID_IRQ_PIN                    	GPIO_PIN_1
#define 	RFID_IRQ_PORT              		GPIOA 


#define     MAX_LEN               16        // Maximum length of an array.

// MF522 MFRC522 error codes.
#define     MI_OK                 0         // Everything A-OK.
#define     MI_NOTAGERR           1         // No tag error
#define     MI_ERR                2         // General error 

// MF522 Command word
#define     MFRC522_IDLE          0x00      // NO action; Cancel the current command
#define     MFRC522_MEM           0x01      // Store 25 byte into the internal buffer.
#define     MFRC522_GENID         0x02      // Generates a 10 byte random ID number.
#define     MFRC522_CALCCRC       0x03      // CRC Calculate or selftest.
#define     MFRC522_TRANSMIT      0x04      // Transmit data
#define     MFRC522_NOCMDCH       0x07      // No command change.
#define     MFRC522_RECEIVE       0x08      // Receive Data
#define     MFRC522_TRANSCEIVE    0x0C      // Transmit and receive data,
#define     MFRC522_AUTHENT       0x0E      // Authentication Key
#define     MFRC522_SOFTRESET     0x0F      // Reset

// Mifare_One tag command word
#define     MF1_REQIDL            0x26      // find the antenna area does not enter hibernation
#define     MF1_REQALL            0x52      // find all the tags antenna area
#define     MF1_ANTICOLL          0x93      // anti-collision
#define     MF1_SELECTTAG         0x93      // election tag
#define     MF1_AUTHENT1A         0x60      // authentication key A
#define     MF1_AUTHENT1B         0x61      // authentication key B
#define     MF1_READ              0x30      // Read Block
#define     MF1_WRITE             0xA0      // write block
#define     MF1_DECREMENT         0xC0      // debit
#define     MF1_INCREMENT         0xC1      // recharge
#define     MF1_RESTORE           0xC2      // transfer block data to the buffer
#define     MF1_TRANSFER          0xB0      // save the data in the buffer
#define     MF1_HALT              0x50      // Sleep


//------------------ PN512 registers---------------
//Page 0:Command and Status
#define     Reserved00            0x00
#define     CommandReg            0x01
#define     CommIEnReg            0x02
#define     DivIEnReg             0x03
#define     CommIrqReg            0x04
#define     DivIrqReg             0x05
#define     ErrorReg              0x06
#define     Status1Reg            0x07
#define     Status2Reg            0x08
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     Reserved01            0x0F
//Page 1:Command
#define     Reserved10            0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     FelNFC1Reg            0x1A
#define     FelNFC2Reg            0x1B
#define     MifNFC            	 	0x1C
#define     ManualRCVReg            0x1D
#define     TypeBReg            0x1E
#define     SerialSpeedReg        0x1F
//Page 2:CFG
#define     Reserved20            0x20
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     GsNOffReg            0x23
#define     ModWidthReg           0x24
#define     TxBitPhaseReg            0x25
#define     RFCfgReg              0x26
#define     GsNOnReg                0x27
#define     CWGsPReg              0x28
#define     ModGsPReg             0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
//Page 3:TestRegister
#define     Reserved30            0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39
#define     TestDAC2Reg           0x3A
#define     TestADCReg            0x3B
#define     Reserved31            0x3C
#define     Reserved32            0x3D
#define     Reserved33            0x3E
#define     Reserved34            0x3F
//-----------------------------------------------

// Commands sent to the PICC.
enum PICC_Command {
	// The commands used by the PCD to manage communication with several PICCs (ISO 14443-3, Type A, section 6.4)
	PICC_CMD_REQA			= 0x26,		// REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
	PICC_CMD_WUPA		= 0x52,		// Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
	PICC_CMD_CT			= 0x88,		// Cascade Tag. Not really a command, but used during anti collision.
	PICC_CMD_SEL_CL1		= 0x93,		// Anti collision/Select, Cascade Level 1
	PICC_CMD_SEL_CL2		= 0x95,		// Anti collision/Select, Cascade Level 2
	PICC_CMD_SEL_CL3		= 0x97,		// Anti collision/Select, Cascade Level 3
	PICC_CMD_HLTA			= 0x50,		// HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.
	// The commands used for MIFARE Classic (from http://www.nxp.com/documents/data_sheet/MF1S503x.pdf, Section 9)
	// Use PCD_MFAuthent to authenticate access to a sector, then use these commands to read/write/modify the blocks on the sector.
	// The read/write commands can also be used for MIFARE Ultralight.
	PICC_CMD_MF_AUTH_KEY_A	= 0x60,		// Perform authentication with Key A
	PICC_CMD_MF_AUTH_KEY_B	= 0x61,		// Perform authentication with Key B
	PICC_CMD_MF_READ			= 0x30,		// Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
	PICC_CMD_MF_WRITE		= 0xA0,		// Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
	PICC_CMD_MF_DECREMENT	= 0xC0,		// Decrements the contents of a block and stores the result in the internal data register.
	PICC_CMD_MF_INCREMENT	= 0xC1,		// Increments the contents of a block and stores the result in the internal data register.
	PICC_CMD_MF_RESTORE		= 0xC2,		// Reads the contents of a block into the internal data register.
	PICC_CMD_MF_TRANSFER		= 0xB0,		// Writes the contents of the internal data register to a block.
	// The commands used for MIFARE Ultralight (from http://www.nxp.com/documents/data_sheet/MF0ICU1.pdf, Section 8.6)
	// The PICC_CMD_MF_READ and PICC_CMD_MF_WRITE can also be used for MIFARE Ultralight.
	PICC_CMD_UL_WRITE		= 0xA2		// Writes one 4 byte page to the PICC.
};


// MIFARE constants that does not fit anywhere else
enum MIFARE_Misc {
	MF_ACK					= 0xA,		// The MIFARE Classic uses a 4 bit ACK/NAK. Any other value than 0xA is NAK.
	MF_KEY_SIZE				= 6			// A Mifare Crypto1 key is 6 bytes.
};

// PICC types we can detect. Remember to update PICC_GetTypeName() if you add more.
enum PICC_Type {
	PICC_TYPE_UNKNOWN		= 0,
	PICC_TYPE_ISO_14443_4		= 1,	// PICC compliant with ISO/IEC 14443-4 
	PICC_TYPE_ISO_18092		= 2, 	// PICC compliant with ISO/IEC 18092 (NFC)
	PICC_TYPE_MIFARE_MINI		= 3,	// MIFARE Classic protocol, 320 bytes
	PICC_TYPE_MIFARE_1K		= 4,	// MIFARE Classic protocol, 1KB
	PICC_TYPE_MIFARE_4K		= 5,	// MIFARE Classic protocol, 4KB
	PICC_TYPE_MIFARE_UL		= 6,	// MIFARE Ultralight or Ultralight C
	PICC_TYPE_MIFARE_PLUS		= 7,	// MIFARE Plus
	PICC_TYPE_TNP3XXX			= 8,	// Only mentioned in NXP AN 10833 MIFARE Type Identification Procedure
	PICC_TYPE_NOT_COMPLETE	= 255	// SAK indicates UID is not complete.
};

// Return codes from the functions in this class. Remember to update GetStatusCodeName() if you add more.
enum StatusCode {
	STATUS_OK					= 1,	// Success
	STATUS_ERROR				= 2,	// Error in communication
	STATUS_COLLISION			= 3,	// Collission detected
	STATUS_TIMEOUT				= 4,	// Timeout in communication.
	STATUS_NO_ROOM			= 5,	// A buffer is not big enough.
	STATUS_INTERNAL_ERROR		= 6,	// Internal error in the code. Should not happen ;
	STATUS_INVALID				= 7,	// Invalid argument.
	STATUS_CRC_WRONG			= 8,	// The CRC_A does not match
	STATUS_MIFARE_NACK		= 9		// A MIFARE PICC responded with NAK.
};

// Control bits to enable and disable the passing of interrupt requests.
// ComIEnReg register bit
#define IRqInv		0x80
#define TXIEn		0x40
#define RXIEn		0x20
#define IdleIEn		0x10
#define HiAlertIEn		0x08
#define LoAlertIEn	0x04
#define ErrIEn		0x02
#define TimerIEn		0x01

void SPIInitController(void);
void RFID_InitController(void);
void RFIDCSHigh(void);
void RFIDCSLow(void);
void RFIDResetHigh(void);
void RFIDResetLow(void);

uint16_t PCD_HaltTag(void);
void PCD_AntennaOn(void);
void PCD_AntennaOff(void); 
void PCD_SoftPowerDown(void);
void PCD_Wakeup(void);
void PCD_SoftReset(void);
void PCD_SelfTest(void);
uint8_t  SPI_TransceiverByte(uint8_t _value);
void PCD_WriteByteToRegister(uint8_t reg,uint8_t value);
uint8_t PCD_ReadByteFromRegister(uint8_t reg);
void PCD_SetRegisterBitMask(	uint8_t reg,uint8_t mask);
void PCD_ClearRegisterBitMask(	uint8_t reg,uint8_t mask);


void PCD_GetFirmwareVersion(void) ;
uint16_t PCD_CommandTag(uint8_t cmd, uint8_t *data, uint16_t dlen, uint8_t *result, uint16_t *rlen) ;
void PCD_CalculateCRC(uint8_t *data, int len, uint8_t *result) ;
uint16_t PCD_RequestTag(uint8_t mode, uint8_t *data);
uint16_t PCD_AntiCollision(uint8_t *serial);
uint8_t PCD_SelectTag(uint8_t *serial);
uint16_t PCD_HaltTag(void);
void PCD_CalculateCRC(uint8_t *data, int len, uint8_t *result);
uint16_t PCD_Authenticate(uint8_t mode, uint8_t block, uint8_t *key, uint8_t *serial) ;
uint16_t PCD_ReadFromTag(uint8_t block, uint8_t *result) ;
uint16_t PCD_WriteToTag(uint8_t block, uint8_t *data);
#endif

