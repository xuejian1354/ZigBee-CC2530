/*
 * protocol.h
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
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include "globals.h"
#include "framelysis.h"
#include "trframelysis.h"

#define GATEWAY_BUFFER_FIX_SIZE		34
#define ZDEVICE_BUFFER_SIZE		23

typedef struct Dev_Info
{
	zidentify_no_t zidentity_no;
	uint16 znet_addr;
	fr_app_type_t zapp_type;
	fr_net_type_t znet_type;
	struct Dev_Info *next;
}dev_info_t;


typedef struct Gw_Info
{
	zidentify_no_t gw_no;
	fr_app_type_t zapp_type;
	uint16 zpanid;
	uint16 zchannel;
	uint32 rand;
	tr_trans_type_t trans_type;
	uint8 ipaddr[IP_ADDR_MAX_SIZE];
	uint8 ip_len;
	uint8 serverip_addr[IP_ADDR_MAX_SIZE];
	uint8 serverip_len;
	dev_info_t *p_dev;
	struct Gw_Info *next;
}gw_info_t;


typedef struct
{
	uint8 *buf;
	int len;
}frhandler_arg_t;

uint8 *get_zdev_buffer_alloc(dev_info_t *dev_info);
void get_zdev_buffer_free(uint8 *p);
dev_info_t *get_zdev_frame_alloc(uint8 *buffer, int length);
void get_zdev_frame_free(dev_info_t *p);

fr_buffer_t *get_gateway_buffer_alloc(gw_info_t *gw_info);

int add_zdev_info(gw_info_t *gw_info, dev_info_t *m_dev);
dev_info_t *query_zdev_info(gw_info_t *gw_info, uint16 znet_addr);
int del_zdev_info(gw_info_t *gw_info, uint16 znet_addr);

frhandler_arg_t *get_frhandler_arg_alloc(uint8 *buf, int len);
void get_frhandler_arg_free(frhandler_arg_t *arg);

gw_info_t *get_gateway_info(void);

int add_zdevice_info(dev_info_t *m_dev);
dev_info_t *query_zdevice_info(uint16 znet_addr);
dev_info_t *query_zdevice_info_with_sn(zidentify_no_t zidentify_no);
int del_zdevice_info(uint16 znet_addr);

void analysis_zdev_frame(frhandler_arg_t *arg);
void analysis_capps_frame(frhandler_arg_t *arg);

fr_buffer_t *get_switch_buffer_alloc(frHeadType_t head_type, void *frame);

#endif  //__PROTOCOL_H__
