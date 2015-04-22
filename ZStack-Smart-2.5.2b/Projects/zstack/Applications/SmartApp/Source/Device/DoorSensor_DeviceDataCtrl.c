/**************************************************************************************************
  Filename:       DoorSensor_DeviceDataCtrl.c
  Revised:        $Date: 2014-12-01 14:27:34 -0800 (Thu, 01 dec 2014) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of door sensor device data operations
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-12-01
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-04-22
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
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
extern devStates_t CommonApp_NwkState;

extern uint8 *optData;
extern uint8 optDataLen;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
 
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
void CommonApp_DetectStatusCB( void *params, uint16 *duration, uint8 *count);

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{
  HAL_TURN_OFF_DIC();
  DIC_DDR &= ~DIC_BV;
}

void HalStatesInit(devStates_t status)
{
  CommonApp_SetUserEvent(DOORSENSOR_DETECT_EVT, CommonApp_DetectStatusCB, 
  	DOORSENSOR_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);
}

void CommonApp_DetectStatusCB( void *params, uint16 *duration, uint8 *count)
{
  if( CommonApp_NwkState == DEV_ROUTER || CommonApp_NwkState == DEV_END_DEVICE)
  {
  	if (HAL_STATE_DIC())
    {
	  Update_Refresh("01", 2);
    }
	else
	{
	  Update_Refresh("00", 2);
	}
  }
}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	return 0;
}

