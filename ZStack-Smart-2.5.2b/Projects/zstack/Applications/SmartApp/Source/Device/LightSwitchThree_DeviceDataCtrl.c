/**************************************************************************************************
  Filename:       LightSwitchThree_DeviceDataCtrl.c
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
#include "hal_lcd.h"
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
extern bool isPermitJoining;

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
  HAL_TURN_OFF_OLC1();
  OLC1_DDR |= OLC1_BV;
  HAL_TURN_OFF_OLC2();
  OLC2_DDR |= OLC2_BV;
  HAL_TURN_OFF_OLC3();
  OLC3_DDR |= OLC3_BV; 
}

#ifdef KEY_PUSH_PORT_1_BUTTON
void DeviceCtrl_HandlePort1Keys(uint16 keys, uint8 keyCounts)
{
  uint8 *keysID = get_keys_id();

  /* Initial Join NWK */
  if(osal_memcmp(keysID, "34543", keyCounts))
  {
	if(devState == DEV_HOLD)
    {
      ZDOInitDevice( 0 );
    }
    else
    {
      if(isPermitJoining)
      {
        CommonApp_PermitJoiningRequest(PERMIT_JOIN_FORBID);
      }
      else
      {
        CommonApp_PermitJoiningRequest(PERMIT_JOIN_TIMEOUT);
      }
    }
  }

  if(keys & (HAL_KEY_PORT_1_SWITCH_3 | HAL_KEY_PORT_1_SWITCH_4 | HAL_KEY_PORT_1_SWITCH_5))
  {
  	/* Output Logic Control */
	if (keyCounts == 1)
	{
	  if(keys & HAL_KEY_PORT_1_SWITCH_3)
	  {
	    HAL_TOGGLE_OLC1();
	  }
	  else if(keys & HAL_KEY_PORT_1_SWITCH_3)
	  {
	    HAL_TOGGLE_OLC2();
	  }
	  else if(keys & HAL_KEY_PORT_1_SWITCH_3)
	  {
	    HAL_TOGGLE_OLC3();
	  }
	}
#if defined(HOLD_INIT_AUTHENTICATION)
	/* Reset Factory Mode */
    else if(devState!=DEV_HOLD && keyCounts==0)
    {
      HalLedBlink ( HAL_LED_4, 0, 50, 100 );
      devStates_t tStates;
      if (ZSUCCESS == osal_nv_item_init( 
                  ZCD_NV_NWK_HOLD_STARTUP, sizeof(tStates),  &tStates))
      {
         tStates = DEV_HOLD;
         osal_nv_write(
                ZCD_NV_NWK_HOLD_STARTUP, 0, sizeof(tStates),  &tStates);
      }

      zgWriteStartupOptions(ZG_STARTUP_SET, ZCD_STARTOPT_DEFAULT_NETWORK_STATE);
      WatchDogEnable( WDTIMX );
    }
#endif
	/* Ouput Keys Combine */
	else
	{
	  CommonApp_SendTheMessage(0x0000, keysID, keyCounts);
	}
  }
}
#endif

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


