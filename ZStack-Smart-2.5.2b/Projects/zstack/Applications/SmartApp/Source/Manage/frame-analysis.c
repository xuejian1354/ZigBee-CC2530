
/**************************************************************************************************
  Filename:       frame-analysis.c
  Revised:        $Date: 2014-04-25 17:16:57 -0800 (Wed, 25 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    Analysis frame format.
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-04-25
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-02-02
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "frame-analysis.h"
#include "serial-comm.h"

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
static UC_t ucFrame;
static UO_t uoFrame;
static UH_t uhFrame;
static UR_t urFrame;
static DE_t deFrame;

static UC_t *p_ucFrame;
static UO_t *p_uoFrame;
static UH_t *p_uhFrame;
static UR_t *p_urFrame;
static DE_t *p_deFrame;


static uint8 pFrameBuffer[FRAME_BUFFER_SIZE] = {0};
static uint8 pFrameLen = 0;

static uint8 aDataBuffer[FRAME_DATA_SIZE] = {0};
static uint8 aDataLen = 0;

const uint8 f_tail[4] = {0x3A, 0x4F, 0x0D, 0x0A}; 

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * FUNCTION DECLARATION
 */

/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SSAFrame_Analysis()
 *
 * @brief   analysis frame
 *
 * @param none
 *
 * @return  none
 */
void *SSAFrame_Analysis(frHeadType_t hType, uint8 SrcBuf[], uint8 SrcLen)
{ 
 	switch(hType)
	{
	case HEAD_UC: 
		if(SrcLen>=FR_UC_DATA_FIX_LEN && !memcmp(SrcBuf, FR_HEAD_UC, 3)
			&& SrcBuf[3] == FR_DEV_COORD && !memcmp(SrcBuf+34, f_tail, 4))
		{
			memcpy(ucFrame.head, SrcBuf, 3);
			ucFrame.type = SrcBuf[3];
			memcpy(ucFrame.ed_type, SrcBuf+4, 2);
			memcpy(ucFrame.short_addr, SrcBuf+6, 4);
			memcpy(ucFrame.ext_addr, SrcBuf+10, 16);
			memcpy(ucFrame.panid, SrcBuf+26, 4);
			memcpy(ucFrame.channel, SrcBuf+30, 4);
			memcpy(ucFrame.tail, SrcBuf+34, 4);

			return (void *)&ucFrame;
		}
		else { goto  FR_Analysis_err;}
		
	case HEAD_UO: 
		if(SrcLen>=FR_UO_DATA_FIX_LEN && !memcmp(SrcBuf, FR_HEAD_UO, 3)
			&& !memcmp(SrcBuf+SrcLen-4, f_tail, 4))
		{
			memcpy(uoFrame.head, SrcBuf, 3);
			uoFrame.type = SrcBuf[3];
			memcpy(uoFrame.ed_type, SrcBuf+4, 2);
			memcpy(uoFrame.short_addr, SrcBuf+6, 4);
			memcpy(uoFrame.ext_addr, SrcBuf+10, 16);
			
			memset(aDataBuffer, 0, sizeof(aDataBuffer));
			memcpy(aDataBuffer, SrcBuf+26, SrcLen-FR_UO_DATA_FIX_LEN);
			aDataLen = SrcLen-FR_UO_DATA_FIX_LEN;
			uoFrame.data_len = aDataLen;
			uoFrame.data = aDataBuffer;
			
			memcpy(uoFrame.tail, SrcBuf+SrcLen-4, 4);

			return (void *)&uoFrame;
		}
		else { goto  FR_Analysis_err;}
		
	case HEAD_UH: 
		if(SrcLen>=FR_UH_DATA_FIX_LEN && !memcmp(SrcBuf, FR_HEAD_UH, 3)
			&& !memcmp(SrcBuf+7, f_tail, 4))
		{
			memcpy(uhFrame.head, SrcBuf, 3);
			memcpy(uhFrame.short_addr, SrcBuf+3, 4);
			memcpy(uhFrame.tail, SrcBuf+7, 4);;

			return (void *)&uhFrame;
		}
		else { goto  FR_Analysis_err;}
		
	case HEAD_UR: 
		if(SrcLen>=FR_HR_DATA_FIX_LEN && !memcmp(SrcBuf, FR_HEAD_UR, 3)
			&& !memcmp(SrcBuf+SrcLen-4, f_tail, 4))
		{
			memcpy(urFrame.head, SrcBuf, 3);
			urFrame.type = SrcBuf[3];
			memcpy(urFrame.ed_type, SrcBuf+4, 2);
			memcpy(urFrame.short_addr, SrcBuf+6, 4);
			
			memset(aDataBuffer, 0, sizeof(aDataBuffer));
			memcpy(aDataBuffer, SrcBuf+10, SrcLen-FR_HR_DATA_FIX_LEN);
			aDataLen = SrcLen-FR_HR_DATA_FIX_LEN;
			urFrame.data_len = aDataLen;
			urFrame.data = aDataBuffer;
			
			memcpy(urFrame.tail, SrcBuf+SrcLen-4, 4);

			return (void *)&urFrame;
		}
		else { goto  FR_Analysis_err;}
	
	case HEAD_DE: 
		if(SrcLen>=FR_DE_DATA_FIX_LEN && !memcmp(SrcBuf, FR_HEAD_DE, 2)
			&& !memcmp(SrcBuf+SrcLen-4, f_tail, 4))
		{
			memcpy(deFrame.head, SrcBuf, 2);
			memcpy(deFrame.cmd, SrcBuf+2, 4);
			memcpy(deFrame.short_addr, SrcBuf+6, 4);
			
			memset(aDataBuffer, 0, sizeof(aDataBuffer));
			memcpy(aDataBuffer, SrcBuf+10, SrcLen-FR_DE_DATA_FIX_LEN);
			aDataLen = SrcLen-FR_DE_DATA_FIX_LEN;
			deFrame.data_len = aDataLen;
			deFrame.data = aDataBuffer;
			
			memcpy(deFrame.tail, SrcBuf+SrcLen-4, 4);

			return (void *)&deFrame;
		}
		else { goto  FR_Analysis_err;}

	default: goto  FR_Analysis_err;
	}

FR_Analysis_err:
	return NULL;
}


/*********************************************************************
 * @fn      SSAFrame_Package()
 *
 * @brief   packet frame
 *
 * @param none
 *
 * @return  none
 */
int8 SSAFrame_Package(frHeadType_t hType, void *data, uint8 **DstBuf, uint16 *DstLen)
{
	if(data == NULL)
		goto  FR_Package_err;
	
	switch(hType)
	{
	case HEAD_UC: 
		p_ucFrame = (UC_t *)data;
		memset(pFrameBuffer, 0, sizeof(pFrameBuffer));
		memcpy(pFrameBuffer, p_ucFrame->head, 3);
		pFrameBuffer[3] = p_ucFrame->type;
		memcpy(pFrameBuffer+4, p_ucFrame->ed_type, 2);
		memcpy(pFrameBuffer+6, p_ucFrame->short_addr, 4);
		memcpy(pFrameBuffer+10, p_ucFrame->ext_addr, 16);
		memcpy(pFrameBuffer+26, p_ucFrame->panid, 4);
		memcpy(pFrameBuffer+30, p_ucFrame->channel, 4);
		memcpy(pFrameBuffer+34, p_ucFrame->tail, 4);

		pFrameLen = FR_UC_DATA_FIX_LEN;

		*DstBuf = pFrameBuffer;
		*DstLen = pFrameLen;
		return 0;
		
	case HEAD_UO: 
		p_uoFrame = (UO_t *)data;
		memset(pFrameBuffer, 0, sizeof(pFrameBuffer));
		memcpy(pFrameBuffer, p_uoFrame->head, 3);
		pFrameBuffer[3] = p_uoFrame->type;
		memcpy(pFrameBuffer+4, p_uoFrame->ed_type, 2);
		memcpy(pFrameBuffer+6, p_uoFrame->short_addr, 4);
		memcpy(pFrameBuffer+10, p_uoFrame->ext_addr, 16);
		memcpy(pFrameBuffer+26, p_uoFrame->data, p_uoFrame->data_len);
		memcpy(pFrameBuffer+26+p_uoFrame->data_len, p_uoFrame->tail, 4);

		pFrameLen = FR_UO_DATA_FIX_LEN+p_uoFrame->data_len;

		*DstBuf = pFrameBuffer;
		*DstLen = pFrameLen;
		return 0;
		
	case HEAD_UH: 
		p_uhFrame = (UH_t *)data;
		memset(pFrameBuffer, 0, sizeof(pFrameBuffer));
		memcpy(pFrameBuffer, p_uhFrame->head, 3);
		memcpy(pFrameBuffer+3, p_uhFrame->short_addr, 4);
		memcpy(pFrameBuffer+7, p_uhFrame->tail, 4);

		pFrameLen = FR_UH_DATA_FIX_LEN;

		*DstBuf = pFrameBuffer;
		*DstLen = pFrameLen;
		return 0;
		
	case HEAD_UR: 
		p_urFrame = (UR_t *)data;
		memset(pFrameBuffer, 0, sizeof(pFrameBuffer));
		memcpy(pFrameBuffer, p_urFrame->head, 3);
		pFrameBuffer[3] = p_urFrame->type;
		memcpy(pFrameBuffer+4, p_urFrame->ed_type, 2);
		memcpy(pFrameBuffer+6, p_urFrame->short_addr, 4);
		memcpy(pFrameBuffer+10, p_urFrame->data, p_urFrame->data_len);
		memcpy(pFrameBuffer+10+p_urFrame->data_len, p_urFrame->tail, 4);

		pFrameLen = FR_HR_DATA_FIX_LEN+p_urFrame->data_len;

		*DstBuf = pFrameBuffer;
		*DstLen = pFrameLen;
		return 0;
	
	case HEAD_DE: 
		p_deFrame = (DE_t *)data;
		memset(pFrameBuffer, 0, sizeof(pFrameBuffer));
		memcpy(pFrameBuffer, p_deFrame->head, 2);
		memcpy(pFrameBuffer+3, p_deFrame->cmd, 4);
		memcpy(pFrameBuffer+6, p_deFrame->short_addr, 4);
		memcpy(pFrameBuffer+10, p_deFrame->data, p_deFrame->data_len);
		memcpy(pFrameBuffer+10+p_deFrame->data_len, p_deFrame->tail, 4);

		pFrameLen = FR_DE_DATA_FIX_LEN+p_deFrame->data_len;

		*DstBuf = pFrameBuffer;
		*DstLen = pFrameLen;
		return 0;

	default: goto  FR_Package_err;
	}

FR_Package_err:
	return -1;
}


