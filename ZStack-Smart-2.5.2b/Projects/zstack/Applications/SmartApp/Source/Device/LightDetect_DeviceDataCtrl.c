/**************************************************************************************************
  Filename:       LightDetect_DeviceDataCtrl.c
  Revised:        $Date: 2015-12-02 11:19:11 -0800 (Wed, 02 Dec 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of light detect device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-12-02
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 
#include "hal_adc.h"
#include "mincode.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern byte CommonApp_TaskID;
extern devStates_t CommonApp_NwkState;

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
static void CommonApp_LightDetectStatusCB( void *params, uint16 *duration, uint8 *count);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

void HalDeviceInit (void)
{}

void HalStatesInit(devStates_t status)
{
	set_user_event(CommonApp_TaskID,
					LIGHTDETECT_DETECT_EVT,
					CommonApp_LightDetectStatusCB, 
  					LIGHTDETECT_TIMEOUT,
  					TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS,
  					NULL);
}

void CommonApp_LightDetectStatusCB( void *params, uint16 *duration, uint8 *count)
{
  if( CommonApp_NwkState == DEV_ROUTER || CommonApp_NwkState == DEV_END_DEVICE)
  {
  	  uint8 adval_str[4];
	  uint16 adval = HalAdcRead(HAL_ADC_CHANNEL_1, HAL_ADC_RESOLUTION_12);
	  incode_xtoc16(adval_str, adval);

	  Update_Refresh(adval_str, 4);
  }
}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	*dataLen = 0;
	return 0;
}