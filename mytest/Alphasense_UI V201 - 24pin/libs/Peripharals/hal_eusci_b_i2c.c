/*******************************************************************************
oooooooooooo               .ooooooo.
`888'     `8              d8P'   `YP
 888          'YI.oooo.   d8P          .ooooo.  'YI.oooo.   .oooooo.   .ooooo.
 888oooo8      88'  'd8   Y8P'88bo.   d8'   `8b  88'  'd8  'Y8'   `   d8'   `8b
 888    "      88    88        `Y8b  d888oooo88b 88    88   Y8P'boo. d888oooo88b 
 888       o   88    88  .o     d8P '8'          88    88       '8Y' '8'
o888ooooood8  o88o   88o `Y8oood8P'   "bP88Y8P" o88o   88o `Y8oodP"   "bP88Y8P"
*******************************************************************************/
//******************************************************************************
//  Microcontroller: MSP430F6736
//  32kHz ACLK, 20MHZ SMCLK
//
//  Description: EUSCI_B I2C driver
//
//  Company: Autotronic Enterprise CO., LTD.
//  Department: Production Application Division
//  Author: SY LAU
//  Create Date: 2016/8/12
//  IAR Embedded Workbench Version: 6.30
//*****************************************************************************
#include "include.h"

#define Retry_Times         2
#define RXTX_BUF_SIZE       64

//Error flag for SCL low timeout
uint8_t flag_clock_low_timeout_fired = 0;

//Buffer for TXRX
static UINT8 rxTxBuf[RXTX_BUF_SIZE];

UINT8 rxCount = 0;  // number of bytes currently received
UINT8 GB_u8rxLength = 0;   // length of rx bytes to receive
UINT8 txCount = 0;  // number of bytes currently transmitted 
UINT8 GB_u8txLength = 0;   // length of tx bytes to receive

UINT32 GB_u32timeout = 1000;
//For NAK retry times
UINT8  GB_u8Retrytimes = 0;
//Failed for timeout or times of NACK more than set 
bool   GB_b1flag_tx_fail = 0;
bool   GB_b1flag_rx_fail = 0;
bool   GB_b1flag_nak = 0;

//Flag for txrx done
bool GB_b1flag_tx_done = 0;
bool GB_b1flag_rx_done = 0;

// initial UCB0_I2C pin
void eusci_b_i2c_port_init() {
  // set P2.0,P2.1 to GPIO and output direction
  P2SEL &= ~(BIT0 + BIT1);//關閉周邊功能
  P2DIR |= (BIT0 + BIT1);//p2.0 2.1設為輸出
  P2REN &= ~(BIT0 + BIT1);//關閉上拉/下拉電阻，設為輸入時才需要開啟
  // set P2.0, P2.1 to high
  P2OUT |= (BIT0 + BIT1);//兩個輸出腳位都拉為high
  // toggle SCL line (P2.0) for more than 9 times, init SHT3x interface reset
  for(int i=0;i<15;i++) {
    P2OUT &= ~(BIT0);
    delay_micro_second(50);
    P2OUT |= (BIT0);
  } 
  delay_micro_second(100);
  
  // Setup P2.0,P2.1 to Multiple function(I2C)
  P2DIR &= ~(BIT0 + BIT1);// 設為輸入
  P2SEL |= (BIT0 + BIT1);//設為周邊
  UCB0CTLW0 |= UCSWRST;                             // Enable SW reset
  UCB0CTLW0 |= UCMST | UCMODE_3 | UCSSEL_2;         // I2C Master, synchronous mode
  UCB0BRW_L = 200;                                  // fSCL = SMCLK/50 = 400kHz; fSCL = SMCLK/200 = 100kHz
  UCB0BRW_H = 0;
  UCB0CTLW1 |= UCCLTO_3;                            // config clock low timeout, 11b = 165 000 MODCLK cycles (~34 ms)
  // 2020/07/06 SY, disable and clear all interrupt flag for reset
  UCB0IE &= ~(UCTXIE0 | UCRXIE0 | UCNACKIE | UCCLTOIE);
  UCB0IFG &= 0xC0;
  // 2020/07/06 SY  end//
  UCB0CTLW0 &= ~UCSWRST;                            // Clear SW reset, resume operation
  
  // Enable TX interrupt
  UCB0IE |= UCTXIE0 | UCRXIE0 | UCNACKIE | UCCLTOIE;
}

UINT8 temp;
UINT8 txCurrentByte;
// i2c tx multiple bytes, return 1 if tx non-ack
bool eusci_b_i2c_tx_multi(UINT8 i2c_address, UINT8 *pTxBuf, UINT8 txLen) {
  
  GB_b1flag_nak = 0;
  GB_b1flag_tx_fail = 0;
  GB_u8Retrytimes = 0;
  GB_b1flag_tx_done = 0;
  GB_u8txLength = txLen;
  
  while (UCB0CTLW0 & UCTXSTP);
  
  //Specify slave address
  UCB0I2CSA = i2c_address;
  
  // copy buffer
  for(int i=0;i<GB_u8txLength;i++) {
    rxTxBuf[i] = pTxBuf[i];
  }
  
  // set transmit counter to zero
  txCount = 0;
  // send first byte
  
  // I2C TX, start condition
  UCB0CTLW0 |= UCTR | UCTXSTT;      
  
  GB_u32timeout = 10000;
  
  // wait until tx done
  while((GB_b1flag_tx_done == 0) && --GB_u32timeout){
    if(GB_b1flag_nak){
      GB_b1flag_nak = 0;
      //Try N times
      if(GB_u8Retrytimes < Retry_Times){
        txCount = 0;
        UCB0CTLW0 |= UCTR | UCTXSTT;
        GB_u8Retrytimes++;
      }
      //TX_failed
      else{
        GB_b1flag_tx_fail = 1;
      }
    }
  }
  
  //timeout tx fail
  if(GB_u32timeout == 0) {
    GB_b1flag_tx_fail = 1;
    eusci_b_i2c_port_init();
  }  
  
  return GB_b1flag_tx_fail;
  
}

UINT8 eusci_b_i2c_rx_multi(UINT8 i2c_address, UINT8* rxBuf,  UINT8 rxByteCount) {
  
  GB_b1flag_nak = 0;
  GB_b1flag_rx_fail = 0;
  GB_u8Retrytimes = 0;
  GB_b1flag_rx_done = 0;
  GB_u8rxLength = rxByteCount;
  
  if(rxByteCount > RXTX_BUF_SIZE) {
    for(int i=0;i<RXTX_BUF_SIZE;i++) {
      rxTxBuf[i] = 0xf0+i;
    }
    return -1;
  }
  
  while (UCB0CTLW0 & UCTXSTP);
  
  //Specify slave address
  UCB0I2CSA = i2c_address;
  
  // reset rx byte count
  rxCount = 0;
  UCB0CTLW0 &= ~UCTR;
  UCB0CTLW0 |= UCTXSTT;
  
  GB_u32timeout = 1000;
  
  // wait until all bytes received
  while((GB_b1flag_rx_done == 0) && --GB_u32timeout){
       if(GB_b1flag_nak){
      GB_b1flag_nak = 0;
      //Try N times
      if(GB_u8Retrytimes < Retry_Times){
        rxCount = 0;
        UCB0CTLW0 &= ~UCSWRST;
        UCB0CTLW0 |= UCTXSTT;
        GB_u8Retrytimes++;
      }//RX_failed
      else{
        GB_b1flag_rx_fail = 1;
      }
    }
  };  
  
  //timeout rx fail
 if(GB_u32timeout == 0) {
    GB_b1flag_rx_fail = 1;
    eusci_b_i2c_port_init();
  }  
  
  if(GB_b1flag_rx_done==1){
      for(int i=0;i<rxByteCount;i++) {
      *(rxBuf+i) = rxTxBuf[i];
    }
  }
  
  return GB_b1flag_rx_fail;
  
}

//------------------------------------------------------------------------------
// The USCIAB0TX_ISR is structured such that it can be used to transmit any
// number of bytes by pre-loading TXByteCtr with the byte count. Also, TXData
// points to the next byte to transmit.
//------------------------------------------------------------------------------
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_B0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_B0_VECTOR)))
#endif
void USCIB0_ISR(void)
{
  switch(__even_in_range(UCB0IV,0x1E))
  {
  case 0x00: break;       // Vector 0: No interrupts break;
  case 0x02: break;       // Vector 2: ALIFG break;
  case 0x04:
      GB_b1flag_nak = 1;
    break;           // Vector 4: NACKIFG break;
  case 0x06: break;       // Vector 6: STT IFG break;
  case 0x08: break;       // Vector 8: STPIFG break;
  case 0x0a: break;       // Vector 10: RXIFG3 break;
  case 0x0c: break;       // Vector 14: TXIFG3 break;
  case 0x0e: break;       // Vector 16: RXIFG2 break;
  case 0x10: break;       // Vector 18: TXIFG2 break;
  case 0x12: break;       // Vector 20: RXIFG1 break;
  case 0x14: break;       // Vector 22: TXIFG1 break;
  case 0x16:// Vector 24: RXIFG0 break;
    if(rxCount < (GB_u8rxLength - 1)) {
        GB_u32timeout = 1000;
        rxTxBuf[rxCount++] = UCB0RXBUF;
        if(rxCount == (GB_u8rxLength - 1))
            UCB0CTLW0 |= UCTXSTP;
    }
    else{
        rxTxBuf[rxCount] = UCB0RXBUF;
        GB_b1flag_rx_done = 1;
        GB_b1flag_rx_fail = 0;
    }
    break;                                                 
  case 0x18:// Vector 26: TXIFG0 break;
    if(txCount < GB_u8txLength) {
        UCB0TXBUF = rxTxBuf[txCount++];
        GB_u32timeout = 1000;
    }
    else {
        UCB0IFG &= ~UCTXIFG;                  // Clear USCI_B0 TX int flag
        UCB0CTL1 |= UCTXSTP;                  // I2C stop condition
        GB_b1flag_tx_done = 1;
        GB_b1flag_tx_fail = 0;
    }
    break;                                      
   case USCI_I2C_UCCLTOIFG: 
      flag_clock_low_timeout_fired = 1;
       break;       // Vector 22: TXIFG1 break;
  default: break;
  }
}

  