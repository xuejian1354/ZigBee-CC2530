/**************************************************************************************************
  Filename:       HeartBeart.c
  Revised:        $Date: 2014-12-01 11:25:17 -0800 (Thu, 29 dec 2014) $
  Revision:       $Revision: 29218 $

  Description:    This file contains heart beat callbacks
**************************************************************************************************/

/**************************************************************************************************
Create by Max_Chen
Date:2014-12-01
**************************************************************************************************/

/**************************************************************************************************
Modify by Max_Chen
Date:2014-12-01
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "HeartBeat.h"
#include "CommonApp.h"

#include "frame-analysis.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */
extern uint8 SHORT_ADDR_G[4];
extern const uint8 f_tail[4]; 
 
/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      CommonApp_HeartBeatCB
 *
 * @brief   handler heart beat event
 *
 * @param   none
 *
 * @return  none
 */

void CommonApp_HeartBeatCB( void *params, uint16 *duration, uint8 *count)
{
	UH_t mFrame;
	uint8 *fBuf;		//pointer data buffer
	uint16 fLen;		//buffer data length

	memcpy(mFrame.head, FR_HEAD_UH, 3);
	memcpy(mFrame.short_addr, SHORT_ADDR_G, 4);
	memcpy(mFrame.tail, f_tail, 4);

	if(!SSAFrame_Package(HEAD_UH, &mFrame, &fBuf, &fLen))
	{
		CommonApp_SendTheMessage(0x0000, fBuf, fLen);
	}
}
