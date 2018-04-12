/*
 * MFRC522.c
 *
 *  Created on: 18-12-2015
 *      Author: Mefiu
 */
#include <avr/io.h>
#include <util/delay.h>
#include "MFRC522.h"
#include "../SPI/SPI.h"

#if MFRC522_USE_SPI == 1
#include "../SPI/SPI.h"
#endif



#if MFRC522_USE_SPI == 1
void MFRC522_InitSPI_Pins()
{
	MFRC522_RST_PORT |= MFRC522_RST;
	MFRC522_RST_DDR |= MFRC522_RST;

	MFRC522_CS_PORT |= MFRC522_CS;
	MFRC522_CS_DDR |= MFRC522_CS;

#if MFRC522_USE_IRQ == 1
	MFRC522_IRQ_PORT &= ~MFRC522_IRQ;
#endif
}
#endif

void MFRC522_Init()
{
	MFRC522_CS_HIGH;
	MFRC522_RST_LOW;
	_delay_ms(50);
	MFRC522_RST_HIGH;
	_delay_ms(50);
	MFRC522_Reset();

  //Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
	MFRC522_WriteToRegister(TModeReg, 0x8D);       // Tauto=1; f(Timer) = 6.78MHz/TPreScaler
	MFRC522_WriteToRegister(TPrescalerReg, 0x3E);  // TModeReg[3..0] + TPrescalerReg
	MFRC522_WriteToRegister(TReloadRegL, 0x1E);
	MFRC522_WriteToRegister(TReloadRegH, 0x00);

	MFRC522_WriteToRegister(TxAutoReg, 0x40);      // 100%ASK
	MFRC522_WriteToRegister(ModeReg, 0x3D);        // CRC initial value 0x6363

	MFRC522_SetBitMask(TxControlReg, 0x03);        // Turn antenna on.
}

void MFRC522_Reset()
{
	MFRC522_WriteToRegister(CommandReg, MFRC522_SOFTRESET);
	_delay_ms(50);
	while(MFRC522_ReadFromRegister(CommandReg & (1<<4)));

}

void MFRC522_WriteToRegister(uint8_t addr, uint8_t val)
{
	MFRC522_CS_LOW;

	//Address format: 0XXXXXX0
	SendSPI((addr<<1)&0x7E);
	SendSPI(val);
	MFRC522_CS_HIGH;
}

uint8_t MFRC522_ReadFromRegister(uint8_t addr)
{
	uint8_t val;
	MFRC522_CS_LOW;
	SendSPI(((addr<<1)&0x7E) | 0x80);
	val = TransferSPI(0x00);
	MFRC522_CS_HIGH;
	return val;
}

void MFRC522_SetBitMask(uint8_t addr, uint8_t mask)
{
	uint8_t current;
	current = MFRC522_ReadFromRegister(addr);
	MFRC522_WriteToRegister(addr, current | mask);
}

void MFRC522_ClearBitMask(uint8_t addr, uint8_t mask)
{
	uint8_t current;
	current = MFRC522_ReadFromRegister(addr);
	MFRC522_WriteToRegister(addr, current & (~mask));
}

uint8_t MFRC522_GetFirmwareVersion()
{
  uint8_t response;
  response = MFRC522_ReadFromRegister(VersionReg);
  return response;
}

int MFRC522_RequestTag(uint8_t mode, uint8_t *data)
{
  int status, len;
  MFRC522_WriteToRegister(BitFramingReg, 0x07);  // TxLastBists = BitFramingReg[2..0]

  data[0] = mode;
  status = MFRC522_CommandTag(MFRC522_TRANSCEIVE, data, 1, data, &len);

  if ((status != MI_OK) || (len != 0x10)) {
    status = MI_ERR;
  }

  return status;
}

int MFRC522_CommandTag(uint8_t cmd, uint8_t *data, int dlen, uint8_t *result, int *rlen)
{
  int status = MI_ERR;
  uint8_t irqEn = 0x00;
  uint8_t waitIRq = 0x00;
  uint8_t lastBits, n;
  int i;

  switch (cmd) {
  case MFRC522_AUTHENT:
    irqEn = 0x12;
    waitIRq = 0x10;
    break;
  case MFRC522_TRANSCEIVE:
    irqEn = 0x77;
    waitIRq = 0x30;
    break;
  default:
    break;
  }

  MFRC522_WriteToRegister(CommIEnReg, irqEn|0x80);    // interrupt request
  MFRC522_ClearBitMask(CommIrqReg, 0x80);             // Clear all interrupt requests bits.
  MFRC522_SetBitMask(FIFOLevelReg, 0x80);             // FlushBuffer=1, FIFO initialization.

  MFRC522_WriteToRegister(CommandReg, MFRC522_IDLE);  // No action, cancel the current command.

  // Write to FIFO
  for (i=0; i < dlen; i++) {
	  MFRC522_WriteToRegister(FIFODataReg, data[i]);
  }

  // Execute the command.
  MFRC522_WriteToRegister(CommandReg, cmd);
  if (cmd == MFRC522_TRANSCEIVE) {
	  MFRC522_SetBitMask(BitFramingReg, 0x80);  // StartSend=1, transmission of data starts
  }

  // Waiting for the command to complete so we can receive data.
  i = 25; // Max wait time is 25ms.
  do {
    _delay_ms(1);
    // CommIRqReg[7..0]
    // Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
    n = MFRC522_ReadFromRegister(CommIrqReg);
    i--;
  } while ((i!=0) && !(n&0x01) && !(n&waitIRq));

  MFRC522_ClearBitMask(BitFramingReg, 0x80);  // StartSend=0

  if (i != 0) { // Request did not time out.
    if(!(MFRC522_ReadFromRegister(ErrorReg) & 0x1D)) {  // BufferOvfl Collerr CRCErr ProtocolErr
      status = MI_OK;
      if (n & irqEn & 0x01) {
        status = MI_NOTAGERR;
      }

      if (cmd == MFRC522_TRANSCEIVE) {
        n = MFRC522_ReadFromRegister(FIFOLevelReg);
        lastBits = MFRC522_ReadFromRegister(ControlReg) & 0x07;
        if (lastBits) {
          *rlen = (n-1)*8 + lastBits;
        } else {
          *rlen = n*8;
        }

        if (n == 0) {
          n = 1;
        }

        if (n > MAX_LEN) {
          n = MAX_LEN;
        }

        // Reading the recieved data from FIFO.
        for (i=0; i<n; i++) {
          result[i] = MFRC522_ReadFromRegister(FIFODataReg);
        }
      }
    } else {
      status = MI_ERR;
    }
  }
  return status;
}

int MFRC522_AntiCollision(uint8_t *serial)
{
  int status, i, len;
  uint8_t check = 0x00;

  MFRC522_WriteToRegister(BitFramingReg, 0x00);  // TxLastBits = BitFramingReg[2..0]

  serial[0] = MF1_ANTICOLL;
  serial[1] = 0x20;
  status = MFRC522_CommandTag(MFRC522_TRANSCEIVE, serial, 2, serial, &len);
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
