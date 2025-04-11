/*******************************************************************************
*
*   Alphasense Fixture Main 
*   
*   IAR EW430 V6.50.1
*   Company: Autotronic Enterprise CO., LTD.
*   Author: Xin.Gan
*   Create Date: 2020/07/27
*
*
*********************************************************************************/
#include "include.h"



void main(void)
{
  //Stop WDT
  WDT_A_hold(WDT_A_BASE);
  
  system_init();
  
  peripheral_init();
  
  task_timer_init();

  __bis_SR_register(GIE);  // Enable global interrupt
  
  check_port_gas_address();
  
#ifdef ENABLE_WATCHDOG
  // start watchdog timer, 3s expire time with VLOCLK (10KHz)
  WDT_A_initWatchdogTimer(WDT_A_BASE, WDT_A_CLOCKSOURCE_VLOCLK, WDT_A_CLOCKDIVIDER_32K);
  WDT_A_start(WDT_A_BASE);
#endif 
  
  while (1)
  {
    task_run();
  }
}
