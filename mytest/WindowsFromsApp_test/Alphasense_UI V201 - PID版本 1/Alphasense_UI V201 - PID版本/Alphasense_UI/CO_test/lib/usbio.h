//****************************************
//**  Copyright  (C)  USBIO  2011-2031  **

//****************************************
//**  DLL for USB interface chip USB2uis**
//**                            **
//****************************************
//
// USB��UART,I2C,SPI ���μh���f�w V1.0
// �@��: USB2uis  antony.wang
// USB2uis.DLL  V1.0
// �B������: Windows XP vista window7

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



// �]�ƨƥ�q���^�յ{��
// �]�ƨƥ�M��e���A(�b�U��w�q): 0=�]�ƩޥX, 0x80=�]�ƴ��J
//typedef	 VOID (CALLBACK* USB_MONITOR_CALLBACK )(BYTE iDevStatus);
//typedef  VOID (CALLBACK* USB_TRIG_CALLBACK )(DWORD iTrigInfo);
 typedef	 VOID (CALLBACK* USB_DLL_CALLBACK )(BYTE iDevIndex, DWORD iDevStatus);
//iTrigInfo�w�q 

//bit0~7�w�qĲ�o���� bit8~15 ���[�H��,��16��O�d
   
#define EXT_IO_TRIG          0xAA    //�~��IO���_Ĳ�o 
#define SPI_READ_TRIG        0xB0    //SPI�wŪ�ШD,bit7~bit15   Ū�ШD���� 0~255���O�N��1~256�r�`
#define SPI_WRITE_TRIG       0xB1    //SPI�g�ШD,  bit7~bit15   �g�ШD���� 0~255���O�N��1~256�r�`
#define SPI_INIT_READ_TRIG   0xB2    //SPI���ܦ^��Ū�_�l��m(��ڤW�O���sŪ,�u�w��Ū�g���פj��256���ƾڬq)
#define SPI_INIT_WRITE_TRIG  0xB3    //SPI���ܦ^��g�_�l��m(��ڤW�O���s�g,�u�w��Ū�g���פj��256���ƾڬq)
#define I2C_READ_TRIG        0xC0    //I2C�wŪ�ШD,bit7~bit15   Ū�ШD���� 0~255���O�N��1~256�r�`
#define I2C_WRITE_TRIG       0xC1    //I2C�g�ШD,  bit7~bit15   �g�ШD���� 0~255���O�N��1~256�r�`

//���o�]�Ƴn�󪩥���
DllExport bool                    //TRUE�G���\
		  USBIO_GetVersion(
							BYTE byIndex ,        // �]�Ƹ�,
							BYTE byType,         //0->DLL�����H��;1->�X�ʵ{�Ǫ����H��;2->�T�󪩥��H��
							LPVOID lpBuffer      //�ΨӫO�s�����H�����w�s
							);

 //���o�i�H�P�ɥ��}���]�ƼƳ̤j��
DllExport BYTE                     //��^�]�Ƽ�
					   USBIO_GetMaxNumofDev(
											void
											);

//���o���w�]�Ƹ����ǦC��
DllExport BYTE      //��^��e�]�ƪ��A   0->��e�]�Ƥ��s�b; 1->���ϥ�;  2->�ϥΤ�

						USBIO_GetSerialNo
										   (
											 BYTE byIndex,     // �]�Ƹ�,
											 char* lpBuff
										   );

//���o���w�]�Ƹ����]�Ƹ��|
DllExport char*      //��^�]�Ƹ��|���w,�p�G�ONULL,���w�]�Ƥ��s�b
						USBIO_GetDevicePath
										   (
											 BYTE byIndex     // �]�Ƹ�,
										   );
//�o���e�u�@�Ҧ�
/*
   *lpMode == 0x02 -->���`�Ҧ�;
			  0x01 -->�ɯżҦ�;
*/
DllExport bool                  //TRUE�G������\
						USBIO_GetWorkMode
							(
                               BYTE byIndex,         //�]�Ƹ�
							   BYTE* lpMode          //�s���Ҧ��Ȫ����w
							);
//�_��]��
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
DllExport bool                    //TRUE�G�_�즨�\
		  USBIO_ResetDevice(
							BYTE byIndex,         // �]�Ƹ�
							BYTE byDevId          //�]������
		 );

//�]�m�^�ը�ơA�ΨӳB�zUSB���ިƥ�

DllExport void   USBIO_SetUSBNotify(
                           bool bLog,
						   USB_DLL_CALLBACK pUSB_CallBack      //USB�^�ը�ƫ��w
						   );

//�]�m�^�ը�ơA�ΨӳB�zIOĲ�o�ƥ�
DllExport void
		 USBIO_SetTrigNotify(
							 BYTE byIndex,          //�]�Ƹ�
							 USB_DLL_CALLBACK pTrig_CallBack   //Ĳ�o�^�ը�ƫ��w
							 );

// �����w�ǦC�����}�@��USB2uis�]��,�@��q0�}�l,�p�G�O��^0xFF,���}�����\
DllExport BYTE
		 USBIO_OpenDeviceByNumber(
							 char* pSerialString     //�]�ƧǦC��
							 );

DllExport  BYTE               // �H�����}�@��USB2uis�]��,�@��q0�}�l,�p�G�O��^0xFF,���}�����\
		 USBIO_OpenDevice   (
								void
							  );


DllExport bool
          USBIO_CloseDevice  (                       // �����@��USB2uis�]��
							   BYTE byIndex          //�]�Ƹ�
							  );

DllExport bool
 USBIO_I2cSetConfig          (                        //�t�mI2c���f
                               BYTE byIndex,          //�]�Ƹ�
							   BYTE byDevAddr,        //�]�Ʀa�}
							   BYTE byRate,           //I2c�����W�v,0~3 ���100k~400k
							   DWORD dwMilliseconds   //I2cŪ�g�W��,���:�@�� ;�C16bit��Ū�W��,��16bit���g�W��
                               );
DllExport bool
 USBIO_I2cGetConfig          (                        //���oI2c���f�t�m
                               BYTE byIndex,           //�]�Ƹ�
							   BYTE* pbyDevAddr,        //�]�Ʀa�}
							   BYTE* byRate,           //I2c�����W�v,0~3 ���100k~400k
							   DWORD* pdwMilliseconds   //I2cŪ�g�W��,���:�@�� ;�C16bit��Ū�W��,��16bit���g�W��
                               );

DllExport bool                                //���Ū�J���r�`��
 USBIO_I2cRead               (                       //�qI2c���fŪ�ƾ�
							   BYTE byIndex,         //�]�Ƹ�
							   BYTE byDevAddr,       //i2c�]�Ʀa�}
							   LPVOID lpParaBuffer,  //�ΨӫO�s�bŪI2c�ƾڤ��e�n�g�J���R�O�Φa�}
							   BYTE  byParaSize,     //�n�g�J���ƾڽw�s����,�p�G���ݭn�g�J,�]��0
							   LPVOID lpReadBuffer,   //Ū�J�ƾڽw�s
							   WORD wReadSize         //�ШDŪ�J�ƾڪ���
							   );
DllExport bool                                       //��ڼg�J���r�`��
 USBIO_I2cWrite              (                       //�g�ƾڨ�I2c���f
							   BYTE byIndex,         //�]�Ƹ�
							   BYTE byDevAddr,       //i2c�]�Ʀa�}
							   LPVOID lpParaBuffer,  //�ΨӫO�s�b�gI2c�ƾڤ��e�n�g�J���R�O�Φa�}
							   BYTE  byParaSize,     //�n�g�J���R�O�Φa�}����,�p�G���ݭn�g�J,�]��0
							   LPVOID lpWriteBuffer,  //�g�J�ƾڽw�s
							   WORD dWriteSize      //�ШD�g�J���ƾڪ���
							   );
 DllExport bool                              //���Ū�J���r�`��
   USBIO_I2cReadEEProm       (                       //Ū�ƾڨ�I2c���f
							   BYTE byIndex,         //�]�Ƹ�
							   BYTE byDevAddr,       //i2c�]�Ʀa�}
							   BYTE byTypeIndex,     //EEProm ���� 0~12 ����24C01~24C4096
							   DWORD dwOffset,       //EEProm �_�l�a�}
							   BYTE* lpReadBuffer,   //Ū�J�ƾڽw�s
							   WORD wReadSize        //�ШDŪ�J�ƾڪ���
							   );
 DllExport bool                               //��ڼg�J���r�`��
   USBIO_I2cWriteEEProm      (                        //�g�ƾڨ�I2c���f
							   BYTE byIndex,          //�]�Ƹ�
							   BYTE byDevAddr,        //i2c�]�Ʀa�}
							   BYTE byTypeIndex,      //EEProm ���� 0~12 ����24C01~24C4096
							   DWORD dwOffset,         //EEProm �_�l�a�}
							   BYTE* lpWriteBuffer,    //�g�J�ƾڽw�s
							   WORD dWriteSize          //�ШD�g�J���ƾڪ���
							   );

 DllExport bool
 USBIO_SPISetConfig          (
							  BYTE byIndex,          //�]�Ƹ�
							  BYTE byRate,           //�C�|��O�i�S�v 0~8 ����200k,400k,600k,800k,1000k,2000k,4000k,6000k,12000K
													  //bit4~bit5�OSPI�u�@�Ҧ�0~3  00=>(POL=0,PHA=0); 01=>(POL=0,PHA=1);10=>(POL=1,PHA=0);11=>(POL=1,PHA=1)
													  //bit6 ����
													  //bit7 SPI�]������  0=>�D�]��; 1=>�q�]��
							  DWORD dwMilliseconds    //SPIŪ�g�W��,���:�@�� ;�C16bit��Ū�W��,��16bit���g�W��
							  );
 DllExport bool
 USBIO_SPIGetConfig  	     (
							  BYTE byIndex,           //�]�Ƹ�
							  BYTE* pbyRate,          //�C�|��O�i�S�v 0~8  ����200k,400k,600k,800k,1000k,2000k,4000k,6000k,12000K
													  //bit4~bit5�OSPI�u�@�Ҧ�0~3  00=>(POL=0,PHA=0); 01=>(POL=0,PHA=1);10=>(POL=1,PHA=0);11=>(POL=1,PHA=1)
													  //bit6 ����
													  //bit7 SPI�]������  0=>�D�]��; 1=>�q�]��
							  DWORD* pdwMilliseconds  //SPIŪ�g�W��,���:�@��;�C16bit��Ū�W��,��16bit���g�W��
							  );
 DllExport bool
 USBIO_SPIRead              (
							  BYTE byIndex,           //�]�Ƹ�
							  LPVOID lpComBuffer,     //�ΨӫO�s�bŪSPI�ƾڤ��e�n�g�J���R�O
							  BYTE  byComSize,        //�n�g�J���R�O����,�p�G���ݭn�g�J,�]��0
							  LPVOID lpBuffer,        //Ū�J�ƾڽw�s
							  WORD wBuffSize          //�ШDŪ�J�ƾڪ���
							  );
 DllExport bool
 USBIO_SPIWrite              (
							  BYTE byIndex,            //�]�Ƹ�
							  LPVOID lpComBuffer,      //�ΨӫO�s�bŪSPI�ƾڤ��e�n�g�J���R�O
							  BYTE  byComSize,         //�n�g�J���R�O����,�p�G���ݭn�g�J,�]��0
							  LPVOID lpBuffer,         //�g�J�ƾڽw�s
							  WORD wBuffSize            //�ШD�g�J�ƾڪ���
							  );
 DllExport bool
  USBIO_HDQSetConfig         (
							  BYTE byIndex,            //�]�Ƹ�
							  DWORD dwMilliseconds,    //HDQŪ�g�W��
							  BYTE* lpParaBuffer,      //HDQ�ɧǰѼ�  �@�ӫ��Vstruct { 	WORD tCYCH,tHW1,tHW0,tBreak,tBR,tCYCD_min,tCYCD_max,tDW1_min,tDW1_max,tDW0_min,tDW0_max,tTO}�����w;}
							  BYTE byParaSize          //HDQ�ɧǰѼƪ��� �A�T�w��24
							  );
 DllExport bool
 USBIO_HDQGetConfig          (
							  BYTE byIndex,        //�]�Ƹ�
							  DWORD* pdwMilliseconds,    //HDQŪ�g�W��
							  BYTE* lpParaBuff,      //HDQ�ɧǰѼ�  �@�ӫ��Vstruct { 	WORD tCYCH,tHW1,tHW0,tBreak,tBR,tCYCD_min,tCYCD_max,tDW1_min,tDW1_max,tDW0_min,tDW0_max,tTO}�����w;
							  BYTE byParaSize       //HDQ�ɧǰѼƪ��� �A�T�w��24
							  );
 DllExport bool
 USBIO_HDQWrite              (
							  BYTE byIndex,         //�]�Ƹ�
							  BYTE addr,             //HDQ�]�Ʀa�},�����b0~0x7F����
							  LPVOID lpBuffer,      //�g�J�w�s
							  BYTE wBuffSize        //�g�J�ƾڪ��סABYTE mode��1�AWORD mode ��2
							  );
 DllExport bool
 USBIO_HDQRead               (
							  BYTE byIndex,       //�]�Ƹ�
							  BYTE addr,          //HDQ�]�Ʀa�},�����b0~0x7F����
							  LPVOID lpBuffer,    //Ū�J�w�s
							  WORD wBuffSize      //Ū�J�ƾڪ��סABYTE mode��1�AWORD mode ��2
							  );
 DllExport bool
 USBIO_SetCE                  //           �]�mHDQ�޸}�q��
							 (
							  BYTE byIndex,      //�]�Ƹ�
							  bool bHigh         //true �����q��, false ���C�q��
							 );
 DllExport bool
 USBIO_GetCE                (
							  BYTE byIndex,      //�]�Ƹ�
							  BYTE* pbyLevel     //���VCE pin�q�������w 1�����q��, 0���C�q��
							  ) ;

 DllExport bool
 USBIO_GetGPIOConfig         (                   //���GPIO��V�t�m 1����J,0����X
							  BYTE byIndex,      //�]�Ƹ�
							  BYTE* pbyValue      //Ū�J�w�s���w   b0~b7���O����J7-10~J7~3
							  );

DllExport bool
USBIO_SetGPIOConfig          (                   //�]�mGPIO��V 1����J,0����X
							  BYTE byIndex,      //�]�Ƹ�
							  BYTE byValue      //�g�J�w�s   b0~b7���O����J7-10~J7~3
							  );
 DllExport bool
 USBIO_GPIOWrite             (                   //�]�mGPIO�q�� 1�����q��,0���C�q��
							  BYTE byIndex,      //�]�Ƹ�
							  BYTE byValue,     //Ū�J�w�s   b0~b7���O����J7-10~J7~3
							  BYTE byMask        //�̽��]�m,   b0~b7��]��1�ɡA�Ӧ쪺�Ȥ�����
							  );

DllExport bool
USBIO_GPIORead               (                   //Ū��GPIO�q�� 1�����q��,0����X
							  BYTE byIndex,      //�]�Ƹ�
							  BYTE* pbyValue     //Ū�J�w�s���w   b0~b7���O����J7-10~J7~3
							  );
DllExport bool
USBIO_TrigGetConfig          (
							   BYTE byIndex,    //�]�Ƹ�
							   BYTE* pbySelect  //Ĳ�o�������w
							   );

DllExport bool
USBIO_TrigSetConfig          (
							   BYTE byIndex,    //�]�Ƹ�
							   BYTE bySelect    //Ĳ�o����  0:�W�ɽt 1:�U���t 2:���q�� 3:�C�q��
							  );

DllExport bool         //�i�JĲ�o���ݪ��A
USBIO_WaitForTrig               (
							  BYTE byIndex      //�]�Ƹ�
							  );

DllExport bool         //�h�XĲ�o���ݪ��A
USBIO_ExitTrig               (
							  BYTE byIndex       //�]�Ƹ�
							  );

DllExport bool
USBIO_GetPWMConfig           (
							  BYTE byIndex,      //�]�Ƹ�
							  BYTE* pbyRate,     //PWM�i�S�v 0~10���O����1k,2k,4k,6k,8k,10k,20k,40k,60k,80k,100k
							  BYTE* pbyNum,      //PWM�q�D��,�C�|����,�ĤC�����PWM�O�_�b�B�� ,1����PWM�b�B��
							  WORD* pwDuty      //�j�p��4�Ӧr���w�s�A�Ψӱ����|�ӳq�D���e�Ť� ,�C�ӳq�D���e�Ť�Ψ��bytes���
							  );

DllExport bool
USBIO_SetPWMConfig           (
							  BYTE byIndex,     //�]�Ƹ�
							  BYTE byRate,      //PWM�i�S�v 0~10���O����1k,2k,4k,6k,8k,10k,20k,40k,60k,80k,100k
							  BYTE byNum,       //PWM�q�D��,�C�|����
							  WORD* pwDuty     //�j�p��4�Ӧr���w�s�A�s���|�ӳq�D���e�Ť�A�C�ӳq�D���e�Ť�Ψ��bytes���
							  );

DllExport bool
USBIO_StartPWM               (
							  BYTE byIndex     //�]�Ƹ�
							 );

DllExport bool
USBIO_StopPWM               (
							  BYTE byIndex     //�]�Ƹ�
							 );

DllExport bool
USBIO_GetADCConfig			(
							 BYTE byIndex,     //�]�Ƹ�
							 BYTE* pbyMask,    //���VADC�q�D���}�������w;Bit0~3���O���q�D1~4,�p�G��1,���ܸӳq�D���}
							 BYTE* pbyIOSelect //���V4��byte�����w,�C��BYTE�����@�ӳq�D��IO���;�C�b�r�`�ļ˳q�D�t����J���,���b�r�`�ļ˳q�D������J���;
											   //��󥿷��q�D��0~9���O����J7-03~J7-10,�����ū׶ǷP���MVDD
											   //���t���q�D��0~9���O����J7-03~J7-10,�����Ѧҹq���MGND
							 );

DllExport bool
USBIO_SetADCConfig           (
							   BYTE byIndex,    //�]�Ƹ�
							   BYTE byMask,     //���wADC�q�D���}��;Bit0~3���O���q�D1~4,�p�G��1,���ܸӳq�D���}
							   BYTE* pbyIOSelect //���V4��byte�����w,�C��BYTE�����@�ӳq�D��IO���;�C�b�r�`�ļ˳q�D�t����J���,���b�r�`�ļ˳q�D������J���;
												 //��󥿷��q�D��0~9���O����J7-03~J7-10,�����ū׶ǷP���MVDD
											     //���t���q�D��0~9���O����J7-03~J7-10,�����Ѧҹq���MGND
							  );

DllExport bool
USBIO_ADCRead               (
							  BYTE byIndex,         //�]�Ƹ�
							  WORD* lpReadBuffer,   //�ΨӦs��ADC�ĥΪ��ƾګ��w
							  WORD wBuffSize        //���wADC lpReadBuffer���j�p,�������q�D�ƪ�����  wBuffSize = chan x �ļ˦���
							 );

DllExport bool
 USBIO_UpdateSector          (                       //��s���Ϥ��e
                               BYTE byIndex,         //�]�Ƹ�
                               WORD wIndex,             //���ϯ���
                               LPVOID lpBuffer,   //�g�J�ƾڽw�s�A512 bytes
                               BYTE* lpResult       //0x00 -->���\; 0x01 -->�R�O�� ;0x02-->crc��;0x04-->������
                              );

//#ifdef __cplusplus
//}
//#endif

#endif		// _mUSBIO_PACKET_LENGTH_DLL_H
