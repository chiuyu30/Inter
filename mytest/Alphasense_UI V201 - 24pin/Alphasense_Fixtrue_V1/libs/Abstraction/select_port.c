#include "include.h"
#include <stdio.h>

#define ALL_GAS_NUMBERS           7
#define I2C_TEST_RESULTS          0xA5A5

UINT8 all_gas_address[ALL_GAS_NUMBERS] = {AL_O3_ADDRESS, AL_NO2_ADDRESS, AL_CO_ADDRESS, AL_SO2_ADDRESS, DD7_NH3_ADDRESS, AL_H2S_ADDRESS, PIDAY5_VOC_ADDRESS};
UINT8 port_address[PORT_COUNT];

void Reset_switch(void)
{
  GPIO_setOutputLowOnPin(GPIO_PORT_P4,GPIO_PIN2);
  delay_micro_second(50);
  GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN2);
  delay_micro_second(50);
}

UINT8 control_multiplexer(UINT8 address, UINT8 command)
{
  UINT8 txData[1];
  UINT8 rxData[2];
  UINT8 status, timeout_count = 0;
  
  txData[0] = command;
  
  do
  {
    Reset_switch();
    if(eusci_b_i2c_tx_multi(address, txData, 1)){
      status = 1;
    }
    else{
      status = eusci_b_i2c_rx_multi(address, rxData, 2);
    }
    timeout_count++;
    if(timeout_count >= 2)
      break;
  }while((!(txData[0] == rxData[0])));
  
  return status;
}

//掃描&尋找每個位置的感測器型號，取得相應的i2c位置
void check_port_gas_address(void)  //The funtion needs ms. 
{
  UINT16 i2c_test_results = 0;
  
#ifdef ENABLE_WATCHDOG
  WDT_A_resetTimer(WDT_A_BASE);
#endif
  
  for(UINT8 port_count = 0; port_count <= 23; port_count++){
    delay_micro_second(100);
    if(switch_port(port_count)){
      //Show err code
//      _NOP();
    }
    else{
#ifdef ENABLE_WATCHDOG
  WDT_A_resetTimer(WDT_A_BASE);
#endif
      //Switch successful
      for(UINT8 gas_address = 0; gas_address <= 7; gas_address++){
        if(get_i2c_test_results(all_gas_address[gas_address], &i2c_test_results)){//all_gas_address:8顆氣體感測器的I2C位置
          //Show err code
//          _NOP();
        }
        else{
          //Switch successful
          if(i2c_test_results == I2C_TEST_RESULTS){
            port_address[port_count] = all_gas_address[gas_address];
            i2c_test_results = 0;
            break;
          }
        }
      }
    }
  }
//  _NOP();
}

UINT8 switch_port(UINT8 port)
{
  UINT8 status;
  switch(port)
  {
  case 0:
    status = control_multiplexer(0x71,0x01);
   break;
  case 1:
    status = control_multiplexer(0x71,0x02);
    break;
  case 2:
    status = control_multiplexer(0x71,0x04);
    break;
  case 3:
    status = control_multiplexer(0x71,0x08);
    break;
  case 4:
    status = control_multiplexer(0x72,0x01);
    break;
  case 5:
    status = control_multiplexer(0x72,0x02);
    break;
  case 6:
    status = control_multiplexer(0x72,0x04);
    break;
  case 7:
    status = control_multiplexer(0x72,0x08);
    break;
  case 8:
    status = control_multiplexer(0x73,0x01);
    break;
  case 9:
    status = control_multiplexer(0x73,0x02);
    break;
  case 10:
    status = control_multiplexer(0x73,0x04);
    break;
  case 11:
    status = control_multiplexer(0x73,0x08);
    break;
  case 12:
    status = control_multiplexer(0x75,0x01);
    break;
  case 13:
    status = control_multiplexer(0x75,0x02);
    break;
  case 14:
    status = control_multiplexer(0x75,0x04);
    break;
  case 15:
    status = control_multiplexer(0x75,0x08);
    break;
  case 16:
    status = control_multiplexer(0x76,0x01);
    break;
  case 17:
    status = control_multiplexer(0x76,0x02);
    break;
  case 18:
    status = control_multiplexer(0x76,0x04);
    break;
  case 19:
    status = control_multiplexer(0x76,0x08);
    break;
  case 20:
    status = control_multiplexer(0x77,0x01);
    break;
  case 21:
    status = control_multiplexer(0x77,0x02);
    break;
  case 22:
    status = control_multiplexer(0x77,0x04);
    break;
  case 23:
    status = control_multiplexer(0x77,0x08);
    break;
  default:
    break;
  }
  return status;
}

UINT8 switch_port_sht(UINT8 port)
{
  UINT8 status;
  switch(port)
  {
  case 0:
    status = control_multiplexer(0x70,0x01);
   break;
  case 1:
    status = control_multiplexer(0x70,0x02);
    break;
  case 2:
    status = control_multiplexer(0x70,0x04);
    break;
  case 3:
    status = control_multiplexer(0x70,0x08);
    break;
  default:
    break;
  }
  return status;
}


