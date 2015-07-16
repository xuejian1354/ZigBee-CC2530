/**************************************************************************************************
  Filename:       HumitureDetection_DeviceDataCtrl.c
  Revised:        $Date: 2015-07-10 08:41:34 -0800 (Fri, 07 Jul 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains interface of humiture detection device data operations
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-07-16
**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"
#include "hal_drivers.h" 
#include "ht-ctrl.h"
#include "mincode.h"

/*********************************************************************
 * MACROS
 */
 
#define HT_VAL_SIZE 	4
#ifdef TRANSCONN_BOARD_ENDNODE
#define HT_OPTDATA_SIZE	(EXT_ADDR_SIZE+HT_VAL_SIZE)
#else
#define HT_OPTDATA_SIZE	HT_VAL_SIZE
#endif

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern byte CommonApp_TaskID;
extern devStates_t CommonApp_NwkState;

extern uint8 EXT_ADDR_G[16];

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
static void Humiture_DetectStatusCB( void *params, uint16 *duration, uint8 *count);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{

}

void HalStatesInit(devStates_t status)
{
  set_user_event(CommonApp_TaskID, 
  					HUMITURE_DETECT_EVT, 
  					Humiture_DetectStatusCB, 
  					HUMITUREDETECT_TIMEOUT, 
  					TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, 
  					NULL);
}

void Humiture_DetectStatusCB( void *params, uint16 *duration, uint8 *count)
{
  if( CommonApp_NwkState == DEV_ROUTER || CommonApp_NwkState == DEV_END_DEVICE)
  {
	HTValue humi_val, temp_val;
	uint8 error, checksum;

	s_connectionreset();

	error = 0;
	error += s_measure((uint8*) &humi_val.i, &checksum, HUMI);  //measure humidity
	error += s_measure((uint8*) &temp_val.i, &checksum, TEMP);  //measure temperature
	if(error != 0) 
	{	
		s_connectionreset();                 //in case of an error: connection reset
		return;
	}

	humi_val.f = (float)humi_val.i;                   //converts integer to float
	temp_val.f = (float)temp_val.i;                   //converts integer to float
	calc_sht11(&humi_val.f, &temp_val.f);            //calculate humidity, temperature

	uint8 temp = (uint8)temp_val.f;
	uint8 humi = (uint8)humi_val.f;

	if(optData != NULL && optDataLen < HT_OPTDATA_SIZE)
	{
		osal_mem_free(optData);
		optData = NULL;
		optDataLen = 0;
	}

	if(optData == NULL)
	{
		optData = osal_mem_alloc(HT_OPTDATA_SIZE);
		optDataLen = HT_OPTDATA_SIZE;
	}

	incode_xtocs(optData, &temp, 1);
	incode_xtocs(optData+2, &humi, 1);
#ifdef TRANSCONN_BOARD_ENDNODE
	osal_memcpy(optData+HT_VAL_SIZE, EXT_ADDR_G, EXT_ADDR_SIZE);
#endif
	optDataLen = HT_OPTDATA_SIZE;
	
	Update_Refresh(optData, optDataLen);
  }
}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	return 0;
}


int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	memcpy(data, optData, optDataLen);
	*dataLen = optDataLen;

	return 0;
}
