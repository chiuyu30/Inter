/*******************************************************************************
*
*   Tasks and timer 
*   
*   IAR EW430 V6.50.1
*   Company: Autotronic Enterprise CO., LTD.
*   Author: Xin.Gan
*   Create Date: 2020/07/27
*
*
*********************************************************************************/
#include "include.h"

//*****************************************************************************
//
// define tasks interval
//
//*****************************************************************************
#define COMPARE_VALUE           32767    // 1second
#define ONE_SECOND_TICK         16
#define FOUR_SECOND_TICK        64
#define UPDATE_MODBUS_INFO_TIME 3600
UINT8 ftick_lag_count = 0;




//*****************************************************************************
//
// Tasks flags, setting flags in ISR
//
//*****************************************************************************
//static UINT8 flag_measureSensors = 0;
//static UINT16 flag_modbus_req_elapse_counter = 0; // count last modbus request elapse time
//static UINT16 flag_update_modbus_info = UPDATE_MODBUS_INFO_TIME;  // perform info update at the begining
//static UINT8 flag_measure_outdoor_rht = 1;


//******************************************************************************
//
// Init tasks timer, define interval by COMPARE_VALUE
//
//******************************************************************************
void task_timer_init(void) {
  //Start timer in continuous mode sourced by ACLK
  Timer_A_initContinuousModeParam initContParam = {0};
  initContParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;
  initContParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
  initContParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
  initContParam.timerClear = TIMER_A_DO_CLEAR;
  initContParam.startTimer = false;
  Timer_A_initContinuousMode(TIMER_A1_BASE, &initContParam);
  
  //Initiaze compare mode
  Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,
                                       TIMER_A_CAPTURECOMPARE_REGISTER_0
                                         );
  
  Timer_A_initCompareModeParam initCompParam = {0};
  initCompParam.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
  initCompParam.compareInterruptEnable =
    TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
  initCompParam.compareOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;
  initCompParam.compareValue = COMPARE_VALUE;
  Timer_A_initCompareMode(TIMER_A1_BASE, &initCompParam);
  
  Timer_A_startCounter(TIMER_A1_BASE,
                       TIMER_A_CONTINUOUS_MODE
                         );
}


void task_run(void)
{
  if(ftick_lag_count >= 1)
  {
    ftick_lag_count = 0;
    measure_all_rht();
    measure_gases_data();
    ui_parameter_post();
  }
  
  if(ftick_lag_count >= 1)
  {
    ftick_lag_count = 0;
    measure_gases_info();//執行超過1s->回傳現在狀態
//    ui_parameter_post();
  }
  
  //Write funtion
  
}

//******************************************************************************
//
//This is the TIMER1_A0 interrupt vector service routine.
//
//******************************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER1_A0_VECTOR)))
#endif
void TIMER1_A0_ISR(void)
{ 
  uint16_t compVal = Timer_A_getCaptureCompareCount(TIMER_A1_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0)
    + COMPARE_VALUE;
  //Add Offset to CCR0
  Timer_A_setCompareValue(TIMER_A1_BASE,
                          TIMER_A_CAPTURECOMPARE_REGISTER_0,
                          compVal
                            );
  ftick_lag_count++;
}