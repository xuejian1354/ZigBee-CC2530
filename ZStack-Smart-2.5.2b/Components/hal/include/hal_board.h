#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!    /* double negation forces result to be '1' */


#define HAL_KEY_MAP_GPIO		/* 自定义映射按键IO  */
#define HAL_KEY_INT_METHOD		/*按键触发方式为中断(默认为查询)*/
#define HAL_KEY_COMBINE_INT_METHOD	/*中断下 按键组合功能 */
#define HAL_UART_DMA_DISABLE_RTCT	/*串口RT , CT 引脚无效*/

#define HOLD_INIT_AUTHENTICATION	/* 自定义初始化认证方式 */

#include "Connector_Board_cfg.h"

#include "Alarm_Board_cfg.h"
#include "DoorSensor_Board_cfg.h"
#include "IRDetection_Board_cfg.h"
#include "IRRelay_Board_cfg.h"
#include "LightSwitchOne_Board_cfg.h"
#include "LightSwitchTwo_Board_cfg.h"
#include "LightSwitchThree_Board_cfg.h"
#include "LightSwitchFour_Board_cfg.h"


#include "hal_board_cfg.h"
