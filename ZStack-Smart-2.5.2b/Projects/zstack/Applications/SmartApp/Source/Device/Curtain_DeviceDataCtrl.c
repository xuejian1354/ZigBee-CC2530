/**************************************************************************************************
  Filename:       Curtain_DeviceDataCtrl.c
  Revised:        $Date: 2015-11-30 14:27:34 -0800 (Mon, 30 Nov 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of curtain device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-11-30
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
#define CURTAIN_DATA_SIZE	2

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
static uint8 statsw = 2;
static uint8 statdir = 0;
/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void ToggleCtrl(void);
/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{
  CTC1_DDR |= CTC1_BV;
  CTC2_DDR |= CTC2_BV;
  HAL_TURN_STOP_CTC();
}

void HalStatesInit(devStates_t status)
{
	HAL_TURN_STOP_CTC();
}

void ToggleCtrl(void)
{
	if(statdir)
	{
		statsw--;
	}
	else
	{
		statsw++;
	}
	
	statsw %= 3;
	switch(statsw)
	{
	case 0:
		HAL_TURN_POSITIVE_CTC();
		statdir = 0;
		break;

	case 1:
		HAL_TURN_STOP_CTC();
		break;

	case 2:
		HAL_TURN_NEGATIVE_CTC();
		statdir = 1;
		break;
	}

	HalLedBlink(HAL_LED_3, 1, 40, 200);
}

#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT

void BindBtn_Ctrl(void)
{
	ToggleCtrl();
}
#endif

void Curtain_KeyHandler(void)
{
	ToggleCtrl();

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

/*
  * "00"	Open
  * "01"	Close
  * Total 6 bits.
 */
int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	if (osal_memcmp(data, "00", 2))
	{
		statsw = 1;
		HAL_TURN_STOP_CTC();
	}
	else if (osal_memcmp(data, "01", 2))
	{
		statsw = 2;
		HAL_TURN_NEGATIVE_CTC();
	}
	else if (osal_memcmp(data, "10", 2))
	{
		statsw = 3;
		HAL_TURN_POSITIVE_CTC();
	}
	else
	{
		if(optData!=NULL && optDataLen<CURTAIN_DATA_SIZE)
		{
			osal_mem_free(optData);
			optData = NULL;
			optDataLen = 0;
		}

		if(optData == NULL)
		{
			optData = osal_mem_alloc(CURTAIN_DATA_SIZE);
			optDataLen = CURTAIN_DATA_SIZE;
		}
		
		osal_memcpy(optData, "FF", 2);
	}

	HalLedBlink(HAL_LED_3, 1, 40, 200);
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	if(optData!=NULL && optDataLen<CURTAIN_DATA_SIZE)
	{
		osal_mem_free(optData);
		optData = NULL;
		optDataLen = 0;
	}

	if(optData == NULL)
	{
		optData = osal_mem_alloc(CURTAIN_DATA_SIZE);
		optDataLen = CURTAIN_DATA_SIZE;
	}

	switch (statsw%3)
	{
	case 0:
		osal_memcpy(optData, "10", 2);
		break;

	case 1:
		osal_memcpy(optData, "00", 2);
		break;

	case 2:
		osal_memcpy(optData, "01", 2);
		break;
	}

	if(data != NULL)
	{
		*dataLen = optDataLen;
		osal_memcpy(data, optData, *dataLen);
	}
	
	return 0;
}

