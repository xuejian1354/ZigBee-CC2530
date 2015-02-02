/**************************************************************************************************
  Filename:       HeartBeart.h
  Revised:        $Date: 2014-12-01 11:20:20 -0800 (Thu, 29 dec 2014) $
  Revision:       $Revision: 29218 $

  Description:    This file contains heart beat callbacks
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-12-01
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2014-12-01
**************************************************************************************************/

#ifndef HEART_BEAT_H
#define HEART_BEAT_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"


/*********************************************************************
 * CONSTANTS
 */


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */
 


/*********************************************************************
 * FUNCTIONS
 */
extern void CommonApp_HeartBeatCB( void *params, uint16 *duration, uint8 *count);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* HEART_BEAT_H */