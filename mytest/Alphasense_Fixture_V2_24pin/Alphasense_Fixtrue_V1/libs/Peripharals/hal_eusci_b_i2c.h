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

#ifndef HAL_EUSCI_B_I2C
#define HAL_EUSCI_B_I2C



extern uint8_t flag_clock_low_timeout_fired;

void eusci_b_i2c_port_init();
bool eusci_b_i2c_tx_single(UINT8 i2c_address, UINT8 txData);
bool eusci_b_i2c_tx_multi(UINT8 i2c_address, UINT8 pTxBuf[], UINT8 txLen);
UINT8 eusci_b_i2c_rx_multi(UINT8 i2c_address, UINT8* rxBuf, UINT8 rxByteCount);
UINT8 * eusci_b_i2c_tx_rx_multi(UINT8 i2c_address, UINT8 command, UINT8 rxByteCount);

#endif