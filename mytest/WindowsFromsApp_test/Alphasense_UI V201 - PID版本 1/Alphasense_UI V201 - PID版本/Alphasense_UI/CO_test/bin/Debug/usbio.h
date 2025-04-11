//****************************************
//**  Copyright  (C)  USBIO  2011-2031  **

//****************************************
//**  DLL for USB interface chip USB2uis**
//**                            **
//****************************************
//
// USB轉UART,I2C,SPI 應用層接口庫 V1.0
// 作者: USB2uis  antony.wang
// USB2uis.DLL  V1.0
// 運行環境: Windows XP vista window7

/***************history*******************
v1.0:
 1>  created        2011.12.07   by antony
*****************end*********************/

#ifndef		USBIO_H
#define		USBIO_H
#define DllExport extern "C"__declspec(dllimport)
#define FM_TIRGER_FUNCTION
//#ifdef __cplusplus
//extern "C" {
//#endif



// 設備事件通知回調程序
// 設備事件和當前狀態(在下行定義): 0=設備拔出, 0x80=設備插入
//typedef	 VOID (CALLBACK* USB_MONITOR_CALLBACK )(BYTE iDevStatus);
//typedef  VOID (CALLBACK* USB_TRIG_CALLBACK )(DWORD iTrigInfo);
 typedef	 VOID (CALLBACK* USB_DLL_CALLBACK )(BYTE iDevIndex, DWORD iDevStatus);
//iTrigInfo定義 

//bit0~7定義觸發類型 bit8~15 附加信息,高16位保留
   
#define EXT_IO_TRIG          0xAA    //外部IO中斷觸發 
#define SPI_READ_TRIG        0xB0    //SPI預讀請求,bit7~bit15   讀請求長度 0~255分別代表1~256字節
#define SPI_WRITE_TRIG       0xB1    //SPI寫請求,  bit7~bit15   寫請求長度 0~255分別代表1~256字節
#define SPI_INIT_READ_TRIG   0xB2    //SPI指示回到讀起始位置(實際上是重新讀,只針對讀寫長度大於256的數據段)
#define SPI_INIT_WRITE_TRIG  0xB3    //SPI指示回到寫起始位置(實際上是重新寫,只針對讀寫長度大於256的數據段)
#define I2C_READ_TRIG        0xC0    //I2C預讀請求,bit7~bit15   讀請求長度 0~255分別代表1~256字節
#define I2C_WRITE_TRIG       0xC1    //I2C寫請求,  bit7~bit15   寫請求長度 0~255分別代表1~256字節

//取得設備軟件版本號
DllExport bool                    //TRUE：成功
		  USBIO_GetVersion(
							BYTE byIndex ,        // 設備號,
							BYTE byType,         //0->DLL版本信息;1->驅動程序版本信息;2->固件版本信息
							LPVOID lpBuffer      //用來保存版本信息的緩存
							);

 //取得可以同時打開的設備數最大值
DllExport BYTE                     //返回設備數
					   USBIO_GetMaxNumofDev(
											void
											);

//取得指定設備號的序列號
DllExport BYTE      //返回當前設備狀態   0->當前設備不存在; 1->未使用;  2->使用中

						USBIO_GetSerialNo
										   (
											 BYTE byIndex,     // 設備號,
											 char* lpBuff
										   );

//取得指定設備號的設備路徑
DllExport char*      //返回設備路徑指針,如果是NULL,指定設備不存在
						USBIO_GetDevicePath
										   (
											 BYTE byIndex     // 設備號,
										   );
//得到當前工作模式
/*
   *lpMode == 0x02 -->正常模式;
			  0x01 -->升級模式;
*/
DllExport bool                  //TRUE：獲取成功
						USBIO_GetWorkMode
							(
                               BYTE byIndex,         //設備號
							   BYTE* lpMode          //存取模式值的指針
							);
//復位設備
 /*
#define DEV_UART     0x80             //unused
#define DEV_I2C      0x81
#define DEV_SPI      0x82
#define DEV_HDQ      0x83
#define DEV_CAN      0x84             //unused
#define DEV_GPIO     0x85
#define DEV_ADC      0x86
#define DEV_PWM      0x87
#define DEV_PARALLEL 0x88             //unused
#define DEV_CE       0x8D             //unused
#define DEV_TRIG     0x8E             //unused
#define DEV_ALL      0x8F
*/
DllExport bool                    //TRUE：復位成功
		  USBIO_ResetDevice(
							BYTE byIndex,         // 設備號
							BYTE byDevId          //設備類型
		 );

//設置回調函數，用來處理USB插拔事件

DllExport void   USBIO_SetUSBNotify(
                           bool bLog,
						   USB_DLL_CALLBACK pUSB_CallBack      //USB回調函數指針
						   );

//設置回調函數，用來處理IO觸發事件
DllExport void
		 USBIO_SetTrigNotify(
							 BYTE byIndex,          //設備號
							 USB_DLL_CALLBACK pTrig_CallBack   //觸發回調函數指針
							 );

// 按指定序列號打開一個USB2uis設備,一般從0開始,如果是返回0xFF,打開不成功
DllExport BYTE
		 USBIO_OpenDeviceByNumber(
							 char* pSerialString     //設備序列號
							 );

DllExport  BYTE               // 隨機打開一個USB2uis設備,一般從0開始,如果是返回0xFF,打開不成功
		 USBIO_OpenDevice   (
								void
							  );


DllExport bool
          USBIO_CloseDevice  (                       // 關閉一個USB2uis設備
							   BYTE byIndex          //設備號
							  );

DllExport bool
 USBIO_I2cSetConfig          (                        //配置I2c接口
                               BYTE byIndex,          //設備號
							   BYTE byDevAddr,        //設備地址
							   BYTE byRate,           //I2c時鐘頻率,0~3 單位100k~400k
							   DWORD dwMilliseconds   //I2c讀寫超時,單位:毫秒 ;低16bit為讀超時,高16bit為寫超時
                               );
DllExport bool
 USBIO_I2cGetConfig          (                        //取得I2c接口配置
                               BYTE byIndex,           //設備號
							   BYTE* pbyDevAddr,        //設備地址
							   BYTE* byRate,           //I2c時鐘頻率,0~3 單位100k~400k
							   DWORD* pdwMilliseconds   //I2c讀寫超時,單位:毫秒 ;低16bit為讀超時,高16bit為寫超時
                               );

DllExport bool                                //實際讀入的字節數
 USBIO_I2cRead               (                       //從I2c接口讀數據
							   BYTE byIndex,         //設備號
							   BYTE byDevAddr,       //i2c設備地址
							   LPVOID lpParaBuffer,  //用來保存在讀I2c數據之前要寫入的命令或地址
							   BYTE  byParaSize,     //要寫入的數據緩存長度,如果不需要寫入,設為0
							   LPVOID lpReadBuffer,   //讀入數據緩存
							   WORD wReadSize         //請求讀入數據長度
							   );
DllExport bool                                       //實際寫入的字節數
 USBIO_I2cWrite              (                       //寫數據到I2c接口
							   BYTE byIndex,         //設備號
							   BYTE byDevAddr,       //i2c設備地址
							   LPVOID lpParaBuffer,  //用來保存在寫I2c數據之前要寫入的命令或地址
							   BYTE  byParaSize,     //要寫入的命令或地址長度,如果不需要寫入,設為0
							   LPVOID lpWriteBuffer,  //寫入數據緩存
							   WORD dWriteSize      //請求寫入的數據長度
							   );
 DllExport bool                              //實際讀入的字節數
   USBIO_I2cReadEEProm       (                       //讀數據到I2c接口
							   BYTE byIndex,         //設備號
							   BYTE byDevAddr,       //i2c設備地址
							   BYTE byTypeIndex,     //EEProm 型號 0~12 對應24C01~24C4096
							   DWORD dwOffset,       //EEProm 起始地址
							   BYTE* lpReadBuffer,   //讀入數據緩存
							   WORD wReadSize        //請求讀入數據長度
							   );
 DllExport bool                               //實際寫入的字節數
   USBIO_I2cWriteEEProm      (                        //寫數據到I2c接口
							   BYTE byIndex,          //設備號
							   BYTE byDevAddr,        //i2c設備地址
							   BYTE byTypeIndex,      //EEProm 型號 0~12 對應24C01~24C4096
							   DWORD dwOffset,         //EEProm 起始地址
							   BYTE* lpWriteBuffer,    //寫入數據緩存
							   WORD dWriteSize          //請求寫入的數據長度
							   );

 DllExport bool
 USBIO_SPISetConfig          (
							  BYTE byIndex,          //設備號
							  BYTE byRate,           //低四位是波特率 0~8 對應200k,400k,600k,800k,1000k,2000k,4000k,6000k,12000K
													  //bit4~bit5是SPI工作模式0~3  00=>(POL=0,PHA=0); 01=>(POL=0,PHA=1);10=>(POL=1,PHA=0);11=>(POL=1,PHA=1)
													  //bit6 未用
													  //bit7 SPI設備類型  0=>主設備; 1=>從設備
							  DWORD dwMilliseconds    //SPI讀寫超時,單位:毫秒 ;低16bit為讀超時,高16bit為寫超時
							  );
 DllExport bool
 USBIO_SPIGetConfig  	     (
							  BYTE byIndex,           //設備號
							  BYTE* pbyRate,          //低四位是波特率 0~8  對應200k,400k,600k,800k,1000k,2000k,4000k,6000k,12000K
													  //bit4~bit5是SPI工作模式0~3  00=>(POL=0,PHA=0); 01=>(POL=0,PHA=1);10=>(POL=1,PHA=0);11=>(POL=1,PHA=1)
													  //bit6 未用
													  //bit7 SPI設備類型  0=>主設備; 1=>從設備
							  DWORD* pdwMilliseconds  //SPI讀寫超時,單位:毫秒;低16bit為讀超時,高16bit為寫超時
							  );
 DllExport bool
 USBIO_SPIRead              (
							  BYTE byIndex,           //設備號
							  LPVOID lpComBuffer,     //用來保存在讀SPI數據之前要寫入的命令
							  BYTE  byComSize,        //要寫入的命令長度,如果不需要寫入,設為0
							  LPVOID lpBuffer,        //讀入數據緩存
							  WORD wBuffSize          //請求讀入數據長度
							  );
 DllExport bool
 USBIO_SPIWrite              (
							  BYTE byIndex,            //設備號
							  LPVOID lpComBuffer,      //用來保存在讀SPI數據之前要寫入的命令
							  BYTE  byComSize,         //要寫入的命令長度,如果不需要寫入,設為0
							  LPVOID lpBuffer,         //寫入數據緩存
							  WORD wBuffSize            //請求寫入數據長度
							  );
 DllExport bool
  USBIO_HDQSetConfig         (
							  BYTE byIndex,            //設備號
							  DWORD dwMilliseconds,    //HDQ讀寫超時
							  BYTE* lpParaBuffer,      //HDQ時序參數  一個指向struct { 	WORD tCYCH,tHW1,tHW0,tBreak,tBR,tCYCD_min,tCYCD_max,tDW1_min,tDW1_max,tDW0_min,tDW0_max,tTO}的指針;}
							  BYTE byParaSize          //HDQ時序參數長度 ，固定為24
							  );
 DllExport bool
 USBIO_HDQGetConfig          (
							  BYTE byIndex,        //設備號
							  DWORD* pdwMilliseconds,    //HDQ讀寫超時
							  BYTE* lpParaBuff,      //HDQ時序參數  一個指向struct { 	WORD tCYCH,tHW1,tHW0,tBreak,tBR,tCYCD_min,tCYCD_max,tDW1_min,tDW1_max,tDW0_min,tDW0_max,tTO}的指針;
							  BYTE byParaSize       //HDQ時序參數長度 ，固定為24
							  );
 DllExport bool
 USBIO_HDQWrite              (
							  BYTE byIndex,         //設備號
							  BYTE addr,             //HDQ設備地址,必須在0~0x7F之間
							  LPVOID lpBuffer,      //寫入緩存
							  BYTE wBuffSize        //寫入數據長度，BYTE mode為1，WORD mode 為2
							  );
 DllExport bool
 USBIO_HDQRead               (
							  BYTE byIndex,       //設備號
							  BYTE addr,          //HDQ設備地址,必須在0~0x7F之間
							  LPVOID lpBuffer,    //讀入緩存
							  WORD wBuffSize      //讀入數據長度，BYTE mode為1，WORD mode 為2
							  );
 DllExport bool
 USBIO_SetCE                  //           設置HDQ引腳電平
							 (
							  BYTE byIndex,      //設備號
							  bool bHigh         //true 為高電平, false 為低電平
							 );
 DllExport bool
 USBIO_GetCE                (
							  BYTE byIndex,      //設備號
							  BYTE* pbyLevel     //指向CE pin電平的指針 1為高電平, 0為低電平
							  ) ;

 DllExport bool
 USBIO_GetGPIOConfig         (                   //獲取GPIO方向配置 1為輸入,0為輸出
							  BYTE byIndex,      //設備號
							  BYTE* pbyValue      //讀入緩存指針   b0~b7分別對應J7-10~J7~3
							  );

DllExport bool
USBIO_SetGPIOConfig          (                   //設置GPIO方向 1為輸入,0為輸出
							  BYTE byIndex,      //設備號
							  BYTE byValue      //寫入緩存   b0~b7分別對應J7-10~J7~3
							  );
 DllExport bool
 USBIO_GPIOWrite             (                   //設置GPIO電平 1為高電平,0為低電平
							  BYTE byIndex,      //設備號
							  BYTE byValue,     //讀入緩存   b0~b7分別對應J7-10~J7~3
							  BYTE byMask        //屏蔽設置,   b0~b7位設為1時，該位的值不改變
							  );

DllExport bool
USBIO_GPIORead               (                   //讀取GPIO電平 1為高電平,0為輸出
							  BYTE byIndex,      //設備號
							  BYTE* pbyValue     //讀入緩存指針   b0~b7分別對應J7-10~J7~3
							  );
DllExport bool
USBIO_TrigGetConfig          (
							   BYTE byIndex,    //設備號
							   BYTE* pbySelect  //觸發類型指針
							   );

DllExport bool
USBIO_TrigSetConfig          (
							   BYTE byIndex,    //設備號
							   BYTE bySelect    //觸發類型  0:上升緣 1:下降緣 2:高電平 3:低電平
							  );

DllExport bool         //進入觸發等待狀態
USBIO_WaitForTrig               (
							  BYTE byIndex      //設備號
							  );

DllExport bool         //退出觸發等待狀態
USBIO_ExitTrig               (
							  BYTE byIndex       //設備號
							  );

DllExport bool
USBIO_GetPWMConfig           (
							  BYTE byIndex,      //設備號
							  BYTE* pbyRate,     //PWM波特率 0~10分別對應1k,2k,4k,6k,8k,10k,20k,40k,60k,80k,100k
							  BYTE* pbyNum,      //PWM通道數,低四位表示,第七位指示PWM是否在運行 ,1指示PWM在運行
							  WORD* pwDuty      //大小為4個字的緩存，用來接收四個通道的占空比 ,每個通道的占空比用兩個bytes表示
							  );

DllExport bool
USBIO_SetPWMConfig           (
							  BYTE byIndex,     //設備號
							  BYTE byRate,      //PWM波特率 0~10分別對應1k,2k,4k,6k,8k,10k,20k,40k,60k,80k,100k
							  BYTE byNum,       //PWM通道數,低四位表示
							  WORD* pwDuty     //大小為4個字的緩存，存有四個通道的占空比，每個通道的占空比用兩個bytes表示
							  );

DllExport bool
USBIO_StartPWM               (
							  BYTE byIndex     //設備號
							 );

DllExport bool
USBIO_StopPWM               (
							  BYTE byIndex     //設備號
							 );

DllExport bool
USBIO_GetADCConfig			(
							 BYTE byIndex,     //設備號
							 BYTE* pbyMask,    //指向ADC通道的開關的指針;Bit0~3分別為通道1~4,如果為1,指示該通道打開
							 BYTE* pbyIOSelect //指向4個byte的指針,每個BYTE對應一個通道的IO選擇;低半字節採樣通道負極輸入選擇,高半字節採樣通道正極輸入選擇;
											   //對於正極通道值0~9分別對應J7-03~J7-10,內部溫度傳感器和VDD
											   //對於負極通道值0~9分別對應J7-03~J7-10,內部參考電壓和GND
							 );

DllExport bool
USBIO_SetADCConfig           (
							   BYTE byIndex,    //設備號
							   BYTE byMask,     //指定ADC通道的開關;Bit0~3分別為通道1~4,如果為1,指示該通道打開
							   BYTE* pbyIOSelect //指向4個byte的指針,每個BYTE對應一個通道的IO選擇;低半字節採樣通道負極輸入選擇,高半字節採樣通道正極輸入選擇;
												 //對於正極通道值0~9分別對應J7-03~J7-10,內部溫度傳感器和VDD
											     //對於負極通道值0~9分別對應J7-03~J7-10,內部參考電壓和GND
							  );

DllExport bool
USBIO_ADCRead               (
							  BYTE byIndex,         //設備號
							  WORD* lpReadBuffer,   //用來存取ADC採用的數據指針
							  WORD wBuffSize        //指定ADC lpReadBuffer的大小,必須為通道數的倍數  wBuffSize = chan x 採樣次數
							 );

DllExport bool
 USBIO_UpdateSector          (                       //更新扇區內容
                               BYTE byIndex,         //設備號
                               WORD wIndex,             //扇區索引
                               LPVOID lpBuffer,   //寫入數據緩存，512 bytes
                               BYTE* lpResult       //0x00 -->成功; 0x01 -->命令錯 ;0x02-->crc錯;0x04-->擦除錯
                              );

//#ifdef __cplusplus
//}
//#endif

#endif		// _mUSBIO_PACKET_LENGTH_DLL_H
