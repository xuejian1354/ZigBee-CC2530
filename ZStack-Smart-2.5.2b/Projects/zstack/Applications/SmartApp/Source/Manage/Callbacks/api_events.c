/**************************************************************************************************
  Filename:       api_events.c
  Revised:        $Date: 2014-04-29 09:03:06 -0800 (Thu, 29 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    Set Events Application Interface
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-04-29
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2014-04-29
**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include "api_events.h"

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
 
/*********************************************************************
 * GLOBAL VARIABLES
 */
//chain list header
ssaUserEvent_t *puser_event = NULL;

/*********************************************************************
 * LOCAL VARIABLES
 */
//event map
#ifdef EVENT_MAP_ID
static ssaUserEvent_t *userevent_map[USER_EVENT_MAP_SIZE] = {NULL};
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

#ifdef EVENT_MAP_ID
/*********************************************************************
 * @fn      event_to_id
 *
 * @brief   get map id from user event flag
 *
 * @param   user process event
 *
 * @return  user map id
 */
uint8 event_to_id(uint16 event)
{
  uint8 i = 0;
  
  for(i=0; i<16; i++)
  	if(event & 1<<i) break;
	
  return i;
}
#endif
 

/*********************************************************************
 * @fn      Add_User_Event
 *
 * @brief   add user event to chain list
 *
 * @param   user process event
 *
 * @return  result(susses or failed)
 */
int8 Add_User_Event(ssaUserEvent_t *ssaUserEvent)
{
  if(ssaUserEvent != NULL)
  {
  	ssaUserEvent->next = NULL;
#ifdef EVENT_MAP_ID
	ssaUserEvent->map_id = event_to_id(ssaUserEvent->event);
#endif
  }
 
  if(puser_event == NULL)
  {
	puser_event = ssaUserEvent;
#ifdef EVENT_MAP_ID
	if(ssaUserEvent->map_id < USER_EVENT_MAP_SIZE)
	{
	  userevent_map[ssaUserEvent->map_id] = ssaUserEvent;
	}
#endif
  }
  else
  {
	ssaUserEvent_t *pre_event = NULL;
	ssaUserEvent_t *t_event = puser_event;
	if(ssaUserEvent->event == t_event->event)
	{
	  return -1;
	}
	else if(ssaUserEvent->duration < t_event->duration)
	{
	  ssaUserEvent->next = t_event;
	  puser_event = ssaUserEvent;
#ifdef EVENT_MAP_ID
	  if(ssaUserEvent->map_id < USER_EVENT_MAP_SIZE)
	  {
	    userevent_map[ssaUserEvent->map_id] = ssaUserEvent;
	  }
#endif
	  return 0;
	}

	while(t_event->next != NULL)
	{
	  pre_event = t_event;
	  t_event = t_event->next;
	  if(ssaUserEvent->event == t_event->event)
	  {
		return -1;
	  }
	  else if(ssaUserEvent->duration < t_event->duration)
	  {
		ssaUserEvent->next = t_event;
		pre_event->next = ssaUserEvent;
#ifdef EVENT_MAP_ID
		if(ssaUserEvent->map_id < USER_EVENT_MAP_SIZE)
		{
		  userevent_map[ssaUserEvent->map_id] = ssaUserEvent;
		}
#endif

		return 0;
	  }
	}
	t_event->next = ssaUserEvent;
  }

  return 0;
}


/*********************************************************************
 * @fn      Query_User_Event
 *
 * @brief   query user event from chain list
 *
 * @param   event flag
 *
 * @return  result(event obj or NULL)
 */
ssaUserEvent_t *Query_User_Event(uint16 event)
{
  ssaUserEvent_t *t_event;
#ifdef EVENT_MAP_ID
  uint8 map_id = event_to_id(event);
  if(map_id<USER_EVENT_MAP_SIZE && userevent_map[map_id]!=NULL)
  	return userevent_map[map_id];
#endif

  t_event = puser_event;
  while(t_event->next!=NULL && t_event->event!=event)
  	t_event = t_event->next;

  if(t_event->event == event)
  	return t_event;
  else
  	return NULL;
}


/*********************************************************************
 * @fn      Del_User_Event
 *
 * @brief   delete user event from chain list
 *
 * @param   event flag
 *
 * @return  result(susses or failed)
 */
int8 Del_User_Event(uint16 event)
{
  ssaUserEvent_t *pre_event = NULL;
  ssaUserEvent_t *t_event = puser_event;
  if(t_event == NULL)
  	return -1;

  while(t_event->next!=NULL && t_event->event != event)
  {
	pre_event = t_event;
	t_event = t_event->next;
  }

  if(t_event->event == event)
  {
	if(pre_event == NULL)
		puser_event = t_event->next;
	else
		pre_event->next = t_event->next;

#ifdef EVENT_MAP_ID
	if(t_event->map_id<USER_EVENT_MAP_SIZE)
		userevent_map[t_event->map_id] = NULL;
#endif
	
	osal_mem_free(t_event);
	return 0;
  }
  else
  	return -2;
  
}


