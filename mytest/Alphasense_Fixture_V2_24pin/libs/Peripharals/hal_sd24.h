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
#ifndef HAL_SD24_H
#define HAL_SD24_H

void sd24_init();
void sd24_measure();
void sd24_start_conv_single();
void sd24_fill_data_registers();

extern UINT8 flag_sd24_measure_done;

#endif

