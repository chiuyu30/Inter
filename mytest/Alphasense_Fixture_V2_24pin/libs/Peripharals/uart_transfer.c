#include "include.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>

UINT8 A0_RxBuf[A0_RX_BUF_SIZE];
UINT8 RxA0_write_counter = 0;
UINT8 RxA0_read_counter = 0;
char ui_cmd_buffer[UI_CMD_BUF_SIZE];
bool ui_cmd_line = 0;
bool ui_mode = false;               //choose ui or modbus  true = ui mode false = modbus   
bool chk_ui_chk_ec_probe = false;       //check ec probetype 
char ui_ec_probe_type =  ATLAS_EC_PROBE_UNKNOW;
UINT8 solar_modbus_RxBuf[16];                                   //uart receive buffer
UINT8 solar_modbus_rx_bytecount = 0; 
UINT8 solar_flag_modbusRequestReceived = 0;

#define SIZE_OF_MODBUS_REQUEST       120
#define MODBUS_TOTAL_SIZE_OF_INFO_FRAME 6
UINT8 solar_modbus_request[SIZE_OF_MODBUS_REQUEST];
UINT8 solar_responseDataCount = 3;              //default error response (3 bytes)
UINT16 solar_modbus_request_length = 8;

UINT16 solar_modbusInterFrameTimeOut = 0;
UINT16 solar_modbusInterCharTimeOut = 0;
UINT8 solar_modbusFrameDelayTimerFired = 0;

void uart_a0_init(void)
{    

   //baudrate 9600===================================
  // Setup P1.2 UCA0RXD, P1.3 UCA0TXD
  P1SEL |= BIT4 | BIT5;                   // Set P1.2, P1.3 to non-IO
  P1DIR |= BIT4 | BIT5;                   // Enable UCA0RXD, UCA0TXD

  // Setup eUSCI_A0
  UCA1CTLW0 |= UCSWRST;                   // **Put state machine in reset**
  UCA1CTLW0 |= UCSSEL_2;//UCSSEL__SMCLK; //                  // SMCLK
  
  // Baud rate 9600 for 20MHz
  UCA1BRW = 130;//130 
  UCA1MCTLW = 0x2500 + UCBRF_3 + UCOS16;  //0xD600;  // Modln UCBRSx=0xB5, UCBRFx=3, over sampling
  
  UCA1CTLW0 &= ~UCSWRST;                  // **Initialize USCI state machine**
  UCA1IE |= UCRXIE;                       // Enable USCI_A0 RX interrupt
  __delay_cycles(60000);


//  //printf("uart init\r\n");
//    ui_print("dbg port init\r\n\r\n");

}

void ui_parse_cmd(char *command)
{
          if(strncmp(command,"uoff",4) == 0) // ui off
        {
            ui_mode = false;                            
            UCA2IE |= UCRXIE;                       // Enable USCI_A0 RX interrupt
        }
        else if(strncmp(command,"saveset",8) == 0)
        {

        }
}
void ui_command_check(void)
{
  volatile unsigned char i,cmd_len;
  if(ui_cmd_line == true)
    {
        ui_parse_cmd(ui_cmd_buffer);

        ui_cmd_line = false;
    }
}

void chk_ui_cmd(void)
{
    unsigned char ui_chk_get = 0;
    static UINT8  ui_cmd_buf_writer = 0;

    if((RxA0_read_counter != RxA0_write_counter))
    {
        ui_cmd_buffer[ui_cmd_buf_writer] =  A0_RxBuf[RxA0_read_counter];
        ui_chk_get = ui_cmd_buffer[ui_cmd_buf_writer];
        ui_cmd_buf_writer++;
        RxA0_read_counter++;

        if((ui_chk_get == '\r') || (ui_chk_get == '\n'))
        {
            ui_cmd_line = true;
            ui_command_check();
            ui_cmd_buf_writer = 0;
            //cmd_buf_writer = 0;
        }
    }
}   





// Timer0_A1 Interrupt Vector (TAIV) handler
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
{
  switch (__even_in_range(TA0IV, 14))
  {
  case  TA0IV_NONE: break;             // No interrupt
  case  TA0IV_TA0CCR1:                     
    TA0CTL = TASSEL_1 + MC_0 + TACLR;         // ACLK, stop, clear TAR
    TA0CCTL1 &= ~CCIE;                        // disable CCR1 interrupt
    if(solar_modbus_rx_bytecount >= 16) {
      solar_modbus_rx_bytecount = 16;
    }
    for(int i=0;i<solar_modbus_rx_bytecount;i++) solar_modbus_request[i] = solar_modbus_RxBuf[i];
    
    solar_modbus_request_length = solar_modbus_rx_bytecount;
    solar_modbus_rx_bytecount = 0;

    TA0CCR0 = solar_modbusInterFrameTimeOut;    // frame delay timer
    TA0CCTL0 = CCIE;                      // enable CCR2 interrupt
    solar_modbusFrameDelayTimerFired = 0;
    TA0CTL = TASSEL_1 + MC_2;         // ACLK, stop, clear TAR
//    solar_modbus_reply(); 
//    TOGGLE_GLED();
    break;
  case  4: break;                      // Reserved
  case  6: break;                      // Reserved
  case  8: break;                      // Reserved
  case 10: break;                      // Reserved
  case 12: break;                      // Reserved
  case TA0IV_TA0IFG:                   // TA0IFG  
  default: break;
  }
}

