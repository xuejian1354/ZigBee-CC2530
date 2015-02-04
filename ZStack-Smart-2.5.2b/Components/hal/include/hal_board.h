#define ACTIVE_LOW        !
#define ACTIVE_HIGH       !!    /* double negation forces result to be '1' */


#define HAL_KEY_MAP_GPIO		/* �Զ���ӳ�䰴��IO  */
#define HAL_KEY_INT_METHOD		/*����������ʽΪ�ж�(Ĭ��Ϊ��ѯ)*/
#define HAL_KEY_COMBINE_INT_METHOD	/*�ж��� ������Ϲ��� */
#define HAL_KEY_MATCH_ID	/*����ƥ��ID */
#define HAL_UART_DMA_DISABLE_RTCT	/*����RT , CT ������Ч*/

#define HOLD_INIT_AUTHENTICATION	/* �Զ����ʼ����֤��ʽ */

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
