#ifndef SENSOR_ABSTRACTION
#define SENSOR_ABSTRACTION


#define AL_O3_ADDRESS              0x62
#define AL_NO2_ADDRESS             0x63
#define AL_CO_ADDRESS              0x64
#define AL_SO2_ADDRESS             0x66
#define DD7_NH3_ADDRESS            0x67
#define AL_H2S_ADDRESS             0x68
#define PIDAY5_VOC_ADDRESS         0x74
#define IRMAT_CH4_ADDRESS          0x78

void measure_gases_data(void);
void measure_gases_info(void);
void measure_rht(UINT8 sht_count);
void measure_all_rht(void);
void gat_all_gas_data(UINT8 port_numbers);
void get_all_gas_info(UINT8 port_numbers);
void datalink_clear(void);

void sensordata_init(void);
void sensorinfo_init(void);
void data_to_UCA(void);
void ui_parameter_post(void);


void Correction_Zero(void);
void Correction_Span(void);
void SHT_to_UCA_count();
void ppb_to_UCA_count(void);
void Sensor_Name_to_UCA_count(void);
void Span_to_UCA_count(void);
void Zero_AE_to_UCA_count(void);
void Zero_WE_to_UCA_count(void);
void ADC_to_UCA_count(void);
void FW_to_UCA_count(void);
void BSN_to_UCA_count(void);
void SSN_to_UCA_count(void);
void AE_to_UCA_count(void);
void WE_to_UCA_count(void);


extern UINT16 Span_range;



#endif