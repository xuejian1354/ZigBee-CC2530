/**************************************************************************************************
  Filename:       ConnectorApp.c
  Revised:        $Date: 2014-04-16 13:53:12 -0800 (Wed, 16 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    Connector Application
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-09-23
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

#include "mincode.h"

#include "CommonApp.h"
#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)
#include "framelysis.h"
#include "TransconnApp.h"
#endif

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
#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
typedef struct PairSrc
{
	uint16 pairNwk;
	ZLongAddr_t pairMac;
	
	struct PairSrc *next;
}PairSrc_t;
#endif

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
#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)
extern byte TransconnApp_TaskID;
#endif
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
extern uint8 *fBuf;		//pointer data buffer
extern uint16 fLen;		//buffer data length

#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
static PairSrc_t *pairList = NULL;
#endif

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */
#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
static int8 ConnectorApp_BindRemoteHandler(uint16 srcAddr, 
															uint8 *data, uint16 len);
static void ConnectorApp_BindSrcClearCB( void *params, 
														uint16 *duration, uint8 *count);
#endif
#ifndef ZDO_COORDINATOR
static void ConnectorApp_HeartBeatEvent(void);
#endif

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
  
#if(HAL_UART==TRUE) && !defined(TRANSCONN_BOARD_GATEWAY)
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
#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
	  if(pkt->cmd.DataLength > 0
	  	 && ConnectorApp_BindRemoteHandler(pkt->srcAddr.addr.shortAddr, 
	  	 			pkt->cmd.Data, pkt->cmd.DataLength) > 0)
	  {
	  	break;
	  }
#endif
      CommonApp_GetDevDataSend(pkt->cmd.Data, pkt->cmd.DataLength);
      break; 
  }
}

#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
int8 ConnectorApp_BindRemoteHandler(uint16 srcAddr, 
														uint8 *TxBuf, uint16 bufLen)
{
	int8 ret = 0;
	uint8 *data = NULL;
	uint16 len = 0;

	if(bufLen>=14 && !memcmp(TxBuf, FR_HEAD_DE, 2)
		&& !memcmp(TxBuf+2, FR_CMD_FAST_CTRL, 4)
		&& !memcmp(TxBuf+bufLen-4, f_tail, 4))
	{
		data = TxBuf+10;
		len = bufLen-14;
	}

	if(len == 0)
	{
		return ret;
	}
	
	switch(data[0])
	{
	case SB_OPT_CFG:
	{
		// reback coord mac
		DE_t mFrame = {0};
	
		osal_memcpy(mFrame.head, FR_HEAD_DE, 2);
		osal_memcpy(mFrame.cmd, FR_CMD_FAST_CTRL, 4);
		incode_xtoc16(mFrame.short_addr, srcAddr);

		mFrame.data_len = 1+16;		//opt+coord
		mFrame.data = osal_mem_alloc(mFrame.data_len);
		osal_memset(mFrame.data, 0, mFrame.data_len);
		mFrame.data[0] = SB_OPT_CFG;
		osal_memcpy(mFrame.data+1, EXT_ADDR_G, 16);
		memcpy(mFrame.tail, f_tail, 4);

		if(!SSAFrame_Package(HEAD_DE, &mFrame, &fBuf, &fLen))
		{
			CommonApp_SendTheMessage(srcAddr, fBuf, fLen);
		}
		osal_mem_free(mFrame.data);	
		
		// use default ret = 0, just hanle CommonApp_GetDevDataSend() retransmit data
	}
		break;
		
	case SB_OPT_PAIR:
		if(len > 20)
		{
			ret = 1;
			
			PairSrc_t *m_PairSrc = osal_mem_alloc(sizeof(PairSrc_t));
			incode_ctoxs(m_PairSrc->pairMac, data+1, 16);
			incode_ctox16(&m_PairSrc->pairNwk, data+17);
			m_PairSrc->next = NULL;

			PairSrc_t *t_PairSrc = pairList;
			if(t_PairSrc == NULL)
			{
				pairList = m_PairSrc;
				goto bind_clear;
			}

			while(t_PairSrc->next != NULL)
			{
				if(osal_memcmp(t_PairSrc->pairMac, 
						m_PairSrc->pairMac, sizeof(ZLongAddr_t)))
				{
					t_PairSrc->pairNwk = m_PairSrc->pairNwk;
					osal_mem_free(m_PairSrc);
					goto bind_clear;
				}
				
				t_PairSrc = t_PairSrc->next;
			}

			if(osal_memcmp(t_PairSrc->pairMac, 
					m_PairSrc->pairMac, sizeof(ZLongAddr_t)))
			{
				t_PairSrc->pairNwk = m_PairSrc->pairNwk;
				osal_mem_free(m_PairSrc);
			}
			else
			{
				t_PairSrc->next = m_PairSrc;
			}

bind_clear:
			update_user_event(CommonApp_TaskID, 
								BINDSRCBTN_CLEAR_EVT, 
								ConnectorApp_BindSrcClearCB, 
								SUPERBUTTON_PAIR_TIMEOUT, 
								TIMER_ONE_EXECUTION, 
								NULL);
		}
		break;

	case SB_OPT_PAIRREG:
		if(len > 20)
		{
			ret = 2;

			DE_t mFrame = {0};
	
			osal_memcpy(mFrame.head, FR_HEAD_DE, 2);
			osal_memcpy(mFrame.cmd, FR_CMD_FAST_CTRL, 4);

			mFrame.data_len = 1+16+4;
			mFrame.data = osal_mem_alloc(mFrame.data_len);
			osal_memcpy(mFrame.data, data, 21);
			memcpy(mFrame.tail, f_tail, 4);

			
			PairSrc_t *t_PairSrc = pairList;
			bool isMatch = 0;
			while(t_PairSrc)
			{
				incode_xtoc16(mFrame.short_addr, t_PairSrc->pairNwk);
				if(!SSAFrame_Package(HEAD_DE, &mFrame, &fBuf, &fLen))
				{
					CommonApp_SendTheMessage(t_PairSrc->pairNwk, fBuf, fLen);
					isMatch = 1;
				}
				
				t_PairSrc = t_PairSrc->next;
			}
			
			osal_mem_free(mFrame.data);	

			if(isMatch)
			{
				update_user_event(CommonApp_TaskID, 
								BINDSRCBTN_CLEAR_EVT, 
								ConnectorApp_BindSrcClearCB, 
								TIMER_NO_LIMIT, 
								TIMER_ONE_EXECUTION, 
								NULL);
			}
		}
		break;
		
	case SB_OPT_MATCH:
		// use default ret = 0, just hanle CommonApp_GetDevDataSend() retransmit data
		break;
		
	case SB_OPT_CTRL:
		break;
		
	case SB_OPT_REMOTE_CTRL:
		break;
	}

	return ret;
}

void ConnectorApp_BindSrcClearCB( void *params, 
												uint16 *duration, uint8 *count)
{
	
	PairSrc_t *pre_PairSrc =  NULL;
	PairSrc_t *t_PairSrc = pairList;

	while(t_PairSrc != NULL)
	{
		pre_PairSrc = t_PairSrc;
		t_PairSrc = t_PairSrc->next;

		osal_mem_free(pre_PairSrc);

		if(pre_PairSrc == pairList)
		{
			pairList = NULL;
		}
	}
}
#endif

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
  if(status == DEV_ZB_COORD || status == DEV_ROUTER)
  {
	nwkAddr = NLME_GetShortAddr();
	incode_2_to_16(SHORT_ADDR_G, (uint8 *)&nwkAddr, 2);
	memcpy(macAddr, NLME_GetExtAddr(), sizeof(ZLongAddr_t));
    incode_2_to_16(EXT_ADDR_G, macAddr, 8);

#ifdef ZDO_COORDINATOR
	UC_t mFrame;

	memcpy(mFrame.head, FR_HEAD_UC, 3);
	mFrame.type = FR_DEV_COORD;
	memcpy(mFrame.ed_type, FR_APP_DEV, 2);
	memcpy(mFrame.short_addr, SHORT_ADDR_G, 4);
	memcpy(mFrame.ext_addr, EXT_ADDR_G, 16);
	incode_2_to_16(mFrame.panid, (uint8 *)&_NIB.nwkPanId, 2);
	uint16 channel = _NIB.nwkLogicalChannel;
	incode_2_to_16(mFrame.channel, (uint8 *)&channel, 2);
    mFrame.data = NULL;
	mFrame.data_len = 0;
	memcpy(mFrame.tail, f_tail, 4);

	if(!SSAFrame_Package(HEAD_UC, &mFrame, &fBuf, &fLen))
	{
		CommonApp_GetDevDataSend(fBuf, fLen);
	}
#else
	UO_t mFrame;
	
	memcpy(mFrame.head, FR_HEAD_UO, 3);
	mFrame.type = FR_DEV_ROUTER;
	memcpy(mFrame.ed_type, FR_APP_DEV, 2);
	memcpy(mFrame.short_addr, SHORT_ADDR_G, 4);
	memcpy(mFrame.ext_addr, EXT_ADDR_G, 16);
	mFrame.data = NULL;
	mFrame.data_len = 0;
	memcpy(mFrame.tail, f_tail, 4);

	if(!SSAFrame_Package(HEAD_UO, &mFrame, &fBuf, &fLen))
	{
		CommonApp_GetDevDataSend(fBuf, fLen);
		CommonApp_SendTheMessage(COORDINATOR_ADDR, fBuf, fLen);
	}

	ConnectorApp_HeartBeatEvent();
#endif

#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)
	TransconnApp_ProcessZDOStates(status);
#endif
  }
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

#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)
	TransconnApp_HandleCombineKeys(keys, keyCounts);
#endif
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

#ifndef ZDO_COORDINATOR
void ConnectorApp_HeartBeatEvent(void)
{
	CommonApp_HeartBeatCB(NULL, NULL, NULL);
	
	set_user_event(CommonApp_TaskID, HEARTBERAT_EVT, CommonApp_HeartBeatCB, 
  		HEARTBEAT_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);
}
#endif


void ConnectorApp_TxHandler(uint8 txBuf[], uint8 txLen)
{
	uint16 Send_shortAddr = 0;

	if(txLen>=16 && !memcmp(txBuf, FR_HEAD_DE, 2)
		&& !memcmp(txBuf+2, FR_CMD_JOIN_CTRL, 4)
		&& !memcmp(txBuf+txLen-4, f_tail, 4))
	{
		if(!memcmp(txBuf+6, SHORT_ADDR_G, 4))
		{
			uint8 cmdData, ret;
			uint8 retData[2] = {0};
		
			cmdData = atox(txBuf+10, 2);

			if(!cmdData)
				ret = !CommonApp_PermitJoiningRequest(PERMIT_JOIN_FORBID);
			else if(cmdData == 1)
				ret = !CommonApp_PermitJoiningRequest(PERMIT_JOIN_TIMEOUT);

			incode_2_to_16(retData, &ret, 1);
			PermitJoin_Refresh(retData, 2);
		}
		else
		{
			incode_16_to_2(&Send_shortAddr, txBuf+6, 4);
			CommonApp_SendTheMessage(Send_shortAddr, txBuf, txLen);
		}
		
	}
	else if(txLen>=14 && !memcmp(txBuf, FR_HEAD_DE, 2)
		&& !memcmp(txBuf+2, FR_CMD_BROCAST_REFRESH, 4)
		&& !memcmp(txBuf+txLen-4, f_tail, 4))
	{
		incode_16_to_2(&Send_shortAddr, txBuf+6, 4);
		if(Send_shortAddr == COORDINATOR_ADDR)
		{
			CommonApp_ProcessZDOStates(DEV_ZB_COORD);
		}
		else
		{
			CommonApp_SendTheMessage(BROADCAST_ADDR, txBuf, txLen);
		}
	}
	else if(txLen>=14 && !memcmp(txBuf, FR_HEAD_DE, 2)
		&& (!memcmp(txBuf+2, FR_CMD_SINGLE_EXCUTE, 4)
		|| !memcmp(txBuf+2, FR_CMD_FAST_CTRL, 4)
		|| !memcmp(txBuf+2, FR_CMD_PEROID_EXCUTE, 4)
		|| !memcmp(txBuf+2, FR_CMD_PEROID_STOP, 4))
		&& !memcmp(txBuf+txLen-4, f_tail, 4))
	{
		incode_16_to_2(&Send_shortAddr, txBuf+6, 4);
		CommonApp_SendTheMessage(Send_shortAddr, txBuf, txLen);
	}
	else if(txLen>=14 && !memcmp(txBuf, FR_HEAD_DE, 2)
		&& !memcmp(txBuf+2, FR_CMD_SINGLE_REFRESH, 4)
		&& !memcmp(txBuf+txLen-4, f_tail, 4))
	{
		incode_16_to_2(&Send_shortAddr, txBuf+6, 4);
		if(Send_shortAddr == COORDINATOR_ADDR) //coord self
		{
			CommonApp_ProcessZDOStates(DEV_ZB_COORD);
		}
		else
		{
			CommonApp_SendTheMessage(Send_shortAddr, txBuf, txLen);
		}
	}
#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)		
	else if(txLen>=26 && !memcmp(txBuf, FR_HEAD_DE, 2)
		&& !memcmp(txBuf+2, FR_CMD_WR_IPADDR, 4)
		&& !memcmp(txBuf+txLen-4, f_tail, 4))
	{
		if(!memcmp(txBuf+6, SHORT_ADDR_G, 4))
		{
			uint8 ipaddr[24] = {0};
			if (ZSUCCESS == osal_nv_item_init( 
	  				ZCD_NV_TRANSCONN_IPADDR, sizeof(ipaddr),  ipaddr))
			{
				osal_nv_write(
					ZCD_NV_TRANSCONN_IPADDR, 0, sizeof(ipaddr),  ipaddr);

				Update_Refresh("WIOK", 4);
			}
			else
			{
				Update_Refresh("WIFAIL", 6);
			}
		}
		else
		{
			incode_16_to_2(&Send_shortAddr, txBuf+6, 4);
			CommonApp_SendTheMessage(Send_shortAddr, txBuf, txLen);
		}
	}
#endif
}

/*********************************************************************
 */

