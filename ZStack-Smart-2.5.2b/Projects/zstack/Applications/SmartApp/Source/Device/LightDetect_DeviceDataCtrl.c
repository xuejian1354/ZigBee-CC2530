/**************************************************************************************************
  Filename:       LightDetect_DeviceDataCtrl.c
  Revised:        $Date: 2015-12-02 11:19:11 -0800 (Wed, 02 Dec 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of light detect device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-12-03
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
#define BIND_CURTAIN_SIZE	128

#define BINDCTRL_LIGHTDETECT_FLAG	"LTD"
#define BINDCTRL_CURTAIN_FLAG		"CTN"

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern byte CommonApp_TaskID;
extern devStates_t CommonApp_NwkState;
extern const uint8 f_tail[4];

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
 
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
uint16 curtain_zaddrs[BIND_CURTAIN_SIZE] = {0};
int curtain_size = 0;
int curtain_index = 0;
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
  	  int i = 0;
  	  uint8 adval_str[6];
	  osal_memcpy(adval_str, "AD", 2);
	  uint16 adval = HalAdcRead(HAL_ADC_CHANNEL_1, HAL_ADC_RESOLUTION_12);
	  incode_xtoc16(adval_str+2, adval);

	  while(i < curtain_size)
	  {
	  	DE_t mFrame;
		uint8 *fBuf;		//pointer data buffer
		uint16 fLen;		//buffer data length
	  
		memcpy(mFrame.head, FR_HEAD_DE, 3);
		memcpy(mFrame.cmd, FR_CMD_SINGLE_EXCUTE, 4);
		incode_xtoc16(mFrame.short_addr, curtain_zaddrs[i]);
		mFrame.data = adval_str;
		mFrame.data_len = 6;
		memcpy(mFrame.tail, f_tail, 4);

		if(!SSAFrame_Package(HEAD_DE, &mFrame, &fBuf, &fLen))
		{
			CommonApp_SendTheMessage(curtain_zaddrs[i], fBuf, fLen);
		}

		i++;
	  }
  }
}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	if(dataLen >= 7)
	{
		uint16 curtain_zaddr;
		if(osal_memcmp(data, BINDCTRL_CURTAIN_FLAG, 3))
		{
			incode_ctox16(&curtain_zaddr, (uint8 *)(data+3));
			curtain_zaddrs[curtain_index++] = curtain_zaddr;
			if(curtain_size < BIND_CURTAIN_SIZE)
			{
				curtain_size++;
			}
		}
		else if(osal_memcmp(data, BINDCTRL_LIGHTDETECT_FLAG, 3))
		{
			int i;
			int num = (dataLen-3)/4;
			for(i=0; i<num; i++)
			{
				incode_ctox16(&curtain_zaddr, (uint8 *)(data+3+(i<<2)));
				curtain_zaddrs[curtain_index++] = curtain_zaddr;
				if(curtain_size < BIND_CURTAIN_SIZE)
				{
					curtain_size++;
				}
			}			
		}
	}

	if(curtain_index >= BIND_CURTAIN_SIZE)
	{
		curtain_index = 0;
	}

	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	*dataLen = 0;
	return 0;
}