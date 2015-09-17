/**************************************************************************************************
  Filename:       EndnodeApp.c
  Revised:        $Date: 2014-04-16 18:34:36 -0800 (Wed, 16 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    End Node Application
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-09-11
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

#include "mincode.h"

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
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static uint8 *fBuf;		//pointer data buffer
static uint16 fLen;		//buffer data length

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void EndNodeApp_HeartBeatEvent(void);
static void Data_Analysis(uint8 *data, uint16 length);
#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
static void DataBind_Ctrl(uint8 *data, uint16 len);
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
 * @brief   Initialization function for the End Node App Task.
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
  ZDOInitDevice( 0 );
}


/*********************************************************************
 * LOCAL FUNCTIONS
 */

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
      Data_Analysis(pkt->cmd.Data, pkt->cmd.DataLength);
      break; 
  }
}



void Data_Analysis(uint8 *data, uint16 length)
{
  DE_t *mdeFrame;

  uint8 buf[FRAME_DATA_SIZE] = {0};
  uint8 len = 0;

  mdeFrame = (DE_t *)SSAFrame_Analysis(HEAD_DE, data, length);

  //处理控制命令
  if(mdeFrame)
  {
//#ifdef __DEBUG__
    //HalUARTWrite(SERIAL_COM_PORT, data, length);
//#endif

    if(!memcmp(mdeFrame->cmd, FR_CMD_BROCAST_REFRESH, 4)
       || !memcmp(mdeFrame->cmd, FR_CMD_SINGLE_REFRESH, 4))
    {
      UO_t mFrame;
			
      memcpy(mFrame.head, FR_HEAD_UO, 3);
#ifdef RTR_NWK
      mFrame.type = FR_DEV_ROUTER;
#else
      mFrame.type = FR_DEV_ENDDEV;
#endif
      memcpy(mFrame.ed_type, FR_APP_DEV, 2);
      memcpy(mFrame.short_addr, SHORT_ADDR_G, 4);
      memcpy(mFrame.ext_addr, EXT_ADDR_G, 16);
      mFrame.data = buf;
      CommonDevice_GetData(mFrame.data, &mFrame.data_len);
      memcpy(mFrame.tail, f_tail, 4);

      if(!SSAFrame_Package(HEAD_UO, &mFrame, &fBuf, &fLen))
      {
        CommonApp_SendTheMessage(COORDINATOR_ADDR, fBuf, fLen);
      }
    }
    else if(!memcmp(mdeFrame->cmd, FR_CMD_SINGLE_EXCUTE, 4))
    {
      DataCmd_Ctrl(mdeFrame->data, mdeFrame->data_len);

      if(!CommonDevice_GetData(buf, &len))
        Update_Refresh(buf, len);
    }
	else if(!memcmp(mdeFrame->cmd, FR_CMD_FAST_CTRL, 4))
    {
/* Super Button */
#if (DEVICE_TYPE_ID==22)
      DataCmd_Ctrl(mdeFrame->data, mdeFrame->data_len);

/* Bind Device */
#elif defined(BIND_SUPERBUTTON_CTRL_SUPPORT)
	  DataBind_Ctrl(mdeFrame->data, mdeFrame->data_len);
#endif
    }
    else if(!memcmp(mdeFrame->cmd, FR_CMD_PEROID_EXCUTE, 4))
    {
      DataCmd_Ctrl(mdeFrame->data, mdeFrame->data_len);

	  if(!CommonDevice_GetData(buf, &len))
        Update_Refresh(buf, len);
	  
      update_user_event(CommonApp_TaskID, CMD_PEROID_EVT, CommonApp_CmdPeroidCB, 
	  	CMD_PEROID_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);
    }
    else if(!memcmp(mdeFrame->cmd, FR_CMD_PEROID_STOP, 4))
    {
      update_user_event(CommonApp_TaskID, CMD_PEROID_EVT, 
	  	NULL, TIMER_NO_LIMIT, TIMER_CLEAR_EXECUTION, NULL);
			
      Update_Refresh(mdeFrame->data, mdeFrame->data_len);
    }
#ifdef RTR_NWK
    else if(!memcmp(mdeFrame->cmd, FR_CMD_JOIN_CTRL, 4))
    {
      if(!memcmp(mdeFrame->short_addr, SHORT_ADDR_G, 4))
      {
        uint8 cmdData, ret;
        uint8 retData[2] = {0};
			
        cmdData = atox(mdeFrame->data, mdeFrame->data_len);

        if(!cmdData)
          ret = !CommonApp_PermitJoiningRequest(PERMIT_JOIN_FORBID);
        else if(cmdData == 1)
          ret = !CommonApp_PermitJoiningRequest(PERMIT_JOIN_TIMEOUT);

        incode_2_to_16(retData, &ret, 1);
        PermitJoin_Refresh(retData, 2);
      }
    }
#endif
  }
}

#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
void DataBind_Ctrl(uint8 *data, uint16 len)
{
	switch(data[0])
	{
	case SB_OPT_DEVREG:
		if(len >= 17)
		{
			DE_t mFrame = {0};
			osal_memcpy(mFrame.head, FR_HEAD_DE, 2);
			osal_memcpy(mFrame.cmd, FR_CMD_FAST_CTRL, 4);
			incode_xtoc16(mFrame.short_addr, COORDINATOR_ADDR);
			mFrame.data_len = 1+16+16+4;	//opt+ctrlmac+ownmac+ownaddr
			mFrame.data = osal_mem_alloc(1+16+16+4);
			mFrame.data[0] = SB_OPT_MATCH;
			osal_memcpy(mFrame.data+1, data+1, 16);
			osal_memcpy(mFrame.data+17, EXT_ADDR_G, 16);
			incode_xtoc16(mFrame.data+33, nwkAddr);
			memcpy(mFrame.tail, f_tail, 4);

			if(!SSAFrame_Package(HEAD_DE, &mFrame, &fBuf, &fLen))
			{
				CommonApp_SendTheMessage(COORDINATOR_ADDR, fBuf, fLen);
			}

			osal_mem_free(mFrame.data);
		}
		break;
		
	case SB_OPT_CTRL:
		if(len >= 7 && osal_memcpy(data, SB_OPT_CTRL_CODE, 6))
		{
			BindBtn_Ctrl();
		}
		break;

	case SB_OPT_REMOTE_CTRL:
		break;
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
  UO_t mFrame;
  uint8 buf[FRAME_DATA_SIZE] = {0};
  
  if( status == DEV_ROUTER || status == DEV_END_DEVICE)
  {
    nwkAddr = NLME_GetShortAddr();
    incode_2_to_16(SHORT_ADDR_G, (uint8 *)&nwkAddr, 2);
    memcpy(macAddr, NLME_GetExtAddr(), sizeof(ZLongAddr_t));
    incode_2_to_16(EXT_ADDR_G, macAddr, 8);

    memcpy(mFrame.head, FR_HEAD_UO, 3);
#ifdef RTR_NWK
    mFrame.type = FR_DEV_ROUTER;
#else
    mFrame.type = FR_DEV_ENDDEV;
#endif
    memcpy(mFrame.ed_type, FR_APP_DEV, 2);
    memcpy(mFrame.short_addr, SHORT_ADDR_G, 4);
    memcpy(mFrame.ext_addr, EXT_ADDR_G, 16);
    mFrame.data = buf;
    CommonDevice_GetData(mFrame.data, &mFrame.data_len);
    memcpy(mFrame.tail, f_tail, 4);

    if(!SSAFrame_Package(HEAD_UO, &mFrame, &fBuf, &fLen))
    {
      CommonApp_SendTheMessage(0x0000, fBuf, fLen);
    }

	EndNodeApp_HeartBeatEvent();
	HalStatesInit(status);
  }
}

#ifndef HAL_KEY_LONG_SHORT_DISTINGUISH
void CommonApp_HandleCombineKeys(uint16 keys, uint8 keyCounts)
{
  if(keys & HAL_KEY_SW_6)
  {
  	switch( keyCounts)
  	{
  	case 0:	//长按事件
#if defined(HOLD_INIT_AUTHENTICATION)
      if(devState != DEV_HOLD)
      {
      	HalLedBlink ( HAL_LED_4, 0, 50, 100 );
      	devStates_t tStates;

#if (DEVICE_TYPE_ID==14) || (DEVICE_TYPE_ID==0xF0)
		SetThresHold(AIRCONTROL_PM25_THRESMODE_UP, AIRCONTROL_PM25_DEFAULT_TRESHOLD);
#endif
		
      	if (ZSUCCESS == osal_nv_item_init( 
                                        ZCD_NV_NWK_HOLD_STARTUP, sizeof(tStates),  &tStates))
      	{
          tStates = DEV_HOLD;
          osal_nv_write(ZCD_NV_NWK_HOLD_STARTUP, 0, sizeof(tStates),  &tStates);
      	}

      	zgWriteStartupOptions(ZG_STARTUP_SET, ZCD_STARTOPT_DEFAULT_NETWORK_STATE);
      	WatchDogEnable( WDTIMX );
      }
#endif
      break;
	
    case 3:
      //转发器允许/禁止入网,入网认证
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
#ifdef BIND_SUPERBUTTON_CTRL_SUPPORT
		DE_t mFrame = {0};
	
		osal_memcpy(mFrame.head, FR_HEAD_DE, 2);
		osal_memcpy(mFrame.cmd, FR_CMD_FAST_CTRL, 4);
		incode_xtoc16(mFrame.short_addr, COORDINATOR_ADDR);
		mFrame.data_len = 1+16+4;
		mFrame.data = osal_mem_alloc(1+16+4);
		osal_memset(mFrame.data, 0, 1+16+4);
		mFrame.data[0] = SB_OPT_PAIRREG;
		osal_memcpy(mFrame.data+1, EXT_ADDR_G, 16);
		osal_memcpy(mFrame.data+17, SHORT_ADDR_G, 4);
		memcpy(mFrame.tail, f_tail, 4);

		if(!SSAFrame_Package(HEAD_DE, &mFrame, &fBuf, &fLen))
		{
			CommonApp_SendTheMessage(COORDINATOR_ADDR, fBuf, fLen);
			//HalLedBlink(HAL_LED_2, 2, 50, 100);
		}

		osal_mem_free(mFrame.data);
#endif
      }
      break;

    default: break;
    }
  }

#ifdef KEY_PUSH_PORT_1_BUTTON
  DeviceCtrl_HandlePort1Keys(keys, keyCounts);
#endif
}
#else
void CommonApp_HandleCombineKeys(uint16 keys, uint8 keyCounts)
{
  uint8 *keysID = get_keys_id();
  uint8 *keysPush = get_keys_push();

  if (keysPush[0] == HAL_KEY_LONG_PUSH)
  {
/* SolenoidValve */
#if (DEVICE_TYPE_ID==0xA2)
	if(keyCounts == 6 )
		
/* Super Button */
#elif (DEVICE_TYPE_ID==22)
	if(keyCounts != 6 )
	{
		SuperButton_LongKeyCountsSettingHandler(keyCounts);
	}
	else if(keyCounts == 6)

#else
	if(keyCounts == 6 &&(osal_memcmp(keysID, "333333", keyCounts)
		|| osal_memcmp(keysID, "444444", keyCounts)
		|| osal_memcmp(keysID, "555555", keyCounts)))
#endif
    { 
#if defined(HOLD_INIT_AUTHENTICATION)
      if(devState != DEV_HOLD)
      {
      	HalLedBlink ( HAL_LED_4, 0, 50, 100 );
        devStates_t tStates;
        if (ZSUCCESS == osal_nv_item_init( 
                  ZCD_NV_NWK_HOLD_STARTUP, sizeof(tStates),  &tStates))
        {
           tStates = DEV_HOLD;
          osal_nv_write(
                ZCD_NV_NWK_HOLD_STARTUP, 0, sizeof(tStates),  &tStates);
        }

        zgWriteStartupOptions(ZG_STARTUP_SET, ZCD_STARTOPT_DEFAULT_NETWORK_STATE);
        WatchDogEnable( WDTIMX );
      }
#endif
    }
/* Not Super Button */
#if (DEVICE_TYPE_ID!=22) && defined(BIND_SUPERBUTTON_CTRL_SUPPORT)
	else if(keyCounts == 3)
	{
		DE_t mFrame = {0};
	
		osal_memcpy(mFrame.head, FR_HEAD_DE, 2);
		osal_memcpy(mFrame.cmd, FR_CMD_FAST_CTRL, 4);
		incode_xtoc16(mFrame.short_addr, COORDINATOR_ADDR);
		mFrame.data_len = 1+16+4;
		mFrame.data = osal_mem_alloc(1+16+4);
		osal_memset(mFrame.data, 0, 1+16+4);
		mFrame.data[0] = SB_OPT_PAIRREG;
		osal_memcpy(mFrame.data+1, EXT_ADDR_G, 16);
		osal_memcpy(mFrame.data+17, SHORT_ADDR_G, 4);
		memcpy(mFrame.tail, f_tail, 4);

		if(!SSAFrame_Package(HEAD_DE, &mFrame, &fBuf, &fLen))
		{
			CommonApp_SendTheMessage(COORDINATOR_ADDR, fBuf, fLen);
			//HalLedBlink(HAL_LED_2, 2, 50, 100);
		}

		osal_mem_free(mFrame.data);
	}
#endif
	else if((keysID[0]=='3' || keysID[0]=='4' || keysID[0]=='5')
		&& keyCounts == 1)
    {
      //转发器允许/禁止入网,入网认证
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
  }
  else if (keysPush[0] == HAL_KEY_SHORT_PUSH)
  {
/* SolenoidValve */
#if (DEVICE_TYPE_ID==0xA2)
	SolenoidValve_KeyHandler();

/* Super Button */
#elif (DEVICE_TYPE_ID==22)
	SuperButton_KeyHandler();
#endif

/* Light Switch */
#ifdef KEY_PUSH_PORT_1_BUTTON
    DeviceCtrl_HandlePort1Keys(keys, keyCounts);
#endif
  }
}
#endif

void EndNodeApp_HeartBeatEvent(void)
{
  CommonApp_HeartBeatCB(NULL, NULL, NULL);

  set_user_event(CommonApp_TaskID, HEARTBERAT_EVT, CommonApp_HeartBeatCB, 
  	HEARTBEAT_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);

#ifdef TRANSCONN_BOARD_ENDNODE
  set_user_event(CommonApp_TaskID, TRANSNODE_UPLOAD_EVT, TransconnApp_HeartBeatCB, 
  	TRANSNODE_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);
#endif
}

