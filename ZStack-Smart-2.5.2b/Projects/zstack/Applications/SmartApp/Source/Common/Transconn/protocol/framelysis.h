/*
 * framelysis.h
 *
 * Copyright (C) 2013 loongsky development.
 *
 * Sam Chen <xuejian1354@163.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __FRAMELYSIS_H__
#define __FRAMELYSIS_H__

#include "globals.h"
#include "frame-analysis.h"

//command method
#define FR_CMD_BROCAST_REFRESH	"/BR/"		//broadcast
#define FR_CMD_SINGLE_REFRESH	"/SR/"		//single refresh
#define FR_CMD_SINGLE_EXCUTE	"/EC/"		//single control
#define FR_CMD_PEROID_EXCUTE	"/ES/"		//recycle request
#define FR_CMD_PEROID_STOP		"/EP/"		//recycle stop request
#define FR_CMD_JOIN_CTRL		"/CJ/"		//join permit
#define FR_CMD_WR_IPADDR		"/WI/"		//write ipaddr

#define FR_UC_DATA_FIX_LEN		38		//UC frame fix len
#define FR_UO_DATA_FIX_LEN		30		//UO frame fix len
#define FR_UH_DATA_FIX_LEN		11		//UH frame fix len
#define FR_UR_DATA_FIX_LEN		14		//HR frame fix len
#define FR_DE_DATA_FIX_LEN		14		//DE frame fix len

#define FR_TAIL ":O\r\n"

typedef enum
{
	FRAPP_CONNECTOR = 0,
	FRAPP_LIGHTSWITCH_ONE = 1,
	FRAPP_LIGHTSWITCH_TWO = 2,
	FRAPP_LIGHTSWITCH_THREE = 3,
	FRAPP_LIGHTSWITCH_FOUR = 4,
	FRAPP_HUELIGHT = 5,
	FRAPP_ALARM = 11,
	FRAPP_IR_DETECTION = 12,
	FRAPP_DOOR_SENSOR = 13,
	FRAPP_ENVDETECTION = 14,
	FRAPP_IR_RELAY = 21,
	FRAPP_SUPER_BUTTON = 22,
	FRAPP_LAMPSWITCH = 31,
	FRAPP_PROJECTOR = 32,
	FRAPP_AIRCONDITIONER = 33,
	FRAPP_CURTAIN = 34,
	FRAPP_DOORLOCK = 35,
	FRAPP_AIRCONTROLLER = 0xF0,
	FRAPP_RELAYSOCKET = 0xF1,
	FRAPP_LIGHTDETECT = 0xF2,
	FRAPP_HUMITURE_DETECTION = 0xA1,
	FRAPP_SOLENOID_VALVE = 0xA2,
	FRAPP_NONE = 0xFF,
}fr_app_type_t;

typedef enum
{
	FRNET_ROUTER = 0,
	FRNET_ENDDEV = 1,
	FRNET_NONE,
}fr_net_type_t;

typedef struct
{
	uint8 data[64];
	uint8 size;
}fr_buffer_t;

frHeadType_t get_frhead_from_str(char *head);
int get_frhead_to_str(char *dst, frHeadType_t head_type);

fr_app_type_t get_frapp_type_from_str(char *app_type);
int get_frapp_type_to_str(char *dst, fr_app_type_t app_type);

fr_net_type_t get_frnet_type_from_str(char net_type);
char get_frnet_type_to_ch(fr_net_type_t net_type);


void *get_frame_alloc(frHeadType_t htype, uint8 buffer[], int length);
void get_frame_free(frHeadType_t htype, void *p);

#endif	//  __FRAMELYSIS_H__
