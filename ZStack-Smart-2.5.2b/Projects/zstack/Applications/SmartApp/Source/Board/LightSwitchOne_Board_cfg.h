/**************************************************************************************************
  Filename:       LightSwitchOne_Board_cfg.h
  Revised:        $Date: 2015-02-02 19:35:16 -0800 (Tue, 02 Feb 2015) $
  Revision:       $Revision: 29217 $

  Description:    Light Switch One Board Configuration.
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2015-02-02
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-06-03
**************************************************************************************************/


#ifndef LIGHTSWITCHONE_BOARD_CFG_H
#define LIGHTSWITCHONE_BOARD_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#if (DEVICE_TYPE_ID==1)

#define HAL_GPIO_FEATURE
#define BIND_SUPERBUTTON_CTRL_SUPPORT	/* 绑定紧急按钮控制 */

#define KEY_PUSH_PORT_0_BUTTON		/* 自定义映射按键IO  */
#define HAL_KEY_INT_METHOD		/*按键触发方式为中断(默认为查询)*/

#define HAL_KEY_COMBINE_INT_METHOD	/*中断下 按键组合功能 */
#define HAL_KEY_MATCH_ID	/*按键匹配ID */
#define HAL_KEY_LONG_SHORT_DISTINGUISH	/*按键长短按识别功能*/


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
#define LED3_BV           BV(1)
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


/* S1 unused*/
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

#define KEY_S1_PORT_BIT		BV(3)

#define PUSH_PORT_1_POLARITY    ~
#define HAL_KEY_PORT_1_BITS (KEY_S1_PORT_BIT)

/* Output Logic Control */
/* OLC 1*/
#define OLC1_BV           BV(6)
#define OLC1_SBIT         P1_6
#define OLC1_DDR          P1DIR
#define OLC1_POLARITY     ACTIVE_HIGH

#define HAL_TURN_OFF_OLC1()       st( OLC1_SBIT = OLC1_POLARITY (0); )
#define HAL_TURN_ON_OLC1()        st( OLC1_SBIT = OLC1_POLARITY (1); )
#define HAL_TOGGLE_OLC1()         st( OLC1_SBIT ^= 1; )
#define HAL_STATE_OLC1()          (OLC1_POLARITY (OLC1_SBIT))

#define HAL_LED FALSE
//#define BLINK_LEDS

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

#endif /* LIGHTSWITCHONE_BOARD_CFG_H */
