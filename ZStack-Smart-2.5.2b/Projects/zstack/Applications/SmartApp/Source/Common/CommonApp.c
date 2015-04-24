/**************************************************************************************************
  Filename:       CommonApp.c
  Revised:        $Date: 2014-04-16 16:00:27 -0800 (Wed, 16 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    Common Application
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-04-16
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-02-05
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "CommonApp.h"

#include "OSAL.h"
#include "OSAL_Nv.h"
#include "OnBoard.h"

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
 * EXTERNAL VARIABLES
 */

//chain list header
extern ssaUserEvent_t *puser_event;


/*********************************************************************
 * GLOBAL VARIABLES
 */
// This list should be filled with Application specific Cluster IDs.
//通用cluster list
const cId_t CommonApp_InClusterList[COMMONAPP_MAX_INCLUSTERS] =
{
  COMMONAPP_CLUSTERID,
};

const cId_t CommonApp_OutClusterList[COMMONAPP_MAX_OUTCLUSTERS] =
{
  COMMONAPP_CLUSTERID,
};

const SimpleDescriptionFormat_t CommonApp_SimpleDesc =
{
  COMMONAPP_ENDPOINT,              //  int Endpoint;
  COMMONAPP_PROFID,                //  uint16 AppProfId[2];
  COMMONAPP_DEVICEID,              //  uint16 AppDeviceId[2];
  COMMONAPP_DEVICE_VERSION,        //  int   AppDevVer:4;
  COMMONAPP_FLAGS,                 //  int   AppFlags:4;
  COMMONAPP_MAX_INCLUSTERS,          //  byte  AppNumInClusters;
  (cId_t *)CommonApp_InClusterList,  //  byte *pAppInClusterList;
  COMMONAPP_MAX_OUTCLUSTERS,          //  byte  AppNumOutClusters;
  (cId_t *)CommonApp_OutClusterList   //  byte *pAppOutClusterList;
};

// This is the Endpoint/Interface description.  It is defined here, but
// filled-in in CommonApp_Init().  Another way to go would be to fill
// in the structure here and make it a "const" (in code space).  The
// way it's defined in this sample app it is define in RAM.
endPointDesc_t CommonApp_epDesc;


byte CommonApp_TaskID;   // Task ID for internal task/event processing
                          // This variable will be received when
                          // CommonApp_Init() is called.
devStates_t CommonApp_NwkState;
byte CommonApp_TransID;  // This is the unique message ID (counter)

afAddrType_t CommonApp_DstAddr;

//network address
uint16 nwkAddr;
//mac address
ZLongAddr_t macAddr;

uint8 SHORT_ADDR_G[4] = "";

uint8 EXT_ADDR_G[16] = "";

#ifdef SSA_ENDNODE
/* operations data */
uint8 *optData = NULL;
uint8 optDataLen = 0;
#endif

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern const uint8 f_tail[4]; 

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void CommonApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg );
static void CommonApp_afDatacfm(afDataConfirm_t *data);
static void CommonApp_HandleKeys( byte shift, uint16 keys );
#ifdef RTR_NWK
static void CommonApp_PermitJoiningLedIndicate(
				void *params, uint16 *duration, uint8 *count);
#endif

static void set_app_event(ssaUserEvent_t *ssaUserEvent);
#if(HAL_UART==TRUE)
static void SerialTx_CallBack(uint8 port, uint8 event);
static void Data_TxHandler(uint8 txBuf[], uint8 txLen);
static void CommonApp_SerialTxCB( void *params, uint16 *duration, uint8 *count);
#endif

/*********************************************************************
 * LOCAL VARIABLES
 */
/*串口缓冲区*/
#if(HAL_UART==TRUE)
//static uint8 Serial_TxSeq;
static uint8 Serial_TxBuf[SERIAL_COM_TX_MAX];
static uint8 Serial_TxLen;

/*解析串口数据得到的有效命令缓冲区*/
static uint8 data_TxLen;
static uint8 data_TxBuf[FRAME_DATA_LENGTH];
static UART_TxHandler mData_TxHandler;
#endif


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      CommonApp_Init
 *
 * @brief   Initialization function for the Commonr App Task.
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

void CommonApp_Init( uint8 task_id )
{
    CommonApp_TaskID = task_id;
    CommonApp_NwkState = DEV_INIT;
    CommonApp_TransID = 0;

#if(HAL_UART==TRUE)
	//串口初始化
    Serial_TxLen = 0;
	mData_TxHandler = Data_TxHandler;
    Serial_Init(SerialTx_CallBack);
#endif

	puser_event = NULL;
  
    // Device hardware initialization can be added here or in main() (Zmain.c).
    // If the hardware is application specific - add it here.
    // If the hardware is other parts of the device add it in main().
  
    CommonApp_DstAddr.addrMode = (afAddrMode_t)AddrBroadcast;
    CommonApp_DstAddr.endPoint = COMMONAPP_ENDPOINT;
    CommonApp_DstAddr.addr.shortAddr = 0xFFFF;

  // Fill out the endpoint description.
    CommonApp_epDesc.endPoint = COMMONAPP_ENDPOINT;
    CommonApp_epDesc.task_id = &CommonApp_TaskID;
    CommonApp_epDesc.simpleDesc
              = (SimpleDescriptionFormat_t *)&CommonApp_SimpleDesc;
    CommonApp_epDesc.latencyReq = noLatencyReqs;
  
    // Register the endpoint description with the AF
    afRegister( &CommonApp_epDesc );
  
    // Register for all key events - This app will handle all key events
    RegisterForKeys( CommonApp_TaskID );
  
    ZDO_RegisterForZDOMsg( CommonApp_TaskID, End_Device_Bind_rsp );
    ZDO_RegisterForZDOMsg( CommonApp_TaskID, Match_Desc_rsp );

	CommonApp_InitConfirm(task_id);

}


/*********************************************************************
 * @fn      CommonrApp_ProcessEvent
 *
 * @brief   Common Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The OSAL assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  none
 */
uint16 CommonApp_ProcessEvent(uint8 task_id, uint16 events)
{
    afIncomingMSGPacket_t *MSGpkt;
    afDataConfirm_t *afDataConfirm;

	ssaUserEvent_t *t_ssaUserEvent;
  
    // Data Confirmation message fields
    byte sentEP;
    ZStatus_t sentStatus;
    byte sentTransID;       // This should match the value sent
    (void)task_id;  // Intentionally unreferenced parameter
    

    if ( events & SYS_EVENT_MSG )
    {
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( CommonApp_TaskID );
      while ( MSGpkt )
      {
        switch ( MSGpkt->hdr.event )
        {
        case ZDO_CB_MSG:
		  CommonApp_ProcessZDOMsgs( (zdoIncomingMsg_t *)MSGpkt );
          break;

        case KEY_CHANGE:
		  CommonApp_HandleKeys( ((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys );
          break;

        case AF_DATA_CONFIRM_CMD:
          // This message is received as a confirmation of a data packet sent.
          // The status is of ZStatus_t type [defined in ZComDef.h]
          // The message fields are defined in AF.h
          afDataConfirm = (afDataConfirm_t *)MSGpkt;
          sentEP = afDataConfirm->endpoint;
          sentStatus = afDataConfirm->hdr.status;
          sentTransID = afDataConfirm->transID;
          (void)sentEP;
          (void)sentTransID;

          // Action taken when confirmation is received.
          if ( sentStatus != ZSuccess )
          {
            CommonApp_afDatacfm( afDataConfirm );
          }
          break;

        case AF_INCOMING_MSG_CMD:
#if defined(HAL_MT7620_GPIO_MAP) || (DEVICE_TYPE_ID==13)
		  HalLedSet( HAL_LED_1,  HAL_LED_MODE_BLINK);
#else
		  HalLedSet(HAL_LED_2, HAL_LED_MODE_BLINK);
#endif
          CommonApp_MessageMSGCB( MSGpkt );
          break;

        case ZDO_STATE_CHANGE:
		  HalLedSet(HAL_LED_3, HAL_LED_MODE_BLINK);
		  CommonApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
		  CommonApp_ProcessZDOStates( CommonApp_NwkState );
          break;

        default:
          break;
      }

      // Release the memory
      osal_msg_deallocate( (uint8 *)MSGpkt );

      // Next
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive( CommonApp_TaskID );
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

#if 0
//#if(HAL_UART==TRUE)
  if(events & SERIAL_CMD_EVT)
  {
  	mData_TxHandler(data_TxBuf, data_TxLen);
	return (events ^ SERIAL_CMD_EVT);
  }
#endif

  t_ssaUserEvent=puser_event;
  while(t_ssaUserEvent)
  {
    if(t_ssaUserEvent->count.branch.count>0
		|| t_ssaUserEvent->count.branch.resident==1)
    {
	  if(events & t_ssaUserEvent->event)
	  {
		if(t_ssaUserEvent->ssa_ProcessUserTaskCB != NULL)
      	  t_ssaUserEvent->ssa_ProcessUserTaskCB(t_ssaUserEvent->ptype, 
	    		&t_ssaUserEvent->duration, &t_ssaUserEvent->count.overall);

	    if(t_ssaUserEvent->count.branch.count != TIMER_LOOP_EXECUTION)
          t_ssaUserEvent->count.branch.count--;

	    if(t_ssaUserEvent->count.branch.count > 0)
		  set_app_event(t_ssaUserEvent);

		return (events ^ t_ssaUserEvent->event);
	  }
  	}
	//只有在count==0 且长驻内存标志resident 为0的情况下
	//将该事件从链表中删除
  	else
  	{
	  Del_User_Event(t_ssaUserEvent->event);
  	}
  
  	t_ssaUserEvent = t_ssaUserEvent->next;
  }	

  // Discard unknown events
  return 0;
}


/*********************************************************************
 * Event Generation Functions
 */
#if(HAL_UART==TRUE)
/*********************************************************************
 * @fn      SerialTx_CallBack()
 *
 * @brief   receive data from serial port
 *
 * @param   serial receive port, event
 *
 * @return  none
 */
void SerialTx_CallBack(uint8 port, uint8 event)
{
  (void)port;

  if ((event & (HAL_UART_RX_FULL | HAL_UART_RX_ABOUT_FULL 
  			| HAL_UART_RX_TIMEOUT)) && !Serial_TxLen)
  {
    if (Serial_TxLen < SERIAL_COM_TX_MAX)
    {
      Serial_TxLen = HalUARTRead( SERIAL_COM_PORT,Serial_TxBuf,SERIAL_COM_TX_MAX);
    }

    if(Serial_TxLen)
    {  
      memset(data_TxBuf, 0, FRAME_DATA_LENGTH);

	  if(Serial_TxLen<FRAME_DATA_LENGTH)
        data_TxLen = Serial_TxLen;
	  else
	  	data_TxLen = FRAME_DATA_LENGTH;
	  
      memcpy(data_TxBuf,Serial_TxBuf,data_TxLen);

      memset(Serial_TxBuf, 0, SERIAL_COM_TX_MAX);
      Serial_TxLen = 0;

      //osal_set_event(CommonApp_TaskID, SERIAL_CMD_EVT);
	  
      //CommonApp_UpdateUserEvent(SERIAL_CMD_EVT, CommonApp_SerialTxCB, 
  			//TIMER_NO_LIMIT, TIMER_ONE_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);
  	  CommonApp_SerialTxCB(NULL, NULL, NULL);
	}
  }
}


/*********************************************************************
 * @fn      CommonApp_SerialTxCB()
 *
 * @brief   deal with serial receice data
 *
 * @param   none
 *
 * @return  none
 */
void CommonApp_SerialTxCB( void *params, uint16 *duration, uint8 *count)
{
  HalLedSet(HAL_LED_2, HAL_LED_MODE_BLINK);
  mData_TxHandler(data_TxBuf, data_TxLen);
}


/*********************************************************************
 * @fn      Data_TxHandler()
 *
 * @brief   deault function to deal with serial receice data
 *
 * @param   point to data, data length
 *
 * @return  none
 */
void Data_TxHandler(uint8 txBuf[], uint8 txLen)
{
  HalUARTWrite(SERIAL_COM_PORT, txBuf, txLen);
}
#endif


/*********************************************************************
 * @fn      CommonApp_ProcessZDOMsgs()
 *
 * @brief   Process response messages
 *
 * @param   incoming message
 *
 * @return  none
 */
void CommonApp_ProcessZDOMsgs( zdoIncomingMsg_t *inMsg )
{
  switch ( inMsg->clusterID )
  {
    case End_Device_Bind_rsp:
      if ( ZDO_ParseBindRsp( inMsg ) == ZSuccess )
      {
        // Light LED
        //HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
      }
#if defined( BLINK_LEDS )
      else
      {
        // Flash LED to show failure
        //HalLedSet ( HAL_LED_4, HAL_LED_MODE_FLASH );
      }
#endif
      break;

    case Match_Desc_rsp:
      {
        ZDO_ActiveEndpointRsp_t *pRsp = ZDO_ParseEPListRsp( inMsg );
        if ( pRsp )
        {
          if ( pRsp->status == ZSuccess && pRsp->cnt )
          {
            CommonApp_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
            CommonApp_DstAddr.addr.shortAddr = pRsp->nwkAddr;
            // Take the first endpoint, Can be changed to search through endpoints
            CommonApp_DstAddr.endPoint = pRsp->epList[0];

            // Light LED
            //HalLedSet( HAL_LED_4, HAL_LED_MODE_ON );
          }
          osal_mem_free( pRsp );
        }
      }
      break;
  }
}


/*********************************************************************
 * @fn      CommonApp_afDatacfm()
 *
 * @brief   Process send data comfirm
 *
 * @param   comfirm message
 *
 * @return  none
 */
void CommonApp_afDatacfm(afDataConfirm_t *data)
{}


/*********************************************************************
 * @fn      CommonApp_HandleKeys
 *
 * @brief   Handles all key events for this device.
 *
 * @param   shift - true if in shift/alt.
 * @param   keys - bit field for key events. Valid entries:
 *                 HAL_KEY_SW_4
 *                 HAL_KEY_SW_3
 *                 HAL_KEY_SW_2
 *                 HAL_KEY_SW_1
 *
 * @return  none
 */
void CommonApp_HandleKeys( uint8 shift, uint16 keys )
{
  zAddrType_t dstAddr;

  // Shift is used to make each button/switch dual purpose.
  if ( shift == HAL_KEY_STATE_SHIFT )
  {
    if ( keys & HAL_KEY_SW_1 )
    {
    }
    if ( keys & HAL_KEY_SW_2 )
    {
    }
    if ( keys & HAL_KEY_SW_3 )
    {
    }
    if ( keys & HAL_KEY_SW_4 )
    {
    }
  }
  else
  {
    if ( keys & HAL_KEY_SW_1 )
    {
      // Since SW1 isn't used for anything else in this application...
#if defined( SWITCH1_BIND )
      // we can use SW1 to simulate SW2 for devices that only have one switch,
      keys |= HAL_KEY_SW_2;
#elif defined( SWITCH1_MATCH )
      // or use SW1 to simulate SW4 for devices that only have one switch
      keys |= HAL_KEY_SW_4;
#endif
    }

    if ( keys & HAL_KEY_SW_2 )
    {
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );

      // Initiate an End Device Bind Request for the mandatory endpoint
      dstAddr.addrMode = Addr16Bit;
      dstAddr.addr.shortAddr = COORDINATOR_ADDR; // Coordinator
      ZDP_EndDeviceBindReq( &dstAddr, NLME_GetShortAddr(),
                            CommonApp_epDesc.endPoint,
                            COMMONAPP_PROFID,
                            COMMONAPP_MAX_INCLUSTERS, (cId_t *)CommonApp_InClusterList,
                            COMMONAPP_MAX_OUTCLUSTERS, (cId_t *)CommonApp_OutClusterList,
                            FALSE );
    }

    if ( keys & HAL_KEY_SW_3 )
    {
    }

    if ( keys & HAL_KEY_SW_4 )
    {
      HalLedSet ( HAL_LED_4, HAL_LED_MODE_OFF );
      // Initiate a Match Description Request (Service Discovery)
      dstAddr.addrMode = AddrBroadcast;
      dstAddr.addr.shortAddr = NWK_BROADCAST_SHORTADDR;
      ZDP_MatchDescReq( &dstAddr, NWK_BROADCAST_SHORTADDR,
                        COMMONAPP_PROFID,
                        COMMONAPP_MAX_INCLUSTERS, (cId_t *)CommonApp_InClusterList,
                        COMMONAPP_MAX_OUTCLUSTERS, (cId_t *)CommonApp_OutClusterList,
                        FALSE );
    }
  }

  if ( (keys & HAL_KEY_SW_6) 
#ifdef KEY_PUSH_PORT_1_BUTTON
		|| (keys & HAL_KEY_PORT_1_SWITCHS)
#endif
  		)
  {
#ifdef HAL_KEY_COMBINE_INT_METHOD	
    CommonApp_HandleCombineKeys(keys, halGetKeyCount());
#endif
  }
}


/******************************************************************************
 * @fn         CommonApp_GetDeviceInfo
 *
 * @brief       The CommonApp_GetDeviceInfo function retrieves a Device Information
 *              Property.
 *
 * @param       param - The identifier for the device information
 *              pValue - A buffer to hold the device information
 *
 * @return      none
 */
void CommonApp_GetDeviceInfo ( uint8 param, void *pValue )
{
  switch(param)
  {
    case ZB_INFO_DEV_STATE:
      osal_memcpy(pValue, &devState, sizeof(uint8));
      break;
    case ZB_INFO_IEEE_ADDR:
      osal_memcpy(pValue, &aExtendedAddress, Z_EXTADDR_LEN);
      break;
    case ZB_INFO_SHORT_ADDR:
      osal_memcpy(pValue, &_NIB.nwkDevAddress, sizeof(uint16));
      break;
    case ZB_INFO_PARENT_SHORT_ADDR:
      osal_memcpy(pValue, &_NIB.nwkCoordAddress, sizeof(uint16));
      break;
    case ZB_INFO_PARENT_IEEE_ADDR:
      osal_memcpy(pValue, &_NIB.nwkCoordExtAddress, Z_EXTADDR_LEN);
      break;
    case ZB_INFO_CHANNEL:
      osal_memcpy(pValue, &_NIB.nwkLogicalChannel, sizeof(uint8));
      break;
    case ZB_INFO_PAN_ID:
      osal_memcpy(pValue, &_NIB.nwkPanId, sizeof(uint16));
      break;
    case ZB_INFO_EXT_PAN_ID:
      osal_memcpy(pValue, &_NIB.extendedPANID, Z_EXTADDR_LEN);
      break;
  }
}


#ifdef SSA_ENDNODE
int8 CommonDevice_SetData(uint8 const *data, uint8 dataLen)
{
	if(optData != NULL && optDataLen < dataLen && dataLen <= MAX_OPTDATA_SIZE)
	{
		osal_mem_free(optData);
		optData = NULL;
	}

	if(dataLen <= MAX_OPTDATA_SIZE)
	{
		if(optData == NULL && dataLen != 0)
		{
			optData = osal_mem_alloc(dataLen);
		}

		osal_memcpy(optData, data, dataLen);
		optDataLen = dataLen;

		return set_device_data(optData, optDataLen);
	}	
	
	return -1;
}


int8 CommonDevice_GetData(uint8 *data, uint8 *dataLen)
{
	return get_device_data(data, dataLen);
}


/*********************************************************************
 * @fn      CommonApp_CmdPeroidCB
 *
 * @brief   command peroid event callback function
 *
 * @param   none
 *
 * @return  none
 */
void CommonApp_CmdPeroidCB( void *params, uint16 *duration, uint8 *count)
{
	uint8 buf[FRAME_DATA_SIZE] = {0};
	uint8 len = 0;
	
	if(!CommonDevice_GetData(buf, &len))
		Update_Refresh(buf, len);
}


/***************************************************************
                     控制命令与返回
*****************************************************************/
int8 DataCmd_Ctrl(uint8 *data, uint8 length)
{
	return CommonDevice_SetData(data, length);
}

void Update_Refresh(uint8 *data, uint8 length)
{
  	UR_t mFrame;
	uint8 *fBuf;		//pointer data buffer
	uint16 fLen;		//buffer data length
  
	memcpy(mFrame.head, FR_HEAD_UR, 3);
#ifdef RTR_NWK
	mFrame.type = FR_DEV_ROUTER;
#else
	mFrame.type = FR_DEV_ENDDEV;
#endif
	memcpy(mFrame.ed_type, FR_APP_DEV, 2);
	memcpy(mFrame.short_addr, SHORT_ADDR_G, 4);

	mFrame.data = data;
	mFrame.data_len = length;
	
	memcpy(mFrame.tail, f_tail, 4);

	if(!SSAFrame_Package(HEAD_UR, &mFrame, &fBuf, &fLen))
	{
		CommonApp_SendTheMessage(COORDINATOR_ADDR, fBuf, fLen);
	}
}
#endif


void PermitJoin_Refresh(uint8 *data, uint8 length)
{
  	uint8 buf[FRAME_BUFFER_SIZE] = {0};
  
	memcpy(buf, FR_HEAD_UJ, 3);
	memcpy(buf+3, SHORT_ADDR_G, 4);

	memcpy(buf+7, data, length);
	
	memcpy(buf+7+length, f_tail, 4);
	
#ifdef SSA_CONNECTOR
	HalUARTWrite(SERIAL_COM_PORT, buf, 11+length);
#else
	CommonApp_SendTheMessage(COORDINATOR_ADDR, buf, 11+length);
#endif
}

bool isPermitJoining = 0;
#ifdef RTR_NWK
void CommonApp_PermitJoiningLedIndicate(
				void *params, uint16 *duration, uint8 *count)
{
	uint8 mode = (int)params;
	switch(mode)
	{
	case HAL_LED_MODE_ON:
		isPermitJoining = TRUE;
#if defined(HAL_MT7620_GPIO_MAP)  || (DEVICE_TYPE_ID==13)
		HalLedSet( HAL_LED_2,  mode);
#else
		HalLedSet( HAL_LED_1,  mode);
#endif
		break;

	case HAL_LED_MODE_OFF:
		isPermitJoining = FALSE;
#if defined(HAL_MT7620_GPIO_MAP) || (DEVICE_TYPE_ID==13)
		HalLedSet( HAL_LED_2,  mode);
#else
		HalLedSet( HAL_LED_1,  mode);
#endif
		break;
	}
}
#endif

ZStatus_t CommonApp_PermitJoiningRequest( byte PermitDuration )
{
#ifdef RTR_NWK
	if(PermitDuration)
	{
		CommonApp_PermitJoiningLedIndicate( (void *)HAL_LED_MODE_ON, NULL, NULL );
		
		CommonApp_UpdateUserEvent(PERMIT_JOIN_EVT, CommonApp_PermitJoiningLedIndicate, 
	  		PermitDuration*1000, TIMER_ONE_EXECUTION, (void *)HAL_LED_MODE_OFF);
	}
	else
	{
		CommonApp_PermitJoiningLedIndicate( (void *)HAL_LED_MODE_OFF, NULL, NULL );

		CommonApp_UpdateUserEvent(PERMIT_JOIN_EVT, 
				NULL, TIMER_NO_LIMIT, TIMER_CLEAR_EXECUTION, NULL);
	}

	return NLME_PermitJoiningRequest(PermitDuration);
#else 
	return 0;
#endif
}

/*********************************************************************
 * @fn      set_app_event
 *
 * @brief   set osal event from app event
 *
 * @param   user process event
 *
 * @return  none
 */
void set_app_event(ssaUserEvent_t *ssaUserEvent)
{
  if(ssaUserEvent->count.branch.count)
  {
    if(ssaUserEvent->duration > TIMER_LOWER_LIMIT)
	  osal_start_timerEx(CommonApp_TaskID, ssaUserEvent->event, 
		  ssaUserEvent->duration);
    else
	  osal_set_event(CommonApp_TaskID, ssaUserEvent->event);
  }
}



/*********************************************************************
 * @fn      CommonApp_SetUserEvent
 *
 * @brief   set user defined events
 *
 * @param   none
 *
 * @return  none
 */
int8 CommonApp_SetUserEvent(uint16 event, 
	ssa_ProcessUserTaskCB_t ssa_ProcessUserTaskCB, uint16 duration, uint8 count, void *ptype)
{
	ssaUserEvent_t *m_ssaUserEvent;
	m_ssaUserEvent = (ssaUserEvent_t *)osal_mem_alloc(sizeof(ssaUserEvent_t));

	m_ssaUserEvent->event = event;
	m_ssaUserEvent->duration = duration;
	m_ssaUserEvent->count.overall = count;
	m_ssaUserEvent->ptype = ptype;
	m_ssaUserEvent->ssa_ProcessUserTaskCB = ssa_ProcessUserTaskCB;

	if(Add_User_Event(m_ssaUserEvent) < 0)
	{
	  osal_mem_free(m_ssaUserEvent);
	  return -1;
	}

	set_app_event(m_ssaUserEvent);
	return 0;
}


/*********************************************************************
 * @fn      CommonApp_UpdateUserEvent
 *
 * @brief   update user defined events
 *
 * @param   none
 *
 * @return  none
 */
int8 CommonApp_UpdateUserEvent(uint16 event, 
	ssa_ProcessUserTaskCB_t ssa_ProcessUserTaskCB, uint16 duration, uint8 count, void *ptype)
{
  ssaUserEvent_t *m_ssaUserEvent;
  m_ssaUserEvent = Query_User_Event(event);

  if(m_ssaUserEvent == NULL)
  {
	return CommonApp_SetUserEvent(event, 
		ssa_ProcessUserTaskCB, duration, count, ptype);
  }

  m_ssaUserEvent->duration = duration;
  m_ssaUserEvent->count.overall = count;
  m_ssaUserEvent->ptype = ptype;
  m_ssaUserEvent->ssa_ProcessUserTaskCB = ssa_ProcessUserTaskCB;

  set_app_event(m_ssaUserEvent);
  return 0;
}


/*********************************************************************
 * @fn      CommonApp_SetUARTTxHandler
 *
 * @brief   update user uart receive handler
 *
 * @param   uart receive handler
 *
 * @return  none
 */
void CommonApp_SetUARTTxHandler(UART_TxHandler txHandler)
{
#if(HAL_UART==TRUE)
  if(txHandler != NULL)
  	mData_TxHandler = txHandler;
#endif
}
 

/*********************************************************************
 * @fn      CommonApp_SendTheMessage
 *
 * @brief   Send "the" message.
 *
 * @param   none
 *
 * @return  none
 */
void CommonApp_SendTheMessage(uint16 dstNwkAddr, uint8 *data, uint8 length)
{

  CommonApp_DstAddr.addrMode = (afAddrMode_t)Addr16Bit;
  CommonApp_DstAddr.endPoint = COMMONAPP_ENDPOINT;
  CommonApp_DstAddr.addr.shortAddr = dstNwkAddr;
  
  AF_DataRequest( &CommonApp_DstAddr, &CommonApp_epDesc,
                       COMMONAPP_CLUSTERID,
                       length,
                       data,
                       &CommonApp_TransID,
                       AF_DISCV_ROUTE, AF_DEFAULT_RADIUS );
}
