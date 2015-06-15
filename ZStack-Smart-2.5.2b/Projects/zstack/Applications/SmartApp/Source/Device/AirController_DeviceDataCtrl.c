/**************************************************************************************************
  Filename:       AirController_DeviceDataCtrl.c
  Revised:        $Date: 2015-06-15 09:49:35 -0800 (Mon, 15 Jun 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of air controller device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-06-15
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 

/*********************************************************************
 * MACROS
 */
#define AIRCONTROLLER_IR_SEND_MED			"SEM"
#define AIRCONTROLLER_IR_LEARN_MED			"LEA"
#define AIRCONTROLLER_PM25_READVAL_MED		"REV"
#define AIRCONTROLLER_PM25_READMODE_MED		"REM"
#define AIRCONTROLLER_PM25_READHOLD_MED		"REH"
#define AIRCONTROLLER_PM25_SETMODE_MED		"WRM"
#define AIRCONTROLLER_PM25_SETHOLD_MED		"WRH"

#define IRRELAY_LEARN_CMD	0x88
#define IRRELAY_SEND_CMD	0x86

typedef enum
{
	AIRCONTROL_IR_SEND,
	AIRCONTROL_IR_LEARN,
	AIRCONTROL_PM25_READVAL,
	AIRCONTROL_PM25_READMODE,
	AIRCONTROL_PM25_READHOLD,
	AIRCONTROL_PM25_SETMODE,
	AIRCONTROL_PM25_SETHOLD,
	AIRCONTROL_NONE,
}AirController_Method_t;

typedef struct AirController_Opt
{
	uint8 PM25_thresmode;	//0, up; 1, down;
	uint16 PM25_threshold;
	uint16 PM25_val;
}AirController_Opt_t;


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
static AirController_Opt_t AirControlOpt;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static AirController_Method_t get_AirControl_Method_from_str(int8 *str);
//static int8 set_AirControl_Method_to_str(AirController_Method_t am, int8 *str);
/*********************************************************************
 * PUBLIC FUNCTIONS
 */

void SetPM25Val(uint16 val)
{
	AirControlOpt.PM25_val = val;
}

AirController_Method_t get_AirControl_Method_from_str(int8 *str)
{
	if(str == NULL)
	{
		return AIRCONTROL_NONE;
	}
	
	if(osal_memcmp(AIRCONTROLLER_IR_SEND_MED, str, 3))
	{
		return AIRCONTROL_IR_SEND;
	}
	else if(osal_memcmp(AIRCONTROLLER_IR_LEARN_MED, str, 3))
	{
		return AIRCONTROL_IR_LEARN;
	}
	else if(osal_memcmp(AIRCONTROLLER_PM25_READVAL_MED, str, 3))
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

	return AIRCONTROL_NONE;
}

/*
int8 set_AirControl_Method_to_str(AirController_Method_t am, int8 *str)
{
	if(str == NULL)
	{
		return -1;
	}
	
	switch(am)
	{
	case AIRCONTROL_IR_SEND: 
		osal_memcpy(str, AIRCONTROLLER_IR_SEND_MED, 3);
		break;
		
	case AIRCONTROL_IR_LEARN: 
		osal_memcpy(str, AIRCONTROLLER_IR_LEARN_MED, 3);
		break;
		
	case AIRCONTROL_PM25_READVAL: 
		osal_memcpy(str, AIRCONTROLLER_PM25_READVAL_MED, 3);
		break;
		
	case AIRCONTROL_PM25_READMODE: 
		osal_memcpy(str, AIRCONTROLLER_PM25_READMODE_MED, 3);
		break;
		
	case AIRCONTROL_PM25_READHOLD: 
		osal_memcpy(str, AIRCONTROLLER_PM25_READHOLD_MED, 3);
		break;
		
	case AIRCONTROL_PM25_SETMODE: 
		osal_memcpy(str, AIRCONTROLLER_PM25_SETMODE_MED, 3);
		break;
		
	case AIRCONTROL_PM25_SETHOLD: 
		osal_memcpy(str, AIRCONTROLLER_PM25_SETHOLD_MED, 3);
		break;

	default: return -1;
	}

	return 0;
}
*/


void HalDeviceInit (void)
{
	memset(&AirControlOpt, 0, sizeof(AirController_Opt_t));
}

void HalStatesInit(devStates_t status)
{}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	uint8 ctrlData[5] = {0};
	
	AirController_Method_t am = AIRCONTROL_NONE;
	if(dataLen >= 3)
	{
		am = get_AirControl_Method_from_str((int8 *)data);
	}

	switch(am)

	{
	case AIRCONTROL_IR_SEND: 
		if(dataLen >= 5)
		{
			ctrlData[0] = IRRELAY_SEND_CMD;
			ctrlData[1] = atox((uint8 *)(data+3), 2);
			ctrlData[4] = ctrlData[0] ^ ctrlData[1];
#ifndef HAL_UART01_BOTH
			HalUARTWrite(SERIAL_COM_PORT, ctrlData, 5);
#else
			HalUARTWrite(SERIAL_COM_PORT0, ctrlData, 5);
#endif
			optData[3] = 'O';
			optData[4] = 'K';
			optDataLen = 5;
		}
		else
		{
			return -1;
		}
		break;
		
	case AIRCONTROL_IR_LEARN: 
		if(dataLen >= 5)
		{
			ctrlData[0] = IRRELAY_LEARN_CMD;
			ctrlData[1] = atox((uint8 *)(data+3), 2);
			ctrlData[4] = ctrlData[0] ^ ctrlData[1];
#ifndef HAL_UART01_BOTH
			HalUARTWrite(SERIAL_COM_PORT, ctrlData, 5);
#else
			HalUARTWrite(SERIAL_COM_PORT0, ctrlData, 5);
#endif
			optData[3] = 'O';
			optData[4] = 'K';
			optDataLen = 5;
		}
		else
		{
			return -1;
		}
		break;
		
	case AIRCONTROL_PM25_READVAL: 
		if(optDataLen < 7)
		{
			osal_mem_free(optData);
			optData = osal_mem_alloc(7);	
		}
		optDataLen = 7;
		osal_memcpy(optData, AIRCONTROLLER_PM25_READVAL_MED, 3);
		incode_2_to_16(optData+3, (uint8 *)&AirControlOpt.PM25_val, 2);
		break;
		
	case AIRCONTROL_PM25_READMODE: 
		if(optDataLen < 5)
		{
			osal_mem_free(optData);
			optData = osal_mem_alloc(5);
		}
		optDataLen = 5;
		osal_memcpy(optData, AIRCONTROLLER_PM25_READMODE_MED, 3);
		incode_2_to_16(optData+3, (uint8 *)&AirControlOpt.PM25_thresmode, 1);
		break;
		
	case AIRCONTROL_PM25_READHOLD: 
		if(optDataLen < 7)
		{
			osal_mem_free(optData);
			optData = osal_mem_alloc(7);
		}
		optDataLen = 7;
		osal_memcpy(optData, AIRCONTROLLER_PM25_READHOLD_MED, 3);
		incode_2_to_16(optData+3, (uint8 *)&AirControlOpt.PM25_threshold, 2);
		break;
		
	case AIRCONTROL_PM25_SETMODE: 
		if(dataLen >= 5)
		{
			AirControlOpt.PM25_thresmode = atox((uint8 *)(data+3), 2);
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
			incode_16_to_2(&AirControlOpt.PM25_threshold, (uint8 *)(data+3), 4);
			optData[3] = 'O';
			optData[4] = 'K';
			optDataLen = 5;
		}
		else
		{
			return -1;
		}
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
