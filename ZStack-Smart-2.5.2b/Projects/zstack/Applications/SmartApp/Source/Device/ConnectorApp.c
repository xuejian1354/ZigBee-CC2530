/**************************************************************************************************
  Filename:       ConnectorApp.c
  Revised:        $Date: 2018-03-21 11:09:54 $
  Revision:       $Revision: 29217 $

  Description:    Connector Application
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2018-03-21
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
#include "ZComDef.h"
#include "OnBoard.h"

#include "CommonApp.h"

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"


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
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern const cId_t CommonApp_InClusterList[];
extern const cId_t CommonApp_OutClusterList[];
extern const SimpleDescriptionFormat_t CommonApp_SimpleDesc;

extern endPointDesc_t CommonApp_epDesc;

extern byte CommonApp_TaskID;
extern devStates_t CommonApp_NwkState;
extern byte CommonApp_TransID;

extern afAddrType_t CommonApp_DstAddr;

//network address
extern uint16 nwkAddr;
//mac address
extern ZLongAddr_t macAddr;

extern uint8 SHORT_ADDR_G[4];

extern uint8 EXT_ADDR_G[16];

extern const uint8 f_tail[4];

extern bool isPermitJoining;

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 *fBuf;		//pointer data buffer
static uint16 fLen;		//buffer data length

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      CommonApp_InitConfirm
 *
 * @brief   Initialization function for the Connector App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by OSAL.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */

void CommonApp_InitConfirm( uint8 task_id )
{
  CommonApp_PermitJoiningRequest(PERMIT_JOIN_FORBID);
  
#if(HAL_UART==TRUE)
  SerialTx_Handler(SERIAL_COM_PORT, ConnectorApp_TxHandler);
#endif
}


/*********************************************************************
 * @fn      CommonApp_MessageMSGCB
 *
 * @brief   Data message processor callback.  This function processes
 *          any incoming data - probably from other devices.  So, based
 *          on cluster ID, perform the intended action.
 *
 * @param   none
 *
 * @return  none
 */

void CommonApp_MessageMSGCB( afIncomingMSGPacket_t *pkt )
{
  switch ( pkt->clusterId )
  {
    case COMMONAPP_CLUSTERID:
      CommonApp_GetDevDataSend(pkt->cmd.Data, pkt->cmd.DataLength);
      break; 
  }
}


/*********************************************************************
 * @fn      CommonApp_ProcessZDOStates
 *
 * @brief   Process when network change
 *
 * @param   network status
 *
 * @return  none
 */
void CommonApp_ProcessZDOStates(devStates_t status)
{
}


#ifndef HAL_KEY_LONG_SHORT_DISTINGUISH
void CommonApp_HandleCombineKeys(uint16 keys, uint8 keyCounts)
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
	case 0: //Long click listen
	//Reset factory mode
#if defined(HOLD_INIT_AUTHENTICATION)
		HalLedBlink ( HAL_LED_4, 0, 50, 100 );
#if !defined(ZDO_COORDINATOR)
		if(devState != DEV_HOLD)
		{
    		devStates_t tStates;
  			if (ZSUCCESS == osal_nv_item_init( 
  				ZCD_NV_NWK_HOLD_STARTUP, sizeof(tStates),  &tStates))
  			{
  				tStates = DEV_HOLD;
    			osal_nv_write(
					ZCD_NV_NWK_HOLD_STARTUP, 0, sizeof(tStates),  &tStates);
  			}
		}
#else
  		devState = DEV_INIT;
#endif

		zgWriteStartupOptions(ZG_STARTUP_SET, ZCD_STARTOPT_DEFAULT_NETWORK_STATE);
		WatchDogEnable( WDTIMX );
#endif
		break;

	case 3:
		//permit\forbid add znet
		if(devState == DEV_HOLD)
		{
			ZDOInitDevice( 0 );
		}
		else
		{
			if(isPermitJoining)
			{
				CommonApp_PermitJoiningRequest(PERMIT_JOIN_FORBID);
			}
			else
			{
				CommonApp_PermitJoiningRequest(PERMIT_JOIN_TIMEOUT);
			}
		}
		break;

	default: break;
	}
  }
}
#else
void CommonApp_HandleCombineKeys(uint16 keys, uint8 keyCounts)
{
  uint8 *keysID = get_keys_id();
  uint8 *keysPush = get_keys_push();

  if (keysPush[0] == HAL_KEY_LONG_PUSH)
  {
  	if(osal_memcmp(keysID, "a", keyCounts) && keyCounts == 1)
    {
		//permit\forbid add znet
		if(devState == DEV_HOLD)
		{
			ZDOInitDevice( 0 );
		}
		else
		{
			if(isPermitJoining)
			{
				CommonApp_PermitJoiningRequest(PERMIT_JOIN_FORBID);
			}
			else
			{
				CommonApp_PermitJoiningRequest(PERMIT_JOIN_TIMEOUT);
			}
		}
    }
    else if(osal_memcmp(keysID, "aaa", keyCounts) && keyCounts == 3)
    {
		//Reset factory mode
#if defined(HOLD_INIT_AUTHENTICATION)
		HalLedBlink ( HAL_LED_4, 0, 50, 100 );
#if !defined(ZDO_COORDINATOR)
		if(devState != DEV_HOLD)
		{
    		devStates_t tStates;
  			if (ZSUCCESS == osal_nv_item_init( 
  				ZCD_NV_NWK_HOLD_STARTUP, sizeof(tStates),  &tStates))
  			{
  				tStates = DEV_HOLD;
    			osal_nv_write(
					ZCD_NV_NWK_HOLD_STARTUP, 0, sizeof(tStates),  &tStates);
  			}
		}
#else
  		devState = DEV_INIT;
#endif

		zgWriteStartupOptions(ZG_STARTUP_SET, ZCD_STARTOPT_DEFAULT_NETWORK_STATE);
		WatchDogEnable( WDTIMX );
#endif
    }
  }
}
#endif


void ConnectorApp_TxHandler(uint8 txBuf[], uint8 txLen)
{
	CommonApp_SendTheMessage(BROADCAST_ADDR, txBuf, txLen);
}

/*********************************************************************
 */

