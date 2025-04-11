//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 44, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHT3x Sample Code (V1.0)
// File      :  i2c_hal.c (V1.0)
// Author    :  RFU
// Date      :  16-Jun-2014
// Controller:  STM32F100RB
// IDE       :  µVision V4.71.2.0
// Compiler  :  Armcc
// Brief     :  I2C hardware abstraction layer
//==============================================================================

//-- Includes ------------------------------------------------------------------
#include "include.h"

//-- Defines -------------------------------------------------------------------
// I2C IO-Pins                         /* -- adapt the defines for your uC -- */
#define SHT3X_SDA_PIN    2
#define SHT3X_SCL_PIN    3

#define SHT3X_PORT_IN   P3IN
#define SHT3X_PORT_OUT  P3OUT
#define SHT3X_PORT_SEL  P3SEL
#define SHT3X_PORT_DIR  P3DIR

#define SHT3X_SCL  BV(SHT3X_SCL_PIN)
#define SHT3X_SDA  BV(SHT3X_SDA_PIN)

// SDA on port B, bit 14
#define SHT3X_SDA_LOW()  do{SHT3X_PORT_DIR |= SHT3X_SDA; SHT3X_PORT_OUT &= ~SHT3X_SDA;}while(0)    // set SDA to low
#define SHT3X_SDA_OPEN() SHT3X_PORT_DIR &= ~SHT3X_SDA         //Input Pull-up mode
#define SHT3X_SDA_READ   ((SHT3X_PORT_IN&SHT3X_SDA) >> SHT3X_SDA_PIN)     // read SDA

// SCL on port B, bit 13               /* -- adapt the defines for your uC -- */
#define SHT3X_SCL_LOW()  do{SHT3X_PORT_DIR |= SHT3X_SCL; SHT3X_PORT_OUT &= ~SHT3X_SCL;}while(0) // set SCL to low
#define SHT3X_SCL_OPEN() SHT3X_PORT_DIR &= ~SHT3X_SCL    // set SCL to open-drain
#define SHT3X_SCL_READ   ((SHT3X_PORT_IN&SHT3X_SCL) >> SHT3X_SCL_PIN)     // read SCL

//==============================================================================
void sht3x_i2c_Init(void){                      /* -- adapt the init for your uC -- */
//==============================================================================
  SHT3X_PORT_SEL &= ~(SHT3X_SCL + SHT3X_SDA);   //set pin as GPIO
  
  SHT3X_SDA_OPEN();                  // I2C-bus idle mode SDA released
  SHT3X_SCL_OPEN();                  // I2C-bus idle mode SCL released
  
}

//==============================================================================
void sht3x_i2c_StartCondition(void){
//==============================================================================
  SHT3X_SDA_OPEN();
  delay_micro_second(1);
  SHT3X_SCL_OPEN();
  delay_micro_second(1);
  SHT3X_SDA_LOW();
  delay_micro_second(10);  // hold time start condition (t_HD;STA)
  SHT3X_SCL_LOW();
  delay_micro_second(10);
}

//==============================================================================
void sht3x_i2c_StopCondition(void){
//==============================================================================
  SHT3X_SCL_LOW();
  delay_micro_second(1);
  SHT3X_SDA_LOW();
  delay_micro_second(1);
  SHT3X_SCL_OPEN();
  delay_micro_second(10);  // set-up time stop condition (t_SU;STO)
  SHT3X_SDA_OPEN();
  delay_micro_second(10);
}

//==============================================================================
etError sht3x_i2c_WaitWhileClockStreching(u8t timeout){
//==============================================================================
	etError error = NO_ERROR;
	
  while(SHT3X_SCL_READ == 0)
  {
    if(timeout-- == 0)	return TIMEOUT_ERROR;
    delay_micro_second(1000);
  }
	
	return error;
}

//==============================================================================
etError sht3x_i2c_WriteByte(u8t txByte){
//==============================================================================
  etError error = NO_ERROR;
  u8t     mask;
  for(mask = 0x80; mask > 0; mask >>= 1)// shift bit for masking (8 times)
  {
    if((mask & txByte) == 0) SHT3X_SDA_LOW(); // masking txByte, write bit to SDA-Line
    else                     SHT3X_SDA_OPEN();
    delay_micro_second(1);               // data set-up time (t_SU;DAT)
    SHT3X_SCL_OPEN();                         // generate clock pulse on SCL
    delay_micro_second(5);               // SCL high time (t_HIGH)
    SHT3X_SCL_LOW();
    delay_micro_second(1);               // data hold time(t_HD;DAT)
  }
  SHT3X_SDA_OPEN();                           // release SDA-line
  SHT3X_SCL_OPEN();                           // clk #9 for ack
  delay_micro_second(1);                 // data set-up time (t_SU;DAT)
  if(SHT3X_SDA_READ) error = ACK_ERROR;       // check ack from i2c slave
  SHT3X_SCL_LOW();
  delay_micro_second(20);                // wait to see byte package on scope
  return error;                         // return error code
}

//==============================================================================
etError sht3x_i2c_ReadByte(u8t *rxByte, etI2cAck ack, u8t timeout){
//==============================================================================
  etError error = NO_ERROR;
  u8t mask;
  *rxByte = 0x00;
  SHT3X_SDA_OPEN();                            // release SDA-line
  for(mask = 0x80; mask > 0; mask >>= 1) // shift bit for masking (8 times)
  { 
    SHT3X_SCL_OPEN();                          // start clock on SCL-line
    delay_micro_second(1);                // clock set-up time (t_SU;CLK)
		error = sht3x_i2c_WaitWhileClockStreching(timeout);// wait while clock streching
    delay_micro_second(3);                // SCL high time (t_HIGH)
    if(SHT3X_SDA_READ) *rxByte |= mask;        // read bit
    SHT3X_SCL_LOW();
    delay_micro_second(1);                // data hold time(t_HD;DAT)
  }
  if(ack == ACK) SHT3X_SDA_LOW();              // send acknowledge if necessary
  else           SHT3X_SDA_OPEN();
  delay_micro_second(1);                  // data set-up time (t_SU;DAT)
  SHT3X_SCL_OPEN();                            // clk #9 for ack
  delay_micro_second(5);                  // SCL high time (t_HIGH)
  SHT3X_SCL_LOW();
  SHT3X_SDA_OPEN();                            // release SDA-line
  delay_micro_second(20);                 // wait to see byte package on scope
	
	return error;                          // return with no error
}



//==============================================================================
etError sht3x_i2c_GeneralCallReset(void){
//==============================================================================
	etError error;
	
	sht3x_i2c_StartCondition();
	                      error = sht3x_i2c_WriteByte(0x00);
	if(error == NO_ERROR) error = sht3x_i2c_WriteByte(0x06);
	
	return error;
}
