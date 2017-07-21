#include "RFID.h"
#include "SPI.h"
#include "VTimer.h"

uint8_t VTimer_SPITimeout_ID;

void SPI_RFIDActive(void);

void RFID_InitController(){	
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.Pin = RFID_NSS_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(RFID_NSS_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.Pin = RFID_RESET_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(RFID_RESET_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.Pin = RFID_IRQ_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(RFID_IRQ_PORT, &GPIO_InitStructure);	
	
	SPI_RFIDActive();

	RFIDCSLow();
	RFIDResetHigh();   

	PCD_SelfTest();
	PCD_GetFirmwareVersion();

	// Config IRQ pin output
	PCD_WriteByteToRegister(CommIEnReg, 0x00);	
	PCD_WriteByteToRegister(DivIEnReg, 0x80);	
	
	PCD_WriteByteToRegister(TModeReg, 0x8D);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
	PCD_WriteByteToRegister(TPrescalerReg, 0x3E);	// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25µs.
	PCD_WriteByteToRegister(TReloadRegL, 30);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
	PCD_WriteByteToRegister(TReloadRegH, 0);

	PCD_WriteByteToRegister(TxAutoReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
	PCD_WriteByteToRegister(ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)

	PCD_WriteByteToRegister(RFCfgReg , 0x70); 
	PCD_AntennaOn();
	
	/*PCD_WriteByteToRegister(TModeReg, 0x80);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
	PCD_WriteByteToRegister(TPrescalerReg, 0xA9);		// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25 s.
	PCD_WriteByteToRegister(TReloadRegH, 0x03);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
	PCD_WriteByteToRegister(TReloadRegL, 0xE8);
	
	PCD_WriteByteToRegister(TxAutoReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
	PCD_WriteByteToRegister(ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
	PCD_WriteByteToRegister(RFCfgReg , 0xF0);*/
}

void SPI_RFIDActive(){
	MX_SPI1_Default();
}
void RFIDCSHigh(){
	HAL_GPIO_WritePin(RFID_NSS_PORT, RFID_NSS_PIN,GPIO_PIN_SET); 
}
void RFIDCSLow(){
	HAL_GPIO_WritePin(RFID_NSS_PORT, RFID_NSS_PIN,GPIO_PIN_RESET); 
}

void RFIDResetHigh(void){
	HAL_GPIO_WritePin(RFID_RESET_PORT, RFID_RESET_PIN,GPIO_PIN_SET); 
}
void RFIDResetLow(void){
	HAL_GPIO_WritePin(RFID_RESET_PORT, RFID_RESET_PIN,GPIO_PIN_RESET); 
}

/**
 * Writes a byte to the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */

void PCD_WriteByteToRegister(uint8_t reg,uint8_t value){
	uint8_t temp[2];
	temp[0] = (reg<<1)&0x7E;
	temp[1] = value;
	RFIDCSLow();
	SPI_SendData(temp,2);
	RFIDCSHigh();
}

/**
 * Reads a byte from the specified register in the MFRC522 chip.
 * The interface is described in the datasheet section 8.1.2.
 */

uint8_t PCD_ReadByteFromRegister(uint8_t reg){
	uint8_t value;
	RFIDCSLow();	
	SPI_TransceiverByte(0x80 | ((reg<<1) & 0x7E));	// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	value = SPI_TransceiverByte(0);				// Read the value back. Send 0 to stop reading.
	RFIDCSHigh();						// Release slave again
	return value;
}
/**
 * Sets the bits given in mask in register reg.
 */
void PCD_SetRegisterBitMask(uint8_t reg,uint8_t mask){
	uint8_t tmp;
	tmp = PCD_ReadByteFromRegister(reg);
	PCD_WriteByteToRegister(reg, tmp | mask);			// set bit mask
}

/**
 * Clears the bits given in mask from register reg.
 */
void PCD_ClearRegisterBitMask(uint8_t reg,uint8_t mask){
	uint8_t tmp;
	tmp = PCD_ReadByteFromRegister(reg);
	PCD_WriteByteToRegister(reg, tmp & (~mask));		// clear bit mask
}


void PCD_SoftReset() {
	PCD_WriteByteToRegister(CommandReg, MFRC522_SOFTRESET);	// Issue the SoftReset command.
	VTimerSet(VTimer_SPITimeout_ID,500);
	while (PCD_ReadByteFromRegister(CommandReg) & (1<<4)){
		if (VTimerIsFired(VTimer_SPITimeout_ID)){
			printf("reset fail\r\n");
			break;
		}
	}
} 

uint8_t MRFC522_firmware_referenceV2_0[] = {
	0x00, 0xEB, 0x66, 0xBA, 0x57, 0xBF, 0x23, 0x95,
	0xD0, 0xE3, 0x0D, 0x3D, 0x27, 0x89, 0x5C, 0xDE,
	0x9D, 0x3B, 0xA7, 0x00, 0x21, 0x5B, 0x89, 0x82,
	0x51, 0x3A, 0xEB, 0x02, 0x0C, 0xA5, 0x00, 0x49,
	0x7C, 0x84, 0x4D, 0xB3, 0xCC, 0xD2, 0x1B, 0x81,
	0x5D, 0x48, 0x76, 0xD5, 0x71, 0x61, 0x21, 0xA9,
	0x86, 0x96, 0x83, 0x38, 0xCF, 0x9D, 0x5B, 0x6D,
	0xDC, 0x15, 0xBA, 0x3E, 0x7D, 0x95, 0x3B, 0x2F

};
void PCD_SelfTest(){
	uint8_t i,n=0;
	PCD_SoftReset();

	// Clear the internal buffer by writing 25 bytes of 00h and implement the Config command
	PCD_WriteByteToRegister(FIFODataReg,0);
	PCD_WriteByteToRegister(CommandReg, MFRC522_MEM);
	
	PCD_WriteByteToRegister(AutoTestReg,0x09);
	PCD_WriteByteToRegister(FIFODataReg,0x00);
	PCD_WriteByteToRegister(CommandReg, MFRC522_CALCCRC);		// Start the calculation
	
	i = 0xFF;
	do {
		n =PCD_ReadByteFromRegister(DivIrqReg);// Wait for CalcCRC done
		i--;
	} while ((i != 0) && ((n & 0x04)==0x04));
	
	for (i=0;i<64;i++){
		if (MRFC522_firmware_referenceV2_0[i] != PCD_ReadByteFromRegister(FIFODataReg)){
			printf("\r\nSelf Test FAIL\r\n");		
			return;
		}
	}
	printf("\r\nSelf Test SUCCESS\r\n");
	PCD_WriteByteToRegister(AutoTestReg,0x00); // Disable Sefttest function
}

void PCD_GetFirmwareVersion() {  
	uint8_t value;  
	value = PCD_ReadByteFromRegister(VersionReg);  
	if (value == 0x80){
		printf("\r\nPN512 Firmware Version: V1.0\r\n");	
	}
	else if (value == 0x82){
		printf("\r\nPN512 Firmware Version: V2.0\r\n");	
	}
	else if (value == 0x90){
		printf("\r\nRC522 Firmware Version: V1.0\r\n");	
	}
	else if (value == 0x92){
		printf("\r\nRC522 Firmware Version: V2.0\r\n");	
	}
	else {
		printf("\r\nCheck fail\r\n");	
	}	
}

/*!

  @brief   Sends a command to a tag.

  @param   cmd     The command to the MFRC522 to send a command to the tag.
  @param   data    The data that is needed to complete the command.
  @param   dlen    The length of the data.
  @param   result  The result returned by the tag.
  @param   rlen    The number of valid bits in the resulting value.

  @returns Returns the status of the calculation.
           MI_ERR        if something went wrong,
           MI_NOTAGERR   if there was no tag to send the command to.
           MI_OK         if everything went OK.

 */
 
uint16_t PCD_CommandTag(uint8_t cmd, uint8_t *data, uint16_t dlen, uint8_t *result, uint16_t *rlen) {
	uint16_t status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits, n;
	uint16_t index;
	switch (cmd) {
		case MFRC522_AUTHENT:
			irqEn = (IdleIEn | ErrIEn); // 0x12
			waitIRq = 0x10;
			break;
		case MFRC522_TRANSCEIVE:
			irqEn = TXIEn | RXIEn | IdleIEn | LoAlertIEn |ErrIEn | TimerIEn ;				// 0x77
			waitIRq = 0x30;
			break;
		default:
			break;
	}

	PCD_WriteByteToRegister(CommandReg, MFRC522_IDLE); 	// Cancels current command execution
	PCD_WriteByteToRegister(CommIEnReg, irqEn|0x80);    	// interrupt request

	// indicates that the marked bits in the ComIrqReg register are cleared
	PCD_ClearRegisterBitMask(CommIrqReg, 0x80);             	// Clear all interrupt requests bits.
	PCD_SetRegisterBitMask(FIFOLevelReg, 0x80);             	// FlushBuffer=1, FIFO initialization.

	PCD_WriteByteToRegister(CommandReg, MFRC522_IDLE);  // No action, cancel the current command.

	// Write to FIFO
	for (index=0; index < dlen; index++) {
		PCD_WriteByteToRegister(FIFODataReg, data[index]);
	}

	// Execute the command.
	PCD_WriteByteToRegister(CommandReg, cmd);
	if (cmd == MFRC522_TRANSCEIVE) {
		PCD_SetRegisterBitMask(BitFramingReg, 0x80);  // StartSend=1, transmission, 0x80);  // StartSend=1, transmission of data starts
	}

	// Waiting for the command to complete so we can receive data.
	index = 25; // Max wait time is 25ms.
	do {
		DelayMs(1);
		n = PCD_ReadByteFromRegister(CommIrqReg);
		index--;
	} while ((index!=0) && (!(n&0x01)) && (!(n&waitIRq)));

	PCD_ClearRegisterBitMask(BitFramingReg, 0x80);  // StartSend=0
	
	if (index != 0) { // Request did not time out.
		if(!(PCD_ReadByteFromRegister(ErrorReg) & 0x1D)) {  // BufferOvfl Collerr CRCErr ProtocolErr
			status = MI_OK;
			if (n & irqEn & 0x01) {
				status = MI_NOTAGERR;
			}

			if (cmd == MFRC522_TRANSCEIVE) {
				n = PCD_ReadByteFromRegister(FIFOLevelReg);
				lastBits = PCD_ReadByteFromRegister(ControlReg) & 0x07;
				if (lastBits) {
					*rlen = (n-1)*8 + lastBits;
				} 
				else {
					*rlen = n*8;
				}

				if (n == 0) {
					n = 1;
				}

				if (n > MAX_LEN) {
					n = MAX_LEN;
				}

				// Reading the recieved data from FIFO.
				for (index=0; index<n; index++) {
					result[index] = PCD_ReadByteFromRegister(FIFODataReg);
					//printf("result = %#x\r\n",result[index]);
				}
			}
		}
		else {
			status = MI_ERR;
		}
	}
	return status;
}

void   PCD_CalculateCRC(uint8_t *data, int len, uint8_t *result) {
	uint16_t i;
	uint8_t n;

	PCD_ClearRegisterBitMask(DivIrqReg, 0x04);   // CRCIrq = 0
	PCD_SetRegisterBitMask(FIFOLevelReg, 0x80);  // Clear the FIFO pointer

	//Writing data to the FIFO.
	for (i = 0; i < len; i++) {
		PCD_WriteByteToRegister(FIFODataReg, data[i]);
	}
	PCD_WriteByteToRegister(CommandReg, MFRC522_CALCCRC);

	// Wait for the CRC calculation to complete.
	i = 0xFF;
	do {
		n = PCD_ReadByteFromRegister(DivIrqReg);
		i--;
	} while ((i != 0) && !(n & 0x04));  //CRCIrq = 1

	// Read the result from the CRC calculation.
	result[0] = PCD_ReadByteFromRegister(CRCResultRegL);
	result[1] = PCD_ReadByteFromRegister(CRCResultRegM);
}


/**************************************************************************/
/*!

  @brief   Checks to see if there is a tag in the vicinity.

  @param   mode  The mode we are requsting in.
  @param   type  If we find a tag, this will be the type of that tag.
                 0x4400 = Mifare_UltraLight
                 0x0400 = Mifare_One(S50)
                 0x0200 = Mifare_One(S70)
                 0x0800 = Mifare_Pro(X)
                 0x4403 = Mifare_DESFire

  @returns Returns the status of the request.
           MI_ERR        if something went wrong,
           MI_NOTAGERR   if there was no tag to send the command to.
           MI_OK         if everything went OK.

 */
/**************************************************************************/
uint16_t PCD_RequestTag(uint8_t mode, uint8_t *data) {
	uint16_t status, len;	
	PCD_WriteByteToRegister(BitFramingReg, 0x07);  // TxLastBists = BitFramingReg[2..0] 
	data[0] = mode;
	status = PCD_CommandTag(MFRC522_TRANSCEIVE, data, 1, data, &len);
	if ((status != MI_OK) || (len != 0x10)) {
		status = MI_ERR;
	}
	/*for (uint8_t i=0;i<(len/8);i++){
		printf("data = %#x\r\n",data[i]);
	}*/
	return status;
}


/*!

  @brief   Handles collisions that might occur if there are multiple
           tags available.

  @param   serial  The serial nb of the tag.

  @returns Returns the status of the collision detection.
           MI_ERR        if something went wrong,
           MI_NOTAGERR   if there was no tag to send the command to.
           MI_OK         if everything went OK.

 */
/**************************************************************************/
uint16_t PCD_AntiCollision(uint8_t *serial) {
	uint16_t status, i, len;
	uint8_t check = 0x00;

	PCD_WriteByteToRegister(BitFramingReg, 0x00);  // TxLastBits = BitFramingReg[2..0]
	 // 0x93 0x20
	serial[0] = MF1_ANTICOLL;
	serial[1] = 0x20;
	status = PCD_CommandTag(MFRC522_TRANSCEIVE, serial, 2, serial, &len);
	len = len / 8; // len is in bits, and we want each byte.
	if (status == MI_OK) {
	// The checksum of the tag is the ^ of all the values.
	for (i = 0; i < len-1; i++) {
	  check ^= serial[i];
	}
	// The checksum should be the same as the one provided from the
	// tag (serial[4]).
	if (check != serial[i]) {
	  status = MI_ERR;
	}
	}

	return status;
}


/**************************************************************************/
/*!

  @brief   Selects a tag for processing.

  @param   serial  The serial number of the tag that is to be selected.

  @returns The SAK response from the tag.

 */
/**************************************************************************/
uint8_t PCD_SelectTag(uint8_t *serial) {
	uint16_t i, status, len;
	uint8_t sak;
	uint8_t buffer[9];

	buffer[0] = MF1_SELECTTAG;
	buffer[1] = 0x70;
	for (i = 0; i < 5; i++) {
		buffer[i+2] = serial[i];
	}
	PCD_CalculateCRC(buffer, 7, &buffer[7]);

	status = PCD_CommandTag(MFRC522_TRANSCEIVE, buffer, 9, buffer, &len);

	if ((status == MI_OK) && (len == 0x18)) {
		sak = buffer[0];
	}
	else {
		sak = 0;
	}

	return sak;
}


/**************************************************************************/
/*!

  @brief   Handles the authentication between the tag and the reader.

  @param   mode    What authentication key to use.
  @param   block   The block that we want to read.
  @param   key     The authentication key.
  @param   serial  The serial of the tag.

  @returns Returns the status of the collision detection.
           MI_ERR        if something went wrong,
           MI_OK         if everything went OK.

 */
/**************************************************************************/
uint16_t PCD_Authenticate(uint8_t mode, uint8_t block, uint8_t *key, uint8_t *serial) {
	uint16_t i, status, len;
	uint8_t buffer[12];

	//Verify the command block address + sector + password + tag serial number
	buffer[0] = mode;          // 0th byte is the mode
	buffer[1] = block;         // 1st byte is the block to address.
	for (i = 0; i < 6; i++) {  // 2nd to 7th byte is the authentication key.
		buffer[i+2] = key[i];
	}
	for (i = 0; i < 4; i++) {  // 8th to 11th byte is the serial of the tag.
		buffer[i+8] = serial[i];
	}

	status = PCD_CommandTag(MFRC522_AUTHENT, buffer, 12, buffer, &len);

	if ((status != MI_OK) || (!(PCD_ReadByteFromRegister(Status2Reg) & 0x08))) {
		status = MI_ERR;
	}

	return status;
}

/**************************************************************************/
/*!

  @brief   Tries to read from the current (authenticated) tag.

  @param   block   The block that we want to read.
  @param   result  The resulting value returned from the tag.

  @returns Returns the status of the collision detection.
           MI_ERR        if something went wrong,
           MI_OK         if everything went OK.

 */
/**************************************************************************/
uint16_t PCD_ReadFromTag(uint8_t block, uint8_t *result) {
  uint16_t status, len;

  result[0] = MF1_READ;
  result[1] = block;
  PCD_CalculateCRC(result, 2, &result[2]);
  status = PCD_CommandTag(MFRC522_TRANSCEIVE, result, 4, result, &len);

  if ((status != MI_OK) || (len != 0x90)) {
    status = MI_ERR;
  }

  return status;
}

/**************************************************************************/
/*!

  @brief   Tries to write to a block on the current tag.

  @param   block  The block that we want to write to.
  @param   data   The data that we shoudl write to the block.

  @returns Returns the status of the collision detection.
           MI_ERR        if something went wrong,
           MI_OK         if everything went OK.

 */
/**************************************************************************/
uint16_t PCD_WriteToTag(uint8_t block, uint8_t *data) {
  uint16_t status, i, len;
  uint8_t buffer[18];

  buffer[0] = MF1_WRITE;
  buffer[1] = block;
  PCD_CalculateCRC(buffer, 2, &buffer[2]);
  status = PCD_CommandTag(MFRC522_TRANSCEIVE, buffer, 4, buffer, &len);

  if ((status != MI_OK) || (len != 4) || ((buffer[0] & 0x0F) != 0x0A)) {
    status = MI_ERR;
  }

  if (status == MI_OK) {
    for (i = 0; i < 16; i++) {
      buffer[i] = data[i];
    }
    PCD_CalculateCRC(buffer, 16, &buffer[16]);
    status = PCD_CommandTag(MFRC522_TRANSCEIVE, buffer, 18, buffer, &len);

    if ((status != MI_OK) || (len != 4) || ((buffer[0] & 0x0F) != 0x0A)) {
      status = MI_ERR;
    }
  }

  return status;
}
uint16_t PCD_HaltTag() {
	uint16_t status, len;
	uint8_t buffer[4];

	buffer[0] = MF1_HALT;
	buffer[1] = 0;
	PCD_CalculateCRC(buffer, 2, &buffer[2]);
	PCD_ClearRegisterBitMask(Status2Reg, 0x08);  // turn off encryption
	status = PCD_CommandTag(MFRC522_TRANSCEIVE, buffer, 4, buffer, &len);

	return status;
}

void PCD_AntennaOn() {
	uint8_t value = PCD_ReadByteFromRegister(TxControlReg);
	if ((value & 0x03) != 0x03) {
		PCD_WriteByteToRegister(TxControlReg, value | 0x03);
	}
}
void PCD_AntennaOff() {
	PCD_ClearRegisterBitMask(TxControlReg, 0x03);
}
void PCD_SoftPowerDown(){
	//PCD_AntennaOff();
	//PCD_SetRegisterBitMask(CommandReg,0x10);	
}
void PCD_Wakeup(){
	//PCD_ClearRegisterBitMask(CommandReg,0x10);
	//DelayMs(10);
	//PCD_AntennaOn();
	
}

/**
 * Calculates the bit pattern needed for the specified access bits. In the [C1 C2 C3] tupples C1 is MSB (=4) and C3 is LSB (=1).
 */

void  MIFARE_SetAccessBits(uint8_t *accessBitsBuffer,uint8_t g0,uint8_t g1,uint8_t g2,uint8_t g3){
	/// g0 : < Access bits [C1 C2 C3] for block 0 
	/// g1 : < Access bits [C1 C2 C3] for block 1 
	/// g2 : < Access bits [C1 C2 C3] for block 2 
	/// g3 : < Access bits [C1 C2 C3] for the sector trailer, block 3 // Just Read, Write command
	uint8_t c1 = ((g3 & 4) << 1) | ((g2 & 4) << 0) | ((g1 & 4) >> 1) | ((g0 & 4) >> 2);
	uint8_t c2 = ((g3 & 2) << 2) | ((g2 & 2) << 1) | ((g1 & 2) << 0) | ((g0 & 2) >> 1);
	uint8_t c3 = ((g3 & 1) << 3) | ((g2 & 1) << 2) | ((g1 & 1) << 1) | ((g0 & 1) << 0);
	
	accessBitsBuffer[0] = (~c2 & 0xF) << 4 | (~c1 & 0xF);
	accessBitsBuffer[1] =   c1 << 4 | (~c3 & 0xF);
	accessBitsBuffer[2] =          c3 << 4 | c2;
}

void Mifare_ReadAccessBits(uint8_t *accessBitsBuffer){


}
