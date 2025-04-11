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
#include "include.h"


//peripheral_init時調用(main.c)         
void hal_uart_txrx_init(void) {
  //Configure UART pins (UCA0TXD/UCA0SIMO, UCA0RXD/UCA0SOMI)
  //設定指定pin腳為input(PDIR=0)，並開啟周邊模式(PSEL=1)
  GPIO_setAsPeripheralModuleFunctionInputPin(
      GPIO_PORT_P1,
      GPIO_PIN4 + GPIO_PIN5//需要設定特定腳位，部分的pin腳有接到周邊ex:timer、ucxrx/tx、ucxsda/scl...
      );
  
  // Configure UART
  // 57600@20MHz 21,11,34,1
  EUSCI_A_UART_initParam param = {0};
  param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;//source設為SMCLK
  param.clockPrescalar = 21;//57600 = 20MHz/(21+11/16)*16(大約)
  param.firstModReg = 11;
  param.secondModReg = 34;
  param.parity = EUSCI_A_UART_NO_PARITY;//同位偵測off
  param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;//移位暫存器從低位開始送出資料
  param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;//1 bit的stop bit
  param.uartMode = EUSCI_A_UART_MODE;//指定uart mode
  param.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;
  
  if(STATUS_FAIL == EUSCI_A_UART_init(EUSCI_A1_BASE, &param))
  {
      return;
  }
  
  //Enable UART module for operation
  EUSCI_A_UART_enable(EUSCI_A1_BASE);
  
  //Enable Receive Interrupt
  EUSCI_A_UART_clearInterrupt(EUSCI_A1_BASE,
                             EUSCI_A_UART_RECEIVE_INTERRUPT);
  EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE,
                              EUSCI_A_UART_RECEIVE_INTERRUPT); 
}

void EUSCI_A_UART_transmitBuf(char *pTxBuf, char txLen) {
    for(char i = 0; i < txLen;i++)  
    {
      if(pTxBuf[i] == 0x00) {
        break;
      }
      EUSCI_A_UART_transmitData(EUSCI_A1_BASE, pTxBuf[i]);
    }
}

