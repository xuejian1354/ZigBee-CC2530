/**************************************************************************************************
  Filename:       SuperButton_DeviceDataCtrl.c
  Revised:        $Date: 2015-09-11 14:03:11 -0800 (Sep, 02 Fri 2015) $
  Revision:       $Revision: 29217 $

  Description:    Super Button Board Data Ctrl.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-09-11
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
#define SW1_DATA_SIZE	2

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern uint8 SHORT_ADDR_G[4];
extern uint8 EXT_ADDR_G[16];
extern const uint8 f_tail[4];

extern bool isPermitJoining;

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
  HAL_TURN_OFF_OLC1();
  OLC1_DDR |= OLC1_BV;
}

void HalStatesInit(devStates_t status)
{}

#ifdef KEY_PUSH_PORT_1_BUTTON
void DeviceCtrl_HandlePort1Keys(uint16 keys, uint8 keyCounts)
{
#ifndef HAL_KEY_LONG_SHORT_DISTINGUISH
  uint8 *keysID = get_keys_id();

  /* Initial Join NWK */
  if(osal_memcmp(keysID, "444444", keyCounts))
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
#endif

  if(keys & HAL_KEY_PORT_1_SWITCH_4)
  {
  	/* Output Logic Control */
	if (keyCounts == 1)
	{
  	  //OLC1_DDR |= OLC1_BV;
      HAL_TOGGLE_OLC1();
	  
	  UO_t mFrame;
      memcpy(mFrame.head, FR_HEAD_UO, 3);
#ifdef RTR_NWK
      mFrame.type = FR_DEV_ROUTER;
#else
      mFrame.type = FR_DEV_ENDDEV;
#endif
      memcpy(mFrame.ed_type, FR_APP_DEV, 2);
      memcpy(mFrame.short_addr, SHORT_ADDR_G, 4);
      memcpy(mFrame.ext_addr, EXT_ADDR_G, 16);
	  get_device_data(NULL, NULL);
      mFrame.data = optData;
	  mFrame.data_len = optDataLen;
      memcpy(mFrame.tail, f_tail, 4);

      uint8 *fBuf;
	  uint16 fLen;
      if(!SSAFrame_Package(HEAD_UO, &mFrame, &fBuf, &fLen))
      {
        CommonApp_SendTheMessage(COORDINATOR_ADDR, fBuf, fLen);
      }
	}
#if defined(HOLD_INIT_AUTHENTICATION) && !defined(HAL_KEY_LONG_SHORT_DISTINGUISH)
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
  }
}
#endif

/*
  * "00"	Open
  * "01"	Close
  * Total 6 bits.
 */
int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	if (osal_memcmp(data, "00", 2))
	{
		HAL_TURN_OFF_OLC1();
	}
	else if (osal_memcmp(data, "01", 2))
	{
		HAL_TURN_ON_OLC1();
	}
	else
	{
		if(optData!=NULL && optDataLen<SW1_DATA_SIZE)
		{
			osal_mem_free(optData);
			optData = NULL;
			optDataLen = 0;
		}

		if(optData == NULL)
		{
			optData = osal_mem_alloc(SW1_DATA_SIZE);
			optDataLen = SW1_DATA_SIZE;
		}
		
		osal_memcpy(optData, "FF", 2);
	}
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	if(optData!=NULL && optDataLen<SW1_DATA_SIZE)
	{
		osal_mem_free(optData);
		optData = NULL;
		optDataLen = 0;
	}

	if(optData == NULL)
	{
		optData = osal_mem_alloc(SW1_DATA_SIZE);
		optDataLen = SW1_DATA_SIZE;
	}

	if (HAL_STATE_OLC1())
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

