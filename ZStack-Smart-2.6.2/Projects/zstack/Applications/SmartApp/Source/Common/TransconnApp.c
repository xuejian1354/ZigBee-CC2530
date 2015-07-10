/**************************************************************************************************
  Filename:       TransconnApp.c
  Revised:        $Date: 2015-07-06 09:41:18 -0800 (Mon, 06 Jul 2015) $
  Revision:       $Revision: 29218 $

  Description:    Transconn Application
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-07-09
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "TransconnApp.h"
#include "globals.h"
#include "protocol.h"

#include "AF.h"
#include "OSAL.h"
#include "OSAL_Nv.h"
#include "OnBoard.h"

#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)
#include "mevent.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// This list should be filled with Application specific Cluster IDs.
//Õ®”√cluster list
const cId_t TransconnApp_InClusterList[TRANSCONNAPP_MAX_INCLUSTERS] =
{
  TRANSCONNAPP_CLUSTERID,
};

const cId_t TransconnApp_OutClusterList[TRANSCONNAPP_MAX_OUTCLUSTERS] =
{
  TRANSCONNAPP_CLUSTERID,
};

const SimpleDescriptionFormat_t TransconnApp_SimpleDesc =
{
  TRANSCONNAPP_ENDPOINT,              //  int Endpoint;
  TRANSCONNAPP_PROFID,                //  uint16 AppProfId[2];
  TRANSCONNAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  TRANSCONNAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  TRANSCONNAPP_FLAGS,                 //  int   AppFlags:4;
  TRANSCONNAPP_MAX_INCLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)TransconnApp_InClusterList,  //  byte *pAppInClusterList;
  TRANSCONNAPP_MAX_OUTCLUSTERS,          //  byte  AppNumOutClusters;
  (cId_t *)TransconnApp_OutClusterList   //  byte *pAppOutClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in TransconnApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t TransconnApp_epDesc;


byte TransconnApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // TransconnApp_Init() is called.


/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void TransconnApp_TxHandler(uint8 txBuf[], uint8 txLen);

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      TransconnApp_Init
 *
 * @brief   Initialization function for the Transconn App Task.
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */

void TransconnApp_Init( uint8 task_id )
{
    TransconnApp_TaskID = task_id;

#if(HAL_UART==TRUE)
#ifndef HAL_UART01_BOTH
	Serial_Init(Data_TxHandler);
	SerialTx_Handler(SERIAL_COM_PORT, TransconnApp_TxHandler);
#else
	Serial_Init(Data0_TxHandler, Data1_TxHandler);
	SerialTx_Handler(SERIAL_COM_PORT1, TransconnApp_TxHandler);
#endif
#endif

  // Fill out the endpoint description.
    TransconnApp_epDesc.endPoint = TRANSCONNAPP_ENDPOINT;
    TransconnApp_epDesc.task_id = &TransconnApp_TaskID;
    TransconnApp_epDesc.simpleDesc
              = (SimpleDescriptionFormat_t *)&TransconnApp_SimpleDesc;
    TransconnApp_epDesc.latencyReq = noLatencyReqs;

    mach_init();
}


/*********************************************************************
 * @fn      TransconnApp_ProcessZDOStates
 *
 * @brief   Process when network change
 *
 * @param   network status
 *
 * @return  none
 */
void TransconnApp_ProcessZDOStates(devStates_t status)
{
  mach_load();

  set_user_event(TransconnApp_TaskID, TIMER_UPLOAD_EVENT, 
	upload_event, 10000, TIMER_LOOP_EXECUTION | TIMER_EVENT_RESIDENTS, NULL);
}


/*********************************************************************
 * @fn      TransconnrApp_ProcessEvent
 *
 * @brief   Transconn Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 TransconnApp_ProcessEvent(uint8 task_id, uint16 events)
{
	return process_event(task_id, events);
}

void TransconnApp_TxHandler(uint8 txBuf[], uint8 txLen)
{
	frhandler_arg_t *frarg = get_frhandler_arg_alloc(txBuf, txLen);
	analysis_capps_frame(frarg);
	get_frhandler_arg_free(frarg);
	
	//ConnectorApp_TxHandler(txBuf, txLen);
}

void TransconnApp_GetCommonDataSend(uint8 *buf, uint16 len)
{
	frhandler_arg_t *frarg = 
		get_frhandler_arg_alloc(buf, len);

	analysis_zdev_frame(frarg);

	//HalUARTWrite(SERIAL_COM_PORT, buf, len);
}
#endif
