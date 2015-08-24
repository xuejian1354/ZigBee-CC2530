/**************************************************************************************************
  Filename:       HueLight_Board_cfg.h
  Revised:        $Date: 2015-08-21 10:27:33 -0800 (Fir, 21 Aug 2015) $
  Revision:       $Revision: 29217 $

  Description:    Hue Light Board Configuration.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-08-24
**************************************************************************************************/


#ifndef HUELIGHT_BOARD_CFG_H
#define HUELIGHT_BOARD_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#if (DEVICE_TYPE_ID==5)

#define HAL_GPIO_FEATURE

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

#if defined (PWM_ALT2)

#define GREEN_LED HAL_T1_CH2
#define RED_LED   HAL_T1_CH1
#define BLUE_LED  HAL_T1_CH4 //Not connected on SmartRF05
#define WHITE_LED HAL_T1_CH4
#define ENABLE_LAMP   P1SEL |= ( 0x1 | 0x2 );

#define DISABLE_LAMP  P1SEL &= ~( 0x1 | 0x2 );\
                      P1    &= ~( 0x1 | 0x2 ); /* P0.3:6 */
#else
#define GREEN_LED HAL_T1_CH3
#define RED_LED   HAL_T1_CH1
#define BLUE_LED  HAL_T1_CH2
#define WHITE_LED HAL_T1_CH4
#define ENABLE_LAMP   P0SEL |= ( 0x08 | 0x10 | 0x20 | 0x40); /* P0.3:6 */\

#define DISABLE_LAMP  P0SEL &= ~( 0x08 | 0x10 | 0x20 | 0x40); /* P0.3:6 */\
                      P0    &= ~( 0x08 | 0x10 | 0x20 | 0x40); /* P0.3:6 */
#endif

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

#endif /* HUELIGHT_BOARD_CFG_H */
