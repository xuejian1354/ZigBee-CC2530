/*
 * trrequest.c
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
#include "CommonApp.h"
#include "trrequest.h"
#include "protocol.h"
#include "mincode.h"
#include "mevent.h"
#include <string.h>
#include <stdio.h>

#ifdef TRANSCONN_BOARD_GATEWAY
void pi_handler(struct sockaddr_in *addr, pi_t *pi)
{
	gw_info_t *p_gw = NULL;
	dev_info_t *p_dev = NULL;
	uint8 *buffer;

	char ipaddr[24] = {0};
	sprintf(ipaddr, "%s:%u", addr->sin_addr, addr->sin_port);
	
	switch(pi->trans_type)
	{
	case TRTYPE_UDP_NORMAL:
		switch(pi->fr_type)
		{
		case TRFRAME_CON:
			break;

		case TRFRAME_GET:
			p_gw = get_gateway_info();
			p_gw->rand = gen_rand(pi->sn);
			memset(p_gw->ipaddr, 0, sizeof(p_gw->ipaddr));
			memcpy(p_gw->ipaddr, pi->data, pi->data_len);
			p_gw->ip_len = pi->data_len;

			memset(p_gw->serverip_addr, 0, sizeof(p_gw->serverip_addr));
			memcpy(p_gw->serverip_addr, ipaddr, strlen(ipaddr));
			p_gw->serverip_len = strlen(ipaddr);
			
			fr_buffer_t *frbuffer = get_gateway_buffer_alloc(p_gw);
			
			bi_t bi;
			memcpy(bi.sn, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
			bi.trans_type = TRTYPE_UDP_NORMAL;
			bi.fr_type = TRFRAME_PUT_GW;
			bi.data = frbuffer->data;
			bi.data_len = frbuffer->size;
			send_frame_udp_request((uint8 *)ipaddr, TRHEAD_BI, &bi);
			
			get_buffer_free(frbuffer);

			p_dev = p_gw->p_dev;
			while(p_dev != NULL)
			{
				buffer = get_zdev_buffer_alloc(p_dev);
				
				bi_t zbi;
				memcpy(zbi.sn, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
				zbi.trans_type = TRTYPE_UDP_NORMAL;
				zbi.fr_type = TRFRAME_PUT_DEV;
				zbi.data = buffer;
				zbi.data_len = ZDEVICE_BUFFER_SIZE;
				send_frame_udp_request((uint8 *)ipaddr, TRHEAD_BI, &zbi);
				
				get_zdev_buffer_free(buffer);
				p_dev = p_dev->next;
			}
			break;
		}
		break;

	case TRTYPE_TCP_LONG:
	case TRTYPE_TCP_SHORT:
		break;
	}
}

void bi_handler(struct sockaddr_in *addr, bi_t *bi)
{
	gw_info_t *p_gw = NULL;
	//dev_info_t *p_dev = NULL;

	char ipaddr[24] = {0};
	sprintf(ipaddr, "%s:%u", addr->sin_addr, addr->sin_port);
	
	switch(bi->trans_type)
	{
	case TRTYPE_UDP_NORMAL:
		switch(bi->fr_type)
		{
		case TRFRAME_REG:
			p_gw = get_gateway_info();
			p_gw->rand = gen_rand(bi->sn);
			memset(p_gw->ipaddr, 0, sizeof(p_gw->ipaddr));
			memcpy(p_gw->ipaddr, bi->data, bi->data_len);
			p_gw->ip_len = bi->data_len;
			
			memset(p_gw->serverip_addr, 0, sizeof(p_gw->serverip_addr));
			memcpy(p_gw->serverip_addr, ipaddr, strlen(ipaddr));
			p_gw->serverip_len = strlen(ipaddr);
			break;
			
		case TRFRAME_PUT_GW:
			break;

		case TRFRAME_PUT_DEV:
			break;
		}
		break;

	case TRTYPE_TCP_LONG:
	case TRTYPE_TCP_SHORT:
		break;
	}
}

void gp_handler(struct sockaddr_in *addr, gp_t *gp)
{
	char ipaddr[24] = {0};
	sprintf(ipaddr, "%s:%u", addr->sin_addr, addr->sin_port);
	fr_buffer_t *frbuffer = NULL;
	dev_info_t *p_dev = NULL;

	if(gp->data_len > IP_ADDR_MAX_SIZE)
	{
		return;
	}

	cli_info_t *m_info = osal_mem_alloc(sizeof(cli_info_t));
	memset(m_info, 0, sizeof(cli_info_t));
	memcpy(m_info->cidentify_no, gp->cidentify_no, sizeof(cidentify_no_t));
	m_info->trans_type = gp->trans_type;
	memcpy(m_info->ipaddr, gp->data, gp->data_len);
	m_info->ip_len = gp->data_len;
	memcpy(m_info->serverip_addr, ipaddr, strlen(ipaddr));
	m_info->serverip_len = strlen(ipaddr);
	m_info->check_count = 3;
	m_info->check_conn = 1;
	m_info->next = NULL;
	
	if(add_client_info(m_info) != 0)
	{
		osal_mem_free(m_info);
	}
	
	if((p_dev=query_zdevice_info_with_sn(gp->zidentify_no)) == NULL)
	{
		return;
	}

	uo_t m_uo;
	memcpy(m_uo.head, FR_HEAD_UO, 3);
	m_uo.type = get_frnet_type_to_ch(p_dev->znet_type);
	get_frapp_type_to_str((char *)m_uo.ed_type, p_dev->zapp_type);
	incode_xtocs(m_uo.ext_addr, p_dev->zidentity_no, 8);
	incode_xtoc16(m_uo.short_addr, p_dev->znet_addr);
	m_uo.data = NULL;
	m_uo.data_len = 0;
	memcpy(m_uo.tail, FR_TAIL, 4);
	
	frbuffer = get_buffer_alloc(HEAD_UO, &m_uo);
	
	rp_t mrp;
	memcpy(mrp.zidentify_no, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
	memcpy(mrp.cidentify_no, gp->cidentify_no, sizeof(cidentify_no_t));
	mrp.trans_type = gp->trans_type;
	mrp.tr_info = TRINFO_UPDATE;
	mrp.data = frbuffer->data;
	mrp.data_len = frbuffer->size;
	send_frame_udp_request((uint8 *)ipaddr, TRHEAD_RP, &mrp);
	
	get_buffer_free(frbuffer);
	set_rp_check(query_client_info(gp->cidentify_no));
}

void rp_handler(struct sockaddr_in *addr, rp_t *rp)
{
	char ipaddr[24] = {0};
	sprintf(ipaddr, "%s:%u", addr->sin_addr, addr->sin_port);
	
	if(memcmp(get_gateway_info()->gw_no, rp->zidentify_no, sizeof(zidentify_no_t))
		|| rp->data_len > IP_ADDR_MAX_SIZE)
	{
		return;
	}

	if(rp->tr_info == TRINFO_CUT)
	{
		set_rp_check(NULL);
		cli_info_t *p_cli = query_client_info(rp->cidentify_no);
		p_cli->trans_type = rp->trans_type;
	}
}

void gd_handler(struct sockaddr_in *addr, gd_t *gd)
{
	char ipaddr[24] = {0};
	sprintf(ipaddr, "%s:%u", addr->sin_addr, addr->sin_port);
	
	if(gd->data_len > IP_ADDR_MAX_SIZE)
	{
		return;
	}

	cli_info_t *m_info = osal_mem_alloc(sizeof(cli_info_t));
	memset(m_info, 0, sizeof(cli_info_t));

	switch(gd->tr_info)
	{
	case TRINFO_IP:
		memcpy(m_info->ipaddr, gd->data, gd->data_len);
		m_info->ip_len = gd->data_len;
		break;

	case TRINFO_HOLD:
	case TRINFO_REG:
		memcpy(m_info->ipaddr, ipaddr, strlen(ipaddr));
		m_info->ip_len = strlen(ipaddr);
		break;
	}
	
	memcpy(m_info->cidentify_no, gd->cidentify_no, sizeof(cidentify_no_t));	
	m_info->trans_type = gd->trans_type;
	GET_SERVER_IP((char *)m_info->serverip_addr);
	m_info->serverip_len = strlen((char *)m_info->serverip_addr);
	m_info->check_count = 0;
	m_info->check_conn = 1;
	m_info->next = NULL;
	
	if(add_client_info(m_info) != 0)
	{
		osal_mem_free(m_info);
	}

	set_cli_check(query_client_info(gd->cidentify_no));

	dev_info_t *p_dev = get_gateway_info()->p_dev;
	uint8 buffer[ZDEVICE_MAX_NUM<<4] = {0};
	int bsize = 0;
	while(p_dev != NULL && bsize < (ZDEVICE_MAX_NUM<<4))
	{
		incode_xtocs(buffer+bsize, p_dev->zidentity_no, 8);
		bsize += 16;
		p_dev = p_dev->next;
	}

	if(gd->tr_info != TRINFO_HOLD)
	{
		gd_t mgd;
		memcpy(mgd.zidentify_no, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
		memcpy(mgd.cidentify_no, gd->cidentify_no, sizeof(cidentify_no_t));
		mgd.trans_type = TRTYPE_UDP_TRAVERSAL;
		mgd.tr_info = TRINFO_REG;
		mgd.data = buffer;
		mgd.data_len = bsize;
		send_frame_udp_request(
			query_client_info(gd->cidentify_no)->ipaddr, TRHEAD_GD, &mgd);
	}

	return;
}

void rd_handler(struct sockaddr_in *addr, rd_t *rd)
{
	char ipaddr[24] = {0};
	sprintf(ipaddr, "%s:%u", addr->sin_addr, addr->sin_port);
	
	cli_info_t *p_cli = query_client_info(rd->cidentify_no);

	p_cli->trans_type = rd->trans_type;
	p_cli->ip_len = strlen(ipaddr);
	memcpy(p_cli->ipaddr, ipaddr, p_cli->ip_len);
	
	set_cli_check(p_cli);
}

void dc_handler(struct sockaddr_in *addr, dc_t *dc)
{
	char ipaddr[24] = {0};
	sprintf(ipaddr, "%s:%u", addr->sin_addr, addr->sin_port);
	
	cli_info_t *m_info = osal_mem_alloc(sizeof(cli_info_t));
	memset(m_info, 0, sizeof(cli_info_t));
	memcpy(m_info->cidentify_no, dc->cidentify_no, sizeof(cidentify_no_t));	
	m_info->trans_type = dc->trans_type;
	if(m_info->trans_type == TRTYPE_UDP_TRAVERSAL)
	{
		memcpy(m_info->ipaddr, ipaddr, strlen(ipaddr));
		m_info->ip_len = strlen(ipaddr);
	}
	else
	{
		memset(m_info->ipaddr, 0, sizeof(m_info->ipaddr));
		m_info->ip_len = 0;
	}
	GET_SERVER_IP((char *)m_info->serverip_addr);
	m_info->serverip_len = strlen((char *)m_info->serverip_addr);
	m_info->check_count = 0;
	m_info->check_conn = 1;
	m_info->next = NULL;
	
	if(add_client_info(m_info) != 0)
	{
		osal_mem_free(m_info);
	}

	frHeadType_t head_type = get_frhead_from_str((char *)dc->data);
	void *p = get_frame_alloc(head_type, dc->data, dc->data_len);
	if(p == NULL)
	{
		return;
	}

	switch(head_type)
	{
	case HEAD_UC:
	{
		uc_t *uc = (uc_t *)p;
		get_frame_free(HEAD_UC, uc);
	}
	break;
		
	case HEAD_UO:
	{
		uo_t *uo = (uo_t *)p;
		get_frame_free(HEAD_UO, uo);
	}
	break;
		
	case HEAD_UH:
	{
		uh_t *uh = (uh_t *)p;
		get_frame_free(HEAD_UH, uh);
	}
	break;
		
	case HEAD_UR:
	{
		ur_t *ur = (ur_t *)p;
		get_frame_free(HEAD_UR, ur);
	}
	break;
		
	case HEAD_DE:
	{
		fr_buffer_t *buffer = NULL;
		gw_info_t *p_mgw = get_gateway_info();
		dev_info_t *dev_info = NULL;
		de_t *de = (de_t *)p;
		if(memcmp(p_mgw->gw_no, dc->zidentify_no, sizeof(zidentify_no_t)))
		{
			dev_info = query_zdevice_info_with_sn(dc->zidentify_no);
		}
		else
		{
			set_devopt_fromstr(p_mgw->zgw_opt, de->data, de->data_len);

			if(p_mgw->zgw_opt && 
				(p_mgw->zgw_opt->type == FRAPP_DOOR_SENSOR
					|| p_mgw->zgw_opt->type == FRAPP_IR_DETECTION
					|| (p_mgw->zgw_opt->type == FRAPP_ENVDETECTION
						&& !memcmp(p_mgw->zgw_opt
									->device.envdetection.current_buffer, 
								DEVOPT_AIRCONTROLLER_UPSETTING, 3))
					|| (p_mgw->zgw_opt->type == FRAPP_AIRCONTROLLER
						&& !memcmp(p_mgw->zgw_opt
									->device.aircontroller.current_buffer, 
								DEVOPT_AIRCONTROLLER_UPSETTING, 3))))
			{
				ur_t ur;
				memcpy(ur.head, FR_HEAD_UR, 3);
				ur.type = get_frnet_type_to_ch(FRNET_ROUTER);
				get_frapp_type_to_str((char *)ur.ed_type, p_mgw->zapp_type);
				incode_xtoc16(ur.short_addr, 0);
				ur.data_len = de->data_len;
				ur.data = de->data;
				memcpy(ur.tail, FR_TAIL, 4);
									
				fr_buffer_t *frbuffer = get_switch_buffer_alloc(HEAD_UR, 
										p_mgw->zgw_opt, &ur);
		
				cli_info_t *p_cli = get_client_list()->p_cli;
				while(p_cli != NULL)
				{
					ub_t ub;
					memcpy(ub.zidentify_no, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
					memcpy(ub.cidentify_no, p_cli->cidentify_no, sizeof(cidentify_no_t));
					ub.trans_type = p_cli->trans_type;
					ub.tr_info = TRINFO_REDATA;
					ub.data = frbuffer->data;
					ub.data_len = frbuffer->size;
					if(p_cli->trans_type == TRTYPE_UDP_NORMAL)
					{
						send_frame_udp_request(p_cli->serverip_addr, TRHEAD_UB, &ub);
					}
					else if(p_cli->trans_type == TRTYPE_UDP_TRAVERSAL)
					{
						send_frame_udp_request(p_cli->ipaddr, TRHEAD_UB, &ub);
					}

					p_cli = p_cli->next;
				}
				get_buffer_free(frbuffer);
				
				goto Handle_UR_free;
			}
			
			buffer = get_switch_buffer_alloc(HEAD_DE, p_mgw->zgw_opt, de);
		}
		
		if(dev_info != NULL)
		{
			set_devopt_fromstr(dev_info->zdev_opt, de->data, de->data_len);

			if(dev_info->zdev_opt && 
				(dev_info->zdev_opt->type == FRAPP_DOOR_SENSOR
					|| dev_info->zdev_opt->type == FRAPP_IR_DETECTION
					|| (dev_info->zdev_opt->type == FRAPP_ENVDETECTION
						&& !memcmp(dev_info->zdev_opt
									->device.envdetection.current_buffer, 
								DEVOPT_AIRCONTROLLER_UPSETTING, 3))
					|| (dev_info->zdev_opt->type == FRAPP_AIRCONTROLLER
						&& !memcmp(dev_info->zdev_opt
									->device.aircontroller.current_buffer, 
								DEVOPT_AIRCONTROLLER_UPSETTING, 3))))
			{
				ur_t ur;
				memcpy(ur.head, FR_HEAD_UR, 3);
				ur.type = get_frnet_type_to_ch(dev_info->znet_type);
				get_frapp_type_to_str((char *)ur.ed_type, dev_info->zapp_type);
				incode_xtoc16(ur.short_addr, dev_info->znet_addr);
				ur.data_len = de->data_len;
				ur.data = de->data;
				memcpy(ur.tail, FR_TAIL, 4);
									
				fr_buffer_t *frbuffer = get_switch_buffer_alloc(HEAD_UR, 
										dev_info->zdev_opt, &ur);
		
				cli_info_t *p_cli = get_client_list()->p_cli;
				while(p_cli != NULL)
				{
					ub_t ub;
					memcpy(ub.zidentify_no, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
					memcpy(ub.cidentify_no, p_cli->cidentify_no, sizeof(cidentify_no_t));
					ub.trans_type = p_cli->trans_type;
					ub.tr_info = TRINFO_REDATA;
					ub.data = frbuffer->data;
					ub.data_len = frbuffer->size;
					if(p_cli->trans_type == TRTYPE_UDP_NORMAL)
					{
						send_frame_udp_request(p_cli->serverip_addr, TRHEAD_UB, &ub);
					}
					else if(p_cli->trans_type == TRTYPE_UDP_TRAVERSAL)
					{
						send_frame_udp_request(p_cli->ipaddr, TRHEAD_UB, &ub);
					}

					p_cli = p_cli->next;
				}
				get_buffer_free(frbuffer);
				
				goto Handle_UR_free;
			}
			
			buffer = get_switch_buffer_alloc(HEAD_DE, 
                                                         dev_info->zdev_opt, de);
		}

		if(buffer != NULL)
		{
			ConnectorApp_TxHandler(buffer->data, buffer->size);
			get_buffer_free(buffer);
		}
Handle_UR_free:
		get_frame_free(HEAD_DE, de);
	}
	break;
		
	default: break;
	}
	
	return;
}

void ub_handler(struct sockaddr_in *addr, ub_t *ub)
{
}

void send_frame_udp_request(uint8 *ipaddr, tr_head_type_t htype, void *frame)
{
	tr_buffer_t *buffer;
	
	switch(htype)
	{
	case TRHEAD_PI: 
	case TRHEAD_BI: 
	case TRHEAD_GP:
	case TRHEAD_RP:
	case TRHEAD_GD:
	case TRHEAD_RD:
	case TRHEAD_DC:
	case TRHEAD_UB:
	{
		if((buffer = get_trbuffer_alloc(htype, frame)) == NULL)
		{
			return;
		}

		//socket_udp_sendto(ipaddr, buffer->data, buffer->size);
		CommonApp_GetDevDataSend(buffer->data, buffer->size);
		get_trbuffer_free(buffer);	
	}
	break;
	}
}
#endif