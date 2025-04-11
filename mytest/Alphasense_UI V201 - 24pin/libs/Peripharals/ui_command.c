#include "include.h"

#include <stdlib.h>
#include <stdio.h>

UINT8 A0_RxBuf[30];
UINT8 RxA0_write_counter = 0;
UINT8 RxA0_read_counter = 0;
UINT16 Device_num;
UINT64 UI_BSN;
UINT16 UI_Span_range;
UINT64 DebugA;
UINT8 tabpage;


void chk_ui_cmd(void)
{
  if((RxA0_read_counter != RxA0_write_counter))//RxA0_read_counter always=0　＞＞　只要RxA0_write_counter不等於０（uart有收到東西）就會一個一個檢查
  {
    command_Write_BSN();
    command_Wrire_Zero();
    command_Wrire_Span();
    command_tabpage();
    RxA0_read_counter = 0;
  }
}

void RxBuf_CMD_Clear(void){
  RxA0_write_counter = 0;
  memset(A0_RxBuf, 0x00, 30);
}

void command_tabpage(void)
{
   if(A0_RxBuf[0] == 't' && A0_RxBuf[1] == 'a' && A0_RxBuf[2] == 'b' && A0_RxBuf[4] == '\r')
  {
     tabpage = A0_RxBuf[3];
     RxBuf_CMD_Clear();
  }
}

void command_Write_BSN(void)
{
  if(A0_RxBuf[0] == 'W' && A0_RxBuf[1] == 'B' && A0_RxBuf[2] == 'S' && A0_RxBuf[3] == 'N' || A0_RxBuf[19] == '\r' || A0_RxBuf[20] == '\r')
  {
    Device_num = 0;
    if(A0_RxBuf[19] >=0x30 && A0_RxBuf[19]<=0x39)
    {
      Device_num += ((int)A0_RxBuf[18]-48)*10;
      Device_num += (int)A0_RxBuf[19]-48;
    }
    else
    {
      Device_num += (int)A0_RxBuf[18]-48;
    }
    
    UI_BSN += ((int)A0_RxBuf[4]-48)*(1000000000000);//把字串轉成數字
    UI_BSN += ((int)A0_RxBuf[5]-48)*(100000000000);
    UI_BSN += ((int)A0_RxBuf[6]-48)*(10000000000);
    DebugA = ((int)A0_RxBuf[7]-48);
    DebugA = DebugA*1000000000;
    UI_BSN += DebugA;
    UI_BSN += ((int)A0_RxBuf[8]-48)*(100000000);
    UI_BSN += ((int)A0_RxBuf[9]-48)*(10000000);
    UI_BSN += ((int)A0_RxBuf[10]-48)*(1000000);
    UI_BSN += ((int)A0_RxBuf[11]-48)*(100000);
    DebugA = ((int)A0_RxBuf[12]-48);
    DebugA = DebugA*10000;
    UI_BSN += DebugA;
    UI_BSN += ((int)A0_RxBuf[13]-48)*(1000);
    UI_BSN += ((int)A0_RxBuf[14]-48)*(100);
    UI_BSN += ((int)A0_RxBuf[15]-48)*(10);
    UI_BSN += ((int)A0_RxBuf[16]-48)*(1);
//    Device_Select_BSN(Device_num);
    RxBuf_CMD_Clear();
    UI_BSN = 0;
  }
}

void command_Wrire_Zero(void)
{
  if(A0_RxBuf[0] == 'W' && A0_RxBuf[1] == 'Z' && A0_RxBuf[2] == 'e' && A0_RxBuf[3] == 'r' && A0_RxBuf[4] == 'o' && A0_RxBuf[5] == '\r')
  {
//    Correction_Zero();
    RxBuf_CMD_Clear();
  }
}

void command_Wrire_Span(void)
{
  if(A0_RxBuf[0] == 'W' && A0_RxBuf[1] == 'S' && A0_RxBuf[2] == 'p' && A0_RxBuf[3] == 'a' && A0_RxBuf[4] == 'n' || A0_RxBuf[8] == '\r' || A0_RxBuf[9] == '\r')
  {
    if(A0_RxBuf[8] >=0x30 && A0_RxBuf[8]<=0x39)//如果A0_RxBuf[8]
    {
      UI_Span_range += ((int)A0_RxBuf[5]-48)*1000;
      UI_Span_range += ((int)A0_RxBuf[6]-48)*100;
      UI_Span_range += ((int)A0_RxBuf[7]-48)*10;
      UI_Span_range += ((int)A0_RxBuf[8]-48)*1;
    }
    else
    {
      UI_Span_range += ((int)A0_RxBuf[5]-48)*100;
      UI_Span_range += ((int)A0_RxBuf[6]-48)*10;
      UI_Span_range += ((int)A0_RxBuf[7]-48)*1;
    }
    Span_range = UI_Span_range;
//    Correction_Span();
    RxBuf_CMD_Clear();
  }
}

#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)   //USCI_A0 RX interrupt service routine
{
  switch(__even_in_range(UCA1IV,4))
  {
  case USCI_NONE:break;                       // Vector 0 - no interrupt
  case USCI_UART_UCRXIFG:                     // Vector 2 - RXIFG
    TA2CTL = TASSEL_1 + MC_0 + TACLR;         // ACLK, stop, clear TAR
    A0_RxBuf[RxA0_write_counter]= UCA1RXBUF;
    RxA0_write_counter++;
    break;
  case USCI_UART_UCTXIFG: break;      // TXIFG
  case USCI_UART_UCSTTIFG: break;     // TTIFG
  case USCI_UART_UCTXCPTIFG: break;   // TXCPTIFG
  default: break;
  }
}