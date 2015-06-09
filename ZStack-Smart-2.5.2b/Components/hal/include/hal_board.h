#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!    /* double negation forces result to be '1' */

#define HAL_UART_DMA_DISABLE_RTCT	/*串口RT , CT 引脚无效*/
#define HOLD_INIT_AUTHENTICATION	/* 自定义初始化认证方式 */

#include "Connector_Board_cfg.h"
#include "DemoBase_Board_cfg.h"

#include "AirController_Board_cfg.h"
#include "LightSwitchOne_Board_cfg.h"
#include "LightSwitchTwo_Board_cfg.h"
#include "LightSwitchThree_Board_cfg.h"
#include "LightSwitchFour_Board_cfg.h"
#include "Alarm_Board_cfg.h"
#include "IRDetection_Board_cfg.h"
#include "DoorSensor_Board_cfg.h"
#include "EnvDetection_Board_cfg.h"
#include "DustSensor_Board_cfg.h"
#include "IRRelay_Board_cfg.h"



#include "hal_board_cfg.h"
