/**************************************************************************************************
  Filename:       SolenoidValve_DeviceDataCtrl.c
  Revised:        $Date: 2015-07-10 08:46:22 -0800 (Fri, 10 Jul 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of solenoid valve device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-07-16
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 

/*********************************************************************
 * MACROS
 */
#define DEVCONTROL_SW_ON	"301"
#define DEVCONTROL_SW_OFF	"300"

#define CTRL_DATA_SIZE		3
#ifdef TRANSCONN_BOARD_ENDNODE
#define VALVESW_DATA_SIZE	(EXT_ADDR_SIZE+CTRL_DATA_SIZE)
#else
#define VALVESW_DATA_SIZE	CTRL_DATA_SIZE
#endif

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern uint8 EXT_ADDR_G[16];

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
	if (osal_memcmp(data, DEVCONTROL_SW_OFF, CTRL_DATA_SIZE))
	{
		HAL_TURN_OFF_VSW();
	}
	else if (osal_memcmp(data, DEVCONTROL_SW_ON, CTRL_DATA_SIZE))
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
			optData = osal_mem_alloc(VALVESW_DATA_SIZE);
			optDataLen = VALVESW_DATA_SIZE;
		}
		
		osal_memcpy(optData, "FFF", CTRL_DATA_SIZE);
#ifdef TRANSCONN_BOARD_ENDNODE
		osal_memcpy(optData+CTRL_DATA_SIZE, EXT_ADDR_G, EXT_ADDR_SIZE);
#endif
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
		optData = osal_mem_alloc(VALVESW_DATA_SIZE);
		optDataLen = VALVESW_DATA_SIZE;
	}

	if (HAL_STATE_VSW())
	{
		osal_memcpy(optData, DEVCONTROL_SW_ON, CTRL_DATA_SIZE);
	}
	else
	{
		osal_memcpy(optData, DEVCONTROL_SW_OFF, CTRL_DATA_SIZE);
	}

#ifdef TRANSCONN_BOARD_ENDNODE
	osal_memcpy(optData+CTRL_DATA_SIZE, EXT_ADDR_G, EXT_ADDR_SIZE);
#endif

	if(data != NULL)
	{
		*dataLen = optDataLen;
		osal_memcpy(data, optData, *dataLen);
	}
	
	return 0;
}
