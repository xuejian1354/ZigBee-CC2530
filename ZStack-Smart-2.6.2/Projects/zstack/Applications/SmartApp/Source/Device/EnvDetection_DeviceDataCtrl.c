/**************************************************************************************************
  Filename:       EnvDetection_DeviceDataCtrl.c
  Revised:        $Date: 2014-12-01 14:27:34 -0800 (Thu, 01 dec 2014) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of environment device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-07-02
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 
#include "OSAL_Nv.h"
#include "OLCD.h"
#include <stdio.h>

/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern byte CommonApp_TaskID;
extern devStates_t CommonApp_NwkState;

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
static AirController_Opt_t EnvDetectionOpt;
static PM25_Threshold_CallBack mPM25_Threshold_CallBack = NULL;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static int8 EnvDetection_PM25_Threshold_CallBack(void);
static AirController_Method_t get_EnvDetection_Method_from_str(int8 *str);
static void EnvDetection_UpDataCmdCB( void *params, uint16 *duration, uint8 *count);
static void EnvDetection_SetGetData(void);
static void EnvDetection_TxHandler(uint8 txBuf[], uint8 txLen);
static void Show_logo(void);
static void Show_val(uint16 val);
static void Show_hold(uint8 mode, uint16 hold);
static void mSprintf(char *dst, char *src1, uint16 src2);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * FUNCTIONS
 */
void SetPM25Val(uint16 val)
{
	EnvDetectionOpt.PM25_val = val;
}

uint16 GetPM25Val(void)
{
	return EnvDetectionOpt.PM25_val;
}

void SetPM25ThresCallBack(uint8 mode, uint16 threshold, PM25_Threshold_CallBack func)
{
	switch(mode)
	{
	case AIRCONTROL_PM25_THRESMODE_UP:
	case AIRCONTROL_PM25_THRESMODE_DOWN: 
		EnvDetectionOpt.PM25_thresmode = mode;
		EnvDetectionOpt.PM25_threshold = threshold;
		break;
		
	default: 
		EnvDetectionOpt.PM25_thresmode = AIRCONTROL_PM25_THRESMODE_UNABLE;
		break;
	}
	
	mPM25_Threshold_CallBack = func;
}

void PM25_Threshold_Handler(void)
{
	if(EnvDetectionOpt.PM25_thresmode == AIRCONTROL_PM25_THRESMODE_UNABLE)
	{
		return;
	}

	if(EnvDetectionOpt.PM25_thresmode == AIRCONTROL_PM25_THRESMODE_UP)
	{
		if(EnvDetectionOpt.PM25_val >= EnvDetectionOpt.PM25_threshold)
		{
			if(EnvDetectionOpt.PM25_threstrigger != 1
				&& mPM25_Threshold_CallBack != NULL
				&& mPM25_Threshold_CallBack() >= 0)
			{
				EnvDetectionOpt.PM25_threstrigger = 1;
			}
		}
		else
		{
			EnvDetectionOpt.PM25_threstrigger = 0;
		}
	}
	else if(EnvDetectionOpt.PM25_thresmode == AIRCONTROL_PM25_THRESMODE_DOWN)
	{
		if(EnvDetectionOpt.PM25_val <= EnvDetectionOpt.PM25_threshold)
		{
			if(EnvDetectionOpt.PM25_threstrigger != 1
				&& mPM25_Threshold_CallBack != NULL
				&& mPM25_Threshold_CallBack() >= 0)
			{
				EnvDetectionOpt.PM25_threstrigger = 1;
			}
		}
		else
		{
			EnvDetectionOpt.PM25_threstrigger = 0;
		}
	}
}

AirController_Method_t get_EnvDetection_Method_from_str(int8 *str)
{
	if(str == NULL)
	{
		return AIRCONTROL_NONE;
	}
	
	if(osal_memcmp(AIRCONTROLLER_PM25_READVAL_MED, str, 3))
	{
		return AIRCONTROL_PM25_READVAL;
	}
	else if(osal_memcmp(AIRCONTROLLER_PM25_READMODE_MED, str, 3))
	{
		return AIRCONTROL_PM25_READMODE;
	}
	else if(osal_memcmp(AIRCONTROLLER_PM25_READHOLD_MED, str, 3))
	{
		return AIRCONTROL_PM25_READHOLD;
	}
	else if(osal_memcmp(AIRCONTROLLER_PM25_SETMODE_MED, str, 3))
	{
		return AIRCONTROL_PM25_SETMODE;
	}
	else if(osal_memcmp(AIRCONTROLLER_PM25_SETHOLD_MED, str, 3))
	{
		return AIRCONTROL_PM25_SETHOLD;
	}
	else if(osal_memcmp(AIRCONTROLLER_GET_DATA_MED, str, 3))
	{
		return AIRCONTROL_GET_DATA;
	}

	return AIRCONTROL_NONE;
}

int8 EnvDetection_PM25_Threshold_CallBack(void)
{
	uint8 buf[10] = {0};
	switch(EnvDetectionOpt.PM25_thresmode)
	{
	case AIRCONTROL_PM25_THRESMODE_UNABLE: 
		osal_memcpy(buf, "Off", 3);
		break;

	case AIRCONTROL_PM25_THRESMODE_UP: 
		osal_memcpy(buf, "Up", 2);
		incode_2_to_16(buf+2, (uint8 *)&EnvDetectionOpt.PM25_threshold, 2);
		break;

	case AIRCONTROL_PM25_THRESMODE_DOWN: 
		osal_memcpy(buf, "Down", 4);
		incode_2_to_16(buf+4, (uint8 *)&EnvDetectionOpt.PM25_threshold, 2);
		break;
	}

	Update_Refresh(buf, strlen((char *)buf));
	return 0;
}

void HalDeviceInit (void)
{
	SerialTx_Handler(SERIAL_COM_PORT, EnvDetection_TxHandler);
	
	memset(&EnvDetectionOpt, 0, sizeof(AirController_Opt_t));
	if (ZSUCCESS == osal_nv_item_init( 
          				ZCD_NV_AIRCONTROLLER_MODE, 
          				sizeof(EnvDetectionOpt.PM25_thresmode),  
          				&EnvDetectionOpt.PM25_thresmode))
    {
      osal_nv_read(ZCD_NV_AIRCONTROLLER_MODE, 
	  				0, 
	  				sizeof(EnvDetectionOpt.PM25_thresmode),  
	  				&EnvDetectionOpt.PM25_thresmode);
    }

	if (ZSUCCESS == osal_nv_item_init( 
          				ZCD_NV_AIRCONTROLLER_HOLD, 
          				sizeof(EnvDetectionOpt.PM25_threshold),  
          				&EnvDetectionOpt.PM25_threshold))
    {
      osal_nv_read(ZCD_NV_AIRCONTROLLER_HOLD, 
	  				0, 
	  				sizeof(EnvDetectionOpt.PM25_threshold),  
	  				&EnvDetectionOpt.PM25_threshold);
    }

	switch(EnvDetectionOpt.PM25_thresmode)
	{
	case AIRCONTROL_PM25_THRESMODE_UP: 
		SetPM25ThresCallBack(AIRCONTROL_PM25_THRESMODE_UP, 
								EnvDetectionOpt.PM25_threshold, 
								EnvDetection_PM25_Threshold_CallBack);
		break;
		
	case AIRCONTROL_PM25_THRESMODE_DOWN: 
		SetPM25ThresCallBack(AIRCONTROL_PM25_THRESMODE_DOWN, 
								EnvDetectionOpt.PM25_threshold, 
								EnvDetection_PM25_Threshold_CallBack);
		break;
		
	case AIRCONTROL_PM25_THRESMODE_UNABLE: 
		SetPM25ThresCallBack(AIRCONTROL_PM25_THRESMODE_UNABLE, 0, NULL);
		break;
		
	default: 
		SetPM25ThresCallBack(AIRCONTROL_PM25_THRESMODE_UP, AIRCONTROL_PM25_DEFAULT_TRESHOLD, 
							EnvDetection_PM25_Threshold_CallBack);

		SetThresHold(AIRCONTROL_PM25_THRESMODE_UP, AIRCONTROL_PM25_DEFAULT_TRESHOLD);
		break;
	}

	EnvDetection_SetGetData();

  	LCD_Init();
  	LCD_Fill(0xff);
  	LCD_Fill(0x00);
  	Show_logo();
}

void SetThresHold(uint8 mode, uint16 hold)
{
	EnvDetectionOpt.PM25_thresmode = mode;
	EnvDetectionOpt.PM25_threshold = hold;
	
	if (ZSUCCESS == osal_nv_item_init( ZCD_NV_AIRCONTROLLER_MODE, 
          				sizeof(EnvDetectionOpt.PM25_thresmode),  
          				&EnvDetectionOpt.PM25_thresmode))
    {
      osal_nv_write(ZCD_NV_AIRCONTROLLER_MODE, 
	  				0, 
	  				sizeof(EnvDetectionOpt.PM25_thresmode),  
	  				&EnvDetectionOpt.PM25_thresmode);
    }

	if (ZSUCCESS == osal_nv_item_init( ZCD_NV_AIRCONTROLLER_HOLD, 
          				sizeof(EnvDetectionOpt.PM25_threshold),  
          				&EnvDetectionOpt.PM25_threshold))
    {
      osal_nv_write(ZCD_NV_AIRCONTROLLER_HOLD, 
	  				0, 
	  				sizeof(EnvDetectionOpt.PM25_threshold),  
	  				&EnvDetectionOpt.PM25_threshold);
    }
}

void HalStatesInit(devStates_t status)
{
  set_user_event(CommonApp_TaskID, AIRCONTROLLER_QUERY_EVT, EnvDetection_UpDataCmdCB, 
  		AIRCONTROLLER_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);
}

void EnvDetection_UpDataCmdCB( void *params, uint16 *duration, uint8 *count)
{
  if( CommonApp_NwkState == DEV_ROUTER || CommonApp_NwkState == DEV_END_DEVICE)
  {
  	EnvDetection_SetGetData();
	Update_Refresh(optData, optDataLen);
  }
}

void EnvDetection_SetGetData(void)
{
	if(optData != NULL)
	{
		osal_mem_free(optData);
	}
	optData = osal_mem_alloc(13);
	optDataLen = 13;
	osal_memcpy(optData, AIRCONTROLLER_GET_DATA_MED, 3);
	
	switch(EnvDetectionOpt.PM25_thresmode)
	{
	case AIRCONTROL_PM25_THRESMODE_UP:	
		osal_memcpy(optData+3, "01", 2);
		break;

	case AIRCONTROL_PM25_THRESMODE_DOWN:
		osal_memcpy(optData+3, "02", 2);
		break;

	default:
		osal_memcpy(optData+3, "03", 2);
		break;
	}
	incode_2_to_16(optData+5, (uint8 *)&EnvDetectionOpt.PM25_threshold, 2);
	incode_2_to_16(optData+9, (uint8 *)&EnvDetectionOpt.PM25_val, 2);
}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	AirController_Method_t am = AIRCONTROL_NONE;
	if(dataLen >= 3)
	{
		am = get_EnvDetection_Method_from_str((int8 *)data);
	}

	switch(am)

	{
	case AIRCONTROL_PM25_READVAL: 
		if(optDataLen < 7)
		{
			osal_mem_free(optData);
			optData = osal_mem_alloc(7);	
		}
		optDataLen = 7;
		osal_memcpy(optData, AIRCONTROLLER_PM25_READVAL_MED, 3);
		incode_2_to_16(optData+3, (uint8 *)&EnvDetectionOpt.PM25_val, 2);
		break;
		
	case AIRCONTROL_PM25_READMODE: 
		if(optDataLen < 5)
		{
			osal_mem_free(optData);
			optData = osal_mem_alloc(5);
		}
		optDataLen = 5;
		osal_memcpy(optData, AIRCONTROLLER_PM25_READMODE_MED, 3);
		incode_2_to_16(optData+3, (uint8 *)&EnvDetectionOpt.PM25_thresmode, 1);
		break;
		
	case AIRCONTROL_PM25_READHOLD: 
		if(optDataLen < 7)
		{
			osal_mem_free(optData);
			optData = osal_mem_alloc(7);
		}
		optDataLen = 7;
		osal_memcpy(optData, AIRCONTROLLER_PM25_READHOLD_MED, 3);
		incode_2_to_16(optData+3, (uint8 *)&EnvDetectionOpt.PM25_threshold, 2);
		break;
		
	case AIRCONTROL_PM25_SETMODE: 
		if(dataLen >= 5)
		{
			EnvDetectionOpt.PM25_thresmode = atox((uint8 *)(data+3), 2);
			uint8 mode = EnvDetectionOpt.PM25_thresmode;
			if (ZSUCCESS == osal_nv_item_init( 
                  ZCD_NV_AIRCONTROLLER_MODE, sizeof(mode),  &mode))
	        {
	          osal_nv_write(ZCD_NV_AIRCONTROLLER_MODE, 0, sizeof(mode),  &mode);
	        }
			
			if(dataLen >= 9)
			{
				EnvDetectionOpt.PM25_threshold = 0;
			  	incode_16_to_2(&EnvDetectionOpt.PM25_threshold, 
												(uint8 *)(data+5), 4);
				uint16 hold = EnvDetectionOpt.PM25_threshold;
				if (ZSUCCESS == osal_nv_item_init( 
	                  ZCD_NV_AIRCONTROLLER_HOLD, sizeof(hold),  &hold))
		        {
		          osal_nv_write(ZCD_NV_AIRCONTROLLER_HOLD, 0, sizeof(hold),  &hold);
		        }
			}

			Show_hold(EnvDetectionOpt.PM25_thresmode, EnvDetectionOpt.PM25_threshold);

			optData[3] = 'O';
			optData[4] = 'K';
			optDataLen = 5;
		}
		else
		{
			return -1;
		}
		break;
		
	case AIRCONTROL_PM25_SETHOLD: 
		if(dataLen >= 7)
		{
			uint16 hold = 0;
			incode_16_to_2(&hold, (uint8 *)(data+3), 4);

			EnvDetectionOpt.PM25_threshold = hold;
			Show_hold(EnvDetectionOpt.PM25_thresmode, EnvDetectionOpt.PM25_threshold);			
			if (ZSUCCESS == osal_nv_item_init( 
                  ZCD_NV_AIRCONTROLLER_HOLD, sizeof(hold),  &hold))
	        {
	          osal_nv_write(ZCD_NV_AIRCONTROLLER_HOLD, 0, sizeof(hold),  &hold);
	        }
			
			optData[3] = 'O';
			optData[4] = 'K';
			optDataLen = 5;
		}
		else
		{
			return -1;
		}
		break;

	case AIRCONTROL_GET_DATA:
		if(optDataLen < 13)
		{
			osal_mem_free(optData);
			optData = osal_mem_alloc(13);
		}
		optDataLen = 13;
		osal_memcpy(optData, AIRCONTROLLER_GET_DATA_MED, 3);
		
		switch(EnvDetectionOpt.PM25_thresmode)
		{
		case AIRCONTROL_PM25_THRESMODE_UP:	
			osal_memcpy(optData+3, "01", 2);
			break;

		case AIRCONTROL_PM25_THRESMODE_DOWN:
			osal_memcpy(optData+3, "02", 2);
			break;

		default:
			osal_memcpy(optData+3, "03", 2);
			break;
		}
		incode_2_to_16(optData+5, (uint8 *)&EnvDetectionOpt.PM25_threshold, 2);
		incode_2_to_16(optData+9, (uint8 *)&EnvDetectionOpt.PM25_val, 2);
		break;

	default: return -1;
	}
	
	return 0;
}

int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	if(optData != NULL && optDataLen > 0)
	{
		osal_memcpy(data, optData, optDataLen);
		*dataLen = optDataLen;
	}
	else
	{
		*dataLen = 0;
	}
	
	return 0;
}

void EnvDetection_TxHandler(uint8 txBuf[], uint8 txLen)
{
	DATA_CMD_T data_cmd;
   	
	osal_memcpy(&data_cmd, txBuf, txLen);
    if(data_cmd.Head==0xAA)
  	{
    	uint16 PM25_val=(data_cmd.PM25[0]
					+(data_cmd.PM25[1]<<8))/10;

		if(PM25_val != GetPM25Val())
		{
			SetPM25Val(PM25_val);
			Show_val(EnvDetectionOpt.PM25_val);
			Show_hold(EnvDetectionOpt.PM25_thresmode, EnvDetectionOpt.PM25_threshold);
			PM25_Threshold_Handler();
		}
  	} 
}

void Show_logo(void)
{   
	LCD_P8x16Str(0, 0, "AirController");
	LCD_P6x8Str(0, 7, "         www.lysoc.cn");
}

void Show_val(uint16 val)
{   
	char val_buf[16] = {0};
	sprintf(val_buf, "PM2.5  %dug/m3 ", val);
	LCD_P8x16Str(0, 3, (uint8 *)val_buf);
}

void Show_hold(uint8 mode, uint16 hold)
{
	char buf[16] = {0};
	switch(mode)
	{
	case AIRCONTROL_PM25_THRESMODE_UNABLE:
		sprintf(buf, "M:%s", "Off          ");
		break;

	case AIRCONTROL_PM25_THRESMODE_UP:
		mSprintf(buf, "Up", hold);
		break;

	case AIRCONTROL_PM25_THRESMODE_DOWN:
		mSprintf(buf, "Down", hold);
		break;
	}

	LCD_P8x16Str(0, 5, (uint8 *)buf);
}

void mSprintf(char *dst, char *src1, uint16 src2)
{
	//sprintf(buf, "M:%-4s H:%-3d", src1, src2);
	uint8 src1Len = strlen(src1);
	switch(src1Len)
	{
	case 0:
		sprintf(dst, "M:     H:%d", src2);
		break;

	case 1:
		sprintf(dst, "M:%s    H:%d", src1, src2);
		break;

	case 2:
		sprintf(dst, "M:%s   H:%d", src1, src2);
		break;

	case 3:
		sprintf(dst, "M:%s  H:%d", src1, src2);
		break;

	default:
		sprintf(dst, "M:%s H:%d", src1, src2);
		break;
	}
} 