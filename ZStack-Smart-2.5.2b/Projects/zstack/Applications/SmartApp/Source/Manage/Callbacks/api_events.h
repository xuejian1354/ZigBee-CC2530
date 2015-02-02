/**************************************************************************************************
  Filename:       api_events.h
  Revised:        $Date: 2014-04-29 09:01:20 -0800 (Thu, 29 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    This file contains set stack events application interface definitions.
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-04-29
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2014-04-29
**************************************************************************************************/

#ifndef API_EVENTS_H
#define API_EVENTS_H

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
#define EVENT_MAP_ID		//get event address easily from event map
#define USER_EVENT_MAP_SIZE		16	//the map addr size of user event


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */
 
/*User Events*/
//type structure of user event callback
//params  (void *)-user defined  (uint16 *)-duration  (uint8 *)-excute count
typedef void (*ssa_ProcessUserTaskCB_t)( void *, uint16 *, uint8 * );

//type structure of user event
typedef struct ssaUserEvent
{
  struct ssaUserEvent *next;		//point to next structure in list

#ifdef EVENT_MAP_ID
  uint8 map_id;		//mapped id for the array of ssa_ProcessUserTaskCB_t 's address
#endif
  uint16 event;		//event flag for user defined
  uint16 duration;	//excute duration for ther next time
  union 
  {
  	uint8 overall;	
	
	struct
	{
	  uint8 count : 7;
	  uint8 resident : 1;
	} branch;
  } count;		//excute count
  
  void *ptype;	//point to data of user defined
  
  ssa_ProcessUserTaskCB_t ssa_ProcessUserTaskCB;	//handle callbacks of this event
} ssaUserEvent_t;


/*********************************************************************
 * FUNCTIONS
 */
#ifdef EVENT_MAP_ID
extern uint8 event_to_id(uint16 event);
#endif
extern int8 Add_User_Event(ssaUserEvent_t *ssaUserEvent);
extern ssaUserEvent_t *Query_User_Event(uint16 event);
extern int8 Del_User_Event(uint16 event);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* API_EVENTS_H */

