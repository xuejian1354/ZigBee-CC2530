/**************************************************************************************************
  Filename:       DoorSensor_Board_cfg.h
  Revised:        $Date: 2015-02-02 19:28:14 -0800 (Tue, 02 Feb 2015) $
  Revision:       $Revision: 29217 $

  Description:    Door Sensor Board Configuration.
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2015-02-02
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-04-22
**************************************************************************************************/


#ifndef DOORSENSOR_BOARD_CFG_H
#define DOORSENSOR_BOARD_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "frame-analysis.h"


#if (DEVICE_TYPE_ID==13)

#define HAL_GPIO_FEATURE

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */
/* 1 - Green */
#define LED1_BV           BV(6)
#define LED1_SBIT         P0_6
#define LED1_DDR          P0DIR
#define LED1_POLARITY     ACTIVE_LOW

/* 2 - Red */
#define LED2_BV           BV(7)
#define LED2_SBIT         P0_7
#define LED2_DDR          P0DIR
#define LED2_POLARITY     ACTIVE_LOW

/* 3 - Yellow */
#define LED3_BV           LED1_BV
#define LED3_SBIT         LED1_SBIT
#define LED3_DDR          LED1_DDR
#define LED3_POLARITY     ACTIVE_HIGH

/* S1 */
#define PUSH1_BV          BV(5)
#define PUSH1_SBIT        P0_5
#define PUSH1_POLARITY    ACTIVE_LOW


#define HAL_PUSH_BUTTON1()        (PUSH1_POLARITY (PUSH1_SBIT))
#define HAL_PUSH_BUTTON2()        (0)
#define HAL_PUSH_BUTTON3()        (0)
#define HAL_PUSH_BUTTON4()        (0)
#define HAL_PUSH_BUTTON5()        (0)
#define HAL_PUSH_BUTTON6()        (0)


#define HAL_TURN_OFF_LED1()       st( LED1_SBIT = LED1_POLARITY (0); )
#define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )
#define HAL_TOGGLE_LED1()         st( if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} )
#define HAL_STATE_LED1()          (LED1_POLARITY (LED1_SBIT))


#define HAL_TURN_OFF_LED2()       st( LED2_SBIT = LED2_POLARITY (0); )
#define HAL_TURN_ON_LED2()        st( LED2_SBIT = LED2_POLARITY (1); )
#define HAL_TOGGLE_LED2()         st( if (LED2_SBIT) { LED2_SBIT = 0; } else { LED2_SBIT = 1;} )
#define HAL_STATE_LED2()          (LED2_POLARITY (LED2_SBIT))

#define HAL_TURN_OFF_LED3()       st( LED3_SBIT = LED3_POLARITY (0); )
#define HAL_TURN_ON_LED3()        st( LED3_SBIT = LED3_POLARITY (1); )
#define HAL_TOGGLE_LED3()         st( if (LED3_SBIT) { LED3_SBIT = 0; } else { LED3_SBIT = 1;} )
#define HAL_STATE_LED3()          (LED3_POLARITY (LED3_SBIT))

#define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()
#define HAL_TURN_ON_LED4()        HAL_TURN_ON_LED1()
#define HAL_TOGGLE_LED4()         HAL_TOGGLE_LED1()
#define HAL_STATE_LED4()          HAL_STATE_LED1()

/* Door Sensor Input Control */
#define DIC_BV           BV(0)
#define DIC_SBIT         P1_0
#define DIC_DDR          P1DIR
#define DIC_POLARITY     ACTIVE_HIGH

#define HAL_TURN_OFF_DIC()       st( DIC_SBIT = DIC_POLARITY (0); )
#define HAL_TURN_ON_DIC()        st( DIC_SBIT = DIC_POLARITY (1); )
#define HAL_TOGGLE_DIC()         st( DIC_SBIT ^= 1;)
#define HAL_STATE_DIC()          (DIC_POLARITY (DIC_SBIT))

#define HAL_LED TRUE
#define BLINK_LEDS

#define HAL_KEY TRUE

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
*********************************************************************/
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOORSENSOR_BOARD_CFG_H */
