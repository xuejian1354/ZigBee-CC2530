/**************************************************************************************************
  Filename:       frame-analysis.h
  Revised:        $Date: 2014-04-25 17:14:12 -0800 (Wed, 25 Apr 2014) $
  Revision:       $Revision: 29217 $

  Description:    Analysis frame format.
**************************************************************************************************/

/**************************************************************************************************
Create by Sam_Chen
Date:2014-04-25
**************************************************************************************************/

/**************************************************************************************************
Modify by Sam_Chen
Date:2015-02-02
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
#define FR_APP_ENDNODE				"00"
#define FR_APP_LIGHTSWITCH_ONE		"01"
#define FR_APP_LIGHTSWITCH_TWO		"02"
#define FR_APP_LIGHTSWITCH_THREE	"03"
#define FR_APP_LIGHTSWITCH_FOUR		"04"
#define FR_APP_ALARM				"11"
#define FR_APP_IR_DETECTION			"12"
#define FR_APP_DOOR_SENSOR			"13"
#define FR_APP_SMOG					"14"
#define FR_APP_IR_RELAY				"21"

#if(DEVICE_TYPE_ID==0)
#define FR_APP_DEV FR_APP_CONNECTOR
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
#define FR_APP_DEV FR_APP_SMOG
#elif(DEVICE_TYPE_ID==21)
#define FR_APP_DEV FR_APP_IR_RELAY
#else
#error "undef FR_APP_DEV !!!"
#endif


//��������
#define FR_CMD_BROCAST_REFRESH	"/BR/"		//�㲥ˢ��
#define FR_CMD_SINGLE_REFRESH	"/SR/"		//����ˢ��
#define FR_CMD_SINGLE_EXCUTE	"/EC/"		//����ִ�з���
#define FR_CMD_PEROID_EXCUTE	"/EP/"		//������ִ�з���
#define FR_CMD_PEROID_STOP		"/ES/"		//ֹͣ�����Է���
#define FR_CMD_JOIN_CTRL		"/CJ/"		//��������/���������

#define FR_UC_DATA_FIX_LEN		38		//UC֡�̶�����
#define FR_UO_DATA_FIX_LEN		30		//UO֡�̶�����
#define FR_UH_DATA_FIX_LEN		11		//UH֡�̶�����
#define FR_HR_DATA_FIX_LEN		14		//HR֡�̶�����
#define FR_DE_DATA_FIX_LEN		14		//DE֡�̶�����

/*********************************************************************
 * MACROS
 */
#define FRAME_DATA_SIZE		32
#define FRAME_BUFFER_SIZE 	64
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
}frHeadType_t;


//Э�������������������������
typedef struct
{
	uint8 head[3];   //UC:
	uint8 type;   //��ʾ�豸���ͣ�0��ʾЭ����
	uint8 ed_type[2]; //��ʾӦ�����ͣ�00��ʾЭ����
	uint8 short_addr[4];   //�豸�̵�ַ
	uint8 ext_addr[16];  //�豸����ַ  
	uint8 panid[4];    //����PANID��
	uint8 channel[4]; //�����ŵ�
	uint8 tail[4];    //:O/r/n
}UC_t;


//�豸��������󷢸�Э����
typedef struct
{
	uint8 head[3];   //UO:
	uint8 type;   //��ʾ�豸����
	uint8 ed_type[2]; //��ʾӦ������
	uint8 short_addr[4];   //�豸�̵�ַ
	uint8 ext_addr[16];  //�豸����ַ  
	uint8 data_len;
	uint8 *data; //�ն��豸��ʼ����
	uint8 tail[4];    //:O/r/n
}UO_t; 

 
 //�豸����
typedef struct
{
	uint8 head[3];   //UH:
	uint8 short_addr[4];   //�豸�̵�ַ
	uint8 tail[4];    //:O/r/n
}UH_t;  
 
 
//�豸ʵʱ���ݷ��ظ�ʽ
typedef struct
{
	uint8 head[3];   //UR:
	uint8 type;   //��ʾ�豸���ͣ�0��ʾЭ����
	uint8 ed_type[2]; //��ʾӦ������
	uint8 short_addr[4];   //�豸�̵�ַ 
	uint8 data_len;
	uint8 *data;    //�ն��豸��ʼ����
	uint8 tail[4];    //:O/r/n
}UR_t;  
 
 
//�豸���������ʽ
typedef struct
{
	uint8 head[2];   //D:
	uint8 cmd[4];   //�����ֶΣ�������ʲô����
	uint8 short_addr[4];   //�豸�̵�ַ 
	uint8 data_len;
	uint8 *data; //�ն��豸��������
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
