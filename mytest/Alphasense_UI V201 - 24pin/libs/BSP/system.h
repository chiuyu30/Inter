#ifndef SYSTEM_H
#define SYSTEM_H

// Common data types
typedef unsigned char		BOOL;

typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned long		DWORD;
typedef unsigned long long	QWORD;

typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef unsigned long		UINT32;
typedef unsigned long long	UINT64;

typedef signed char             INT8;
typedef signed short		INT16;
typedef signed long             INT32;
typedef signed long long	INT64;

/// \brief Error codes
typedef enum {
  ERROR_NONE              = 0x00, // no error
  ERROR_ACK               = 0x01, // no acknowledgment error
  ERROR_CHECKSUM          = 0x02, // checksum mismatch error
  ERROR_IVALID_PARAMETER  = 0xFF, // invalid parameter
} Error;

// Common values
#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE 1
#endif
#ifndef NULL
	#define NULL 0
#endif

#define SENSOR_BOARD_ID_EN      1

//Useful stuff
#define BV(n) (1 << (n))
#define BF(x,b,s) (((x) & (b)) >> (s))
#define MIN(n,m) (((n) < (m)) ? (n) : (m))
#define MAX(n,m) (((n) < (m)) ? (m) : (n))
#define ABS(n) ((n < 0) ? -(n) : (n))

void delay_ms(UINT16 timeout);
void delay_micro_second(UINT32 us);
void LED_init(void);
void system_init(void);



#endif
