/*******************************************************************************
*  32kHz ACLK, 20MHZ SMCLK
*  
*  Description: AECL 4E EC Gas modules (PB14XX, PB15XX) I2C driver
*
*   Company: Autotronic Enterprise CO., LTD.
*   Author: SY LAU
*   Create Date: 2017/12/15
*
*
*******************************************************************************/
#ifndef HAL_AECL_MULTIGASES_H
#define HAL_AECL_MULTIGASES_H

#define CMD_READ_HEAD       0x20

#define CMD_READ_FW_VER     0x00
#define CMD_READ_GAS_NAME   0x02
#define CMD_READ_BSN        0x0A
#define CMD_READ_CAL_DATE   0x10
#define CMD_READ_CAL_ZERO   0x14
#define CMD_READ_CAL_SPAN   0x18
#define CMD_READ_CAL_A      0x1C
#define CMD_READ_CAL_B      0x20
#define CMD_READ_ADC_1      0x24
#define CMD_READ_ADC_2      0x28
#define CMD_READ_ADC_3      0x2C
#define CMD_READ_TEMPERATURE      0x30
#define CMD_READ_CONCENTRATIONS   0x32
#define CMD_READ_I2C_TEST   0xA5

#define CMD_WRITE_HEAD      0x30

#define CMD_WRITE_GAS_NAME  0x00
#define CMD_WRITE_BSN       0x08
#define CMD_WRITE_CAL_DATE  0x0E
#define CMD_WRITE_CAL_ZERO  0x11
#define CMD_WRITE_CAL_SPAN  0x14
#define CMD_WRITE_CAL_A     0x17
#define CMD_WRITE_CAL_B     0x20

INT16 get_i2c_test_results(UINT8 address, UINT16* data);
INT16 aeclmg_getAdc_1(UINT8 address, UINT32* data);
INT16 aeclmg_getAdc_2(UINT8 address, UINT32* data);
INT16 aeclmg_get_gas_name(UINT8 address, UINT64* data);
INT16 aeclmg_getSerialId(UINT8 address, UINT64* data);
INT16 aeclmg_get_fw_version(UINT8 address, UINT16* data);
INT16 aeclmg_get_cal_date(UINT8 address, UINT16* data);
INT16 aeclmg_get_gas_concentrations(UINT8 address, UINT16* data);
INT16 aeclmg_get_gas_cal_zero(UINT8 address, INT32* data);
INT16 aeclmg_get_gas_cal_span(UINT8 address, float* data);
INT16 aeclmg_get_gas_cal_a(UINT8 address, float* data);
INT16 aeclmg_get_gas_cal_b(UINT8 address, INT32* data);

INT16 write_gas_bsn(UINT8 address, UINT64 w_data);
INT16 write_gas_cal_date(UINT8 address, UINT32 w_data);
INT16 write_gas_cal_zero(UINT8 address, INT32 w_data);
INT16 write_gas_cal_span(UINT8 address, float w_data);
INT16 write_gas_cal_a(UINT8 address, float w_data);
INT16 write_gas_cal_b(UINT8 address, INT32 w_data);

INT16 write_and_read_gas_bsn(UINT8 address, UINT64 w_data, UINT64* r_data);
INT16 write_and_read_gas_cal_date(UINT8 address, UINT32 w_data, UINT32* r_data);
INT16 write_and_read_gas_cal_zero(UINT8 address, INT32 w_data, INT32* r_data);
INT16 write_and_read_gas_cal_span(UINT8 address, float w_data, float* r_data);
INT16 write_and_read_gas_cal_a(UINT8 address, float w_data, float* r_data);
INT16 write_and_read_gas_cal_b(UINT8 address, INT32 w_data, INT32* r_data);

//////  AL_NO2_ADDRESS
//  write_gas_bsn(GAS_1_ADDRESS, 2410046382033, &GB_u64gas_3_gas_bsn);
//  write_gas_cal_date(GAS_1_ADDRESS, 241004, &GB_u16gas_3_cal_date);
//  write_gas_cal_zero(GAS_1_ADDRESS, 9999, &GB_i32gas_3_cal_zero);
//  write_gas_cal_span(GAS_1_ADDRESS, 9999.0, &GB_floatgas_3_cal_span);
//  write_gas_cal_a(GAS_1_ADDRESS, 1.0, &GB_floatgas_3_cal_a);
//  write_gas_cal_b(GAS_1_ADDRESS, 10, &GB_i32gas_3_cal_b);
////  AL_O3_ADDRESS
//  write_gas_bsn(GAS_2_ADDRESS, 2410046202247, &GB_u64gas_3_gas_bsn);
//  write_gas_cal_date(GAS_2_ADDRESS, 241004, &GB_u16gas_3_cal_date);
//  write_gas_cal_zero(GAS_2_ADDRESS, -60095, &GB_i32gas_3_cal_zero);
//  write_gas_cal_span(GAS_2_ADDRESS, 1610.73, &GB_floatgas_3_cal_span);
//  write_gas_cal_a(GAS_2_ADDRESS, 1.0, &GB_floatgas_3_cal_a);
//  write_gas_cal_b(GAS_2_ADDRESS, 0, &GB_i32gas_3_cal_b);
////  AL_CO_ADDRESS
//  write_gas_bsn(GAS_3_ADDRESS, 2410076481822, &GB_u64gas_3_gas_bsn);
//  write_gas_cal_date(GAS_3_ADDRESS, 241007, &GB_u16gas_3_cal_date);
//  write_gas_cal_zero(GAS_3_ADDRESS, -43546, &GB_i32gas_3_cal_zero);
//  write_gas_cal_span(GAS_3_ADDRESS, -148.11, &GB_floatgas_3_cal_span);
//  write_gas_cal_a(GAS_3_ADDRESS, 1.0, &GB_floatgas_3_cal_a);
//  write_gas_cal_b(GAS_3_ADDRESS, 0, &GB_i32gas_3_cal_b);
////  NH3
//  write_gas_bsn(GAS_4_ADDRESS, 2410046718103, &GB_u64gas_3_gas_bsn);
//  write_gas_cal_date(GAS_4_ADDRESS, 241004, &GB_u16gas_3_cal_date);
//  write_gas_cal_zero(GAS_4_ADDRESS, 12525110, &GB_i32gas_3_cal_zero);
//  write_gas_cal_span(GAS_4_ADDRESS, -289.86, &GB_floatgas_3_cal_span);
//  write_gas_cal_a(GAS_4_ADDRESS, 1.0, &GB_floatgas_3_cal_a);
//  write_gas_cal_b(GAS_4_ADDRESS, 0, &GB_i32gas_3_cal_b);
////  H2S
//  write_gas_bsn(GAS_4_ADDRESS,0x0230E53068A5);
//  write_gas_cal_date(GAS_4_ADDRESS,0x0003AD06);
//  write_gas_cal_zero(GAS_4_ADDRESS,0xFFFFD475);
//  write_gas_cal_span(GAS_4_ADDRESS,-349.16);
//  write_gas_cal_a(GAS_4_ADDRESS,1.0);
//  write_gas_cal_b(GAS_4_ADDRESS,0);
////  pid
//  write_gas_bsn(GAS_5_ADDRESS, 2410046312345, &GB_u64gas_3_gas_bsn);
//  write_gas_cal_date(GAS_5_ADDRESS, 250606, &GB_u16gas_3_cal_date);
//  write_gas_cal_zero(GAS_5_ADDRESS, 12512345, &GB_i32gas_3_cal_zero);
//  write_gas_cal_span(GAS_5_ADDRESS, -123.45, &GB_floatgas_3_cal_span);
//  write_gas_cal_a(GAS_5_ADDRESS, 321.0, &GB_floatgas_3_cal_a);
//  write_gas_cal_b(GAS_5_ADDRESS, 12345, &GB_i32gas_3_cal_b);

#endif