//#define POWER_AMPLIFIER_POSITION_1	/*映射在功放位置1(default)*/
//#define POWER_AMPLIFIER_POSITION_2	/*映射在功放位置1(custom)*/
//#define HAL_MT7620_LED_MAP		/*MT7620上LED映射位置*/
#define HAL_KEY_MAP_GPIO5		/* 映射按键IO 为P0.5 */
#define HAL_KEY_INT_METHOD		/*按键触发方式为中断(默认为查询)*/
#define HAL_KEY_COMBINE_INT_METHOD	/*中断下 按键组合功能 */
#define HAL_UART_DMA_DISABLE_RTCT	/*串口RT , CT 引脚无效*/

#define HOLD_INIT_AUTHENTICATION	/* 自定义初始化认证方式 */


#include "hal_board_cfg.h"

#define RETRANSCOUNT	2