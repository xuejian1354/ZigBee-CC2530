/**************************************************************************************************
  Filename:       HueLight_DeviceDataCtrl.c
  Revised:        $Date: 2015-08-21 10:27:35 -0800 (Fir, 21 Aug 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of hue light device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-08-21
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include "OSAL_Nv.h"
#include "OnBoard.h"

#include "ZComDef.h"
#include "CommonApp.h"

/* HAL */
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "hal_drivers.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
 
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{}

void HalStatesInit(devStates_t status)
{}


int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	*dataLen = 0;
	return 0;
}

