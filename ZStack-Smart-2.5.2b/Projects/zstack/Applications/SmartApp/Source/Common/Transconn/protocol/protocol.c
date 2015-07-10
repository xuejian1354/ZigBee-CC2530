/*
 * protocol.c
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
#include "protocol.h"
#include "mincode.h"
#include "trframelysis.h"
#include "trrequest.h"
#include "mevent.h"

#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)
extern uint8 _client_no[8];

static gw_info_t gw_info;

gw_info_t *get_gateway_info(void)
{
	return &gw_info;
}

uint8 *get_zdev_buffer_alloc(dev_info_t *dev_info)
{
	uint8 *dev_buffer = osal_mem_alloc(ZDEVICE_BUFFER_SIZE);
	memset(dev_buffer, 0, ZDEVICE_BUFFER_SIZE);
	incode_xtocs(dev_buffer, dev_info->zidentity_no, 8);
	incode_xtoc16(dev_buffer+16, dev_info->znet_addr);
	get_frapp_type_to_str((char *)dev_buffer+20, dev_info->zapp_type);
	dev_buffer[22] = get_frnet_type_to_ch(dev_info->znet_type);

	return dev_buffer;
}


void get_zdev_buffer_free(uint8 *p)
{
	osal_mem_free(p);
}

dev_info_t *get_zdev_frame_alloc(uint8 *buffer, int length)
{
	if(length < ZDEVICE_BUFFER_SIZE || length > TR_BUFFER_SIZE)
	{
		return NULL;
	}

	dev_info_t *dev_info = osal_mem_alloc(sizeof(dev_info_t));
	memset(dev_info, 0, sizeof(dev_info_t));
	incode_ctoxs(dev_info->zidentity_no, buffer, 16);
	incode_ctox16(&dev_info->znet_addr, buffer+16);
	dev_info->zapp_type = get_frapp_type_from_str((char *)(buffer+20));
	dev_info->znet_type = get_frnet_type_from_str(buffer[22]);
	dev_info->next = NULL;

	return dev_info;
}


void get_zdev_frame_free(dev_info_t *p)
{	
	osal_mem_free(p);
}


fr_buffer_t *get_gateway_buffer_alloc(gw_info_t *gw_info)
{
	if(gw_info->ip_len > IP_ADDR_MAX_SIZE)
	{
		return NULL;
	}
	
	fr_buffer_t *gw_buffer = osal_mem_alloc(sizeof(fr_buffer_t));
	memset(gw_buffer, 0, sizeof(fr_buffer_t));
	gw_buffer->size = GATEWAY_BUFFER_FIX_SIZE + gw_info->ip_len + 2;
	gw_buffer->data = osal_mem_alloc(gw_buffer->size);
	memset(gw_buffer->data, 0, gw_buffer->size);
	incode_xtocs(gw_buffer->data, gw_info->gw_no, 8);
	get_frapp_type_to_str((char *)(gw_buffer->data+16), gw_info->zapp_type);
	incode_xtoc16(gw_buffer->data+18, gw_info->zpanid);
	incode_xtoc16(gw_buffer->data+22, gw_info->zchannel);
	incode_xtoc32(gw_buffer->data+26, gw_info->rand);
	memcpy(gw_buffer->data+34, gw_info->ipaddr, gw_info->ip_len);
	memcpy(gw_buffer->data+gw_buffer->size-2, "00", 2);
	
	return gw_buffer;
}

int add_zdev_info(gw_info_t *gw_info, dev_info_t *m_dev)
{
	dev_info_t *pre_dev =  NULL;
	dev_info_t *t_dev = gw_info->p_dev;

	if(m_dev == NULL)
	{
		return -1;
	}
	else
	{
		m_dev->next = NULL;
	}

	while(t_dev != NULL)
	{
		if(t_dev->znet_addr != m_dev->znet_addr)
		{
			pre_dev = t_dev;
			t_dev = t_dev->next;
		}
		else
		{
			if(memcmp(t_dev->zidentity_no, m_dev->zidentity_no, 8)
				|| t_dev->zapp_type != m_dev->zapp_type
				|| t_dev->znet_type != m_dev->znet_type)
			{
				memcpy(t_dev->zidentity_no, m_dev->zidentity_no, 8);
				t_dev->zapp_type = m_dev->zapp_type;
				t_dev->znet_type = m_dev->znet_type;
			}

			if(pre_dev != NULL)
			{
				pre_dev->next = t_dev->next;
				t_dev->next = gw_info->p_dev;
				gw_info->p_dev = t_dev;
			}
			
			return 1;
		}
	}

	m_dev->next = gw_info->p_dev;
	gw_info->p_dev = m_dev;

	return 0;
}



dev_info_t *query_zdev_info(gw_info_t *gw_info, uint16 znet_addr)
{
	dev_info_t *t_dev = gw_info->p_dev;


	while(t_dev != NULL)
	{
		if(t_dev->znet_addr != znet_addr)
		{
			t_dev = t_dev->next;
		}
		else
		{
			return t_dev;
		}
	}

	return NULL;
}

int del_zdev_info(gw_info_t *gw_info, uint16 znet_addr)
{
	dev_info_t *pre_dev =  NULL;
	dev_info_t *t_dev = gw_info->p_dev;


	while(t_dev != NULL)
	{
		if(t_dev->znet_addr != znet_addr)
		{
			pre_dev = t_dev;
			t_dev = t_dev->next;
		}
		else
		{
			if(pre_dev != NULL)
			{
				pre_dev->next = t_dev->next;
			}
			else
			{
				gw_info->p_dev = t_dev->next;
			}

			osal_mem_free(t_dev);
			return 0;
		}
	}

	return -1;
}


frhandler_arg_t *get_frhandler_arg_alloc(uint8 *buf, int len)
{
	if(len > MAXSIZE)
	{
		return NULL;
	}

	frhandler_arg_t *arg = osal_mem_alloc(sizeof(frhandler_arg_t));
	memset(arg, 0, sizeof(frhandler_arg_t));
	arg->buf = osal_mem_alloc(len);
	memset(arg->buf, 0, len);
	
	if(buf != NULL)
	{
		memcpy(arg->buf, buf, len);
		arg->len = len;
	}
	else
	{
		osal_mem_free(arg->buf);
		arg->buf = NULL;
		arg->len = 0;
	}

	return arg;
}

void get_frhandler_arg_free(frhandler_arg_t *arg)
{
	if(arg != NULL)
	{
		osal_mem_free(arg->buf);
		osal_mem_free(arg);
	}
}

int add_zdevice_info(dev_info_t *m_dev)
{
	return add_zdev_info(get_gateway_info(), m_dev);
}

dev_info_t *query_zdevice_info(uint16 znet_addr)
{
	return query_zdev_info(get_gateway_info(), znet_addr);
}

dev_info_t *query_zdevice_info_with_sn(zidentify_no_t zidentify_no)
{
	dev_info_t *t_dev = get_gateway_info()->p_dev;


	while(t_dev != NULL)
	{
		if(memcmp(t_dev->zidentity_no, zidentify_no, sizeof(zidentify_no_t)))
		{
			t_dev = t_dev->next;
		}
		else
		{
			return t_dev;
		}
	}

	return NULL;
}

int del_zdevice_info(uint16 znet_addr)
{
	return del_zdev_info(get_gateway_info(), znet_addr);
}


void analysis_zdev_frame(frhandler_arg_t *arg)
{
	if(arg == NULL)
	{
		return;
	}
	
	dev_info_t *dev_info;
	uint16 znet_addr;

	frHeadType_t head_type = get_frhead_from_str((char *)arg->buf);
	
	void *p = get_frame_alloc(head_type, arg->buf, arg->len);

	if(p == NULL)
	{
		return;
	}

	switch(head_type)
	{
	case HEAD_UC:
	{
		UC_t *uc = (UC_t *)p;
		incode_ctoxs(get_gateway_info()->gw_no, uc->ext_addr, 16);
		get_gateway_info()->zapp_type = get_frapp_type_from_str((char *)uc->ed_type);
		incode_ctox16(&(get_gateway_info()->zpanid), uc->panid);
		incode_ctox16(&(get_gateway_info()->zchannel), uc->channel);
		get_gateway_info()->rand = gen_rand(get_gateway_info()->gw_no);
		
		fr_buffer_t *buffer = get_gateway_buffer_alloc(get_gateway_info());
		
		bi_t bi;
		memcpy(bi.sn, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
		bi.trans_type = TRTYPE_UDP_NORMAL;
		bi.fr_type = TRFRAME_PUT_GW;
		bi.data = buffer->data;
		bi.data_len = buffer->size;
		send_frame_udp_request(TRHEAD_BI, &bi);
		
		get_buffer_free(buffer);
		get_frame_free(HEAD_UC, uc);
	}
	break;
		
	case HEAD_UO:
	{
		UO_t *uo = (UO_t *)p;
		dev_info = osal_mem_alloc(sizeof(dev_info_t));
		memset(dev_info, 0, sizeof(dev_info_t));
		incode_ctoxs(dev_info->zidentity_no, uo->ext_addr, 16);
		incode_ctox16(&dev_info->znet_addr, uo->short_addr);
		dev_info->zapp_type = get_frapp_type_from_str((char *)uo->ed_type);
		dev_info->znet_type = get_frnet_type_from_str(uo->type);

		set_zdev_check(dev_info->znet_addr);
		uint16 znet_addr = dev_info->znet_addr;
		
		if(add_zdevice_info(dev_info) != 0)
		{
			osal_mem_free(dev_info);
		}

		dev_info = query_zdevice_info(znet_addr);
		if(dev_info != NULL)
		{
			fr_buffer_t *frbuffer = get_switch_buffer_alloc(HEAD_UO, uo);
		
			ub_t ub;
			memcpy(ub.zidentify_no, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
			memcpy(ub.cidentify_no, _client_no, sizeof(cidentify_no_t));
			ub.trans_type = TRTYPE_UDP_NORMAL;
			ub.tr_info = TRINFO_REDATA;
			ub.data = frbuffer->data;
			ub.data_len = frbuffer->size;

			send_frame_udp_request(TRHEAD_UB, &ub);
			get_buffer_free(frbuffer);
		}
		
		get_frame_free(HEAD_UO, uo);
	}
	break;
		
	case HEAD_UH:
	{
		UH_t *uh = (UH_t *)p;
		incode_ctox16(&znet_addr, uh->short_addr);
		dev_info = query_zdevice_info(znet_addr);
		if(dev_info == NULL)
		{
			uint8 mbuf[16] = {0};
			memcpy(mbuf, "D:/SR/", 6);
			incode_xtoc16(mbuf+6, znet_addr);
			memcpy(mbuf+10, ":O\r\n", 4);
			ConnectorApp_TxHandler(mbuf, 14);
		}
		else
		{
			set_zdev_check(znet_addr);
		}
		get_frame_free(HEAD_UH, uh);
	}
	break;
		
	case HEAD_UR:
	{
		fr_buffer_t *frbuffer = NULL;
		UR_t *ur = (UR_t *)p;
		incode_ctox16(&znet_addr, ur->short_addr);
		
		dev_info = query_zdevice_info(znet_addr);
		if(dev_info == NULL && znet_addr != 0)
		{
			uint8 mbuf[16] = {0};
			memcpy(mbuf, "D:/SR/", 6);
			incode_xtoc16(mbuf+6, znet_addr);
			memcpy(mbuf+10, ":O\r\n", 4);
			ConnectorApp_TxHandler(mbuf, 14);
		}
		else
		{

			frbuffer = get_switch_buffer_alloc(HEAD_UR, ur);
		}

		if(frbuffer != NULL)
		{
			ub_t ub;
			memcpy(ub.zidentify_no, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
			memcpy(ub.cidentify_no, _client_no, sizeof(cidentify_no_t));
			ub.trans_type = TRTYPE_UDP_NORMAL;
			ub.tr_info = TRINFO_REDATA;
			ub.data = frbuffer->data;
			ub.data_len = frbuffer->size;
			send_frame_udp_request(TRHEAD_UB, &ub);

			get_buffer_free(frbuffer);
		}
		get_frame_free(HEAD_UR, ur);
	}
	break;
		
	case HEAD_DE:
	{
		DE_t *de = (DE_t *)p;
		get_frame_free(HEAD_DE, de);
	}
	break;
		
	default: break;
	}

	get_frhandler_arg_free(arg);
}

void analysis_capps_frame(frhandler_arg_t *arg)
{
	if(arg == NULL)
	{
		return;
	}
	
	tr_head_type_t head_type = get_trhead_from_str((char *)arg->buf);
	void *p = get_trframe_alloc(head_type, arg->buf, arg->len);
	
	if(p == NULL)
	{
		return;
	}
	
	switch(head_type)
	{
	case TRHEAD_PI:
	{
		pi_handler(p);
		get_trframe_free(TRHEAD_PI, p);
	}
	break;
		
	case TRHEAD_BI:
	{
		bi_handler(p);
		get_trframe_free(TRHEAD_BI, p);
	}
	break;
	
	case TRHEAD_DC:
	{
		dc_handler(p);
		get_trframe_free(TRHEAD_DC, p);
	}
	break;
		
	case TRHEAD_UB:
	{
		ub_handler(p);
		get_trframe_free(TRHEAD_UB, p);
	}
	break;

	default: break;
	}
}

fr_buffer_t *get_switch_buffer_alloc(frHeadType_t head_type, void *frame)
{
	fr_buffer_t *frbuffer = NULL;
	
	switch(head_type)
	{
	case HEAD_UC:
	{
		frbuffer = get_buffer_alloc(HEAD_UC, (UC_t *)frame);
		break;
	}

	case HEAD_UO:
	{
		frbuffer = get_buffer_alloc(HEAD_UO, (UO_t *)frame);
		break;
	}

	case HEAD_UR:
	{
		frbuffer = get_buffer_alloc(HEAD_UR, (UR_t *)frame);
		break;
	}

	case HEAD_DE:
	{
		frbuffer = get_buffer_alloc(HEAD_DE, (DE_t *)frame);
		break;
	}

	default: break;
	}

	return frbuffer;
}
#endif