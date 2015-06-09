/**************************************************************************************************
  Filename:       AirController_Board_cfg.h
  Revised:        $Date: 2015-02-02 21:06:12 -0800 (Tue, 02 Feb 2015) $
  Revision:       $Revision: 29217 $

  Description:    AirController Board Configuration.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-06-09
**************************************************************************************************/


#ifndef AIRCONTROLLER_BOARD_CFG_H
#define AIRCONTROLLER_BOARD_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "frame-analysis.h"


#if (DEVICE_TYPE_ID==0)

#define HAL_GPIO_FEATURE
#define HAL_MT7620_GPIO_MAP		/*MT7620上GPIO 映射位置*/

#define KEY_PUSH_PORT_0_BUTTON		/* 自定义映射按键IO  */
#define HAL_KEY_INT_METHOD		/*按键触发方式为中断(默认为查询)*/

#define HAL_KEY_COMBINE_INT_METHOD	/*中断下 按键组合功能 */
#define HAL_KEY_MATCH_ID	/*按键匹配ID */
//#define HAL_KEY_LONG_SHORT_DISTINGUISH	/*按键长短按识别功能*/


/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */
#ifdef HAL_MT7620_GPIO_MAP
#define LED1_BV           BV(3)
#define LED1_SBIT         P1_3
#define LED1_DDR          P1DIR
#define LED1_POLARITY     ACTIVE_HIGH

#define LED2_BV           BV(0)
#define LED2_SBIT         P1_0
#define LED2_DDR          P1DIR
#define LED2_POLARITY     ACTIVE_LOW

#define LED3_BV           LED1_BV
#define LED3_SBIT         LED1_SBIT
#define LED3_DDR          LED1_DDR
#define LED3_POLARITY     LED1_POLARITY

/* Unused */
#define PUSH1_BV          BV(5)
#define PUSH1_SBIT        P0_5
#define PUSH1_POLARITY    ACTIVE_LOW

/*Real Key*/
#define KEY_PUSH_PORT_1_BUTTON

#define KEY_S1_PORT_BIT		BV(2)

#define PUSH_PORT_1_POLARITY    ~
#define HAL_KEY_PORT_1_BITS KEY_S1_PORT_BIT


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

#define HAL_TURN_OFF_LED3()       HAL_TURN_OFF_LED1()
#define HAL_TURN_ON_LED3()        HAL_TURN_ON_LED1()
#define HAL_TOGGLE_LED3()         HAL_TOGGLE_LED1()
#define HAL_STATE_LED3()          HAL_STATE_LED1()

#define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()
#define HAL_TURN_ON_LED4()        HAL_TURN_ON_LED1()
#define HAL_TOGGLE_LED4()         HAL_TOGGLE_LED1()
#define HAL_STATE_LED4()          HAL_STATE_LED1()


#define HAL_LED TRUE
#define BLINK_LEDS

#define HAL_KEY TRUE

#else
/* 1 - Green */
#define LED1_BV           BV(0)
#define LED1_SBIT         P1_0
#define LED1_DDR          P1DIR
#define LED1_POLARITY     ACTIVE_HIGH

/* 2 - Red */
#define LED2_BV           BV(1)
#define LED2_SBIT         P1_1
#define LED2_DDR          P1DIR
#define LED2_POLARITY     ACTIVE_HIGH

/* 3 - Yellow */
#define LED3_BV           BV(4)
#define LED3_SBIT         P1_4
#define LED3_DDR          P1DIR
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


#define HAL_LED TRUE
#define BLINK_LEDS

#define HAL_KEY TRUE
#endif

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

#endif /* AIRCONTROLLER_BOARD_CFG_H */
