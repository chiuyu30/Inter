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
#define COMPARE_VALUE           32767    // 16HZ
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
  Timer_A_initContinuousModeParam initContParam = {0};                      //Timer_A_initContinuousModeParam是struct類別，initContParam = {0}可將initContParam內的所有成員初始化為0
  initContParam.clockSource = TIMER_A_CLOCKSOURCE_ACLK;                     //ACLK <- XT1CLK，32768Hz，16bit
  initContParam.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;         //divider = 1，16bit
  initContParam.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE; //如果功能打開(enable)，當timer_a計數器超過最大值時會自動產生中斷請求，跳轉到(ISR)，16bit
  initContParam.timerClear = TIMER_A_DO_CLEAR;//
  initContParam.startTimer = false;
  Timer_A_initContinuousMode(TIMER_A1_BASE, &initContParam);                //預設updown mode
  
  //Initiaze compare mode
  Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,
                                       TIMER_A_CAPTURECOMPARE_REGISTER_0    //reset TA1CCTL0的CCIFG(CCR0 interrupt flag)，TIMER_A_CAPTURECOMPARE_REGISTER_0 = TA1CCTL0(Capture/Compare Control)
                                         );
  
  Timer_A_initCompareModeParam initCompParam = {0};
  initCompParam.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
  initCompParam.compareInterruptEnable =
    TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;                                //開啟CAPTURECOMPARE中斷
  initCompParam.compareOutputMode = TIMER_A_OUTPUTMODE_OUTBITVALUE;         //輸出模式設為OUTBIT，RIGISTER的OUTPUT固定     
  initCompParam.compareValue = COMPARE_VALUE;                               //設定TA1CCR0
  Timer_A_initCompareMode(TIMER_A1_BASE, &initCompParam);                   //自動將CAP清0(設為COMPARE MODE)
  
  Timer_A_startCounter(TIMER_A1_BASE,
                       TIMER_A_CONTINUOUS_MODE                              //START TIMER_A1, SET COUNTINUOUS_MODE(TA1CTL0.MC = 10b)
                         );
}


void task_run(void)
{
  if(ftick_lag_count >= 1)
  {
    ftick_lag_count = 0;
    measure_all_rht();
    measure_gases_data();
//    ui_parameter_post();
  }
  
  if(ftick_lag_count >= 1)
  {
    ftick_lag_count = 0;
    measure_gases_info();
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
#pragma vector=TIMER1_A0_VECTOR                                             //在IAR編譯時將以下函式綁定TIMER1_A0_VECTOR中斷向量
__interrupt                                                                 //指定以下函式為中斷服務函式(ISR)，在CPU進入中斷前會自動將PC(Program Counter)存入其他暫存器，ISR結束後會自動執行RETI)返回中斷處
#elif defined(__GNUC__)
__attribute__((interrupt(TIMER1_A0_VECTOR)))
#endif
void TIMER1_A0_ISR(void)
{ 
  uint16_t compVal = Timer_A_getCaptureCompareCount(TIMER_A1_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0)
    + COMPARE_VALUE;                                                        //每次中斷都在CCR0加上COMPARE_VALUE(發生溢位也沒關係)，確保每次觸發時間間隔COMPARE_VALUE個週期。EX:TIME1->CCR0=32767，TIME2->CCR0=65534，TIME3->CCR0=32765
  //Add Offset to CCR0
  Timer_A_setCompareValue(TIMER_A1_BASE,
                          TIMER_A_CAPTURECOMPARE_REGISTER_0,
                          compVal
                            );
  ftick_lag_count++;
}