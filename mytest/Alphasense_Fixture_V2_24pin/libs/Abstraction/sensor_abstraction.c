#include "include.h"
#include <stdio.h>

#define CONV_BUF_MAX     20
#define ERROR_NO_NONE    0x01
#define GAS_PORT         24

#define DIMENSIONS       12

#define WE_DIMENSIONS              0
#define AE_DIMENSIONS              1
#define ADC_DIMENSIONS             2
#define FW_DIMENSIONS              3
#define GAS_NAME_DIMENSIONS        4
#define BSN_DIMENSIONS             5
#define ZERO_DIMENSIONS            6
#define SPAN_DIMENSIONS            7
#define A_DIMENSIONS               8
#define B_DIMENSIONS               9
#define CAL_DATA_DIMENSIONS        10
#define CONCENTRATIONS_DIMENSIONS  11


char conv_buf[CONV_BUF_MAX];

INT64 datalink[GAS_PORT][DIMENSIONS]; //24 port , 0:WE, 1:AE, 2:ADC, 3:FW version, 4:Gas name, 5:BSN, 6:ZERO, 7:Span, 8:A, 9:B, 10:

UINT16 fw_version;
UINT16 cal_data;
UINT64 bsn;
UINT64 gas_name;
INT32 zero;
float span;
float a_value;
INT32 b_value;
UINT16 concentrations;

UINT32 we;
UINT32 ae;


UINT8 i,j,k;
UINT8 port_count;
UINT8 Sensor_Name = 0; //1:DD-CO  2:NO2  3:O3  4:NH3  5:H2S 6:AL-CO 7:SO2
UINT16 Span_range;
INT16 SHT3x_RHT[8];



void measure_gases_data(void)
{
#ifdef ENABLE_WATCHDOG
  WDT_A_resetTimer(WDT_A_BASE);
#endif
  datalink_clear();//清空資料暫存
  for(port_count=0; port_count<24; port_count++){
    delay_micro_second(100);
    if(switch_port(port_count)){//跟感測器說現在要對哪一顆做操作
      //Show err code
      _NOP();
    }
    else{
      //Switch successful
      _NOP();
      sensordata_init();//we ae concentrations(濃度)歸零
      gat_all_gas_data(port_count);
//      chk_ui_cmd();
    }
  }
}

void measure_gases_info(void)
{
#ifdef ENABLE_WATCHDOG
  WDT_A_resetTimer(WDT_A_BASE);
#endif
  datalink_clear();
  for(port_count=0; port_count<24; port_count++){
    delay_micro_second(100);
    if(switch_port(port_count)){
      //Show err code
      _NOP();
    }
    else{
      //Switch successful
      _NOP();
      sensorinfo_init();
      get_all_gas_info(port_count);
//      chk_ui_cmd();
    }
  }
}

void gat_all_gas_data(UINT8 port_numbers)
{
#ifdef ENABLE_WATCHDOG
  WDT_A_resetTimer(WDT_A_BASE);
#endif
  //Read WE
  if(aeclmg_getAdc_2(port_address[port_numbers], &we)){//第n個位置的感測器的i2c位置{AL_O3_ADDRESS, AL_NO2_ADDRESS, AL_CO_ADDRESS, AL_SO2_ADDRESS, DD7_NH3_ADDRESS, AL_H2S_ADDRESS, PIDAY5_VOC_ADDRESS}其中一個
    //Show err code
    _NOP();
  }
  else{
    datalink[port_numbers][WE_DIMENSIONS] = we;
#ifdef ENABLE_WATCHDOG
  WDT_A_resetTimer(WDT_A_BASE);
#endif
    //Read AE
    if(aeclmg_getAdc_1(port_address[port_numbers], &ae)){
      //Show err code
      _NOP();
    }
    else{
      datalink[port_numbers][AE_DIMENSIONS] = ae;
      //Read ADC
      datalink[port_numbers][ADC_DIMENSIONS] = (we - ae);
     
#ifdef ENABLE_WATCHDOG
      WDT_A_resetTimer(WDT_A_BASE);
#endif
      //Read concentrations
      if(aeclmg_get_gas_concentrations(port_address[port_numbers], &concentrations)){
        //Show err code
        _NOP();
      }
      else{
        datalink[port_numbers][CONCENTRATIONS_DIMENSIONS] = concentrations;    
      }
    }
  }
}

void get_all_gas_info(UINT8 port_numbers)
{
#ifdef ENABLE_WATCHDOG
  WDT_A_resetTimer(WDT_A_BASE);
#endif
  //Read gas_name
  if(aeclmg_get_gas_name(port_address[port_numbers], &gas_name)){
    //Show err code
    _NOP();
  }
  else{
    datalink[port_numbers][GAS_NAME_DIMENSIONS] = gas_name;
#ifdef ENABLE_WATCHDOG
    WDT_A_resetTimer(WDT_A_BASE);
#endif
    //Read bsn
    if(aeclmg_getSerialId(port_address[port_numbers], &bsn)){
      //Show err code
      _NOP();
    }
    else{
      datalink[port_numbers][BSN_DIMENSIONS] = bsn;
#ifdef ENABLE_WATCHDOG
      WDT_A_resetTimer(WDT_A_BASE);
#endif
      //Read FW
      if(aeclmg_get_fw_version(port_address[port_numbers], &fw_version)){
        //Show err code
        _NOP();
      }
      else{
        datalink[port_numbers][FW_DIMENSIONS] = fw_version;
#ifdef ENABLE_WATCHDOG
        WDT_A_resetTimer(WDT_A_BASE);
#endif
        //Read CAL DATA 
        if(aeclmg_get_cal_date(port_address[port_numbers], &cal_data)){
          //Show err code
          _NOP();
        }
        else{
          datalink[port_numbers][CAL_DATA_DIMENSIONS] = cal_data;
#ifdef ENABLE_WATCHDOG
          WDT_A_resetTimer(WDT_A_BASE);
#endif
          //Read zero 
          if(aeclmg_get_gas_cal_zero(port_address[port_numbers], &zero)){
            //Show err code
            _NOP();
          }
          else{
            datalink[port_numbers][ZERO_DIMENSIONS] = zero;
#ifdef ENABLE_WATCHDOG
            WDT_A_resetTimer(WDT_A_BASE);
#endif
            //Read span 
            if(aeclmg_get_gas_cal_span(port_address[port_numbers], &span)){
              //Show err code
              _NOP();
            }
            else{
              datalink[port_numbers][SPAN_DIMENSIONS] = (UINT16)(span*1000);//Check it
#ifdef ENABLE_WATCHDOG
              WDT_A_resetTimer(WDT_A_BASE);
#endif
              //Read a_value 
              if(aeclmg_get_gas_cal_a(port_address[port_numbers], &a_value)){
                //Show err code
                _NOP();
              }
              else{
                datalink[port_numbers][A_DIMENSIONS] = (UINT16)(a_value*1000);//Check it
#ifdef ENABLE_WATCHDOG
                WDT_A_resetTimer(WDT_A_BASE);
#endif
                //Read b_value 
                if(aeclmg_get_gas_cal_b(port_address[port_numbers], &b_value)){
                  //Show err code
                  _NOP();
                }
                else{
                  datalink[port_numbers][B_DIMENSIONS] = b_value;
                  
                }
              }
            }
          }
        }
      }
    }
  }
}

void measure_all_rht(void)
{
#ifdef ENABLE_WATCHDOG    
  WDT_A_resetTimer(WDT_A_BASE);
#endif
  for(UINT8 port_sht=0; port_sht<=3; port_sht++){
    if(switch_port_sht(port_sht)){
      //Show err code
      _NOP();
    }
    else{
      //Switch successful
      measure_rht(port_sht);
    }
  }
  chk_ui_cmd(); //CHECK  
}

void measure_rht(UINT8 sht_count)
{
  INT16 rht_data[2];
  etError   error;

  memset(rht_data,0x00,2);
  
  error = SHT3X_GetTempAndHumi(&temperature, &humidity, REPEATAB_HIGH, MODE_POLLING, 50);   
  if(error != NO_ERROR)
  {
    _NOP();
  }
  else
  {
    rht_data[0] = (INT16) (temperature * 100);
    rht_data[1] = (INT16) (humidity * 100);
  }
  
  switch(sht_count)
  {
  case 0:
    SHT3x_RHT[0] = rht_data[0];
    SHT3x_RHT[1] = rht_data[1];
    break;
  case 1:
    SHT3x_RHT[2] = rht_data[0];
    SHT3x_RHT[3] = rht_data[1];
    break;
  case 2:
    SHT3x_RHT[4] = rht_data[0];
    SHT3x_RHT[5] = rht_data[1];
    break;
  case 3:
    SHT3x_RHT[6] = rht_data[0];
    SHT3x_RHT[7] = rht_data[1];
    break;
  }
}

void datalink_clear(void)
{
  for(i=0;i<24;i++){
    for(j=0;j<11;j++){
      datalink[i][j] = 0;
    }
  }
}

void sensordata_init(void)
{
  we = 0;
  ae = 0;
  concentrations = 0;
}

void sensorinfo_init(void)
{
  fw_version = 0;
  cal_data = 0;
  bsn = 0;
  gas_name = 0;
  zero = 0;
  span = 0;
  a_value = 0;
  b_value = 0;
}

void int_to_str_func(UINT64 conver_int)
{
  memset(conv_buf, 0x00, CONV_BUF_MAX);
  sprintf(conv_buf,"%lld",conver_int);
}

void ui_print(char *character)
{
  while (*character != '\0')
  {
    while (!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = *character++;
  }
}

void ADC_produce()
{
  if(datalink[i][0] >= datalink[i][1]){
    datalink[i][4] = datalink[i][0] - datalink[i][1];
  }
  
  if(datalink[i][0] < datalink[i][1]){
    datalink[i][4] = datalink[i][0] - datalink[i][1] ;
  }
}

void Correction_Zero()
{
  for(port_count=0;port_count<24;port_count++){
//    switch_port(port_count);
//    Write_Zero(CO_ADDRESS, CMD_W_ZERO, 0);
//    delay_ms(50);
//    Write_Zero(NO2_ADDRESS, CMD_W_ZERO, 0);
//    delay_ms(50);
//    Write_Zero(O3_ADDRESS, CMD_W_ZERO, 0);
//    delay_ms(50);
//    Write_Zero(NH3_ADDRESS, CMD_W_ZERO, 0);
//    delay_ms(50);
//    Write_Zero(H2S_ADDRESS, CMD_W_ZERO, 0);
//    delay_ms(50);
//    Write_Zero(AL_CO_B4_ADDRESS, CMD_W_ZERO, 0);
//    delay_ms(50);
//    Write_Zero(SO2_ADDRESS, CMD_W_ZERO, 0);
//    delay_ms(50);
  }
//  ui_print("\r");
//  ui_print("\ZeroOK:");
//  conv_buf[0] = 'Z';conv_buf[1] = 'e';conv_buf[2] = 'r';conv_buf[3] = 'o';conv_buf[4] = 'O';conv_buf[5] = 'K';
//  ui_print(conv_buf);
//  ui_print("\r\n");
}

void Correction_Span()
{
//  float Span;
//  if(datalink[j][1] == 0)
//  {
//    for(port_count=0;port_count<24;port_count++){
//      Span = ((datalink[port_count][0]-datalink[port_count][6])/Span_range);
//      datalink[port_count][8] = Span;
//      switch_port(port_count);
//      Write_Span(CO_ADDRESS, CMD_W_SPAN, Span);
//      delay_ms(50);
//      Write_Span(NO2_ADDRESS, CMD_W_SPAN, Span);
//      delay_ms(50);
//      Write_Span(O3_ADDRESS, CMD_W_SPAN, Span);
//      delay_ms(50);
//      Write_Span(NH3_ADDRESS, CMD_W_SPAN, Span);
//      delay_ms(50);
//      Write_Span(H2S_ADDRESS, CMD_W_SPAN, Span);
//      delay_ms(50);
//      Write_Span(AL_CO_B4_ADDRESS, CMD_W_SPAN, Span);
//      delay_ms(50);
//      Write_Span(SO2_ADDRESS, CMD_W_SPAN, Span);
//      delay_ms(50);
//    }
//    if (datalink[j][1] != 0)
//    {
//      for(port_count=0;port_count<24;port_count++){
//        Span = ((datalink[port_count][0]-datalink[port_count][6]) - (datalink[port_count][1]-datalink[port_count][7])/Span_range);
//        datalink[port_count][8] = Span;
//        switch_port(port_count);
//        Write_Span(CO_ADDRESS, CMD_W_SPAN, Span);
//        delay_ms(50);
//        Write_Span(NO2_ADDRESS, CMD_W_SPAN, Span);
//        delay_ms(50);
//        Write_Span(O3_ADDRESS, CMD_W_SPAN, Span);
//        delay_ms(50);
//        Write_Span(NH3_ADDRESS, CMD_W_SPAN, Span);
//        delay_ms(50);
//        Write_Span(H2S_ADDRESS, CMD_W_SPAN, Span);
//        delay_ms(50);
//        Write_Span(AL_CO_B4_ADDRESS, CMD_W_SPAN, Span);
//        delay_ms(50);
//        Write_Span(SO2_ADDRESS, CMD_W_SPAN, Span);
//        delay_ms(50);
//      }
//    }
//  }
  //  ui_print("\r");
  //  ui_print("\SpanOK:");
  //  conv_buf[0] = 'S';conv_buf[1] = 'p';conv_buf[2] = 'a';conv_buf[3] = 'n';conv_buf[4] = 'O';conv_buf[5] = 'K';
  //  ui_print(conv_buf);
  //  ui_print("\r\n");
}

void WE_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
    ui_print("\r");
    ui_print("label_WE0:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 1:
    ui_print("\r");
    ui_print("label_WE1:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 2:
    ui_print("\r");
    ui_print("label_WE2:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 3:
    ui_print("\r");
    ui_print("label_WE3:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 4:
    ui_print("\r");
    ui_print("label_WE4:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 5:
    ui_print("\r");
    ui_print("label_WE5:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 6:
    ui_print("\r");
    ui_print("label_WE6:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 7:
    ui_print("\r");
    ui_print("label_WE7:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 8:
    ui_print("\r");
    ui_print("label_WE8:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 9:
    ui_print("\r");
    ui_print("label_WE9:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 10:
    ui_print("\r");
    ui_print("label_WE10:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
  case 11:
    ui_print("\r");
    ui_print("label_WE11:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 12:
    ui_print("\r");
    ui_print("label_WE12:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 13:
    ui_print("\r");
    ui_print("label_WE13:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 14:
    ui_print("\r");
    ui_print("label_WE14:");
    ui_print(conv_buf);
    ui_print("\r\n");
    break;
    case 15:
    ui_print("\r");
    ui_print("label_WE15:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 16:
    ui_print("\r");
    ui_print("label_WE16:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 17:
    ui_print("\r");
    ui_print("label_WE17:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 18:
    ui_print("\r");
    ui_print("label_WE18:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 19:
    ui_print("\r");
    ui_print("label_WE19:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 20:
    ui_print("\r");
    ui_print("label_WE20:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 21:
    ui_print("\r");
    ui_print("label_WE21:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 22:
    ui_print("\r");
    ui_print("label_WE22:");
    ui_print(conv_buf);
    ui_print("\r\n");
    case 23:
    ui_print("\r");
    ui_print("label_WE23:");
    ui_print(conv_buf);
    ui_print("\r\n");
  }
}

void AE_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
    ui_print("\r");
    ui_print("label_AE0:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 1:
    ui_print("\r");
    ui_print("label_AE1:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 2:
    ui_print("\r");
    ui_print("label_AE2:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 3:
    ui_print("\r");
    ui_print("label_AE3:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 4:
    ui_print("\r");
    ui_print("label_AE4:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 5:
    ui_print("\r");
    ui_print("label_AE5:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 6:
    ui_print("\r");
    ui_print("label_AE6:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 7:
    ui_print("\r");
    ui_print("label_AE7:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 8:
    ui_print("\r");
    ui_print("label_AE8:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 9:
    ui_print("\r");
    ui_print("label_AE9:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 10:
    ui_print("\r");
    ui_print("label_AE10:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 11:
    ui_print("\r");
    ui_print("label_AE11:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 12:
    ui_print("\r");
    ui_print("label_AE12:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 13:
    ui_print("\r");
    ui_print("label_AE13:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 14:
    ui_print("\r");
    ui_print("label_AE14:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 15:
    ui_print("\r");
    ui_print("label_AE15:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 16:
    ui_print("\r");
    ui_print("label_AE16:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 17:
    ui_print("\r");
    ui_print("label_AE17:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 18:
    ui_print("\r");
    ui_print("label_AE18:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 19:
    ui_print("\r");
    ui_print("label_AE19:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 20:
    ui_print("\r");
    ui_print("label_AE20:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 21:
    ui_print("\r");
    ui_print("label_AE21:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 22:
    ui_print("\r");
    ui_print("label_AE22:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 23:
    ui_print("\r");
    ui_print("label_AE23:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  }
}

void SSN_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
    ui_print("\r");
    ui_print("label_Serial0:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 1:
    ui_print("\r");
    ui_print("label_Serial1:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 2:
    ui_print("\r");
    ui_print("label_Serial2:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 3:
    ui_print("\r");
    ui_print("label_Serial3:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 4:
    ui_print("\r");
    ui_print("label_Serial4:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 5:
    ui_print("\r");
    ui_print("label_Serial5:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 6:
    ui_print("\r");
    ui_print("label_Serial6:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 7:
    ui_print("\r");
    ui_print("label_Serial7:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 8:
    ui_print("\r");
    ui_print("label_Serial8:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 9:
    ui_print("\r");
    ui_print("label_Serial9:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 10:
    ui_print("\r");
    ui_print("label_Serial10:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 11:
    ui_print("\r");
    ui_print("label_Serial11:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 12:
    ui_print("\r");
    ui_print("label_Serial12:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 13:
    ui_print("\r");
    ui_print("label_Serial13:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 14:
    ui_print("\r");
    ui_print("label_Serial14:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 15:
    ui_print("\r");
    ui_print("label_Serial15:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 16:
    ui_print("\r");
    ui_print("label_Serial16:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 17:
    ui_print("\r");
    ui_print("label_Serial17:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 18:
    ui_print("\r");
    ui_print("label_Serial18:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 19:
    ui_print("\r");
    ui_print("label_Serial19:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 20:
    ui_print("\r");
    ui_print("label_Serial20:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 21:
    ui_print("\r");
    ui_print("label_Serial21:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 22:
    ui_print("\r");
    ui_print("label_Serial22:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 23:
    ui_print("\r");
    ui_print("label_Serial23:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  }
}

void BSN_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
  ui_print("\r");
  ui_print("label_BSN0:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 1:
  ui_print("\r");
  ui_print("label_BSN1:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 2:
  ui_print("\r");
  ui_print("label_BSN2:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 3:
  ui_print("\r");
  ui_print("label_BSN3:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 4:
  ui_print("\r");
  ui_print("label_BSN4:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 5:
  ui_print("\r");
  ui_print("label_BSN5:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 6:
  ui_print("\r");
  ui_print("label_BSN6:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 7:
  ui_print("\r");
  ui_print("label_BSN7:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 8:
  ui_print("\r");
  ui_print("label_BSN8:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 9:
  ui_print("\r");
  ui_print("label_BSN9:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 10:
  ui_print("\r");
  ui_print("label_BSN10:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 11:
  ui_print("\r");
  ui_print("label_BSN11:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 12:
  ui_print("\r");
  ui_print("label_BSN12:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 13:
  ui_print("\r");
  ui_print("label_BSN13:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 14:
  ui_print("\r");
  ui_print("label_BSN14:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 15:
  ui_print("\r");
  ui_print("label_BSN15:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 16:
  ui_print("\r");
  ui_print("label_BSN16:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 17:
  ui_print("\r");
  ui_print("label_BSN17:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 18:
  ui_print("\r");
  ui_print("label_BSN18:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 19:
  ui_print("\r");
  ui_print("label_BSN19:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 20:
  ui_print("\r");
  ui_print("label_BSN20:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 21:
  ui_print("\r");
  ui_print("label_BSN21:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 22:
  ui_print("\r");
  ui_print("label_BSN22:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 23:
  ui_print("\r");
  ui_print("label_BSN23:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  }
}

void FW_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
    ui_print("\r");
    ui_print("FW_0:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 1:
    ui_print("\r");
    ui_print("FW_1:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 2:
    ui_print("\r");
    ui_print("FW_2:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 3:
    ui_print("\r");
    ui_print("FW_3:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 4:
    ui_print("\r");
    ui_print("FW_4:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 5:
    ui_print("\r");
    ui_print("FW_5:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 6:
    ui_print("\r");
    ui_print("FW_6:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 7:
    ui_print("\r");
    ui_print("FW_7:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 8:
    ui_print("\r");
    ui_print("FW_8:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 9:
    ui_print("\r");
    ui_print("FW_9:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 10:
    ui_print("\r");
    ui_print("FW_10:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  case 11:
    ui_print("\r");
    ui_print("FW_11:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 12:
    ui_print("\r");
    ui_print("FW_12:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 13:
    ui_print("\r");
    ui_print("FW_13:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 14:
    ui_print("\r");
    ui_print("FW_14:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 15:
    ui_print("\r");
    ui_print("FW_15:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 16:
    ui_print("\r");
    ui_print("FW_16:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 17:
    ui_print("\r");
    ui_print("FW_17:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 18:
    ui_print("\r");
    ui_print("FW_18:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 19:
    ui_print("\r");
    ui_print("FW_19:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 20:
    ui_print("\r");
    ui_print("FW_20:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 21:
    ui_print("\r");
    ui_print("FW_21:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 22:
    ui_print("\r");
    ui_print("FW_22:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
    case 23:
    ui_print("\r");
    ui_print("FW_23:");
    ui_print(conv_buf);
    ui_print("\r\n");break;
  }
}

void ADC_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
  ui_print("\r");
  ui_print("label_ADC0:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 1:
  ui_print("\r");
  ui_print("label_ADC1:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 2:
  ui_print("\r");
  ui_print("label_ADC2:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 3:
  ui_print("\r");
  ui_print("label_ADC3:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 4:
  ui_print("\r");
  ui_print("label_ADC4:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 5:
  ui_print("\r");
  ui_print("label_ADC5:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 6:
  ui_print("\r");
  ui_print("label_ADC6:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 7:
  ui_print("\r");
  ui_print("label_ADC7:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 8:
  ui_print("\r");
  ui_print("label_ADC8:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 9:
  ui_print("\r");
  ui_print("label_ADC9:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 10:
  ui_print("\r");
  ui_print("label_ADC10:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 11:
  ui_print("\r");
  ui_print("label_ADC11:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 12:
  ui_print("\r");
  ui_print("label_ADC12:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 13:
  ui_print("\r");
  ui_print("label_ADC13:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 14:
  ui_print("\r");
  ui_print("label_ADC14:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 15:
  ui_print("\r");
  ui_print("label_ADC15:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 16:
  ui_print("\r");
  ui_print("label_ADC16:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 17:
  ui_print("\r");
  ui_print("label_ADC17:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 18:
  ui_print("\r");
  ui_print("label_ADC18:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 19:
  ui_print("\r");
  ui_print("label_ADC19:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 20:
  ui_print("\r");
  ui_print("label_ADC20:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 21:
  ui_print("\r");
  ui_print("label_ADC21:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 22:
  ui_print("\r");
  ui_print("label_ADC22:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 23:
  ui_print("\r");
  ui_print("label_ADC23:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  }
}

void Zero_WE_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
  ui_print("\r");
  ui_print("label_Zero_WE0:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 1:
  ui_print("\r");
  ui_print("label_Zero_WE1:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 2:
  ui_print("\r");
  ui_print("label_Zero_WE2:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 3:
  ui_print("\r");
  ui_print("label_Zero_WE3:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 4:
  ui_print("\r");
  ui_print("label_Zero_WE4:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 5:
  ui_print("\r");
  ui_print("label_Zero_WE5:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 6:
  ui_print("\r");
  ui_print("label_Zero_WE6:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 7:
  ui_print("\r");
  ui_print("label_Zero_WE7:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 8:
  ui_print("\r");
  ui_print("label_Zero_WE8:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 9:
  ui_print("\r");
  ui_print("label_Zero_WE9:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 10:
  ui_print("\r");
  ui_print("label_Zero_WE10:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 11:
  ui_print("\r");
  ui_print("label_Zero_WE11:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 12:
  ui_print("\r");
  ui_print("label_Zero_WE12:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 13:
  ui_print("\r");
  ui_print("label_Zero_WE13:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 14:
  ui_print("\r");
  ui_print("label_Zero_WE14:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 15:
  ui_print("\r");
  ui_print("label_Zero_WE15:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 16:
  ui_print("\r");
  ui_print("label_Zero_WE16:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 17:
  ui_print("\r");
  ui_print("label_Zero_WE17:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 18:
  ui_print("\r");
  ui_print("label_Zero_WE18:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 19:
  ui_print("\r");
  ui_print("label_Zero_WE19:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 20:
  ui_print("\r");
  ui_print("label_Zero_WE20:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 21:
  ui_print("\r");
  ui_print("label_Zero_WE21:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 22:
  ui_print("\r");
  ui_print("label_Zero_WE22:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 23:
  ui_print("\r");
  ui_print("label_Zero_WE23:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  }
}

void Zero_AE_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
  ui_print("\r");
  ui_print("label_Zero_AE0:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 1:
  ui_print("\r");
  ui_print("label_Zero_AE1:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 2:
  ui_print("\r");
  ui_print("label_Zero_AE2:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 3:
  ui_print("\r");
  ui_print("label_Zero_AE3:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 4:
  ui_print("\r");
  ui_print("label_Zero_AE4:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 5:
  ui_print("\r");
  ui_print("label_Zero_AE5:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 6:
  ui_print("\r");
  ui_print("label_Zero_AE6:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 7:
  ui_print("\r");
  ui_print("label_Zero_AE7:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 8:
  ui_print("\r");
  ui_print("label_Zero_AE8:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 9:
  ui_print("\r");
  ui_print("label_Zero_AE9:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 10:
  ui_print("\r");
  ui_print("label_Zero_AE10:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 11:
  ui_print("\r");
  ui_print("label_Zero_AE11:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 12:
  ui_print("\r");
  ui_print("label_Zero_AE12:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 13:
  ui_print("\r");
  ui_print("label_Zero_AE13:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 14:
  ui_print("\r");
  ui_print("label_Zero_AE14:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 15:
  ui_print("\r");
  ui_print("label_Zero_AE15:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 16:
  ui_print("\r");
  ui_print("label_Zero_AE16:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 17:
  ui_print("\r");
  ui_print("label_Zero_AE17:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 18:
  ui_print("\r");
  ui_print("label_Zero_AE18:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 19:
  ui_print("\r");
  ui_print("label_Zero_AE19:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 20:
  ui_print("\r");
  ui_print("label_Zero_AE20:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 21:
  ui_print("\r");
  ui_print("label_Zero_AE21:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 22:
  ui_print("\r");
  ui_print("label_Zero_AE22:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 23:
  ui_print("\r");
  ui_print("label_Zero_AE23:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  }
}

void Span_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
  ui_print("\r");
  ui_print("label_Span0:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 1:
  ui_print("\r");
  ui_print("label_Span1:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 2:
  ui_print("\r");
  ui_print("label_Span2:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 3:
  ui_print("\r");
  ui_print("label_Span3:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 4:
  ui_print("\r");
  ui_print("label_Span4:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 5:
  ui_print("\r");
  ui_print("label_Span5:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 6:
  ui_print("\r");
  ui_print("label_Span6:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 7:
  ui_print("\r");
  ui_print("label_Span7:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 8:
  ui_print("\r");
  ui_print("label_Span8:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 9:
  ui_print("\r");
  ui_print("label_Span9:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 10:
  ui_print("\r");
  ui_print("label_Span10:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 11:
  ui_print("\r");
  ui_print("label_Span11:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 12:
  ui_print("\r");
  ui_print("label_Span12:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 13:
  ui_print("\r");
  ui_print("label_Span13:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 14:
  ui_print("\r");
  ui_print("label_Span14:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 15:
  ui_print("\r");
  ui_print("label_Span15:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 16:
  ui_print("\r");
  ui_print("label_Span16:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 17:
  ui_print("\r");
  ui_print("label_Span17:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 18:
  ui_print("\r");
  ui_print("label_Span18:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 19:
  ui_print("\r");
  ui_print("label_Span19:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 20:
  ui_print("\r");
  ui_print("label_Span20:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 21:
  ui_print("\r");
  ui_print("label_Span21:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 22:
  ui_print("\r");
  ui_print("label_Span22:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 23:
  ui_print("\r");
  ui_print("label_Span23:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  }
}

void ppb_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
  ui_print("\r");
  ui_print("label_ppb_Value0:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 1:
  ui_print("\r");
  ui_print("label_ppb_Value1:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 2:
  ui_print("\r");
  ui_print("label_ppb_Value2:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 3:
  ui_print("\r");
  ui_print("label_ppb_Value3:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 4:
  ui_print("\r");
  ui_print("label_ppb_Value4:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 5:
  ui_print("\r");
  ui_print("label_ppb_Value5:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 6:
  ui_print("\r");
  ui_print("label_ppb_Value6:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 7:
  ui_print("\r");
  ui_print("label_ppb_Value7:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 8:
  ui_print("\r");
  ui_print("label_ppb_Value8:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 9:
  ui_print("\r");
  ui_print("label_ppb_Value9:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 10:
  ui_print("\r");
  ui_print("label_ppb_Value10:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 11:
  ui_print("\r");
  ui_print("label_ppb_Value11:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 12:
  ui_print("\r");
  ui_print("label_ppb_Value12:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 13:
  ui_print("\r");
  ui_print("label_ppb_Value13:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 14:
  ui_print("\r");
  ui_print("label_ppb_Value14:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 15:
  ui_print("\r");
  ui_print("label_ppb_Value15:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 16:
  ui_print("\r");
  ui_print("label_ppb_Value16:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 17:
  ui_print("\r");
  ui_print("label_ppb_Value17:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 18:
  ui_print("\r");
  ui_print("label_ppb_Value18:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 19:
  ui_print("\r");
  ui_print("label_ppb_Value19:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 20:
  ui_print("\r");
  ui_print("label_ppb_Value20:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 21:
  ui_print("\r");
  ui_print("label_ppb_Value21:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 22:
  ui_print("\r");
  ui_print("label_ppb_Value22:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 23:
  ui_print("\r");
  ui_print("label_ppb_Value23:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  }
}

void Sensor_Name_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
  ui_print("\r");
  ui_print("label_Sensor_Name0:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 1:
  ui_print("\r");
  ui_print("label_Sensor_Name1:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 2:
  ui_print("\r");
  ui_print("label_Sensor_Name2:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 3:
  ui_print("\r");
  ui_print("label_Sensor_Name3:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 4:
  ui_print("\r");
  ui_print("label_Sensor_Name4:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 5:
  ui_print("\r");
  ui_print("label_Sensor_Name5:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 6:
  ui_print("\r");
  ui_print("label_Sensor_Name6:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 7:
  ui_print("\r");
  ui_print("label_Sensor_Name7:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 8:
  ui_print("\r");
  ui_print("label_Sensor_Name8:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 9:
  ui_print("\r");
  ui_print("label_Sensor_Name9:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 10:
  ui_print("\r");
  ui_print("label_Sensor_Name10:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 11:
  ui_print("\r");
  ui_print("label_Sensor_Name11:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 12:
  ui_print("\r");
  ui_print("label_Sensor_Name12:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 13:
  ui_print("\r");
  ui_print("label_Sensor_Name13:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 14:
  ui_print("\r");
  ui_print("label_Sensor_Name14:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 15:
  ui_print("\r");
  ui_print("label_Sensor_Name15:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 16:
  ui_print("\r");
  ui_print("label_Sensor_Name16:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 17:
  ui_print("\r");
  ui_print("label_Sensor_Name17:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 18:
  ui_print("\r");
  ui_print("label_Sensor_Name18:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 19:
  ui_print("\r");
  ui_print("label_Sensor_Name19:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 20:
  ui_print("\r");
  ui_print("label_Sensor_Name20:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 21:
  ui_print("\r");
  ui_print("label_Sensor_Name21:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 22:
  ui_print("\r");
  ui_print("label_Sensor_Name22:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 23:
  ui_print("\r");
  ui_print("label_Sensor_Name23:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  }
}

void SHT_to_UCA_count(void)
{
  switch(i)
  {
  case 0:
  ui_print("\r");
  ui_print("SHT_RH_Value0:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 1:
  ui_print("\r");
  ui_print("SHT_Temp_Value0:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 2:
  ui_print("\r");
  ui_print("SHT_RH_Value1:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 3:
  ui_print("\r");
  ui_print("SHT_Temp_Value1:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 4:
  ui_print("\r");
  ui_print("SHT_RH_Value2:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 5:
  ui_print("\r");
  ui_print("SHT_Temp_Value2:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 6:
  ui_print("\r");
  ui_print("SHT_RH_Value3:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  case 7:
  ui_print("\r");
  ui_print("SHT_Temp_Value3:");
  ui_print(conv_buf);
  ui_print("\r\n");break;
  }
}

void data_to_UCA(void)
{
  for(i=0;i<24;i++)
  {
    if(tabpage == '1'){
      int_to_str_func(datalink[i][0]);
      WE_to_UCA_count(); 
      int_to_str_func(datalink[i][1]);
      AE_to_UCA_count(); 
      
      ADC_produce();
      int_to_str_func(datalink[i][4]);
      ADC_to_UCA_count();
    }
    else{
      int_to_str_func(datalink[i][6]);
      Zero_WE_to_UCA_count();
      
      int_to_str_func(datalink[i][7]);
      Zero_AE_to_UCA_count();
      
      int_to_str_func(datalink[i][8]);
      Span_to_UCA_count();
      
      int_to_str_func(datalink[i][3]);
      FW_to_UCA_count();
      
      int_to_str_func(datalink[i][5]);
      BSN_to_UCA_count();
      
      int_to_str_func(datalink[i][2]);
      SSN_to_UCA_count();
      
      int_to_str_func(datalink[i][10]);
      Sensor_Name_to_UCA_count();
      

      int_to_str_func(datalink[i][9]);
      ppb_to_UCA_count();
    }
      chk_ui_cmd();//measure_all_rht已經使用過
  }
  for(i=0;i<8;i++)
  {
  int_to_str_func(SHT3x_RHT[i]);
  SHT_to_UCA_count();
  }
}

void ui_parameter_post(void)
{
  data_to_UCA();
  k++;
  if(k >= 10){
    k = 0;
  }
}





