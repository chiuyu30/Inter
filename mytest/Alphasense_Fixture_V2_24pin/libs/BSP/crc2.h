/*
----------------------------------------------------------------------
File    : CRC.c
Purpose : Generic CRC interface module by Gary 
Copyright (c) 2014 Chen MIng-Jui(Gary). All rights reserved.
--------  END-OF-HEADER  ---------------------------------------------
*/

/** @defgroup LED    
 * @ingroup Generic LED interface module
 * @{
 */
 
#ifndef CRC_H                       /* avoid multiple inclusion */
#define CRC_H

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#include "include.h"
/*********************************************************************
*
*       Functions
*
**********************************************************************
*/
UINT16 mCRC16(UINT8 *puchMsg , UINT16 usDataLen);
#endif                                  /* avoid multiple inclusion */

/*************************** End of file ****************************/

