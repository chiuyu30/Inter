/*******************************************************************************
        .o.       oooooooooooo   .oooooo.   ooooo        
       .888.      `888'     `8  d8P'  `Y8b  `888'        
      .8"888.      888         888           888         
     .8' `888.     888oooo8    888           888         
    .88ooo8888.    888    "    888           888         
   .8'     `888.   888       o `88b    ooo   888       o 
  o88o     o8888o o888ooooood8  `Y8bood8P'  o888ooooood8 
*********************************************************************************/
//******************************************************************************
//  Microcontroller: MSP430F6726
//  32kHz ACLK, 20MHZ SMCLK
//
//  Description: SD24 driver
//
//  Company: Autotronic Enterprise CO., LTD.
//  Department: Production Application Division
//  Author: SY LAU
//  Create Date: 2017/12/2
//  IAR Embedded Workbench Version: 6.30
//*****************************************************************************
#include "include.h"

#define BOARD_SD24_DATA_SIZE          2

uint32_t results[3];
UINT16 sd24_board_V;
INT16 sd24_board_A;

UINT8 flag_sd24_measure_done = 0;
UINT8 flag_sd24_avg_init = 0;

////////////////////////////////////////////////////////////////////////////////
// Fill Modbus data registers and reset average data/flag
// This routine is called inside modbus_reply_04(),
// V/A value average until this routine is called
// After modbus data registers filled, reset buffer and re-init error code
// 
// Error handling: 
//      If it is not called after 1 min (Modbus master didn't request data)
//      sd24_reset_avg_data() must be called in task timer ISR, 
//      to reset buffer and error code to avoid long averaging
//
// Note: 
//      sd24_fill_data_registers() called in modbus_reply_04() (inside ISR)
//
////////////////////////////////////////////////////////////////////////////////
void sd24_reset_avg_data() {
  sd24_board_V = 0;
  sd24_board_A = 0;
  flag_sd24_avg_init = 1;
}

INT16 sd24_data[2];
void sd24_fill_data_registers() {
  
  sd24_data[0] = sd24_board_V;
  sd24_data[1] = sd24_board_A;
//  modbus_data_frame_update(sd24_data, MODBUS_OFFSET_BOARD_DATA, MODBUS_SIZE_OF_BOARD_DATA);
  
  sd24_reset_avg_data();
}

void sd24_init() {
  SD24_B_initParam initParam = {0};
  initParam.clockSourceSelect = SD24_B_CLOCKSOURCE_SMCLK;
  initParam.clockPreDivider = SD24_B_PRECLOCKDIVIDER_1;
  initParam.clockDivider = SD24_B_CLOCKDIVIDER_1;
  initParam.referenceSelect = SD24_B_REF_INTERNAL;
  SD24_B_init(SD24_BASE, &initParam);

  SD24_B_initConverterParam initConverterParam0 = {0};
  initConverterParam0.converter = SD24_B_CONVERTER_0;
  initConverterParam0.alignment = SD24_B_ALIGN_LEFT;
  initConverterParam0.startSelect = SD24_B_CONVERSION_SELECT_GROUP1;
  initConverterParam0.conversionMode = SD24_B_CONTINUOUS_MODE;
  SD24_B_initConverter(SD24_BASE, &initConverterParam0);

  SD24_B_initConverterParam initConverterParam1 = {0};
  initConverterParam1.converter = SD24_B_CONVERTER_1;
  initConverterParam1.alignment = SD24_B_ALIGN_LEFT;
  initConverterParam1.startSelect = SD24_B_CONVERSION_SELECT_GROUP1;
  initConverterParam1.conversionMode = SD24_B_CONTINUOUS_MODE;
  SD24_B_initConverter(SD24_BASE, &initConverterParam1);

//  SD24_B_initConverterParam initConverterParam2 = {0};
//  initConverterParam2.converter = SD24_B_CONVERTER_2;
//  initConverterParam2.alignment = SD24_B_ALIGN_RIGHT;
//  initConverterParam2.startSelect = SD24_B_CONVERSION_SELECT_GROUP1;
//  initConverterParam2.conversionMode = SD24_B_CONTINUOUS_MODE;
//  SD24_B_initConverter(SD24_BASE, &initConverterParam2);

  SD24_B_clearInterrupt(SD24_BASE,
                        SD24_B_CONVERTER_1,
                        SD24_B_CONVERTER_INTERRUPT);                       // Clear channel 2 interrupt
  SD24_B_enableInterrupt(SD24_BASE,
                         SD24_B_CONVERTER_1,
                         SD24_B_CONVERTER_INTERRUPT);                      // Enable channel 2 interrupt
  
  sd24_reset_avg_data();   // init data buffer
}
void sd24_start_conv_single() {
  SD24_B_startGroupConversion(SD24_BASE, SD24_B_CONVERTER_1);           // Set bit to start conversion
}


//UINT16 temp_1[16];
//INT16 temp_2[16];
//UINT8 index = 0;
//UINT8 count = 0;
//INT16 sum_current = 0;
void sd24_measure() {
  INT16 current;
  UINT16 voltage;

  if(flag_sd24_avg_init == 1) {
    flag_sd24_avg_init = 0;
    voltage = (results[1]>>16);
    sd24_board_V = voltage;
    current = (results[0]>>16)-(results[1]>>16);
    sd24_board_A = current;
//    count++;
  }
  else {
    voltage = results[1]>>16;
    sd24_board_V = (sd24_board_V>>1) + (voltage>>1);
    current = (results[0]>>16)-(results[1]>>16);
    sd24_board_A += current;
    
//    count++;
  }
//  if(count >= 16) {
//    temp_2[index] = sd24_board_A;
//    temp_1[index] = sd24_board_V;
//    index++;
//    sum_current += sd24_board_A;
//    count = 0;
////    TOGGLE_RLED();
//    sd24_reset_avg_data();
//    if(index >= 16) {
//      index = 0;
//      sum_current = 0;
//    }
//  }
}





#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SD24B_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(SD24B_VECTOR)))
#endif
void SD24BISR(void)
{
    switch(SD24BIV)
    {
    case SD24BIV_SD24OVIFG:                 // SD24MEM Overflow
        break;
    case SD24BIV_SD24TRGIFG:                // SD24 Trigger IFG
        break;
    case SD24BIV_SD24IFG0:                  // SD24MEM0 IFG
        break;
    case SD24BIV_SD24IFG1:                  // SD24MEM1 IFG
      results[0] = SD24_B_getResults(SD24_BASE, SD24_B_CONVERTER_0);                             // Save CH0 results (clears IFG)
      results[1] = SD24_B_getResults(SD24_BASE, SD24_B_CONVERTER_1);                             // Save CH1 results (clears IFG)
      flag_sd24_measure_done = 1;
      SD24_B_stopGroupConversion(SD24_BASE, SD24_B_CONVERTER_1);            // Clear bit for next conversion
      break;
        break;
    }
//    __bic_SR_register_on_exit(LPM0_bits);   // Exit LPM0
}

