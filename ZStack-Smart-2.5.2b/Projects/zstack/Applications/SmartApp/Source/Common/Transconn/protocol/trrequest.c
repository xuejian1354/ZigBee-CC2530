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

#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)
#include "TransconnApp.h"

extern GPRS_States_t gprs_states;
extern uint8 sbuf[FRAME_BUFFER_SIZE];
extern uint8 slen;

void pi_handler(pi_t *pi)
{
	gw_info_t *p_gw = NULL;
	
	switch(pi->trans_type)
	{
	case TRTYPE_UDP_NORMAL:
	case TRTYPE_UDP_TRANS:
		switch(pi->fr_type)
		{
		case TRFRAME_GET:
			p_gw = get_gateway_info();
			p_gw->rand = gen_rand(pi->sn);
			memset(p_gw->ipaddr, 0, sizeof(p_gw->ipaddr));
			memcpy(p_gw->ipaddr, pi->data, pi->data_len);
			p_gw->ip_len = pi->data_len;

			memset(p_gw->serverip_addr, 0, sizeof(p_gw->serverip_addr));
			GET_SERVER_IP((char *)p_gw->serverip_addr);
			p_gw->serverip_len = strlen((char *)p_gw->serverip_addr);
			
			fr_buffer_t *frbuffer = get_gateway_buffer_alloc(p_gw);
			
			bi_t bi;
			memcpy(bi.sn, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
			bi.trans_type = TRTYPE_UDP_TRANS;
			bi.fr_type = TRFRAME_PUT_GW;
			bi.data = frbuffer->data;
			bi.data_len = frbuffer->size;
			send_frame_udp_request(TRHEAD_BI, &bi);
			break;
		}
		break;
	}
}

void bi_handler(bi_t *bi)
{
	gw_info_t *p_gw = NULL;
	
	switch(bi->trans_type)
	{
	case TRTYPE_UDP_NORMAL:
	case TRTYPE_UDP_TRANS:
		switch(bi->fr_type)
		{
		case TRFRAME_REG:
			p_gw = get_gateway_info();
			p_gw->rand = gen_rand(bi->sn);
			memset(p_gw->ipaddr, 0, sizeof(p_gw->ipaddr));
			memcpy(p_gw->ipaddr, bi->data, bi->data_len);
			p_gw->ip_len = bi->data_len;
			
			memset(p_gw->serverip_addr, 0, sizeof(p_gw->serverip_addr));
			GET_SERVER_IP((char *)p_gw->serverip_addr);
			p_gw->serverip_len = strlen((char *)p_gw->serverip_addr);
			break;
		}
		break;
	}
}

void dc_handler(dc_t *dc)
{	
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
		UC_t *uc = (UC_t *)p;
		get_frame_free(HEAD_UC, uc);
	}
	break;
		
	case HEAD_UO:
	{
		UO_t *uo = (UO_t *)p;
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
		get_frame_free(HEAD_UR, ur);
	}
	break;
		
	case HEAD_DE:
	{
		DE_t *de = (DE_t *)p;

		fr_buffer_t *buffer = get_switch_buffer_alloc(HEAD_DE, de);
		ConnectorApp_TxHandler(buffer->data, buffer->size);
		
		get_frame_free(HEAD_DE, de);
	}
	break;
		
	default: break;
	}
	
	return;
}

void ub_handler(ub_t *ub)
{
}

void send_frame_udp_request(tr_head_type_t htype, void *frame)
{
	tr_buffer_t *buffer;
	
	switch(htype)
	{
	case TRHEAD_PI: 
	case TRHEAD_BI: 
	case TRHEAD_DC:
	case TRHEAD_UB:
	{
		if((buffer = get_trbuffer_alloc(htype, frame)) == NULL)
		{
			return;
		}

		//socket_udp_sendto(ipaddr, buffer->data, buffer->size);
		if(gprs_states == GPRS_CONNECT_OK)
		{
			char tbuf[64] = {0};
			
			slen = buffer->size;
			osal_memcpy(sbuf, buffer->data, buffer->size);
			
			sprintf(tbuf, "AT+CIPSEND\r\n");
			HalUARTWrite(SERIAL_COM_PORT, (uint8 *)tbuf, strlen(tbuf));

		}
		get_trbuffer_free(buffer);	
	}
	break;
	}
}
#endif