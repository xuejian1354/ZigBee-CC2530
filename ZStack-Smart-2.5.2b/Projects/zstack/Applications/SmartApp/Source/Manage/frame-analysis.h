/**************************************************************************************************
  Filename:       frame-analysis.h
  Revised:        $Date: 2014-04-25 17:14:12 -0800 (Wed, 25 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    Analysis frame format.
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-07-31
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
//֡ͷ
#define FR_HEAD_UC	"UC:"	//Э�������������ϴ�
#define FR_HEAD_UO	"UO:"	//�豸���������ϴ�
#define FR_HEAD_UH	"UH:"	//�豸�������ݸ�ʽ
#define FR_HEAD_DE	"D:"	//��������
#define FR_HEAD_UR	"UR:"	//���Ʒ���
#define FR_HEAD_UJ	"UJ:"	//������������


//�豸����
#define FR_DEV_COORD	'0'
#define FR_DEV_ROUTER	'0'
#define FR_DEV_ENDDEV	'1'

//Ӧ������
#define FR_APP_CONNECTOR			"00"
#define FR_APP_DEMOBASE				"FE"
#define FR_APP_AIRCONTROLLER 		"F0"
#define FR_APP_LIGHTSWITCH_ONE		"01"
#define FR_APP_LIGHTSWITCH_TWO		"02"
#define FR_APP_LIGHTSWITCH_THREE	"03"
#define FR_APP_LIGHTSWITCH_FOUR		"04"
#define FR_APP_ALARM				"11"
#define FR_APP_IR_DETECTION			"12"
#define FR_APP_DOOR_SENSOR			"13"
#define FR_APP_ENVDETECTION			"14"
#define FR_APP_SMOG					"15"
#define FR_APP_IR_RELAY				"21"
#define FR_APP_HUMITURE_DETECTION	"A1"
#define FR_APP_SOLENOID_VALVE		"A2"

/*Connector*/
#if(DEVICE_TYPE_ID==0)
#define FR_APP_DEV FR_APP_CONNECTOR

/*DemoBase*/
#elif(DEVICE_TYPE_ID==0xFE)
#define FR_APP_DEV FR_APP_DEMOBASE

/*AirController*/
#elif(DEVICE_TYPE_ID==0xF0)
#define FR_APP_DEV FR_APP_AIRCONTROLLER

/*Smart Home*/
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

/*Smart Farm*/
#elif(DEVICE_TYPE_ID==0xA1)
#define FR_APP_DEV FR_APP_HUMITURE_DETECTION
#elif(DEVICE_TYPE_ID==0xA2)
#define FR_APP_DEV FR_APP_SOLENOID_VALVE

#else
#error "undef FR_APP_DEV !!!"
#endif


//��������
#define FR_CMD_BROCAST_REFRESH	    "/BR/"		//�㲥ˢ��
#define FR_CMD_SINGLE_REFRESH	    "/SR/"		//����ˢ��
#define FR_CMD_SINGLE_EXCUTE	    "/EC/"		//����ִ�з���
#define FR_CMD_PEROID_EXCUTE	    "/ES/"		//������ִ�з���
#define FR_CMD_PEROID_STOP	    "/EP/"		//ֹͣ�����Է���
#define FR_CMD_JOIN_CTRL	    "/CJ/"		//��������/���������

#define FR_UC_DATA_FIX_LEN		38		//UC֡�̶�����
#define FR_UO_DATA_FIX_LEN		30		//UO֡�̶�����
#define FR_UH_DATA_FIX_LEN		11		//UH֡�̶�����
#define FR_HR_DATA_FIX_LEN		14		//HR֡�̶�����
#define FR_DE_DATA_FIX_LEN		14		//DE֡�̶�����

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

//Coordinator info frame
typedef struct
{
	uint8 head[3];   //UC:
	uint8 type;   //net type, 0 support route
	uint8 ed_type[2]; //app type, 00 coordinator
	uint8 short_addr[4];   //net address
	uint8 ext_addr[16];  //mac address
	uint8 panid[4];    //net panid
	uint8 channel[4]; //net channel
	uint8 data_len;
	uint8 *data; //data
	uint8 tail[4];    //:O/r/n
}UC_t;


//Device info frame
typedef struct
{
	uint8 head[3];   //UO:
	uint8 type;   //net type
	uint8 ed_type[2]; //app type
	uint8 short_addr[4];   //net address
	uint8 ext_addr[16];  //mac address
	uint8 data_len;
	uint8 *data; //data
	uint8 tail[4];    //:O/r/n
}UO_t; 

 
 //Device Heart Beat frame
typedef struct
{
	uint8 head[3];   //UH:
	uint8 short_addr[4];   //net address
	uint8 tail[4];    //:O/r/n
}UH_t;  
 
 
//Device returns frame
typedef struct
{
	uint8 head[3];   //UR:
	uint8 type;   //net type
	uint8 ed_type[2]; //app type
	uint8 short_addr[4];   //net address
	uint8 data_len;
	uint8 *data;    //data
	uint8 tail[4];    //:O/r/n
}UR_t;  
 
 
//Device control frame
typedef struct
{
	uint8 head[2];   //D:
	uint8 cmd[4];   //cmmand
	uint8 short_addr[4];   //net address
	uint8 data_len;
	uint8 *data; //data
	uint8 tail[4]; //:O/r/n
 }DE_t; 

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
