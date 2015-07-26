/**************************************************************************************************
  Filename:       TransconnApp.h
  Revised:        $Date: 2015-07-06 09:41:18 -0800 (Mon, 06 Jul 2015) $
  Revision:       $Revision: 29218 $

  Description:    This file contains the Transconn Application definitions.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-07-26
**************************************************************************************************/


#ifndef TRANSCONNAPP_H
#define TRANSCONNAPP_H

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

#include "CommonApp.h"
#include "api_events.h"

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */
#define TRANSCONNAPP_ENDPOINT			0x11
#define TRANSCONNAPP_PROFID             0x0F09
#define TRANSCONNAPP_DEVICEID           0x0002
#define TRANSCONNAPP_DEVICE_VERSION     1
#define TRANSCONNAPP_FLAGS              0

#define TRANSCONNAPP_CLUSTERID          1
#define TRANSCONNAPP_MAX_INCLUSTERS     1
#define TRANSCONNAPP_MAX_OUTCLUSTERS    1

#define TIMER_UPLOAD_EVENT      0x0001
#define TIMER_DELAY_EVENT    	0x0002

/*********************************************************************
 * TYPEDEFS
 */
typedef enum
{
	GPRS_INIT,
	GPRS_READY,
	GPRS_OK,
	GPRS_CONNECT_OK,
	GPRS_ERROR,
	GPRS_CLOSE,
	GPRS_SHUT,
}GPRS_States_t;

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the Transconn Application
 */
extern void TransconnApp_Init( byte task_id );

/*
 * Task Event Processor for the Transconn Application
 */
extern UINT16 TransconnApp_ProcessEvent( byte task_id, UINT16 events );

/*********************************************************************
 * IMPLEMENTS
 */
extern void TransconnApp_ProcessZDOStates(devStates_t status);
extern void TransconnApp_GetCommonDataSend(uint8 *buf, uint16 len);
extern void TransconnApp_HandleCombineKeys(uint16 keys, uint8 keyCounts);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* TRANSCONNAPP_H */

