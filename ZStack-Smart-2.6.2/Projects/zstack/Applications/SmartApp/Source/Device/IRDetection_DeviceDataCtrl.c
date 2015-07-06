/**************************************************************************************************
  Filename:       IRDetection_DeviceDataCtrl.c
  Revised:        $Date: 2014-12-01 14:27:34 -0800 (Thu, 01 dec 2014) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of infrared detection device data operations
**************************************************************************************************/
/**************************************************************************************************
Create by Sam_Chen
Date:2014-12-01
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-05-20
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 

/*********************************************************************
 * MACROS
 */
#define HAL_IDC_DEBOUNCE_VALUE  100

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

/*********************************************************************
 * LOCAL FUNCTIONS
 */
void CommonApp_IRDetectStatusCB( void *params, uint16 *duration, uint8 *count);
void CommonApp_IRDetectQueryShowCB( void *params, uint16 *duration, uint8 *count);
/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{
  IDC_SEL &= ~(IDC_BV);
  IDC_DDR &= ~(IDC_BV);

  PICTL &= ~0x06;
  PICTL |= 0x06;

  IDC_ICTL |= IDC_BV;
  IDC_IEN |= BV(4);
  IDC_PXIFG &= ~(IDC_BV);
}

void HalStatesInit(devStates_t status)
{
  set_user_event(CommonApp_TaskID, IRDETECT_DETECT_EVT, CommonApp_IRDetectStatusCB, 
  	IRDETECT_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, (void *)TRIGGER_MODE_QUERY);

  set_user_event(CommonApp_TaskID, IRDETECT_QUERY_EVT, CommonApp_IRDetectQueryShowCB, 
  	IRDETECT_QUERY_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);
}

void CommonApp_IRDetectStatusCB( void *params, uint16 *duration, uint8 *count)
{
  int Trigger_Mode = (int)params;
  if( CommonApp_NwkState == DEV_ROUTER || CommonApp_NwkState == DEV_END_DEVICE)
  {
	switch(Trigger_Mode)
	{
	case TRIGGER_MODE_QUERY:
		if (!HAL_STATE_IDC())
		{
		  Update_Refresh("00", 2);
		}
		
	case TRIGGER_MODE_ISR:
		if (HAL_STATE_IDC())
	    {
		  Update_Refresh("01", 2);
	    }
		break;
	}
  }
}


void CommonApp_IRDetectQueryShowCB( void *params, uint16 *duration, uint8 *count)
{
	if (HAL_STATE_IDC())
	{
		HAL_TURN_ON_LED2();
	}
	else
	{
		HAL_TURN_OFF_LED2();
	}
}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	*dataLen = 0;
	return 0;
}


#if (DEVICE_TYPE_ID==12)
#ifndef KEY_PUSH_PORT_1_BUTTON
HAL_ISR_FUNCTION( halKeyPort1Isr, P1INT_VECTOR )
{
  HAL_ENTER_ISR();

  if (IDC_PXIFG & IDC_BV)
  {
    IDC_PXIFG &= ~IDC_BV;
	
	update_user_event(CommonApp_TaskID, IRDETECT_ISR_EVT, CommonApp_IRDetectStatusCB, 
  		HAL_IDC_DEBOUNCE_VALUE, TIMER_ONE_EXECUTION|TIMER_EVENT_RESIDENTS, (void *)TRIGGER_MODE_ISR);
  }

  /*
    Clear the CPU interrupt flag for Port_1
    PxIFG has to be cleared before PxIF
  */
  IDC_PXIFG = 0;
  IDC_IF = 0;
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}

#else
#error conflict with ISR function
#endif
#endif

