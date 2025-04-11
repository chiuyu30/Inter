#include "include.h"

void Reset_switch_init(void)
{
  GPIO_setAsOutputPin(GPIO_PORT_P4,GPIO_PIN2);
}

void peripheral_init(void) {
  eusci_b_i2c_port_init();
  hal_uart_txrx_init();
  Reset_switch_init();
  SHT3X_Init(0x44); // Address: 0x44 = Sensor on EvalBoard connector//沒用，腳位空接
}