/**************************************************************************************************
  Filename:       frame-analysis.h
  Revised:        $Date: 2014-04-25 17:14:12 -0800 (Wed, 25 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    Analysis frame format.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-07-09
**************************************************************************************************/


#ifndef FRAME_ANALYSIS_H
#define FRAME_ANALYSIS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ZComDef.h"
#include "OSAL.h"

/*********************************************************************
 * CONSTANTS
 */
//帧头
#define FR_HEAD_UC	"UC:"	//协调器建网数据上传
#define FR_HEAD_UO	"UO:"	//设备入网数据上传
#define FR_HEAD_UH	"UH:"	//设备心跳数据格式
#define FR_HEAD_DE	"D:"	//控制命令
#define FR_HEAD_UR	"UR:"	//控制返回
#define FR_HEAD_UJ	"UJ:"	//控制入网返回


//设备类型
#define FR_DEV_COORD	'0'
#define FR_DEV_ROUTER	'0'
#define FR_DEV_ENDDEV	'1'

//应用类型
#define FR_APP_CONNECTOR		"00"
#define FR_APP_DEMOBASE			"FE"
#define FR_APP_AIRCONTROLLER 		"F0"
#define FR_APP_LIGHTSWITCH_ONE		"01"
#define FR_APP_LIGHTSWITCH_TWO		"02"
#define FR_APP_LIGHTSWITCH_THREE	"03"
#define FR_APP_LIGHTSWITCH_FOUR		"04"
#define FR_APP_ALARM			"11"
#define FR_APP_IR_DETECTION		"12"
#define FR_APP_DOOR_SENSOR		"13"
#define FR_APP_ENVDETECTION		"14"
#define FR_APP_SMOG			"15"
#define FR_APP_IR_RELAY			"21"

#if(DEVICE_TYPE_ID==0)
#define FR_APP_DEV FR_APP_CONNECTOR
#elif(DEVICE_TYPE_ID==0xFE)
#define FR_APP_DEV FR_APP_DEMOBASE
#elif(DEVICE_TYPE_ID==0xF0)
#define FR_APP_DEV FR_APP_AIRCONTROLLER
#elif(DEVICE_TYPE_ID==1)
#define FR_APP_DEV FR_APP_LIGHTSWITCH_ONE
#elif(DEVICE_TYPE_ID==2)
#define FR_APP_DEV FR_APP_LIGHTSWITCH_TWO
#elif(DEVICE_TYPE_ID==3)
#define FR_APP_DEV FR_APP_LIGHTSWITCH_THREE
#elif(DEVICE_TYPE_ID==4)
#define FR_APP_DEV FR_APP_LIGHTSWITCH_FOUR
#elif(DEVICE_TYPE_ID==11)
#define FR_APP_DEV FR_APP_ALARM
#elif(DEVICE_TYPE_ID==12)
#define FR_APP_DEV FR_APP_IR_DETECTION
#elif(DEVICE_TYPE_ID==13)
#define FR_APP_DEV FR_APP_DOOR_SENSOR
#elif(DEVICE_TYPE_ID==14)
#define FR_APP_DEV FR_APP_ENVDETECTION
#elif(DEVICE_TYPE_ID==15)
#define FR_APP_DEV FR_APP_SMOG
#elif(DEVICE_TYPE_ID==21)
#define FR_APP_DEV FR_APP_IR_RELAY
#else
#error "undef FR_APP_DEV !!!"
#endif


//控制命令
#define FR_CMD_BROCAST_REFRESH	    "/BR/"		//广播刷新
#define FR_CMD_SINGLE_REFRESH	    "/SR/"		//单播刷新
#define FR_CMD_SINGLE_EXCUTE	    "/EC/"		//单次执行发送
#define FR_CMD_PEROID_EXCUTE	    "/EP/"		//周期性执行发送
#define FR_CMD_PEROID_STOP	    "/ES/"		//停止周期性发送
#define FR_CMD_JOIN_CTRL	    "/CJ/"		//网关允许/不允许加入

#define FR_UC_DATA_FIX_LEN		38		//UC帧固定长度
#define FR_UO_DATA_FIX_LEN		30		//UO帧固定长度
#define FR_UH_DATA_FIX_LEN		11		//UH帧固定长度
#define FR_HR_DATA_FIX_LEN		14		//HR帧固定长度
#define FR_DE_DATA_FIX_LEN		14		//DE帧固定长度

/*********************************************************************
 * MACROS
 */
#define FRAME_DATA_SIZE		128
#define FRAME_BUFFER_SIZE 	256
#define MAX_OPTDATA_SIZE	FRAME_DATA_SIZE

/*********************************************************************
 * TYPEDEFS
 */
typedef byte ZIdentifyNo_t[8];

typedef enum
{
	HEAD_UC,
	HEAD_UO,
	HEAD_UH,
	HEAD_UR,
	HEAD_DE,
	HEAD_NONE,
}frHeadType_t;


/*********************************************************************
 * FUNCTIONS
 */

void *SSAFrame_Analysis(frHeadType_t hType, uint8 SrcBuf[], uint8 SrcLen);
int8 SSAFrame_Package(frHeadType_t hType, void *data, uint8 **DstBuf, uint16 *DstLen);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* FRAME_ANALYSIS_H */
