/**************************************************************************************************
  Filename:       Alarm_DeviceDataCtrl.c
  Revised:        $Date: 2014-12-01 14:27:34 -0800 (Thu, 01 dec 2014) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of alarm device data operations
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-12-01
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-02-05
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
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

}

void HalStatesInit(devStates_t status)
{}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	return 0;
}
