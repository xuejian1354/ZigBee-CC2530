/**************************************************************************************************
  Filename:       RelaySocket_DeviceDataCtrl.c
  Revised:        $Date: 2015-10-26 11:19:11 -0800 (Mon, 02 Oct 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of relay socket device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-10-26
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 
#include "hal_led.h"
#include "mincode.h"

/*********************************************************************
 * MACROS
 */
#define SSC_DATA_SIZE	2

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern byte CommonApp_TaskID;

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
  HAL_TURN_ON_SSC();
  SSC_DDR |= SSC_BV;
}

void HalStatesInit(devStates_t status)
{}

#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
void BindBtn_Ctrl(void)
{
  HAL_TOGGLE_SSC();
}
#endif

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	if (osal_memcmp(data, "00", 2))
	{
		HAL_TURN_OFF_SSC();
	}
	else if (osal_memcmp(data, "01", 2))
	{
		HAL_TURN_ON_SSC();
	}
	else
	{
		if(optData!=NULL && optDataLen<SSC_DATA_SIZE)
		{
			osal_mem_free(optData);
			optData = NULL;
			optDataLen = 0;
		}

		if(optData == NULL)
		{
			optData = osal_mem_alloc(SSC_DATA_SIZE);
			optDataLen = SSC_DATA_SIZE;
		}
		
		osal_memcpy(optData, "FF", 2);
	}
	
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	if(optData!=NULL && optDataLen<SSC_DATA_SIZE)
	{
		osal_mem_free(optData);
		optData = NULL;
		optDataLen = 0;
	}

	if(optData == NULL)
	{
		optData = osal_mem_alloc(SSC_DATA_SIZE);
		optDataLen = SSC_DATA_SIZE;
	}

	if (HAL_STATE_SSC())
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