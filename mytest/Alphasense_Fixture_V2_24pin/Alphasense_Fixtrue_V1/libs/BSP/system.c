/*******************************************************************************
*
*   This file contain the system initialization routines
*   
*   1. low level init: stop watchdog timer
*   2. Unified clock system init
*   3. LEDs, switches I/O init
*   4. System timer init
*
*   Company: Autotronic Enterprise CO., LTD.
*   Author: SY LAU
*   Create Date: 2010/2/16
*
*
*********************************************************************************/
#include "include.h"

//*****************************************************************************
//
//Target frequency for MCLK in kHz
//
//*****************************************************************************
#define UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ   20000

//*****************************************************************************
//
//MCLK/FLLRef Ratio
//
//*****************************************************************************
#define UCS_MCLK_FLLREF_RATIO   611

//*****************************************************************************
//
//Variable to store status of Oscillator fault flags
//
//*****************************************************************************
uint16_t status;

void system_init(void) {
  //Set VCore = 3 for 20MHz clock
  PMM_setVCore(PMM_CORE_LEVEL_3);
  
  //Set DCO FLL reference = REFO
  UCS_initClockSignal(
                      UCS_FLLREF,
                      UCS_REFOCLK_SELECT,
                      UCS_CLOCK_DIVIDER_1
                        );
  
  //Set ACLK = REFO
  UCS_initClockSignal(
                      UCS_ACLK,
                      UCS_REFOCLK_SELECT,
                      UCS_CLOCK_DIVIDER_1
                        );
  
  //Set Ratio and Desired MCLK Frequency  and initialize DCO
  //and SMCLK
  UCS_initFLLSettle(
                    UCS_MCLK_DESIRED_FREQUENCY_IN_KHZ,
                    UCS_MCLK_FLLREF_RATIO
                      );
  
  // Enable global oscillator fault flag
  SFR_clearInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
  SFR_enableInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
  
}


  
void LED_init(void) {

}

// delay time is ms
void delay_ms(UINT16 timeout) {
  // 2 * (304 + 1) * 32768 = 19.98848MHz
  do {
    __delay_cycles(20000);   // approximate 1ms
  } while (--timeout);
}




/******************************************************************************
 * @brief        delay micro second (depends on the system clock setting above
 * @param[in]    None
 * @return       None
 * @Note         default system clock 20MHz, 20 cycles/us
 * @update       2014-05-12  
 *****************************************************************************/
void delay_micro_second(UINT32 us) {
  UINT32 i;
  for(i=0;i<us;i++) {
    __delay_cycles(20);//systemclk(MCLK) = 20MHz
  }
}

//*****************************************************************************
//
// ISR for Oscillator fault trap
//
//*****************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=UNMI_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(UNMI_VECTOR)))
#endif
void NMI_ISR(void)//應該要改成UNMI_ISR
{
  do
  {
    // If it still can't clear the oscillator fault flags after the timeout,
    // trap and wait here.
    status = UCS_clearAllOscFlagsWithTimeout(1000);
    //        TOGGLE_RLED();   // inidicate OSC fault
  }
  while(status != 0);
}

