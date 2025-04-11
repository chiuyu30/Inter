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
//  Description: transmit data from UART
//
//  Company: Autotronic Enterprise CO., LTD.
//  Department: Production Application Division
//  Author: SY LAU
//  Create Date: 2016/8/14
//  IAR Embedded Workbench Version: 6.30
//*****************************************************************************
#ifndef HAL_UART_TXRX_H
#define HAL_UART_TXRX_H

void hal_uart_txrx_init(void);
void EUSCI_A_UART_transmitBuf(char *pTxBuf, char txLen);



#endif