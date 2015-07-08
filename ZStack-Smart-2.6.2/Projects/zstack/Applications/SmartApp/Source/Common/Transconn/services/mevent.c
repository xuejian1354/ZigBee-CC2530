/*
 * mevent.c
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

#include "mevent.h"
#include "protocol.h"
#include "trframelysis.h"
#include "trrequest.h"
#include "mincode.h"
#include "api_events.h"
#include "TransconnApp.h"

#ifdef TRANSCONN_BOARD_GATEWAY
static void zdev_watch( void *params, uint16 *duration, uint8 *count);
static void cli_watch( void *params, uint16 *duration, uint8 *count);
static void rp_watch( void *params, uint16 *duration, uint8 *count);

extern byte TransconnApp_TaskID;

void upload_event( void *params, uint16 *duration, uint8 *count)
{
	char ipaddr[24] = {0};
	GET_SERVER_IP(ipaddr);
	
	dev_info_t *p_dev = get_gateway_info()->p_dev;
	uint8 buffer[ZDEVICE_MAX_NUM<<2] = {0};
	int bsize = 0;
	while(p_dev != NULL && bsize < (ZDEVICE_MAX_NUM<<2))
	{
		incode_xtoc16(buffer+bsize, p_dev->znet_addr);
		bsize += 4;
		p_dev = p_dev->next;
	}
	
	pi_t pi;
	memcpy(pi.sn, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
	pi.trans_type = TRTYPE_UDP_NORMAL;
	pi.fr_type = TRFRAME_CON;
	pi.data = buffer;
	pi.data_len = bsize;
	
	send_frame_udp_request((uint8 *)ipaddr, TRHEAD_PI, &pi);
}

void stand_event( void *params, uint16 *duration, uint8 *count)
{
 	cli_info_t *p_cli = get_client_list()->p_cli;

	dev_info_t *p_dev = get_gateway_info()->p_dev;
	uint8 buffer[ZDEVICE_MAX_NUM<<4] = {0};
	int bsize = 0;
	while(p_dev != NULL && bsize < (ZDEVICE_MAX_NUM<<4))
	{
		incode_xtocs(buffer+bsize, p_dev->zidentity_no, 8);
		bsize += 16;
		p_dev = p_dev->next;
	}

	char ipaddr[24] = {0};
	GET_SERVER_IP(ipaddr);

	while(p_cli != NULL)
	{
		if(memcmp(ipaddr, p_cli->ipaddr, p_cli->ip_len))
		{
			gd_t gd;
			memcpy(gd.zidentify_no, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
			memcpy(gd.cidentify_no, p_cli->cidentify_no, sizeof(cidentify_no_t));
			gd.trans_type = TRTYPE_UDP_TRAVERSAL;
			gd.tr_info = TRINFO_REG;
			gd.data = buffer;
			gd.data_len = bsize;
			send_frame_udp_request(p_cli->ipaddr, TRHEAD_GD, &gd);
		}

		if(p_cli->check_conn)
		{
			set_cli_check(p_cli);
			p_cli->check_conn = 0;
		}
		
		p_cli = p_cli->next;
	}
}

void zdev_watch( void *params, uint16 *duration, uint8 *count)
{
	dev_info_t *p_dev = get_gateway_info()->p_dev;
	while(p_dev)
	{
		uint16 znet_addr = p_dev->znet_addr;
		p_dev = p_dev->next;
		
		del_zdevice_info(znet_addr);
	}
}

void cli_watch( void *params, uint16 *duration, uint8 *count)
{
	cli_info_t *p_cli = get_client_list()->p_cli;

	while(p_cli)
	{
		cli_info_t *pre_cli = p_cli;
		p_cli = p_cli->next;
		
		del_client_info(pre_cli->cidentify_no);
	}
}


void rp_watch( void *params, uint16 *duration, uint8 *count)
{
	cli_info_t *p_cli = (cli_info_t *)params;
	if(p_cli == NULL)
	{
		return;
	}
	
	if(p_cli->check_count-- != 0)
	{
		rp_t rp;
		memcpy(rp.zidentify_no, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
		memcpy(rp.cidentify_no, p_cli->cidentify_no, sizeof(cidentify_no_t));
		rp.trans_type = TRTYPE_UDP_TRAVERSAL;
		rp.tr_info = TRINFO_UPDATE;
		rp.data = NULL;
		rp.data_len = 0;
		send_frame_udp_request(p_cli->ipaddr, TRHEAD_RP, &rp);
		
		set_rp_check(p_cli);
	}
}

void set_zdev_check(uint16 net_addr)
{
	update_user_event(TransconnApp_TaskID, ZDEVICE_WATCH_EVENT,
		zdev_watch, 40000, TIMER_ONE_EXECUTION | TIMER_EVENT_RESIDENTS, (void *)((int)net_addr));
}

void set_cli_check(cli_info_t *p_cli)
{
	if(p_cli == NULL)
	{
		return;
	}
	
	update_user_event(TransconnApp_TaskID, CLIENT_WATCH_EVENT,
		cli_watch, 17000, TIMER_ONE_EXECUTION | TIMER_EVENT_RESIDENTS, NULL);
}

void set_rp_check(cli_info_t *p_cli)
{
	if(p_cli == NULL)
	{
		return;
	}

	update_user_event(TransconnApp_TaskID, RP_CHECK_EVENT, 
		rp_watch, 1000, TIMER_ONE_EXECUTION | TIMER_EVENT_RESIDENTS, p_cli);
}
#endif
