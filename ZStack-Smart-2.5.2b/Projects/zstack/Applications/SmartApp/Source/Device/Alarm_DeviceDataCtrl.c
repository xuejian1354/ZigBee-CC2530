/**************************************************************************************************
  Filename:       Alarm_DeviceDataCtrl.c
  Revised:        $Date: 2014-12-01 14:27:34 -0800 (Thu, 01 dec 2014) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of alarm device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-09-23
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 

/*********************************************************************
 * MACROS
 */
#define ALARM_DATA_SIZE	2
/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
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

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{
	ALARMIO_DDR |= ALARMIO_BV;
	HAL_TURN_OFF_ALARM();
}

void HalStatesInit(devStates_t status)
{
	
}

#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
void BindBtn_Ctrl(void)
{
	HAL_TOGGLE_ALARM();
}
#endif

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	if (osal_memcmp(data, "00", 2))
	{
		HAL_TURN_OFF_ALARM();
	}
	else if (osal_memcmp(data, "01", 2))
	{
		HAL_TURN_ON_ALARM();
	}
	else
	{
		if(optData!=NULL && optDataLen<ALARM_DATA_SIZE)
		{
			osal_mem_free(optData);
			optData = NULL;
			optDataLen = 0;
		}

		if(optData == NULL)
		{
			optData = osal_mem_alloc(ALARM_DATA_SIZE);
			optDataLen = ALARM_DATA_SIZE;
		}
		
		osal_memcpy(optData, "FF", 2);
	}
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	if(optData!=NULL && optDataLen<ALARM_DATA_SIZE)
	{
		osal_mem_free(optData);
		optData = NULL;
		optDataLen = 0;
	}

	if(optData == NULL)
	{
		optData = osal_mem_alloc(ALARM_DATA_SIZE);
		optDataLen = ALARM_DATA_SIZE;
	}

	if (HAL_STATE_ALARM())
	{
		osal_memcpy(optData, "01", 2);
	}
	else
	{
		osal_memcpy(optData, "00", 2);
	}

	if(data != NULL)
	{
		*dataLen = optDataLen;
		osal_memcpy(data, optData, *dataLen);
	}
	
	return 0;
}
