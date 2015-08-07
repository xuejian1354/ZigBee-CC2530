/**************************************************************************************************
  Filename:       TransconnApp.c
  Revised:        $Date: 2015-07-06 09:41:18 -0800 (Mon, 06 Jul 2015) $
  Revision:       $Revision: 29218 $

  Description:    Transconn Application
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-08-07
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
#include "hal_led.h"
#include "hal_key.h"

#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)
#include "mevent.h"
#include <stdio.h>
/*********************************************************************
 * MACROS
 */
#define GPRS_MODEL_READY 		"Call Ready"
#define GPRS_MODEL_OK			"OK"
#define GPRS_MODEL_CONNECT_OK	"CONNECT OK"
#define GPRS_MODEL_ALREADY		"ALREADY CO"
#define GPRS_MODEL_SEND_OK		"SEND OK"
#define GPRS_MODEL_CLOSE_OK		"CLOSE OK"
#define GPRS_MODEL_SHUT_OK		"SHUT OK"
#define GPRS_MODEL_ERROR		"ERROR"
#define GPRS_MODEL_SENACK		"> "


#define DEFAULT_IPADDR		"115.28.214.50:11578"
#define DEFAULT_IPSIZE		19

#define DEFAULT_IPSET		54

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
//Í¨ÓÃcluster list
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
static void TransconnApp_GPRSStatesHandler(GPRS_States_t states);
static void set_ipaddr(void);
static void ipaddr_to_ipport(uint8 *ipaddr, uint8 size);
static void delay_event( void *params, uint16 *duration, uint8 *count);
static void TransconnApp_GPRSStepHandler(uint8 step);

/*********************************************************************
 * LOCAL VARIABLES
 */
uint8 sbuf[FRAME_BUFFER_SIZE] = {0};
uint8 slen = 0;
GPRS_States_t gprs_states = GPRS_INIT;
static uint8 gprs_Step = 0;
uint8 gprs_SenACK = 0;

static uint8 ip[18] = {0};
static uint8 port[6] = {0};

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

	HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);

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

  update_user_event(TransconnApp_TaskID, TIMER_UPLOAD_EVENT, 
	upload_event, 11000, TIMER_LOOP_EXECUTION | TIMER_EVENT_RESIDENTS, NULL);
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

void TransconnApp_HandleCombineKeys(uint16 keys, uint8 keyCounts)
{
  if(
#ifdef KEY_PUSH_PORT_1_BUTTON
    keys & HAL_KEY_PORT_1_SWITCH_3
#else
	keys & HAL_KEY_SW_6
#endif
  )
  {
	switch( keyCounts)
	{
	case 2:
		if(gprs_states == GPRS_SHUT)
		{
			gprs_Step = 3;
			TransconnApp_GPRSStatesHandler(GPRS_OK);
		}
		else if(gprs_states != GPRS_CONNECT_OK)
		{
			TransconnApp_GPRSStatesHandler(GPRS_READY);
		}
		break;
		
	case 4:
		if(gprs_states == GPRS_CONNECT_OK)
		{
			TransconnApp_GPRSStatesHandler(GPRS_CLOSE);
		}
		break;
	}
  }
}

void TransconnApp_TxHandler(uint8 txBuf[], uint8 txLen)
{	
	if(osal_memcmp(txBuf+txLen-12, GPRS_MODEL_READY, 10))
	{
		TransconnApp_GPRSStatesHandler(GPRS_READY);
	}
	else if(txLen >= 2 && osal_memcmp(txBuf+txLen-2, GPRS_MODEL_SENACK, 2))
	{
		if(gprs_states == GPRS_CONNECT_OK)
		{
#if (HAL_UART==TRUE)
#ifndef HAL_UART01_BOTH
			HalUARTWrite(SERIAL_COM_PORT, sbuf, slen);
			HalUARTWrite(SERIAL_COM_PORT, "\x1A", 1);
#else
	  		HalUARTWrite(SERIAL_COM_PORT1, sbuf, slen);
			HalUARTWrite(SERIAL_COM_PORT1, "\x1A", 1);
#endif
#endif
		}
	}
	else if(txLen >= 10 && osal_memcmp(txBuf+txLen-9, GPRS_MODEL_SEND_OK, 7))
	{
		HalLedBlink ( HAL_LED_2, 1, 50, 100 );
		return;
	}
	else if(txLen >= 10 && (osal_memcmp(txBuf+txLen-12, GPRS_MODEL_CONNECT_OK, 10)
		|| osal_memcmp(txBuf+txLen-10, GPRS_MODEL_ALREADY, 10)))
	{
		TransconnApp_GPRSStatesHandler(GPRS_CONNECT_OK);
	}
	else if(txLen >= 8 && osal_memcmp(txBuf+txLen-10, GPRS_MODEL_CLOSE_OK, 8))
	{
		TransconnApp_GPRSStatesHandler(GPRS_SHUT);
	}
	else if(txLen >= 7 && osal_memcmp(txBuf+txLen-9, GPRS_MODEL_SHUT_OK, 7))
	{
		gprs_states = GPRS_SHUT;
		HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
	}
	else if(txLen >= 2 && osal_memcmp(txBuf+txLen-4, GPRS_MODEL_OK, 2))
	{
		if(gprs_Step < 5)
		{
			TransconnApp_GPRSStatesHandler(GPRS_OK);
		}
		else
		{
			TransconnApp_GPRSStatesHandler(GPRS_CONNECT_OK);
		}
	}
	else if(txLen > 3 && txBuf[2] == ':')
	{
		HalLedBlink ( HAL_LED_2, 1, 50, 100 );
		frhandler_arg_t *frarg = get_frhandler_arg_alloc(txBuf, txLen);
		analysis_capps_frame(frarg);
		get_frhandler_arg_free(frarg);
	}
	else if(gprs_states != GPRS_CONNECT_OK)
	{
		TransconnApp_GPRSStatesHandler(GPRS_ERROR);
	}
	
	//ConnectorApp_TxHandler(txBuf, txLen);
}

void TransconnApp_GetCommonDataSend(uint8 *buf, uint16 len)
{
	if(gprs_states != GPRS_CONNECT_OK)
	{
		//TransconnApp_GPRSStatesHandler(gprs_states);
		return;
	}

	frhandler_arg_t *frarg = get_frhandler_arg_alloc(buf, len);
	analysis_zdev_frame(frarg);
	get_frhandler_arg_free(frarg);
	
	//HalUARTWrite(SERIAL_COM_PORT, buf, len);
}

void TransconnApp_GPRSStatesHandler(GPRS_States_t states)
{		
	if(states == GPRS_READY)
	{
		gprs_states = GPRS_READY;
		gprs_Step = 0;
	}
	else if(gprs_states < GPRS_READY)
	{
		return;
	}

	switch(states)
	{
	case GPRS_READY:
	case GPRS_OK:
		if(gprs_Step < 5)
		{
			gprs_Step++;
		}
		update_user_event(TransconnApp_TaskID, TIMER_DELAY_EVENT, 
			delay_event, 200, TIMER_ONE_EXECUTION, NULL);
		break;
		
	case GPRS_CONNECT_OK:
		HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
		gprs_states = GPRS_CONNECT_OK;
		break;
		
	case GPRS_ERROR:
		update_user_event(TransconnApp_TaskID, TIMER_DELAY_EVENT, 
			delay_event, 3000, TIMER_ONE_EXECUTION, NULL);
		break;	

	case GPRS_CLOSE:
		gprs_Step = 6;
		update_user_event(TransconnApp_TaskID, TIMER_DELAY_EVENT, 
			delay_event, 200, TIMER_ONE_EXECUTION, NULL);

	case GPRS_SHUT:
		gprs_Step = 7;
		update_user_event(TransconnApp_TaskID, TIMER_DELAY_EVENT, 
			delay_event, 200, TIMER_ONE_EXECUTION, NULL);
		break;	
	}
}

void set_ipaddr(void)
{
	uint8 ipset = 0;
	uint8 ipaddr[24] = {0};
	
	if ((ZSUCCESS == osal_nv_item_init( 
			ZCD_NV_TRANSCONN_IPASET, sizeof(ipset),  &ipset))
			&& (SUCCESS == osal_nv_read( 
					ZCD_NV_TRANSCONN_IPASET, 0, sizeof(ipset),  &ipset))
			&& ipset != DEFAULT_IPSET)
	{
		ipset = DEFAULT_IPSET;
		
		osal_nv_write( 
			ZCD_NV_TRANSCONN_IPASET, 0, sizeof(ipset),  &ipset);

		osal_memcpy(ipaddr, DEFAULT_IPADDR, DEFAULT_IPSIZE);
		if(ZSUCCESS == osal_nv_item_init( 
				ZCD_NV_TRANSCONN_IPADDR, sizeof(ipaddr),  &ipaddr))
		{
			osal_nv_write( 
				ZCD_NV_TRANSCONN_IPADDR, 0, sizeof(ipaddr),  &ipaddr);
		}
	}

	if(ipset == DEFAULT_IPSET
		&& (ZSUCCESS == osal_nv_item_init( 
				ZCD_NV_TRANSCONN_IPADDR, sizeof(ipaddr),  &ipaddr))
		&& (SUCCESS == osal_nv_read( 
				ZCD_NV_TRANSCONN_IPADDR, 0, sizeof(ipaddr),  &ipaddr)))
	{}
	else
	{
		osal_memcpy(ipaddr, DEFAULT_IPADDR, DEFAULT_IPSIZE);
	}

	ipaddr_to_ipport(ipaddr, strlen((char *)ipaddr));
}

void ipaddr_to_ipport(uint8 *ipaddr, uint8 size)
{
	int i;
	for(i=0; i<size; i++)
	{
		if(*(ipaddr+i) == ':')
		{
			break;
		}
	}

	if(i > 0 && i < size)
	{
		osal_memcpy(ip, ipaddr, i);
		osal_memcpy(port, ipaddr+i+1, size-i-1);
	}
}

void delay_event( void *params, uint16 *duration, uint8 *count)
{
	HalLedBlink ( HAL_LED_2, 1, 50, 100 );
	TransconnApp_GPRSStepHandler(gprs_Step);
}

void TransconnApp_GPRSStepHandler(uint8 step)
{
	char tbuf[64] = {0};
	
	switch(step)
	{
	case 1:
		sprintf(tbuf, "AT+CGCLASS=\"B\"\r\n");
		HalUARTWrite(SERIAL_COM_PORT, (uint8 *)tbuf, strlen(tbuf));
		break;

	case 2:
		sprintf(tbuf, "AT+CGATT=1\r\n");
		HalUARTWrite(SERIAL_COM_PORT, (uint8 *)tbuf, strlen(tbuf));
		break;

	case 3:
		sprintf(tbuf, "AT+CIPCSGP=1,\"CMNET\"\r\n");
		HalUARTWrite(SERIAL_COM_PORT, (uint8 *)tbuf, strlen(tbuf));
		break;

	case 4:
		sprintf(tbuf, "AT+CLPORT=\"UDP\",\"11576\"\r\n");
		HalUARTWrite(SERIAL_COM_PORT, (uint8 *)tbuf, strlen(tbuf));
		break;

	case 5:
		set_ipaddr();
		sprintf(tbuf, "AT+CIPSTART=\"UDP\",\"%s\",\"%s\"\r\n", ip, port);
		HalUARTWrite(SERIAL_COM_PORT, (uint8 *)tbuf, strlen(tbuf));
		break;

	case 6:
		sprintf(tbuf, "AT+CIPCLOSE\r\n");
		HalUARTWrite(SERIAL_COM_PORT, (uint8 *)tbuf, strlen(tbuf));
		break;

	case 7:
		sprintf(tbuf, "AT+CIPSHUT\r\n");
		HalUARTWrite(SERIAL_COM_PORT, (uint8 *)tbuf, strlen(tbuf));
		break;
	}
}
#endif
