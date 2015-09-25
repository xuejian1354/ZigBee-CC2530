/**************************************************************************************************
  Filename:       DoorSensor_DeviceDataCtrl.c
  Revised:        $Date: 2014-12-01 14:27:34 -0800 (Thu, 01 dec 2014) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of door sensor device data operations
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-12-01
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-05-12
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 

/*********************************************************************
 * MACROS
 */
#define HAL_DIC_DEBOUNCE_VALUE  100

#define TRIGGER_MODE_QUERY 	1
#define TRIGGER_MODE_ISR	2
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
void CommonApp_DetectStatusCB( void *params, uint16 *duration, uint8 *count);

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{
  HAL_TURN_OFF_DIC();
  DIC_DDR &= ~(DIC_BV);


  DIC_SEL &= ~(DIC_BV);
  DIC_DDR &= ~(DIC_BV);

  //PICTL &= ~0x06;
  PICTL |= 0x06;

  DIC_ICTL |= DIC_BV;
  DIC_IEN |= BV(4);
  DIC_PXIFG &= ~(DIC_BV);
}

void HalStatesInit(devStates_t status)
{
  set_user_event(CommonApp_TaskID, DOORSENSOR_DETECT_EVT, CommonApp_DetectStatusCB, 
  	DOORSENSOR_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, (void *)TRIGGER_MODE_QUERY);
}

void CommonApp_DetectStatusCB( void *params, uint16 *duration, uint8 *count)
{
  int Trigger_Mode = (int)params;
  if( CommonApp_NwkState == DEV_ROUTER || CommonApp_NwkState == DEV_END_DEVICE)
  {
	switch(Trigger_Mode)
	{
	case TRIGGER_MODE_QUERY:
		if (!HAL_STATE_DIC())
		{
		  Update_Refresh("00", 2);
		}
		
	case TRIGGER_MODE_ISR:
		if (HAL_STATE_DIC())
	    {
		  Update_Refresh("01", 2);
	    }
		break;
	}
  }
}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	if (HAL_STATE_DIC())
	{
	  memcpy(data, "01", 2);
	  *dataLen = 2;
	}
	else
	{
	  memcpy(data, "00", 2);
	  *dataLen = 2;
	}
	
	return 0;
}

#if (DEVICE_TYPE_ID==13)
#ifndef KEY_PUSH_PORT_1_BUTTON
HAL_ISR_FUNCTION( halKeyPort1Isr, P1INT_VECTOR )
{
  HAL_ENTER_ISR();

  if (DIC_PXIFG & DIC_BV)
  {
    DIC_PXIFG &= ~DIC_BV;
	
    update_user_event(CommonApp_TaskID, DOORSENSOR_ISR_EVT, CommonApp_DetectStatusCB, 
                      HAL_DIC_DEBOUNCE_VALUE, TIMER_ONE_EXECUTION|TIMER_EVENT_RESIDENTS, (void *)TRIGGER_MODE_ISR);
  }

  /*
    Clear the CPU interrupt flag for Port_1
    PxIFG has to be cleared before PxIF
  */
  DIC_PXIFG = 0;
  DIC_IF = 0;
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}

#else
#error conflict with ISR function
#endif
#endif
