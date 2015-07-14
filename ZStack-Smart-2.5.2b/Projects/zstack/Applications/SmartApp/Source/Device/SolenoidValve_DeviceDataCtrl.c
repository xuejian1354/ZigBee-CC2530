/**************************************************************************************************
  Filename:       SolenoidValve_DeviceDataCtrl.c
  Revised:        $Date: 2015-07-10 08:46:22 -0800 (Fri, 10 Jul 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of solenoid valve device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-06-14
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 

/*********************************************************************
 * MACROS
 */
#define VALVESW_DATA_SIZE	2

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
  HAL_TURN_OFF_VSW();
  VSW_DDR |= VSW_BV;
}

void HalStatesInit(devStates_t status)
{}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	if (osal_memcmp(data, "00", 2))
	{
		HAL_TURN_OFF_VSW();
	}
	else if (osal_memcmp(data, "01", 2))
	{
		HAL_TURN_ON_VSW();
	}
	else
	{
		if(optData != NULL && optDataLen < VALVESW_DATA_SIZE)
		{
			osal_mem_free(optData);
			optData = NULL;
			optDataLen = 0;
		}

		if(optData == NULL)
		{
			osal_mem_alloc(VALVESW_DATA_SIZE);
			optDataLen = VALVESW_DATA_SIZE;
		}
		
		osal_memcpy(optData, "FF", 2);
	}
	
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	if(optData != NULL && optDataLen < VALVESW_DATA_SIZE)
	{
		osal_mem_free(optData);
		optData = NULL;
		optDataLen = 0;
	}

	if(optData == NULL)
	{
		osal_mem_alloc(VALVESW_DATA_SIZE);
		optDataLen = VALVESW_DATA_SIZE;
	}

	if (HAL_STATE_VSW())
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
