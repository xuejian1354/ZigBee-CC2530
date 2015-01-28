/**************************************************************************************************
  Filename:       CommonApp.h
  Revised:        $Date: 2014-04-16 14:41:24 -0800 (Wed, 16 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    This file contains the Common Application definitions.
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-04-16
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2014-12-28
**************************************************************************************************/


#ifndef COMMONAPP_H
#define COMMONAPP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"

#include "api_events.h"
#include "HeartBeat.h"
#include "incode.h"

#include "serial-com.h"
#include "frame-analysis.h"

/*********************************************************************
 * CONSTANTS
 */

/*通用设备描述信息*/
#define COMMONAPP_ENDPOINT			 0x10
#define COMMONAPP_PROFID             0x0F08
#define COMMONAPP_DEVICEID           0x0001
#define COMMONAPP_DEVICE_VERSION     1
#define COMMONAPP_FLAGS              0

#define COMMONAPP_CLUSTERID          1
#define COMMONAPP_MAX_INCLUSTERS     1
#define COMMONAPP_MAX_OUTCLUSTERS    1

#define COORDINATOR_ADDR		0x0000
#define BROADCAST_ADDR		0xFFFF

//USER
#define TIMER_LOWER_LIMIT	99	//lower limit of user event's timer count
#define TIMER_NO_LIMIT	0	//set the timer event immediately

#define TIMER_CLEAR_EXECUTION	0x00	//clear timer event from list
#define TIMER_ONE_EXECUTION		0x01	//set the timer event once
#define TIMER_LOOP_EXECUTION	0x7F	//set the timer event always

#define TIMER_EVENT_RESIDENTS	0x80	//set the timer event and resident always

#define FRAME_DATA_LENGTH   64		//frame data length by serial port

/*********************************************************************
 * MACROS
 */
//User events message
#define SERIAL_CMD_EVT		0x4000		//serial receive event by user defined
#define HEARTBERAT_EVT		0x0001		//heart beat event by user defined
#define CMD_PEROID_EVT		0x0002		//cmd period control event
#define PERMIT_JOIN_EVT		0x0004

//event peroid
#define HEARTBEAT_TIMEOUT   30000	//Every 30 Seconds
#define CMD_PEROID_TIMEOUT	5000	//Every 5 Seconds

#define PERMIT_JOIN_TIMEOUT	10		//10 Seconds, <=255s
#define PERMIT_JOIN_FORBID	0
#define PERMIT_JOIN_ALWAYS	0xFF

// Device Info Constants
#define ZB_INFO_DEV_STATE                 0
#define ZB_INFO_IEEE_ADDR                 1
#define ZB_INFO_SHORT_ADDR                2
#define ZB_INFO_PARENT_SHORT_ADDR         3
#define ZB_INFO_PARENT_IEEE_ADDR          4
#define ZB_INFO_CHANNEL                   5
#define ZB_INFO_PAN_ID                    6
#define ZB_INFO_EXT_PAN_ID                7


/*********************************************************************
 * TYPEDEFS
 */
//type structure of UART receive handler
typedef void(*UART_TxHandler)(uint8[], uint8);


/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Common Application
 */
extern void CommonApp_Init( byte task_id );

/*
 * Task Event Processor for the Common Application
 */
extern UINT16 CommonApp_ProcessEvent( byte task_id, UINT16 events );

/*
 * Task Send the Message for the Common Application
 */
extern void CommonApp_SendTheMessage(uint16 dstNwkAddr, uint8 *data, uint8 length);

/*
 * Task Initialization for the Common Application Confirm
 */
extern void CommonApp_InitConfirm( byte task_id );
extern void CommonApp_MessageMSGCB( afIncomingMSGPacket_t *pckt );
extern void CommonApp_ProcessZDOStates(devStates_t status);
extern void CommonApp_HandleCombineKeys(uint8 keyCounts);


extern void CommonApp_CmdPeroidCB( void *params, uint16 *duration, uint8 *count);
extern int8 DataCmd_Ctrl(uint8 *data, uint8 length);
extern void Update_Refresh(uint8 *data, uint8 length);
extern void PermitJoin_Refresh(uint8 *data, uint8 length);
ZStatus_t CommonApp_PermitJoiningRequest( byte PermitDuration );


/*********************************************************************
 * different interface depend on device
 *********************************************************************/
extern int8 CommonDevice_SetData(uint8 const *data, uint8 dataLen);
extern int8 CommonDevice_GetData(uint8 *data, uint8 *dataLen);


/*********************************************************************
 * IMPLEMENTS
 */
extern void CommonApp_GetDeviceInfo ( uint8 param, void *pValue );


/*
 * Task Set User Defined Events for the Common Application
 */
extern int8 CommonApp_SetUserEvent(uint16 event, 
	ssa_ProcessUserTaskCB_t ssa_ProcessUserTaskCB, uint16 duration, uint8 count, void *ptype);

/*
 * Task Update User Defined Events for the Common Application
 */
extern int8 CommonApp_UpdateUserEvent(uint16 event, 
	ssa_ProcessUserTaskCB_t ssa_ProcessUserTaskCB, uint16 duration, uint8 count, void *ptype);

/*
 *Task Update UART Receive Handler for the Common Application
 */
extern void CommonApp_SetUARTTxHandler(UART_TxHandler txHandler);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* COMMONAPP_H */

