/******************************************************************************
*  32kHz ACLK, 20MHZ SMCLK
*  
*  Description: AECL 4E EC Gas modules (PB14XX, PB15XX) I2C driver
*
*   Company: Autotronic Enterprise CO., LTD.
*   Author: SY LAU
*   Create Date: 2017/12/15
*
*
*********************************************************************************/
#include "include.h"

//#define AECLMG_I2C_ADDRESS              0x51 
#define AECLMG_MEASURE_RESISTANCE       0X2024
#define AECLMG_INIT_AIR_QUALITY         0X2003
#define AECLMG_MEASURE_AIR_QUALITY      0X2008
#define AECLMG_MEASURE_TEST             0X20FD
// Generator polynomial for CRC
#define POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

UINT32 aeclmg_adc;
UINT32 aeclmg_ppb;
//UINT16 aeclmg_serialId[3];

//-----------------------------------------------------------------------------
static UINT8 aeclmg_calcCrc(UINT8 data[], UINT8 nbrOfBytes)
{
  u8t bit;        // bit mask
  u8t crc = 0xFF; // calculated checksum
  u8t byteCtr;    // byte counter
  
  // calculates 8-Bit checksum with given polynomial
  for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)
  {
    crc ^= (data[byteCtr]);
    for(bit = 8; bit > 0; --bit)
    {
      if(crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else           crc = (crc << 1);
    }
  }
  
  return crc;
}

INT16  aeclmg_getAdc(UINT8 address, UINT16 command, UINT32* data) {
  UINT8 aeclmg_buffer[6]; //從機回傳資料緩存區
  UINT8 txData[3]; //主機要送出的資料&從機回傳的資料
  UINT8 checksum; //從從機收到的驗證碼
  UINT8 crc;//主機計算出的驗證碼
  UINT16 tempData[2];//處理後從機回傳資料
  UINT8 status;//通訊狀態(是否失敗)
  
  txData[0] = command >> 8;
  txData[1] = command;
  txData[2] = 0x00;
  
  aeclmg_adc = 0;

  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  delay_micro_second(50);
  // wrap around, prevent read value from previous command防止上次有未完全寫入的資料殘留
  status = 0;
  status = eusci_b_i2c_tx_multi(address, txData, 3);//輸入指令到從機
  
  if(status == 0) {  // tx ack success, receive data
    delay_ms(10);
    
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);//如果從機正確接收，對從機要求讀取6個byte的資料，包含2筆2byte的資料，每筆資料跟著1byte的checksum
    
    if(status == 0) {  // tx ack success, receive serial number如果主機正確接收從機資料，開始對資料做整理
      for(int i=0;i<6;i+=3) { 
        txData[0] = *(aeclmg_buffer + i);
        txData[1] = *(aeclmg_buffer + (i+1));
        checksum = *(aeclmg_buffer + (i+2));
        crc = aeclmg_calcCrc(txData, 2);//計算驗證碼
        if(crc == checksum) {//若計算出的跟接收到的相同
          tempData[i/3] = (txData[0] << 8) + txData[1];//temp[0]=當次tx[0]+tx[1]共16bit
        }
        else {
          //crc error
//          tempData[i/3] = 0xffff; // max value indicate crc error
          return ERROR_CHECKSUM;
        }
      }
      *data = (((INT32) tempData[0]) << 16) + (INT32) tempData[1];//回傳32bit的資料，包含2個16bit的資料
    }
    else {
      return ERROR_ACK;
    }
  }
  else {
    return ERROR_ACK;
  }

  return ERROR_NONE;

}


INT16 aeclmg_getSerialId(UINT8 address, UINT16* data) {
  UINT8 aeclmg_buffer[9];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT8 status;

  
  txData[0] = 0x36;
  txData[1] = 0x82;
  txData[2] = 0x00;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {  // tx ack success, receive serial number
    delay_ms(5);
    
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 9);
    
    if(status == 0) {  // tx ack success, receive serial number
      for(int i=0;i<9;i+=3) { 
        txData[0] = *(aeclmg_buffer + i);
        txData[1] = *(aeclmg_buffer + (i+1));
        checksum = *(aeclmg_buffer + (i+2));
        crc = aeclmg_calcCrc(txData, 2);
        if(crc == checksum) {
          *(data+(i/3)) = (txData[0] << 8) + txData[1];
        }
        else {
          //crc error
//          tempData[i/3] = 0xffff; // max value indicate crc error
          return ERROR_CHECKSUM;
        }
      }
    }
    else {
      return ERROR_ACK;
    }
  }
  else {
    return ERROR_ACK;
  }

  return ERROR_NONE;
 
}

INT16  aeclmg_get_fw_version(UINT8 address, UINT16* data) {
  UINT8 aeclmg_buffer[3];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT8 status;
  
  txData[0] = 0x60;
  txData[1] = 0x61;
  txData[2] = 0x00;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {  // tx ack success, receive data
    delay_ms(10);
    
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 3);
    
    if(status == 0) {  // tx ack success, receive serial number
      txData[0] = *(aeclmg_buffer);
      txData[1] = *(aeclmg_buffer + 1);
      checksum = *(aeclmg_buffer + 2);
      crc = aeclmg_calcCrc(txData, 2);
      if(crc == checksum) {
        *data = (txData[0] << 8) + txData[1];
      }
      else {
        //crc error
        return ERROR_CHECKSUM;
      }
    }
    else {
      return ERROR_ACK;
    }
  }
  else {
    return ERROR_ACK;
  }

  return ERROR_NONE;

}


/*
INT32 aeclmg_getPpb(UINT8 address) {
  UINT8 *aeclmg_buffer;
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT16 tempData[2];
  UINT8 status;
  
  txData[0] = 0x20;
  txData[1] = 0x08;
  txData[2] = 0x00;
  
  aeclmg_ppb = 0;

  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {  // tx ack success, receive data
    delay_ms(10);
    
    aeclmg_buffer = eusci_b_i2c_rx_multi(address, 6);
   
    for(int i=0;i<6;i+=3) { 
      txData[0] = *(aeclmg_buffer + i);
      txData[1] = *(aeclmg_buffer + (i+1));
      checksum = *(aeclmg_buffer + (i+2));
      crc = aeclmg_calcCrc(txData, 2);
      if(crc == checksum) {
        tempData[i/3] = (txData[0] << 8) + txData[1];
      }
      else {
        //crc error
        tempData[i/3] = 0xffff; // max value indicate crc error
      }
    }

    aeclmg_ppb = (((INT32) tempData[0]) << 16) + (INT32) tempData[1];
  }

  return aeclmg_ppb;

}
*/

