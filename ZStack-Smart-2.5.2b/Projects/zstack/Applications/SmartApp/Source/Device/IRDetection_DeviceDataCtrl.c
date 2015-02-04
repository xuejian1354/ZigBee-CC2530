/**************************************************************************************************
  Filename:       IRDetection_DeviceDataCtrl.c
  Revised:        $Date: 2014-12-01 14:27:34 -0800 (Thu, 01 dec 2014) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of device common data operations
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-12-01
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-02-04
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

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
 
/*********************************************************************
 * GLOBAL VARIABLES
 */
static uint8 *optData = NULL;
static uint8 optDataLen = 0;

/*********************************************************************
 * LOCAL VARIABLES
 */
//static uint8 devData[FRAME_DATA_SIZE] = {0};
//static uint8 devDataLen = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{

}

int8 CommonDevice_SetData(uint8 const *data, uint8 dataLen)
{
	if(optData != NULL && optDataLen < dataLen && dataLen <= MAX_OPTDATA_SIZE)
	{
		osal_mem_free(optData);
		optData = NULL;
	}

	if(dataLen <= MAX_OPTDATA_SIZE)
	{
		if(optData == NULL && dataLen != 0)
		{
			optData = osal_mem_alloc(dataLen);
		}

		osal_memcpy(optData, data, dataLen);
		optDataLen = dataLen;

		return 0;
	}	
	
	return -1;
}


int8 CommonDevice_GetData(uint8 *data, uint8 *dataLen)
{
	*dataLen = optDataLen;
	osal_memcpy(data, optData, *dataLen);
	
	return 0;
}


