/*******************************************************************************
oooooooooooo               .ooooooo.
`888'     `8              d8P'   `YP
 888          'YI.oooo.   d8P          .ooooo.  'YI.oooo.   .oooooo.   .ooooo.
 888oooo8      88'  'd8   Y8P'88bo.   d8'   `8b  88'  'd8  'Y8'   `   d8'   `8b
 888    "      88    88        `Y8b  d888oooo88b 88    88   Y8P'boo. d888oooo88b 
 888       o   88    88  .o     d8P '8'          88    88       '8Y' '8'
o888ooooood8  o88o   88o `Y8oood8P'   "bP88Y8P" o88o   88o `Y8oodP"   "bP88Y8P"
*******************************************************************************/
/*******************************************************************************
*  32kHz ACLK, 20MHZ SMCLK
*  
*  Description: AECL 4E EC Gas modules (PB14XX, PB15XX) I2C driver
*
*   Company: Autotronic Enterprise CO., LTD.
*   Author: SY LAU
*   Create Date: 2017/12/15
*
*
*******************************************************************************/
#include "include.h"

// Generator polynomial for CRC
#define POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

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

INT16 get_i2c_test_results(UINT8 address, UINT16* data) {
  UINT8 aeclmg_buffer[6];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT8 status;
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_I2C_TEST;
  crc = aeclmg_calcCrc(txData, 2);
  txData[2] = crc;
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 3);
    
    if(status == 0) {
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

INT16  aeclmg_getAdc_1(UINT8 address, UINT32* data) {
  
  UINT8 aeclmg_buffer[6];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT16 tempData[2];
  UINT8 status;
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_ADC_1;//read AE
  crc = aeclmg_calcCrc(txData, 2);
  txData[2] = crc;
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    if(status == 0) {
      for(int i=0;i<6;i+=3) { 
        txData[0] = *(aeclmg_buffer + i);
        txData[1] = *(aeclmg_buffer + (i+1));
        checksum = *(aeclmg_buffer + (i+2));
        crc = aeclmg_calcCrc(txData, 2);
        if(crc == checksum) {
          tempData[i/3] = (txData[0] << 8) + txData[1];
        }
        else {
          return ERROR_CHECKSUM;
        }
      }
      *data = (((INT32) tempData[0]) << 16) + (INT32) tempData[1];
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

INT16  aeclmg_getAdc_2(UINT8 address, UINT32* data) {
  
  UINT8 aeclmg_buffer[6];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT16 tempData[2];
  UINT8 status;
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_ADC_2;//read WE
  crc = aeclmg_calcCrc(txData, 2);
  txData[2] = crc;
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
      for(int i=0;i<6;i+=3) { 
        txData[0] = *(aeclmg_buffer + i);
        txData[1] = *(aeclmg_buffer + (i+1));
        checksum = *(aeclmg_buffer + (i+2));
        crc = aeclmg_calcCrc(txData, 2);
        if(crc == checksum) {
          tempData[i/3] = (txData[0] << 8) + txData[1];
        }
        else {
          return ERROR_CHECKSUM;
        }
      }
      *data = (((INT32) tempData[0]) << 16) + (INT32) tempData[1];
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

INT16 aeclmg_get_gas_name(UINT8 address, UINT64* data) {
  
  UINT8 aeclmg_buffer[12];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT8 status;
  UINT16 tempData[4];
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_GAS_NAME;
  crc = aeclmg_calcCrc(txData, 2);
  txData[2] = crc;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 12);
    
    if(status == 0) {
      for(int i=0;i<12;i+=3) { 
        txData[0] = *(aeclmg_buffer + i);
        txData[1] = *(aeclmg_buffer + (i+1));
        checksum = *(aeclmg_buffer + (i+2));
        crc = aeclmg_calcCrc(txData, 2);
        if(crc == checksum) {
          tempData[i/3] = (txData[0] << 8) + txData[1];
        }
        else {
          //crc error
          //tempData[i/3] = 0xffff; // max value indicate crc error
          return ERROR_CHECKSUM;
        }
      }
      *data = (((UINT64) tempData[0]) << 48) +(((UINT64) tempData[1]) << 32) +
        (((UINT64) tempData[2]) << 16) + (UINT64) tempData[3];
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

INT16 aeclmg_getSerialId(UINT8 address, UINT64* data) {
  
  UINT8 aeclmg_buffer[9];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT8 status;
  UINT16 tempData[4];
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_BSN;
  crc = aeclmg_calcCrc(txData, 2);
  txData[2] = crc;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);    
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 9);

    if(status == 0) {
      for(int i=0;i<9;i+=3) { 
        txData[0] = *(aeclmg_buffer + i);
        txData[1] = *(aeclmg_buffer + (i+1));
        checksum = *(aeclmg_buffer + (i+2));
        crc = aeclmg_calcCrc(txData, 2);
        if(crc == checksum) {
          tempData[i/3] = (txData[0] << 8) + txData[1];
        }
        else {
          //crc error
          //tempData[i/3] = 0xffff; // max value indicate crc error
          return ERROR_CHECKSUM;
        }
      }
      tempData[3] = 0x00; //Clear invalid values  
      *data = (((UINT64) tempData[0]) << 48) +(((UINT64) tempData[1]) << 32) +
        (((UINT64) tempData[2]) << 16) + (UINT64) tempData[3];
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
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_FW_VER;
  crc = aeclmg_calcCrc(txData, 2);
  txData[2] = crc;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 3);
    
    if(status == 0) {
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

INT16  aeclmg_get_cal_date(UINT8 address, UINT16* data) {
  
  UINT8 aeclmg_buffer[6];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT8 status;
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_CAL_DATE;
  crc = aeclmg_calcCrc(txData, 2);
  txData[2] = crc;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
      for(int i=0;i<6;i+=3) { 
        txData[0] = *(aeclmg_buffer + i);
        txData[1] = *(aeclmg_buffer + (i+1));
        checksum = *(aeclmg_buffer + (i+2));
        crc = aeclmg_calcCrc(txData, 2);
        if(crc == checksum) {
          *(data+(i/3+1)) = (txData[0] << 8) + txData[1];
        }
        else {
          //crc error
          //tempData[i/3] = 0xffff; // max value indicate crc error
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

INT16  aeclmg_get_gas_concentrations(UINT8 address, UINT16* data) {
  
  UINT8 aeclmg_buffer[3];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT8 status;
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_CONCENTRATIONS;
  crc = aeclmg_calcCrc(txData, 2);
  txData[2] = crc;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {  // tx ack success, receive data
    delay_ms(1);
    
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

INT16 aeclmg_get_gas_cal_zero(UINT8 address, INT32* data) {
  
  UINT8 aeclmg_buffer[6];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT8 status;
  UINT16 data_temp[2];
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_CAL_ZERO;
  crc = aeclmg_calcCrc(txData, 2);
  txData[2] = crc;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);
    
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
      for(int i=0;i<6;i+=3) {
        txData[0] = *(aeclmg_buffer + i);
        txData[1] = *(aeclmg_buffer + (i+1));
        checksum = *(aeclmg_buffer + (i+2));
        crc = aeclmg_calcCrc(txData, 2);
        if(crc == checksum) {
          data_temp[i/3] = (txData[0] << 8) + txData[1];
        }
        else {
          //crc error
          //tempData[i/3] = 0xffff; // max value indicate crc error
          return ERROR_CHECKSUM;
        }
      }
      *data = ((INT32)data_temp[0] << 16) + data_temp[1];
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

INT16 aeclmg_get_gas_cal_span(UINT8 address, float* data) {
  
  UINT8 aeclmg_buffer[6];
  UINT8 txData[6];
  UINT8 data_buffer[4];
  UINT8 crc_1, crc_2;
  UINT8 status;
  float float_buffer = 0.0;
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_CAL_SPAN;
  crc_1 = aeclmg_calcCrc(txData, 2);
  txData[2] = crc_1;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
      crc_1 = aeclmg_calcCrc(aeclmg_buffer, 2);
      crc_2 = aeclmg_calcCrc((aeclmg_buffer+3), 2);
      if((crc_1 == aeclmg_buffer[2]) && (crc_2 == aeclmg_buffer[5])){
        data_buffer[3] = aeclmg_buffer[0];
        data_buffer[2] = aeclmg_buffer[1];
        data_buffer[1] = aeclmg_buffer[3];
        data_buffer[0] = aeclmg_buffer[4];
        memcpy(&float_buffer, data_buffer, sizeof(float));
        *data = float_buffer;
      }
      else {
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

INT16 aeclmg_get_gas_cal_a(UINT8 address, float* data) {
  
  UINT8 aeclmg_buffer[6];
  UINT8 txData[6];
  UINT8 data_buffer[4];
  UINT8 crc_1, crc_2;
  UINT8 status;
  float float_buffer = 0.0;
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_CAL_A;
  crc_1 = aeclmg_calcCrc(txData, 2);
  txData[2] = crc_1;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
      crc_1 = aeclmg_calcCrc(aeclmg_buffer, 2);
      crc_2 = aeclmg_calcCrc((aeclmg_buffer+3), 2);
      if((crc_1 == aeclmg_buffer[2]) && (crc_2 == aeclmg_buffer[5])){
        data_buffer[3] = aeclmg_buffer[0];
        data_buffer[2] = aeclmg_buffer[1];
        data_buffer[1] = aeclmg_buffer[3];
        data_buffer[0] = aeclmg_buffer[4];
        memcpy(&float_buffer, data_buffer, sizeof(float));
        *data = float_buffer;
      }
      else {
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

INT16 aeclmg_get_gas_cal_b(UINT8 address, INT32* data) {
  
  UINT8 aeclmg_buffer[6];
  UINT8 txData[3];
  UINT8 checksum;
  UINT8 crc;
  UINT8 status;
  UINT16 data_temp[2];
  
  txData[0] = CMD_READ_HEAD;
  txData[1] = CMD_READ_CAL_B;
  crc = aeclmg_calcCrc(txData, 2);
  txData[2] = crc;
  
  status = eusci_b_i2c_tx_multi(address, txData, 3);
  
  if(status == 0) {
    delay_ms(1);
    
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
      for(int i=0;i<6;i+=3) { 
        txData[0] = *(aeclmg_buffer + i);
        txData[1] = *(aeclmg_buffer + (i+1));
        checksum = *(aeclmg_buffer + (i+2));
        crc = aeclmg_calcCrc(txData, 2);
        if(crc == checksum) {
          data_temp[i/3] = (txData[0] << 8) + txData[1];
        }
        else {
          //crc error
          //tempData[i/3] = 0xffff; // max value indicate crc error
          return ERROR_CHECKSUM;
        }
      }
      *data = ((INT32)data_temp[0] << 16) + data_temp[1];
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

INT16 write_gas_bsn(UINT8 address, UINT64 w_data) {

  UINT8 txData[12];
  UINT8 status;
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_BSN;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (w_data >> 40);
  txData[4] = (w_data >> 32);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (w_data >> 24);
  txData[7] = (w_data >> 16);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  txData[9] = (w_data >> 8);
  txData[10] = w_data;
  txData[11] = aeclmg_calcCrc((txData+9), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 12);
  
  if(status == 0) {
//    delay_ms(300);
  }
  else {
    return ERROR_ACK;
  }
  return ERROR_NONE;
}

INT16 write_gas_cal_date(UINT8 address, UINT32 w_data) {

  UINT8 txData[9];
  UINT8 status;
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_CAL_DATE;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (w_data >> 24);
  txData[4] = (w_data >> 16);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (w_data >> 8);
  txData[7] = (w_data);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 9);
  
  if(status == 0) {
//    delay_ms(300);
  }
  else {
    return ERROR_ACK;
  }
  return ERROR_NONE;
}

INT16 write_gas_cal_zero(UINT8 address, INT32 w_data) {

  UINT8 txData[9];
  UINT8 status;
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_CAL_ZERO;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (w_data >> 24);
  txData[4] = (w_data >> 16);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (w_data >> 8);
  txData[7] = (w_data);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 9);
  
  if(status == 0) {
//    delay_ms(300);
  }
  else {
    return ERROR_ACK;
  }
  return ERROR_NONE;
}

INT16 write_gas_cal_span(UINT8 address, float w_data) {
  
  UINT8 txData[9];
  UINT8 status;
  UINT8 data_buff[sizeof(float)];
  
  memcpy(data_buff, &w_data, sizeof(float));
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_CAL_SPAN;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (data_buff[3]);
  txData[4] = (data_buff[2]);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (data_buff[1]);
  txData[7] = (data_buff[0]);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 9);
  
  if(status == 0) {
//    delay_ms(300);
  }
  else {
    return ERROR_ACK;
  }
  return ERROR_NONE;
}

INT16 write_gas_cal_a(UINT8 address, float w_data) {

  UINT8 txData[9];
  UINT8 status;
  UINT8 data_buff[sizeof(float)];
  
  memcpy(data_buff, &w_data, sizeof(float));
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_CAL_A;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (data_buff[3]);
  txData[4] = (data_buff[2]);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (data_buff[1]);
  txData[7] = (data_buff[0]);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 9);

  if(status == 0) {
//    delay_ms(300);
  }
  else {
    return ERROR_ACK;
  }
  return ERROR_NONE;
}

INT16 write_gas_cal_b(UINT8 address, INT32 w_data) {

  UINT8 txData[9];
  UINT8 status;
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_CAL_B;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (w_data >> 24);
  txData[4] = (w_data >> 16);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (w_data >> 8);
  txData[7] = (w_data);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 9);
  
  if(status == 0) {
//    delay_ms(300);
  }
  else {
    return ERROR_ACK;
  }
  return ERROR_NONE;
}

INT16 write_and_read_gas_bsn(UINT8 address, UINT64 w_data, UINT64* r_data) {

  UINT8 aeclmg_buffer[9];
  UINT8 checksum;
  UINT8 crc;
  UINT8 txData[12];
  UINT8 status;
  UINT16 tempData[4];
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_BSN;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (w_data >> 40);
  txData[4] = (w_data >> 32);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (w_data >> 24);
  txData[7] = (w_data >> 16);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  txData[9] = (w_data >> 8);
  txData[10] = w_data;
  txData[11] = aeclmg_calcCrc((txData+9), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 12);
  
  if(status == 0) {
    delay_ms(300);   
    txData[0] = CMD_READ_HEAD;
    txData[1] = CMD_READ_BSN;
    crc = aeclmg_calcCrc(txData, 2);
    txData[2] = crc;
    status = eusci_b_i2c_tx_multi(address, txData, 3);
    delay_ms(1);
    
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 9);
    
    if(status == 0) {
      for(int i=0;i<9;i+=3) { 
        txData[0] = *(aeclmg_buffer + i);
        txData[1] = *(aeclmg_buffer + (i+1));
        checksum = *(aeclmg_buffer + (i+2));
        crc = aeclmg_calcCrc(txData, 2);
        if(crc == checksum) {
          tempData[i/3] = (txData[0] << 8) + txData[1];
        }
        else {
          //crc error
          //tempData[i/3] = 0xffff; // max value indicate crc error
          return ERROR_CHECKSUM;
        }
      }
      tempData[3] = 0x00; //Clear invalid values  
      *r_data = (((UINT64) tempData[0]) << 32) +(((UINT64) tempData[1]) << 16) +
        (((UINT64) tempData[2]));
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

INT16 write_and_read_gas_cal_date(UINT8 address, UINT32 w_data, UINT32* r_data) {

  UINT8 aeclmg_buffer[6];
  UINT8 checksum;
  UINT8 crc;
  UINT16 tempData[4];
  UINT8 txData[9];
  UINT8 status;
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_CAL_DATE;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (w_data >> 24);
  txData[4] = (w_data >> 16);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (w_data >> 8);
  txData[7] = (w_data);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 9);
  
  if(status == 0) {
    delay_ms(300);
    txData[0] = CMD_READ_HEAD;
    txData[1] = CMD_READ_CAL_DATE;
    crc = aeclmg_calcCrc(txData, 2);
    txData[2] = crc;
    status = eusci_b_i2c_tx_multi(address, txData, 3);
    delay_ms(1);
    
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
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
          //tempData[i/3] = 0xffff; // max value indicate crc error
          return ERROR_CHECKSUM;
        }
      }
      tempData[3] = 0x00; //Clear invalid values  
      *r_data = (((UINT32) tempData[0]) << 16) +(((UINT32) tempData[1]));
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

INT16 write_and_read_gas_cal_zero(UINT8 address, INT32 w_data, INT32* r_data) {
  UINT8 aeclmg_buffer[6];
  UINT8 checksum;
  UINT8 crc;
  UINT16 tempData[4];
  UINT8 txData[9];
  UINT8 status;
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_CAL_ZERO;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (w_data >> 24);
  txData[4] = (w_data >> 16);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (w_data >> 8);
  txData[7] = (w_data);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 9);
  
  if(status == 0) {
    delay_ms(300);
    txData[0] = CMD_READ_HEAD;
    txData[1] = CMD_READ_CAL_ZERO;
    crc = aeclmg_calcCrc(txData, 2);
    txData[2] = crc;
    status = eusci_b_i2c_tx_multi(address, txData, 3);
    delay_ms(1);
    
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
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
          //tempData[i/3] = 0xffff; // max value indicate crc error
          return ERROR_CHECKSUM;
        }
      }
      tempData[3] = 0x00; //Clear invalid values  
      *r_data = (((UINT32) tempData[0]) << 16) +(((UINT32) tempData[1]));
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

INT16 write_and_read_gas_cal_span(UINT8 address, float w_data, float* r_data) {
  
  UINT8 aeclmg_buffer[6];
  UINT8 crc_1, crc_2;
  float float_buffer = 0.0;
  
  UINT8 txData[9];
  UINT8 status;
  UINT8 data_buff[sizeof(float)];
  
  memcpy(data_buff, &w_data, sizeof(float));
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_CAL_SPAN;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (data_buff[3]);
  txData[4] = (data_buff[2]);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (data_buff[1]);
  txData[7] = (data_buff[0]);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 9);
  
  if(status == 0) {
    delay_ms(300);
    txData[0] = CMD_READ_HEAD;
    txData[1] = CMD_READ_CAL_SPAN;
    crc_1 = aeclmg_calcCrc(txData, 2);
    txData[2] = crc_1;
    status = eusci_b_i2c_tx_multi(address, txData, 3);
    delay_ms(1); 
    
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
      crc_1 = aeclmg_calcCrc(aeclmg_buffer, 2);
      crc_2 = aeclmg_calcCrc((aeclmg_buffer+3), 2);
      if((crc_1 == aeclmg_buffer[2]) && (crc_2 == aeclmg_buffer[5])){
        data_buff[3] = aeclmg_buffer[0];
        data_buff[2] = aeclmg_buffer[1];
        data_buff[1] = aeclmg_buffer[3];
        data_buff[0] = aeclmg_buffer[4];
        memcpy(&float_buffer, data_buff, sizeof(float));
        *r_data = float_buffer;
      }
      else {
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

INT16 write_and_read_gas_cal_a(UINT8 address, float w_data, float* r_data) {

  UINT8 aeclmg_buffer[6];
  UINT8 crc_1, crc_2;
  float float_buffer = 0.0;
  UINT8 txData[9];
  UINT8 status;
  UINT8 data_buff[sizeof(float)];
  
  memcpy(data_buff, &w_data, sizeof(float));
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_CAL_A;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (data_buff[3]);
  txData[4] = (data_buff[2]);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (data_buff[1]);
  txData[7] = (data_buff[0]);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 9);

  if(status == 0) {
    delay_ms(300);
    
    txData[0] = CMD_READ_HEAD;
    txData[1] = CMD_READ_CAL_A;
    crc_1 = aeclmg_calcCrc(txData, 2);
    txData[2] = crc_1;
    status = eusci_b_i2c_tx_multi(address, txData, 3);
    
    delay_ms(1);
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
      crc_1 = aeclmg_calcCrc(aeclmg_buffer, 2);
      crc_2 = aeclmg_calcCrc((aeclmg_buffer+3), 2);
      if((crc_1 == aeclmg_buffer[2]) && (crc_2 == aeclmg_buffer[5])){
        data_buff[3] = aeclmg_buffer[0];
        data_buff[2] = aeclmg_buffer[1];
        data_buff[1] = aeclmg_buffer[3];
        data_buff[0] = aeclmg_buffer[4];
        memcpy(&float_buffer, data_buff, sizeof(float));
        *r_data = float_buffer;
      }
      else {
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

INT16 write_and_read_gas_cal_b(UINT8 address, INT32 w_data, INT32* r_data) {

  UINT8 aeclmg_buffer[6];
  UINT8 checksum;
  UINT8 crc;
  UINT16 tempData[4];
  UINT8 txData[9];
  UINT8 status;
  
  txData[0] = CMD_WRITE_HEAD;
  txData[1] = CMD_WRITE_CAL_B;
  txData[2] = aeclmg_calcCrc(txData, 2);
  
  txData[3] = (w_data >> 24);
  txData[4] = (w_data >> 16);
  txData[5] = aeclmg_calcCrc((txData+3), 2);
  
  txData[6] = (w_data >> 8);
  txData[7] = (w_data);
  txData[8] = aeclmg_calcCrc((txData+6), 2);
  
  status = eusci_b_i2c_tx_multi(address, txData, 9);
  
  if(status == 0) {
    delay_ms(300);
    txData[0] = CMD_READ_HEAD;
    txData[1] = CMD_READ_CAL_B;
    crc = aeclmg_calcCrc(txData, 2);
    txData[2] = crc;
    status = eusci_b_i2c_tx_multi(address, txData, 3);
    
    delay_ms(1);
    status = eusci_b_i2c_rx_multi(address, aeclmg_buffer, 6);
    
    if(status == 0) {
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
          //tempData[i/3] = 0xffff; // max value indicate crc error
          return ERROR_CHECKSUM;
        }
      }
      tempData[3] = 0x00; //Clear invalid values  
      *r_data = (((UINT32) tempData[0]) << 16) +(((UINT32) tempData[1]));
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