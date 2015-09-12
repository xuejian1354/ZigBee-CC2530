/**************************************************************************************************
  Filename:       SuperButton_DeviceDataCtrl.c
  Revised:        $Date: 2015-09-11 14:03:11 -0800 (Sep, 02 Fri 2015) $
  Revision:       $Revision: 29217 $

  Description:    Super Button Board Data Ctrl.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-09-12
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
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "hal_drivers.h"

#include <stdio.h>
#include <stdlib.h>

/*********************************************************************
 * MACROS
 */
#define SB_DATA_MAXSIZE		64

#define SB_FIRST_SET_CODE	93		//random, but must be lt 0xEF

/*********************************************************************
 * CONSTANTS
 */
typedef struct FastCtrlBindSrc
{
	ZLongAddr_t coordMac;
	zAddrType_t dst;

	struct FastCtrlBindSrc *next;
}FastCtrlBindSrc_t;


/*********************************************************************
 * EXTERNAL VARIABLES
 */
extern byte CommonApp_TaskID;

extern uint8 SHORT_ADDR_G[4];
extern uint8 EXT_ADDR_G[16];
extern const uint8 f_tail[4];

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
static uint8 *sBuf = NULL;
static uint16 sLen = 0;

static ZLongAddr_t coordAddr;

static FastCtrlBindSrc_t *bindList = NULL;
static uint16 bindLen = 0;

static bool isAllowPair = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static int8 SuperButton_SendBindCmd(uint16 dstNwkAddr, 
										char bindCmd, uint8 *data, uint8 length);

static void SuperButton_PairCB( void *params, uint16 *duration, uint8 *count);

static void write_nv_bindsrc(ZLongAddr_t bindSrc);
static int8 read_nv_bindsrc(ZLongAddr_t *bindSrc, uint16 bindSize);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */
void HalDeviceInit (void)
{
	uint8 firstSetCode;
	//first set, clear nv
	if (ZSUCCESS == osal_nv_item_init( ZCD_NV_SUPERBUTTON_ISFIRST,
											sizeof(firstSetCode),  
											&firstSetCode))
  	{
      	osal_nv_read(ZCD_NV_SUPERBUTTON_ISFIRST, 
	  					0, 
	  					sizeof(firstSetCode),  
	  					&firstSetCode);

		if(firstSetCode != SB_FIRST_SET_CODE)
		{
			firstSetCode = SB_FIRST_SET_CODE;
			osal_nv_write(ZCD_NV_SUPERBUTTON_ISFIRST, 
		  					0, 
		  					sizeof(firstSetCode), 
		  					&firstSetCode);	

			uint16 size = 0;
			if (ZSUCCESS == osal_nv_item_init( ZCD_NV_SUPERBUTTON_SIZE,
											sizeof(size),  
											&size))
			{
				osal_nv_write(ZCD_NV_SUPERBUTTON_SIZE, 
			  					0, 
			  					sizeof(size), 
			  					&size);
			}
		}
	}
}

void HalStatesInit(devStates_t status)
{
	if( status == DEV_ROUTER || status == DEV_END_DEVICE)
	{
		SuperButton_SendBindCmd(COORDINATOR_ADDR, SB_OPT_CFG, NULL, 0);
	}
}

int8 SuperButton_SendBindCmd(uint16 dstNwkAddr, 
										char bindCmd, uint8 *data, uint8 length)
{
	DE_t mFrame = {0};
	
	osal_memcpy(mFrame.head, FR_HEAD_DE, 2);
	osal_memcpy(mFrame.cmd, FR_CMD_FAST_CTRL, 4);
	incode_xtoc16(mFrame.short_addr, dstNwkAddr);
	
	switch(bindCmd)
	{
	case SB_OPT_CFG:
	{
		uint16 size = 0;
		if (ZSUCCESS == osal_nv_item_init( ZCD_NV_SUPERBUTTON_SIZE,
										sizeof(size),  
										&size))
		{
			osal_nv_read(ZCD_NV_SUPERBUTTON_SIZE, 
		  					0, 
		  					sizeof(size), 
		  					&size);

			ZLongAddr_t *bindSrc = osal_mem_alloc(Z_EXTADDR_LEN*size);
			if(read_nv_bindsrc(bindSrc, size))
			{
				break;
			}
			
			mFrame.data_len = 1+16+16*size;		//opt+extaddr+bindsrc
			mFrame.data = osal_mem_alloc(mFrame.data_len);
			osal_memset(mFrame.data, 0, mFrame.data_len);
			mFrame.data[0] = SB_OPT_CFG;
			osal_memcpy(mFrame.data+1, EXT_ADDR_G, 16);

			uint16 i = 0;
			while(i<size)
			{
				incode_xtocs(mFrame.data+17+Z_EXTADDR_LEN*i, 
								(uint8 *)&bindSrc[i], 
								Z_EXTADDR_LEN);

				i++;
			}
			
			memcpy(mFrame.tail, f_tail, 4);

			if(!SSAFrame_Package(HEAD_DE, &mFrame, &sBuf, &sLen))
			{
				CommonApp_SendTheMessage(dstNwkAddr, sBuf, sLen);
			}

			osal_mem_free(mFrame.data);			
			osal_mem_free(bindSrc);
		}
	}
		break;
		
	case SB_OPT_PAIR:
		isAllowPair ^= 1;
		if(isAllowPair)
		{
			mFrame.data_len = 1+16+4;
			mFrame.data = osal_mem_alloc(1+16+4);
			mFrame.data[0] = SB_OPT_PAIR;
			osal_memcpy(mFrame.data+1, EXT_ADDR_G, 16);
			osal_memcpy(mFrame.data+17, SHORT_ADDR_G, 4);
			memcpy(mFrame.tail, f_tail, 4);

			if(!SSAFrame_Package(HEAD_DE, &mFrame, &sBuf, &sLen))
			{
				CommonApp_SendTheMessage(dstNwkAddr, sBuf, sLen);
			}

			osal_mem_free(mFrame.data);
			
			HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
			update_user_event(CommonApp_TaskID, 
								SUPERBUTTON_PAIR_EVT, 
								SuperButton_PairCB, 
								SUPERBUTTON_PAIR_TIMEOUT, 
								TIMER_ONE_EXECUTION | TIMER_EVENT_RESIDENTS, 
								NULL);
		}
		else
		{
			HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
			update_user_event(CommonApp_TaskID, 
								SUPERBUTTON_PAIR_EVT, 
								NULL, 
								TIMER_NO_LIMIT, 
								TIMER_ONE_EXECUTION | TIMER_EVENT_RESIDENTS, 
								NULL);
		}
		break;

	case SB_OPT_CTRL:
		if(dstNwkAddr != SB_REMOTE_ADDR)
		{
			mFrame.data_len = 1+6;
			mFrame.data = osal_mem_alloc(1+6);
			mFrame.data[0] = SB_OPT_CTRL;
			osal_memcpy(mFrame.data+1, SB_OPT_CTRL_CODE, 6);
			memcpy(mFrame.tail, f_tail, 4);

			if(!SSAFrame_Package(HEAD_DE, &mFrame, &sBuf, &sLen))
			{
				CommonApp_SendTheMessage(dstNwkAddr, sBuf, sLen);
			}
			
			osal_mem_free(mFrame.data);	
		}
		else if(length >= 32)
		{
			mFrame.data_len = 32+1+6;
			mFrame.data = osal_mem_alloc(32+1+6);
			mFrame.data[0] = SB_OPT_REMOTE_CTRL;
			osal_memcpy(mFrame.data+1, data, 32);
			osal_memcpy(mFrame.data+33, SB_OPT_CTRL_CODE, 6);
			memcpy(mFrame.tail, f_tail, 4);

			if(!SSAFrame_Package(HEAD_DE, &mFrame, &sBuf, &sLen))
			{
				CommonApp_SendTheMessage(COORDINATOR_ADDR, sBuf, sLen);
			}
			
			osal_mem_free(mFrame.data);	
		}
		break;
	}
	
	return 0;
}

void SuperButton_PairCB( void *params, uint16 *duration, uint8 *count)
{
	if(isAllowPair)
	{
		isAllowPair = 0;
		HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
	}
}

void SuperButton_KeyHandler(void)
{
	FastCtrlBindSrc_t *p_BindSrc = bindList;
	while( p_BindSrc )
	{
		if( osal_memcmp(p_BindSrc->coordMac, coordAddr , Z_EXTADDR_LEN) 
			&& p_BindSrc->dst.addrMode == Addr16Bit )
		{
			SuperButton_SendBindCmd(p_BindSrc->dst.addr.shortAddr, SB_OPT_CTRL, NULL, 0);
		}
		else if( osal_memcmp(p_BindSrc->coordMac, coordAddr , Z_EXTADDR_LEN) 
			&& p_BindSrc->dst.addrMode == Addr64Bit )
		{
			uint8 sData[32] = {0};
			incode_xtocs(sData, p_BindSrc->coordMac, Z_EXTADDR_LEN);
			incode_xtocs(sData+16, p_BindSrc->dst.addr.extAddr, Z_EXTADDR_LEN);

			SuperButton_SendBindCmd(SB_REMOTE_ADDR, SB_OPT_CTRL, sData, 32);
		}
		
		p_BindSrc = p_BindSrc->next;
	}
}


void SuperButton_LongKeyCountsSettingHandler(uint8 keyCounts)
{	
	switch(keyCounts)	//Warning, don't conflict with factory set operation's counts
	{
	case 3:	//pair
		SuperButton_SendBindCmd(COORDINATOR_ADDR, SB_OPT_PAIR, NULL, 0);
		break;

	case 5:	//reset
	{
		FastCtrlBindSrc_t *t_BindSrc = bindList;
		FastCtrlBindSrc_t *pre_BindSrc;

		if(t_BindSrc == NULL)
		{
			break;
		}
		
		while(t_BindSrc->next != NULL)
		{
			pre_BindSrc = t_BindSrc;
			t_BindSrc = t_BindSrc->next;
			osal_mem_free(pre_BindSrc);
		}
		osal_mem_free(t_BindSrc);
		bindList = NULL;
		bindLen = 0;

		if (ZSUCCESS == osal_nv_item_init( ZCD_NV_SUPERBUTTON_SIZE,
										sizeof(bindLen),  
										&bindLen))
		{
			osal_nv_write(ZCD_NV_SUPERBUTTON_SIZE, 
		  					0, 
		  					sizeof(bindLen), 
		  					&bindLen);
		}
	}
		break;

	default: 
		break;
	}
}

int8 set_device_data(uint8 const *data, uint8 dataLen)
{
	if(dataLen > SB_DATA_MAXSIZE)
	{
		return -1;
	}
	
	if(dataLen > 0)
	{	
		switch(data[0])
		{
		case SB_OPT_CFG:
			if( dataLen > 16 )
			{
				incode_ctoxs(coordAddr, (uint8 *)(data+1), 16);
			}
			break;

		case SB_OPT_PAIRREG:
			if(isAllowPair && dataLen > 20)
			{
				ZLongAddr_t bindSrcMac;
				FastCtrlBindSrc_t *m_BindSrc = osal_mem_alloc(sizeof(FastCtrlBindSrc_t));

				incode_ctoxs(bindSrcMac, (uint8 *)(data+1), 16);
				
				osal_memcpy(m_BindSrc->coordMac, coordAddr, Z_EXTADDR_LEN);
				m_BindSrc->dst.addrMode = Addr16Bit;
				incode_ctox16(&m_BindSrc->dst.addr.shortAddr, (uint8 *)(data+17));
				m_BindSrc->next = NULL;

				write_nv_bindsrc(bindSrcMac);
				
				if(bindList == NULL)
				{
					bindList = m_BindSrc;
				}
				else
				{
					FastCtrlBindSrc_t *t_BindSrc = bindList;
					while(t_BindSrc->next != NULL)
					{
						t_BindSrc = t_BindSrc->next;
					}
					
					t_BindSrc->next = m_BindSrc;
				}
				
				bindLen++;
				HalLedBlink(HAL_LED_2, 2, 50, 100);

				update_user_event(CommonApp_TaskID, 
								SUPERBUTTON_PAIR_EVT, 
								SuperButton_PairCB, 
								TIMER_NO_LIMIT, 
								TIMER_ONE_EXECUTION | TIMER_EVENT_RESIDENTS, 
								NULL);
			}
			break;
			
		case SB_OPT_MATCH:
			if( dataLen > 16 )
			{
				FastCtrlBindSrc_t *m_BindSrc = osal_mem_alloc(sizeof(FastCtrlBindSrc_t));
				incode_ctoxs(m_BindSrc->coordMac, (uint8 *)(data+1), 16);
				if( osal_memcmp(m_BindSrc->coordMac, data, Z_EXTADDR_LEN) 
					&& dataLen > 36 )
				{
					ZLongAddr_t bindSrcMac;
					incode_ctoxs(bindSrcMac, (uint8 *)(data+17), 16);
					write_nv_bindsrc(bindSrcMac);
					
					m_BindSrc->dst.addrMode = Addr16Bit;
					incode_ctox16(&m_BindSrc->dst.addr.shortAddr, (uint8 *)(data+33));
					m_BindSrc->next = NULL;
				}
				else if( !osal_memcmp(m_BindSrc->coordMac, coordAddr, Z_EXTADDR_LEN) 
					&& dataLen > 32 )
				{
					m_BindSrc->dst.addrMode = Addr64Bit;
					incode_ctoxs(m_BindSrc->dst.addr.extAddr, (uint8 *)(data+17), 16);
					m_BindSrc->next = NULL;

					write_nv_bindsrc(m_BindSrc->dst.addr.extAddr);
				}
				else
				{
					osal_mem_free(m_BindSrc);
					m_BindSrc = NULL;
				}

				if(m_BindSrc != NULL)
				{
					if(bindList == NULL)
					{
						bindList = m_BindSrc;
					}
					else
					{
						FastCtrlBindSrc_t *t_BindSrc = bindList;
						while(t_BindSrc->next != NULL)
						{
							t_BindSrc = t_BindSrc->next;
						}
						
						t_BindSrc->next = m_BindSrc;
					}
					
					bindLen++;
				}
			}
			break;
		}
	}
	
	return 0;
}

void write_nv_bindsrc(ZLongAddr_t bindSrc)
{
	uint16 size;
	if (ZSUCCESS == osal_nv_item_init( ZCD_NV_SUPERBUTTON_SIZE,
											sizeof(size),  
											&size))
  	{
      	osal_nv_read(ZCD_NV_SUPERBUTTON_SIZE, 
	  					0, 
	  					sizeof(size),  
	  					&size);

		uint16 dataLen = Z_EXTADDR_LEN*(size+1);
		ZLongAddr_t *data = osal_mem_alloc(dataLen);
		if (ZSUCCESS == osal_nv_item_init( ZCD_NV_SUPERBUTTON_DATA,
												dataLen,  
												data))
		{
			osal_nv_read(ZCD_NV_SUPERBUTTON_DATA, 
	  						0, 
	  						dataLen,  
	  						data);

			uint16 i = 0;
			uint8 isMatch = 0;
			while(i<size)
			{
				if(osal_memcmp(bindSrc, data[i], Z_EXTADDR_LEN))
				{
					isMatch = 1;
					break;
				}
				
				i++;
			}

			if(!isMatch)
			{
				osal_memcpy(&data[size], bindSrc, Z_EXTADDR_LEN);
				osal_nv_write(ZCD_NV_SUPERBUTTON_DATA, 
	  							0, 
	  							dataLen,  
	  							data);

				size++;
				osal_nv_write(ZCD_NV_SUPERBUTTON_SIZE, 
	  							0, 
	  							sizeof(size),  
								&size);
			}
		}
  	}
}

int8 read_nv_bindsrc(ZLongAddr_t *bindSrc, uint16 bindSize)
{
	if (ZSUCCESS == osal_nv_item_init( ZCD_NV_SUPERBUTTON_DATA,
												sizeof(ZLongAddr_t)*bindSize,  
												bindSrc))
	{
		if(ZSUCCESS == osal_nv_read(ZCD_NV_SUPERBUTTON_DATA, 
	  						0, 
	  						sizeof(ZLongAddr_t)*bindSize,  
	  						bindSrc))
		{
			return 0;
		}
	}

	return -1;
}

int8 get_device_data(uint8 *data, uint8 *dataLen)
{
	*dataLen = 0;
	return 0;
}

