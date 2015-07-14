/**************************************************************************************************
  Filename:       SolenoidValve_Board_cfg.h
  Revised:        $Date: 2015-07-10 08:46:22 -0800 (Fri, 10 Jul 2015) $
  Revision:       $Revision: 29218 $

  Description:    Solenoid Valve Board Configuration.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-07-14
**************************************************************************************************/


#ifndef SOLENOIDVALVE_BOARD_CFG_H
#define SOLENOIDVALVE_BOARD_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "frame-analysis.h"


#if (DEVICE_TYPE_ID==0xA2)

#define HAL_GPIO_FEATURE

#define KEY_PUSH_PORT_0_BUTTON		/* �Զ���ӳ�䰴��IO  */
#define HAL_KEY_INT_METHOD		/*����������ʽΪ�ж�(Ĭ��Ϊ��ѯ)*/

#define HAL_KEY_COMBINE_INT_METHOD	/*�ж��� ������Ϲ��� */
#define HAL_KEY_MATCH_ID	/*����ƥ��ID */
//#define HAL_KEY_LONG_SHORT_DISTINGUISH	/*�������̰�ʶ����*/
#define POWERON_FACTORY_SETTING		/*��Դ���ؼƴλָ���������*/

//#define TRANSCONN_BOARD_GATEWAY		/*�弶����Ӧ��ת���㹦��*/

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


/* Output Logic Control */
/* VALVE  Switch*/
#define VSW_BV           BV(6)
#define VSW_SBIT         P1_6
#define VSW_DDR          P1DIR
#define VSW_POLARITY     ACTIVE_HIGH

#define HAL_TURN_OFF_VSW()       st( VSW_SBIT = VSW_POLARITY (0); )
#define HAL_TURN_ON_VSW()        st( VSW_SBIT = VSW_POLARITY (1); )
#define HAL_TOGGLE_VSW()         st( VSW_SBIT ^= 1; )
#define HAL_STATE_VSW()          (VSW_POLARITY (VSW_SBIT))

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

#endif /* SOLENOIDVALVE_BOARD_CFG_H */
