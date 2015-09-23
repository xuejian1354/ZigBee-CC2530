/**************************************************************************************************
  Filename:       HueLight_DeviceDataCtrl.c
  Revised:        $Date: 2015-08-21 10:27:35 -0800 (Fir, 21 Aug 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of hue light device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-09-23
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
#include "mincode.h"

#include "hw_light_ctrl.h"

/* HAL */
#include "hal_timer.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "hal_drivers.h"

/*********************************************************************
 * MACROS
 */
#define HUELIGHT_GETSTATUS		0x10
#define HUELIGHT_HEAD_FLAG		139
#define HUELIGHT_COUNT_TRESHOLD		2000u

/*********************************************************************
 * CONSTANTS
 */
#define HUELIGHT_DATA_SIZE	8


/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern uint8 *optData;
extern uint8 optDataLen;

extern byte CommonApp_TaskID;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
 
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 onOff = 1;
static uint8 stepCount = 1;
uint8 bright = 0xFE;
uint8 hue = 0x80;
uint8 saturation = 0;

static uint32 sysOClock;


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void HueLight_CountCB( void *params, uint16 *duration, uint8 *count);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{
  	PERCFG |= 0x03;
	HalTimer1Init(0);
	
	DISABLE_LAMP();

	uint8 nvData[5];
	if (ZSUCCESS == osal_nv_item_init( 
		ZCD_NV_HUELIGHT_DATASET, sizeof(nvData),  &nvData))
	{
		osal_nv_read(
			ZCD_NV_HUELIGHT_DATASET, 0, sizeof(nvData),  &nvData);

		if(nvData[0] == HUELIGHT_HEAD_FLAG)
		{
			onOff = nvData[1];
			bright = nvData[2];
			hue = nvData[3];
			saturation = nvData[4];
		}
	}

	if(optData != NULL && optDataLen < HUELIGHT_DATA_SIZE)
	{
		osal_mem_free(optData);
		optData = NULL;
		optDataLen = 0;
	}

	if(optData == NULL)
	{
		optData = osal_mem_alloc(HUELIGHT_DATA_SIZE);
	}
	
	incode_xtocs(optData, &onOff, 1);
	incode_xtocs(optData+2, &bright, 1);
	incode_xtocs(optData+4, &hue, 1);
	incode_xtocs(optData+6, &saturation, 1);

	optDataLen = HUELIGHT_DATA_SIZE;
}

void HalStatesInit(devStates_t status)
{
	ENABLE_LAMP();
	
	hwLight_UpdateOnOff(onOff);
	hwLight_UpdateLampColorHueSat(hue, saturation, bright);

	sysOClock = osal_GetSystemClock();
}

#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
void BindBtn_Ctrl(void)
{
	if(stepCount < 8)
	{
		stepCount = hue/32;
		stepCount++;
	}
	
	switch(stepCount)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		onOff = 1;
		bright = 0xFD;
		saturation = 0xFD;
		hue = 32*stepCount;
		if(stepCount == 8)
		{
			stepCount++;
		}
		hwLight_UpdateOnOff(onOff);
		hwLight_UpdateLampColorHueSat(hue-32, saturation, bright);
		break;

	case 9:
		onOff = 1;
		bright = 0xFD;
		saturation = 0;
		hwLight_UpdateOnOff(onOff);
		hwLight_UpdateLampColorHueSat(hue, saturation, bright);
		stepCount++;
		break;

	default:
		onOff = 0;
		hue = 0;
		stepCount = 0;
		hwLight_UpdateOnOff(onOff);
		break;
	}
}
#endif

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	uint8 mOnOff;
	uint8 mBright;
	uint8 mHue;
	uint8 mSaturation;
	
	if(dataLen < HUELIGHT_DATA_SIZE)
	{
		return 0;
	}

	incode_ctoxs(&mOnOff, (uint8 *)(data), 2);
	if(mOnOff & HUELIGHT_GETSTATUS)
	{
		mOnOff = onOff | HUELIGHT_GETSTATUS;
		incode_xtocs(optData, &mOnOff, 1);
		incode_xtocs(optData+2, &bright, 1);
		incode_xtocs(optData+4, &hue, 1);
		incode_xtocs(optData+6, &saturation, 1);

		optDataLen = HUELIGHT_DATA_SIZE;
		return 0;
	}
	else if(mOnOff != onOff)
	{
		onOff = mOnOff;
		hwLight_UpdateOnOff(onOff);
	}
	
	if(onOff)
	{
		incode_ctoxs(&mBright, (uint8 *)(data+2), 2);
		incode_ctoxs(&mHue, (uint8 *)(data+4), 2);
		incode_ctoxs(&mSaturation, (uint8 *)(data+6), 2);

		if(bright != mBright || hue != mHue || saturation != mSaturation)
		{
			bright = mBright;
			hue = mHue;
			saturation = mSaturation;

			hwLight_UpdateLampColorHueSat(hue, saturation, bright);

			uint8 nvData[5];
			if (ZSUCCESS == osal_nv_item_init( 
								ZCD_NV_HUELIGHT_DATASET, sizeof(nvData),  &nvData)
					&& (osal_GetSystemClock()-sysOClock) > HUELIGHT_COUNT_TRESHOLD)
			{
				nvData[0] = HUELIGHT_HEAD_FLAG;
				nvData[1] = onOff;
				nvData[2] = bright;
				nvData[3] = hue;
				nvData[4] = saturation;
				
				osal_nv_write(
					ZCD_NV_HUELIGHT_DATASET, 0, sizeof(nvData),  &nvData);

				sysOClock = osal_GetSystemClock();
				osal_stop_timerEx(CommonApp_TaskID, HUELIGHT_COUNT_EVT);
			}
			else
			{
				update_user_event(CommonApp_TaskID, HUELIGHT_COUNT_EVT, HueLight_CountCB, 
	  				HUELIGHT_TIMEOUT, TIMER_ONE_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);
			}
		}
	}
	
	return 0;
}

void HueLight_CountCB( void *params, uint16 *duration, uint8 *count)
{
	uint8 nvData[5];
	if (ZSUCCESS == osal_nv_item_init( 
						ZCD_NV_HUELIGHT_DATASET, sizeof(nvData),  &nvData))
	{
		nvData[0] = HUELIGHT_HEAD_FLAG;
		nvData[1] = onOff;
		nvData[2] = bright;
		nvData[3] = hue;
		nvData[4] = saturation;
		
		osal_nv_write(
			ZCD_NV_HUELIGHT_DATASET, 0, sizeof(nvData),  &nvData);

		sysOClock = osal_GetSystemClock();
	}
}

int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	osal_memcpy(data, optData, optDataLen);
	*dataLen = optDataLen;
	return 0;
}

