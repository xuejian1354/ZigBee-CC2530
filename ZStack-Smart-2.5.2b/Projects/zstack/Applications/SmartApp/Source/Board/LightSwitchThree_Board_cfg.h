/**************************************************************************************************
  Filename:       LightSwitchThree_Board_cfg.h
  Revised:        $Date: 2015-02-02 19:37:18 -0800 (Tue, 02 Feb 2015) $
  Revision:       $Revision: 29217 $

  Description:    Light Switch Three Board Configuration.
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2015-02-02
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-02-04
**************************************************************************************************/


#ifndef LIGHTSWITCHTHREE_BOARD_CFG_H
#define LIGHTSWITCHTHREE_BOARD_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "frame-analysis.h"


#if (DEVICE_TYPE_ID==3)

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
#define LED1_BV           BV(0)
#define LED1_SBIT         P1_0
#define LED1_DDR          P1DIR
#define LED1_POLARITY     ACTIVE_LOW

/* 2 - Red */
#define LED2_BV           BV(0)
#define LED2_SBIT         P1_0
#define LED2_DDR          P1DIR
#define LED2_POLARITY     ACTIVE_LOW

/* 3 - Yellow */
#define LED3_BV           BV(0)
#define LED3_SBIT         P1_0
#define LED3_DDR          P1DIR
#define LED3_POLARITY     ACTIVE_LOW


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

#define KEY_PUSH_PORT_1_BUTTON

#define KEY_S1_PORT_BIT		BV(2)
#define KEY_S2_PORT_BIT		BV(3)
#define KEY_S3_PORT_BIT		BV(4)

#define PUSH_PORT_1_POLARITY    ~
#define HAL_KEY_PORT_1_BITS (KEY_S1_PORT_BIT | KEY_S2_PORT_BIT | KEY_S3_PORT_BIT)


/* Output Logic Control */
/* OLC 1*/
#define OLC1_BV           BV(5)
#define OLC1_SBIT         P1_5
#define OLC1_DDR          P1DIR
#define OLC1_POLARITY     ACTIVE_LOW

/* OLC 2*/
#define OLC2_BV           BV(6)
#define OLC2_SBIT         P1_6
#define OLC2_DDR          P1DIR
#define OLC2_POLARITY     ACTIVE_LOW

/* OLC 3*/
#define OLC3_BV           BV(7)
#define OLC3_SBIT         P1_7
#define OLC3_DDR          P1DIR
#define OLC3_POLARITY     ACTIVE_LOW


#define HAL_TURN_OFF_OLC1()       st( OLC1_SBIT = OLC1_POLARITY (0); )
#define HAL_TURN_ON_OLC1()        st( OLC1_SBIT = OLC1_POLARITY (1); )
#define HAL_TOGGLE_OLC1()         st( OLC1_SBIT ^= 1;)
#define HAL_STATE_OLC1()          (OLC1_POLARITY (OLC1_SBIT))


#define HAL_TURN_OFF_OLC2()       st( OLC2_SBIT = OLC2_POLARITY (0); )
#define HAL_TURN_ON_OLC2()        st( OLC2_SBIT = OLC2_POLARITY (1); )
#define HAL_TOGGLE_OLC2()         st( OLC2_SBIT ^= 1; )
#define HAL_STATE_OLC2()          (OLC2_POLARITY (OLC2_SBIT))

#define HAL_TURN_OFF_OLC3()       st( OLC3_SBIT = OLC3_POLARITY (0); )
#define HAL_TURN_ON_OLC3()        st( OLC3_SBIT = OLC3_POLARITY (1); )
#define HAL_TOGGLE_OLC3()         st( OLC3_SBIT ^= 1; )
#define HAL_STATE_OLC3()          (OLC3_POLARITY (OLC3_SBIT))


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

#endif /* LIGHTSWITCHTHREE_BOARD_CFG_H */