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
extern uint8 _common_no[8];

static gw_info_t gw_info;
static fr_buffer_t frbuffer = 
{
	{0},
	0,
};

gw_info_t *get_gateway_info(void)
{
	return &gw_info;
}

fr_buffer_t *get_gateway_buffer_alloc(gw_info_t *gw_info)
{
	if(gw_info->ip_len > IP_ADDR_MAX_SIZE)
	{
		return NULL;
	}
	
	fr_buffer_t *gw_buffer = &frbuffer;
	memset(gw_buffer, 0, sizeof(fr_buffer_t));
	
	gw_buffer->size = GATEWAY_BUFFER_FIX_SIZE + gw_info->ip_len + 2;
	
	incode_xtocs(gw_buffer->data, gw_info->gw_no, 8);
	get_frapp_type_to_str((char *)(gw_buffer->data+16), gw_info->zapp_type);
	incode_xtoc16(gw_buffer->data+18, gw_info->zpanid);
	incode_xtoc16(gw_buffer->data+22, gw_info->zchannel);
	incode_xtoc32(gw_buffer->data+26, gw_info->rand);
	memcpy(gw_buffer->data+34, gw_info->ipaddr, gw_info->ip_len);
	memcpy(gw_buffer->data+gw_buffer->size-2, "00", 2);
	
	return gw_buffer;
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


void analysis_zdev_frame(frhandler_arg_t *arg)
{
	if(arg == NULL)
	{
		return;
	}

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
		bi.trans_type = TRTYPE_UDP_TRANS;
		bi.fr_type = TRFRAME_PUT_GW;
		bi.data = buffer->data;
		bi.data_len = buffer->size;
		send_frame_udp_request(TRHEAD_BI, &bi);
		
		get_frame_free(HEAD_UC, uc);
	}
	break;
		
	case HEAD_UO:
	{
		UO_t *uo = (UO_t *)p;

		uint8 *dev_buffer = (uint8 *)osal_mem_alloc(ZDEVICE_BUFFER_SIZE);
		osal_memcpy(dev_buffer, uo->ext_addr, 16);
		osal_memcpy(dev_buffer+16, uo->short_addr, 4);
		osal_memcpy(dev_buffer+20, uo->ed_type, 2);
		dev_buffer[22] = uo->type;

		bi_t bi;
		memcpy(bi.sn, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
		bi.trans_type = TRTYPE_UDP_TRANS;
		bi.fr_type = TRFRAME_PUT_DEV;
		bi.data = dev_buffer;
		bi.data_len = ZDEVICE_BUFFER_SIZE;
		send_frame_udp_request(TRHEAD_BI, &bi);
		
		osal_mem_free(dev_buffer);
		get_frame_free(HEAD_UO, uo);
	}
	break;
		
	case HEAD_UH:
	{
		UH_t *uh = (UH_t *)p;
		get_frame_free(HEAD_UH, uh);
	}
	break;
		
	case HEAD_UR:
	{
		UR_t *ur = (UR_t *)p;

		ub_t ub;
		
		if(ur->data_len >= EXT_ADDR_SIZE)
		{
			incode_ctoxs(ub.zidentify_no, 
                                        ur->data+ur->data_len-EXT_ADDR_SIZE, 
                                        EXT_ADDR_SIZE);
		}
		else
		{
			memcpy(ub.zidentify_no, 
					get_gateway_info()->gw_no, 
					sizeof(zidentify_no_t));
		}

		fr_buffer_t *frbuffer = get_switch_buffer_alloc(HEAD_UR, ur);

		memcpy(ub.cidentify_no, _common_no, sizeof(cidentify_no_t));
		ub.trans_type = TRTYPE_UDP_TRANS;
		ub.tr_info = TRINFO_REDATA;
		ub.data = frbuffer->data;
		ub.data_len = frbuffer->size;
		send_frame_udp_request(TRHEAD_UB, &ub);
			
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
	if(frame == NULL)
	{
		return NULL;
	}

	memset(&frbuffer, 0, sizeof(fr_buffer_t));
	
	if(head_type == HEAD_UC)
	{
		UC_t *p_uc = (UC_t *)frame;
		if(p_uc->data_len > FRAME_DATA_SIZE)
		{
			return NULL;
		}
		frbuffer.size = FR_UC_DATA_FIX_LEN+p_uc->data_len;
		
		memcpy(frbuffer.data, p_uc->head, 3);
		frbuffer.data[3] = p_uc->type;
		memcpy(frbuffer.data+4, p_uc->ed_type, 2);
		memcpy(frbuffer.data+6, p_uc->short_addr, 4);
		memcpy(frbuffer.data+10, p_uc->ext_addr, 16);
		memcpy(frbuffer.data+26, p_uc->panid, 4);
		memcpy(frbuffer.data+30, p_uc->channel, 4);
		memcpy(frbuffer.data+34, p_uc->data, p_uc->data_len);
		memcpy(frbuffer.data+34+p_uc->data_len, p_uc->tail, 4);
	}
	else if(head_type == HEAD_UO)
	{
		UO_t *p_uo = (UO_t *)frame;
		if(p_uo->data_len > FRAME_DATA_SIZE)
		{
			return NULL;
		}
		frbuffer.size = FR_UO_DATA_FIX_LEN+p_uo->data_len;
		
		memcpy(frbuffer.data, p_uo->head, 3);
		frbuffer.data[3] = p_uo->type;
		memcpy(frbuffer.data+4, p_uo->ed_type, 2);
		memcpy(frbuffer.data+6, p_uo->short_addr, 4);
		memcpy(frbuffer.data+10, p_uo->ext_addr, 16);
		memcpy(frbuffer.data+26, p_uo->data, p_uo->data_len);
		memcpy(frbuffer.data+26+p_uo->data_len, p_uo->tail, 4);
	}
	else if(head_type == HEAD_UH)
	{
		UH_t *p_uh = (UH_t *)frame;
		frbuffer.size = FR_UH_DATA_FIX_LEN;
		
		memcpy(frbuffer.data, p_uh->head, 3);
		memcpy(frbuffer.data+3, p_uh->short_addr, 4);
		memcpy(frbuffer.data+7, p_uh->tail, 4);
	}
	else if(head_type == HEAD_UR)
	{
		UR_t *p_ur = (UR_t *)frame;
		if(p_ur->data_len > FRAME_DATA_SIZE)
		{
			return NULL;
		}

		if(p_ur->data_len >= EXT_ADDR_SIZE)
		{
			p_ur->data_len -= EXT_ADDR_SIZE;
		}
		frbuffer.size = FR_UR_DATA_FIX_LEN+p_ur->data_len;
		
		memcpy(frbuffer.data, p_ur->head, 3);
		frbuffer.data[3] = p_ur->type;
		memcpy(frbuffer.data+4, p_ur->ed_type, 2);
		memcpy(frbuffer.data+6, p_ur->short_addr, 4);
		memcpy(frbuffer.data+10, p_ur->data, p_ur->data_len);
		memcpy(frbuffer.data+10+p_ur->data_len, p_ur->tail, 4);
	}
	else if(head_type == HEAD_DE)
	{
		DE_t *p_de = (DE_t *)frame;
		if(p_de->data_len > FRAME_DATA_SIZE)
		{
			return NULL; 
		}
		frbuffer.size = FR_DE_DATA_FIX_LEN+p_de->data_len;
		
		memcpy(frbuffer.data, p_de->head, 2);
		memcpy(frbuffer.data+2, p_de->cmd, 4);
		memcpy(frbuffer.data+6, p_de->short_addr, 4);
		memcpy(frbuffer.data+10, p_de->data, p_de->data_len);
		memcpy(frbuffer.data+10+p_de->data_len, p_de->tail, 4);
	}

	return &frbuffer;
}
#endif