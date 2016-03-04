/**************************************************************************************************
  Filename:       Doorlock_DeviceDataCtrl.c
  Revised:        $Date: 2015-11-30 14:27:34 -0800 (Mon, 30 Nov 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of doorlock device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-12-08
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
#define DOORLOCK_DATA_SIZE	2

#define DOORLOCK_TRIGGER_TIME	500

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern byte CommonApp_TaskID;

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
void Doorlock_triggler_ctrl(void);
void Doorlock_TriggerUpCB( void *params, uint16 *duration, uint8 *count);
void Doorlock_TriggerDownCB( void *params, uint16 *duration, uint8 *count);

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{
  DLC_DDR |= DLC_BV;
  HAL_TURN_ON_DLC();
}

void HalStatesInit(devStates_t status)
{}

void Doorlock_KeyHandler(void)
{
	Doorlock_triggler_ctrl();
}

void Doorlock_triggler_ctrl(void)
{
	HAL_TURN_OFF_DLC();
	update_user_event(CommonApp_TaskID,
						DOORLOCK_UP_EVT,
						Doorlock_TriggerUpCB, 
  						DOORLOCK_TRIGGER_TIME/2,
  						TIMER_ONE_EXECUTION|TIMER_EVENT_RESIDENTS,
  						NULL);
}

void Doorlock_TriggerUpCB( void *params, uint16 *duration, uint8 *count)
{
	HAL_TURN_ON_DLC();
	update_user_event(CommonApp_TaskID,
						DOORLOCK_UP_EVT,
						Doorlock_TriggerUpCB, 
  						DOORLOCK_TRIGGER_TIME,
  						TIMER_ONE_EXECUTION|TIMER_EVENT_RESIDENTS,
  						NULL);
}

void Doorlock_TriggerDownCB( void *params, uint16 *duration, uint8 *count)
{
	HAL_TURN_OFF_DLC();
}

#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
void BindBtn_Ctrl(void)
{
	Doorlock_triggler_ctrl();
}
#endif

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	if(data == NULL || dataLen < 2)
	{
		return -1;
	}

	if (osal_memcmp(data, "01", 2))
	{
		Doorlock_triggler_ctrl();
	}
	return 0;
}

int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	*dataLen = 0;
	return 0;
}

