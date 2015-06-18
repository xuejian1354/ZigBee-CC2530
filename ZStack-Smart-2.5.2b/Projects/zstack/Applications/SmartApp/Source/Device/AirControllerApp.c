/**************************************************************************************************
  Filename:       AirControllerApp.c
  Revised:        $Date: 2014-04-16 13:53:12 -0800 (Wed, 16 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    AirController Application
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-06-16
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
#include "OLCD.h"

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
typedef union DATA_CMD
{
   uint8 data_buf[10];
   struct cn_data_t
   {
       uint8 Head; //52 
       uint8 CMD;//80
       uint8 PM25[2]; //
       uint8 PM10[2]; //
       uint8 data[2];//保留
       uint8 Check_sum;//
       uint8 Tail;//53
   }data_core;
}DATA_CMD_T;

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
static void AirControllerApp_TxHandler(uint8 txBuf[], uint8 txLen);
#ifdef HAL_UART01_BOTH
static void AirControllerDetect_TxHandler(uint8 txBuf[], uint8 txLen);
#endif
#ifndef ZDO_COORDINATOR
static void AirControllerApp_HeartBeatEvent(void);
#endif
static void Show_company(void);
static void Show_PM25(void);

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      CommonApp_InitConfirm
 *
 * @brief   Initialization function for the AirController App Task.
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
#ifndef HAL_UART01_BOTH
  CommonApp_SetUARTTxHandler(SERIAL_COM_PORT, AirControllerApp_TxHandler);
#else
  /*UART1:Comm*/
  CommonApp_SetUARTTxHandler(SERIAL_COM_PORT0, AirControllerDetect_TxHandler);
  /*UART0: Device*/
  CommonApp_SetUARTTxHandler(SERIAL_COM_PORT1, AirControllerApp_TxHandler);
#endif

  SetPM25ThresCallBack(AIRCONTROL_PM25_THRESMODE_UNABLE, 0, NULL);

  LCD_Init();			//oled 初始化  
  LCD_Fill(0xff);		//屏全亮 
  LCD_Fill(0x00);
  //LCD_CLS(); 
  Show_company();		//显示抬头
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
#ifndef HAL_UART01_BOTH
      HalUARTWrite(SERIAL_COM_PORT, pkt->cmd.Data, pkt->cmd.DataLength);
#else
	  HalUARTWrite(SERIAL_COM_PORT1, pkt->cmd.Data, pkt->cmd.DataLength);
#endif
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
  uint8 buf[FRAME_DATA_SIZE] = {0};
  
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
	DataCmd_Ctrl(AIRCONTROLLER_GET_DATA_MED, 3);
	mFrame.data = buf;
    CommonDevice_GetData(mFrame.data, &mFrame.data_len);
	memcpy(mFrame.tail, f_tail, 4);

	if(!SSAFrame_Package(HEAD_UC, &mFrame, &fBuf, &fLen))
	{
#ifndef HAL_UART01_BOTH
		HalUARTWrite(SERIAL_COM_PORT, fBuf, fLen);
#else
		HalUARTWrite(SERIAL_COM_PORT1, fBuf, fLen);
#endif
	}
#else
	UO_t mFrame;
	
	memcpy(mFrame.head, FR_HEAD_UO, 3);
	mFrame.type = FR_DEV_ROUTER;
	memcpy(mFrame.ed_type, FR_APP_DEV, 2);
	memcpy(mFrame.short_addr, SHORT_ADDR_G, 4);
	memcpy(mFrame.ext_addr, EXT_ADDR_G, 16);
#if(DEVICE_TYPE_ID==0)
	mFrame.data = NULL;
	mFrame.data_len = 0;
#else
	mFrame.data = buf;
    CommonDevice_GetData(mFrame.data, &mFrame.data_len);
#endif
	memcpy(mFrame.tail, f_tail, 4);

	if(!SSAFrame_Package(HEAD_UO, &mFrame, &fBuf, &fLen))
	{
#ifndef HAL_UART01_BOTH
		HalUARTWrite(SERIAL_COM_PORT, fBuf, fLen);
#else
		HalUARTWrite(SERIAL_COM_PORT1, fBuf, fLen);
#endif
		CommonApp_SendTheMessage(COORDINATOR_ADDR, fBuf, fLen);
	}
	
	AirControllerApp_HeartBeatEvent();
	HalStatesInit(status);
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
void AirControllerApp_HeartBeatEvent(void)
{
	CommonApp_HeartBeatCB(NULL, NULL, NULL);
	
	CommonApp_SetUserEvent(HEARTBERAT_EVT, CommonApp_HeartBeatCB, 
  		HEARTBEAT_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);
}
#endif

#ifdef HAL_UART01_BOTH
void AirControllerDetect_TxHandler(uint8 txBuf[], uint8 txLen)
{
	DATA_CMD_T data_cmd;
   	
	osal_memcpy(data_cmd.data_buf, txBuf, txLen);
    if(data_cmd.data_core.Head==0xAA)
  	{
    	uint16 PM25_val=(data_cmd.data_core.PM25[0]
					+data_cmd.data_core.PM25[1]<<8)/10;

		if(PM25_val != GetPM25Val())
		{
			SetPM25Val(PM25_val);
			Show_PM25();
			PM25_Threshold_Handler();
		}
  	} 
}
#endif

void AirControllerApp_TxHandler(uint8 txBuf[], uint8 txLen)
{
	uint8 buf[FRAME_DATA_SIZE] = {0};
	uint8 len = 0;
	uint16 Send_shortAddr = 0;

	//Command Handler
	{
		if(txLen>=16 && osal_memcmp(txBuf, FR_HEAD_DE, 2)
			&& osal_memcmp(txBuf+2, FR_CMD_JOIN_CTRL, 4)
			&& osal_memcmp(txBuf+txLen-4, f_tail, 4))
		{
			if(osal_memcmp(txBuf+6, SHORT_ADDR_G, 4))
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
		else if(txLen>=14 && osal_memcmp(txBuf, FR_HEAD_DE, 2)
			&& osal_memcmp(txBuf+2, FR_CMD_BROCAST_REFRESH, 4)
			&& osal_memcmp(txBuf+txLen-4, f_tail, 4))
		{
			incode_16_to_2(&Send_shortAddr, txBuf+6, 4);
			CommonApp_ProcessZDOStates(DEV_ZB_COORD);
			CommonApp_SendTheMessage(BROADCAST_ADDR, txBuf, txLen);
		}
		else if(txLen>=14 && osal_memcmp(txBuf, FR_HEAD_DE, 2)
			&& osal_memcmp(txBuf+2, FR_CMD_SINGLE_EXCUTE, 4)
			&& osal_memcmp(txBuf+txLen-4, f_tail, 4))
	    {
		  incode_16_to_2(&Send_shortAddr, txBuf+6, 4);
		  if(nwkAddr == Send_shortAddr)
		  {
	        DataCmd_Ctrl(txBuf+10, txLen-FR_DE_DATA_FIX_LEN);

	        if(!CommonDevice_GetData(buf, &len))
	        {
	          Update_Refresh(buf, len);
	        }
		  }
		  else
		  {
		    CommonApp_SendTheMessage(Send_shortAddr, txBuf, txLen);
		  }
	    }
	    else if(txLen>=14 && osal_memcmp(txBuf, FR_HEAD_DE, 2)
			&& osal_memcmp(txBuf+2, FR_CMD_PEROID_EXCUTE, 4)
			&& osal_memcmp(txBuf+txLen-4, f_tail, 4))
	    {
		  incode_16_to_2(&Send_shortAddr, txBuf+6, 4);
		  if(nwkAddr == Send_shortAddr)
		  {
	        CommonApp_SetUserEvent(CMD_PEROID_EVT, CommonApp_CmdPeroidCB, 
	        	CMD_PEROID_TIMEOUT, TIMER_LOOP_EXECUTION|TIMER_EVENT_RESIDENTS, NULL);
				
	        Update_Refresh(txBuf+10, txLen-FR_DE_DATA_FIX_LEN);
		  }
		  else
		  {
		    CommonApp_SendTheMessage(Send_shortAddr, txBuf, txLen);
		  }
	    }
	    else if(txLen>=14 && osal_memcmp(txBuf, FR_HEAD_DE, 2)
			&& osal_memcmp(txBuf+2, FR_CMD_PEROID_STOP, 4)
			&& osal_memcmp(txBuf+txLen-4, f_tail, 4))
	    {
		  incode_16_to_2(&Send_shortAddr, txBuf+6, 4);
		  if(nwkAddr == Send_shortAddr)
		  {
	        CommonApp_UpdateUserEvent(CMD_PEROID_EVT, 
	        	NULL, TIMER_NO_LIMIT, TIMER_CLEAR_EXECUTION, NULL);
				
	        Update_Refresh(txBuf+10, txLen-FR_DE_DATA_FIX_LEN);
		  }
		  else
		  {
		    CommonApp_SendTheMessage(Send_shortAddr, txBuf, txLen);
		  }
	    }
		else if(txLen>=14 && osal_memcmp(txBuf, FR_HEAD_DE, 2)
			&& osal_memcmp(txBuf+2, FR_CMD_SINGLE_REFRESH, 4)
			&& osal_memcmp(txBuf+txLen-4, f_tail, 4))
		{
			incode_16_to_2(&Send_shortAddr, txBuf+6, 4);
			if(nwkAddr == Send_shortAddr) //coord self
			{
				CommonApp_ProcessZDOStates(DEV_ZB_COORD);
			}
			else
			{
				CommonApp_SendTheMessage(Send_shortAddr, txBuf, txLen);
			}
		}
	}
}

/*********************************************************************
 */
void Show_company(void)
{   
	uint8 i;
   	for(i=0; i<7; i++)
	{
		LCD_P16x16Ch(i*16, 0, i);  //点阵显示,y为显示的行数
		//LCD_P16x16Ch(i*16,2,i+8);
		// LCD_P16x16Ch(i*16,4,i+16);
		// LCD_P16x16Ch(i*16,6,i+24);
	} 
}

void Show_PM25(void)
{
	uint8 PM25_Buf[9];
	//uint8 PM10_DATA[9];
	//uint16 DATA_PM10;

	uint16 PM25_val = GetPM25Val();
	
	PM25_Buf[0]=PM25_val/100+'0';
	PM25_Buf[1]=(PM25_val/10)%10+'0';
	PM25_Buf[2]=PM25_val%10+'0';
	PM25_Buf[3]='u';
	PM25_Buf[4]='g';
	PM25_Buf[5]='/';
	PM25_Buf[6]='m';
	PM25_Buf[7]=3+'0';
	PM25_Buf[8]='\0';
	//注意小端模式
	/* DATA_PM10=(data_cmd.data_core.PM10[0]+data_cmd.data_core.PM10[1]*256)/10;
   	PM10_DATA[0]=DATA_PM10/100+'0';
	PM10_DATA[1]=(DATA_PM10/10)%10+'0';
	PM10_DATA[2]=DATA_PM10%10+'0';
	PM10_DATA[3]='u';
	PM10_DATA[4]='g';
	PM10_DATA[5]='/';
	PM10_DATA[6]='m';
	PM10_DATA[7]=3+'0';
	PM10_DATA[8]='\0';
	*/
	//LCD_CLS();
	LCD_P8x16Str(0, 3, "PM2.5");
	LCD_P8x16Str(0, 6, PM25_Buf);
	// LCD_P8x16Str(64,0,"PM10");
	// LCD_P8x16Str(64,4,PM10_DATA);
}
