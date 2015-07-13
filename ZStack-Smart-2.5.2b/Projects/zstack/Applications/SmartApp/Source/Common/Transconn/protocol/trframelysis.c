/*
 * trframelysis.c
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

#include "trframelysis.h"
#include "mincode.h"
#include "OSAL.h"
#include <string.h>

#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)
tr_head_type_t get_trhead_from_str(char *head)
{
	if(!strncmp(TR_HEAD_PI, head, 3))
	{
		return TRHEAD_PI;
	}
	else if(!strncmp(TR_HEAD_BI, head, 3))
	{
		return TRHEAD_BI;
	}
	else if(!strncmp(TR_HEAD_DC, head, 3))
	{
		return TRHEAD_DC;
	}
	else if(!strncmp(TR_HEAD_UB, head, 2))
	{
		return TRHEAD_UB;
	}
	
	return TRHEAD_NONE;
}

tr_trans_type_t get_trtrans_from_ch(char trans)
{
	switch(trans)
	{
	case TR_TYPE_UDP_NORMAL:
		return TRTYPE_UDP_NORMAL;

	case TR_TYPE_UDP_TRAVERSAL:
		return TRTYPE_UDP_TRAVERSAL;

	case TR_TYPE_TCP_LONG:
		return TRTYPE_TCP_LONG;

	case TR_TYPE_TCP_SHORT:
		return TRTYPE_TCP_SHORT;

	default:
		break;
	}

	return TRTYPE_NONE;
}

char get_trtrans_to_ch(tr_trans_type_t trans)
{
	switch(trans)
	{
	case TRTYPE_UDP_NORMAL:
		return TR_TYPE_UDP_NORMAL;

	case TRTYPE_UDP_TRAVERSAL:
		return TR_TYPE_UDP_TRAVERSAL;

	case TRTYPE_TCP_LONG:
		return TR_TYPE_TCP_LONG;

	case TRTYPE_TCP_SHORT:
		return TR_TYPE_TCP_SHORT;

	default:
		break;
	}

	return '0';
}

tr_frame_type_t get_trframe_from_ch(char trfra)
{
	switch(trfra)
	{
	case TR_FRAME_CON:
		return TRFRAME_CON;

	case TR_FRAME_REG:
		return TRFRAME_REG;

	case TR_FRAME_GET:
		return TRFRAME_GET;

	case TR_FRAME_PUT_GW:
		return TRFRAME_PUT_GW;

	case TR_FRAME_PUT_DEV:
		return TRFRAME_PUT_DEV;

	default:
		break;
	}

	return TRFRAME_NONE;
}

char get_trframe_to_ch(tr_frame_type_t trfra)
{
	switch(trfra)
	{
	case TRFRAME_CON:
		return TR_FRAME_CON;

	case TRFRAME_REG:
		return TR_FRAME_REG;

	case TRFRAME_GET:
		return TR_FRAME_GET;

	case TRFRAME_PUT_GW:
		return TR_FRAME_PUT_GW;

	case TRFRAME_PUT_DEV:
		return TR_FRAME_PUT_DEV;

	default:
		break;
	}

	return '0';
}

tr_info_type_t get_trinfo_from_ch(char trinfo)
{
	switch(trinfo)
	{
	case TR_INFO_IP:
		return TRINFO_IP;

	case TR_INFO_DATA:
		return TRINFO_DATA;

	case TR_INFO_UPDATE:
		return TRINFO_UPDATE;

	case TR_INFO_CUT:
		return TRINFO_CUT;

	case TR_INFO_REG:
		return TRINFO_REG;

	case TR_INFO_HOLD:
		return TRINFO_HOLD;

	case TR_INFO_FOUND:
		return TRINFO_FOUND;

	case TR_INFO_DISMATCH:
		return TRINFO_DISMATCH;

	case TR_INFO_CONTROL:
		return TRINFO_CONTROL;

	case TR_INFO_QUERY:
		return TRINFO_QUERY;

	case TR_INFO_REDATA:
		return TRINFO_REDATA;

	default:
		break;
	}

	return TRINFO_NONE;
}

char get_trinfo_to_ch(tr_info_type_t trinfo)
{
	switch(trinfo)
	{
	case TRINFO_IP:
		return TR_INFO_IP;

	case TRINFO_DATA:
		return TR_INFO_DATA;

	case TRINFO_UPDATE:
		return TR_INFO_UPDATE;

	case TRINFO_CUT:
		return TR_INFO_CUT;

	case TRINFO_REG:
		return TR_INFO_REG;

	case TRINFO_HOLD:
		return TR_INFO_HOLD;

	case TRINFO_FOUND:
		return TR_INFO_FOUND;

	case TRINFO_DISMATCH:
		return TR_INFO_DISMATCH;

	case TRINFO_CONTROL:
		return TR_INFO_CONTROL;

	case TRINFO_QUERY:
		return TR_INFO_QUERY;

	case TRINFO_REDATA:
		return TR_INFO_REDATA;

	default:
		break;
	}

	return TR_INFO_NONE;
}

void *get_trframe_alloc(tr_head_type_t head_type, uint8 buffer[], int length)
{ 
	if(length > TR_BUFFER_SIZE)
	{
		goto tr_analysis_err;
	}
	
 	switch(head_type)
	{
	case TRHEAD_PI: 
		if(length>=TR_PI_DATA_FIX_LEN && !memcmp(buffer, TR_HEAD_PI, 3)
			&& !memcmp(buffer+length-4, TR_TAIL, 4))
		{
			pi_t *pi = osal_mem_alloc(sizeof(pi_t));
			memset(pi, 0, sizeof(pi_t));
			incode_ctoxs(pi->sn, buffer+3, 16);
			pi->trans_type = get_trtrans_from_ch(buffer[19]);
			pi->fr_type = get_trframe_from_ch(buffer[20]);

			pi->data_len = 0;
			pi->data = NULL;

			return (void *)pi;
		}
		else { goto  tr_analysis_err;}
		
	case TRHEAD_BI:
		if(length>=TR_BI_DATA_FIX_LEN && !memcmp(buffer, TR_HEAD_BI, 3)
			&& !memcmp(buffer+length-4, TR_TAIL, 4))
		{
			bi_t *bi = osal_mem_alloc(sizeof(bi_t));
			memset(bi, 0, sizeof(bi_t));
			incode_ctoxs(bi->sn, buffer+3, 16);
			bi->trans_type = get_trtrans_from_ch(buffer[19]);
			bi->fr_type = get_trframe_from_ch(buffer[20]);

			bi->data_len = 0;
			bi->data = NULL;

			return (void *)bi;
		}
		else { goto  tr_analysis_err;}
		
	case TRHEAD_DC:
		if(length>=TR_DC_DATA_FIX_LEN && !memcmp(buffer, TR_HEAD_DC, 3)
			&& !memcmp(buffer+length-4, TR_TAIL, 4))
		{
			dc_t *dc = osal_mem_alloc(sizeof(dc_t));
			memset(dc, 0, sizeof(dc_t));
			incode_ctoxs(dc->zidentify_no, buffer+3, 16);
			incode_ctoxs(dc->cidentify_no, buffer+19, 16);
			dc->trans_type = get_trtrans_from_ch(buffer[35]);
			dc->tr_info = get_trinfo_from_ch(buffer[36]);
			if(length-TR_DC_DATA_FIX_LEN > 0)
			{
				uint8 *data_buffer = osal_mem_alloc(length-TR_DC_DATA_FIX_LEN);
				memset(data_buffer, 0, length-TR_DC_DATA_FIX_LEN);
				memcpy(data_buffer, buffer+37, length-TR_DC_DATA_FIX_LEN);
				dc->data_len = length-TR_DC_DATA_FIX_LEN;
				dc->data = data_buffer;
			}
			else
			{
				dc->data_len = 0;
				dc->data = NULL;
			}
			
			return (void *)dc;
		}
		else { goto  tr_analysis_err;}
		
	case TRHEAD_UB:
		if(length>=TR_UB_DATA_FIX_LEN && !memcmp(buffer, TR_HEAD_UB, 3)
			&& !memcmp(buffer+length-4, TR_TAIL, 4))
		{
			ub_t *ub = osal_mem_alloc(sizeof(ub_t));
			memset(ub, 0, sizeof(ub_t));
			incode_ctoxs(ub->zidentify_no, buffer+3, 16);
			incode_ctoxs(ub->cidentify_no, buffer+19, 16);
			ub->trans_type = get_trtrans_from_ch(buffer[35]);
			ub->tr_info = get_trinfo_from_ch(buffer[36]);
			if(length-TR_UB_DATA_FIX_LEN > 0)
			{
				uint8 *data_buffer = osal_mem_alloc(length-TR_UB_DATA_FIX_LEN);
				memset(data_buffer, 0, length-TR_UB_DATA_FIX_LEN);
				memcpy(data_buffer, buffer+37, length-TR_UB_DATA_FIX_LEN);
				ub->data_len = length-TR_UB_DATA_FIX_LEN;
				ub->data = data_buffer;
			}
			else
			{
				ub->data_len = 0;
				ub->data = NULL;
			}

			return (void *)ub;
		}
		else { goto  tr_analysis_err;}

	default: goto  tr_analysis_err;
	}

tr_analysis_err:
	return NULL;
}

void get_trframe_free(tr_head_type_t head_type, void *p)
{
	switch(head_type)
	{
	case TRHEAD_PI:
	{	
		pi_t *p_pi = (pi_t *)p;
		if(p_pi != NULL && p_pi->data != NULL)
		{
			osal_mem_free(p_pi->data);
		}
		osal_mem_free(p_pi);
	}
	break;
		
	case TRHEAD_BI:
	{
		bi_t *p_bi = (bi_t *)p;
		if(p_bi != NULL && p_bi->data != NULL)
		{
			osal_mem_free(p_bi->data);
		}
		osal_mem_free(p);
	}
	break;
                
	case TRHEAD_DC:
	{
		dc_t *p_dc = (dc_t *)p;
		if(p_dc != NULL && p_dc->data != NULL)
		{
			osal_mem_free(p_dc->data);
		}
		osal_mem_free(p);
	}
	break;
		
	case TRHEAD_UB:
	{
		ub_t *p_ub = (ub_t *)p;
		if(p_ub != NULL && p_ub->data != NULL)
		{
			osal_mem_free(p_ub->data);
		}
		osal_mem_free(p);
	}
	break;
		
	default: break;
	}
}


tr_buffer_t *get_trbuffer_alloc(tr_head_type_t type, void *frame)
{
	pi_t *p_pi; bi_t *p_bi;
        dc_t *p_dc; ub_t *p_ub;
	
	tr_buffer_t *frame_buffer;
        int buffer_len;
	
	if(frame == NULL)
		goto  tr_package_err;
	
	switch(type)
	{
	case TRHEAD_PI: 
		p_pi = (pi_t *)frame;
		frame_buffer = osal_mem_alloc(sizeof(tr_buffer_t));
		memset(frame_buffer, 0, sizeof(tr_buffer_t));
		frame_buffer->size = TR_PI_DATA_FIX_LEN+p_pi->data_len;
        buffer_len = frame_buffer->size;
		if(buffer_len > TR_BUFFER_SIZE)
		{
			osal_mem_free(frame_buffer);
			goto tr_package_err;
		}
		frame_buffer->data = osal_mem_alloc(frame_buffer->size);
		memset(frame_buffer->data, 0, frame_buffer->size);
		
		memcpy(frame_buffer->data, TR_HEAD_PI, 3);
		incode_xtocs(frame_buffer->data+3, p_pi->sn, 8);
		frame_buffer->data[19] = get_trtrans_to_ch(p_pi->trans_type);
		frame_buffer->data[20] = get_trframe_to_ch(p_pi->fr_type);
		memcpy(frame_buffer->data+21, p_pi->data, p_pi->data_len);
		memcpy(frame_buffer->data+frame_buffer->size-4, TR_TAIL, 4);

		return frame_buffer;
		
	case TRHEAD_BI:
		p_bi = (bi_t *)frame;
		frame_buffer = osal_mem_alloc(sizeof(tr_buffer_t));
		memset(frame_buffer, 0, sizeof(tr_buffer_t));
		frame_buffer->size = TR_BI_DATA_FIX_LEN+p_bi->data_len;
		buffer_len = frame_buffer->size;
		if(buffer_len > TR_BUFFER_SIZE)
		{
			osal_mem_free(frame_buffer);
			goto tr_package_err;
		}
		frame_buffer->data = osal_mem_alloc(frame_buffer->size);
		memset(frame_buffer->data, 0, frame_buffer->size);
		
		memcpy(frame_buffer->data, TR_HEAD_BI, 3);
		incode_xtocs(frame_buffer->data+3, p_bi->sn, 8);
		frame_buffer->data[19] = get_trtrans_to_ch(p_bi->trans_type);
		frame_buffer->data[20] = get_trframe_to_ch(p_bi->fr_type);
		memcpy(frame_buffer->data+21, p_bi->data, p_bi->data_len);
		memcpy(frame_buffer->data+frame_buffer->size-4, TR_TAIL, 4);

		return frame_buffer;
		
	case TRHEAD_DC:
		p_dc = (dc_t *)frame;
		frame_buffer = osal_mem_alloc(sizeof(tr_buffer_t));
		memset(frame_buffer, 0, sizeof(tr_buffer_t));
		frame_buffer->size = TR_DC_DATA_FIX_LEN+p_dc->data_len;
		buffer_len = frame_buffer->size;
		if(buffer_len > TR_BUFFER_SIZE)
		{
			osal_mem_free(frame_buffer);
			return NULL;
		}
		frame_buffer->data = osal_mem_alloc(frame_buffer->size);
		memset(frame_buffer->data, 0, frame_buffer->size);
		
		memcpy(frame_buffer->data, TR_HEAD_DC, 3);
		incode_xtocs(frame_buffer->data+3, p_dc->zidentify_no, 8);
		incode_xtocs(frame_buffer->data+19, p_dc->cidentify_no, 8);
		frame_buffer->data[35] = get_trtrans_to_ch(p_dc->trans_type);
		frame_buffer->data[36] = get_trinfo_to_ch(p_dc->tr_info);
		memcpy(frame_buffer->data+37, p_dc->data, p_dc->data_len);
		memcpy(frame_buffer->data+frame_buffer->size-4, TR_TAIL, 4);

		return frame_buffer;
		
	case TRHEAD_UB:
		p_ub = (ub_t *)frame;
		frame_buffer = osal_mem_alloc(sizeof(tr_buffer_t));
		memset(frame_buffer, 0, sizeof(tr_buffer_t));
		frame_buffer->size = TR_UB_DATA_FIX_LEN+p_ub->data_len;
		buffer_len = frame_buffer->size;
		if(buffer_len > TR_BUFFER_SIZE)
		{
			osal_mem_free(frame_buffer);
			return NULL;
		}
		frame_buffer->data = osal_mem_alloc(frame_buffer->size);
		memset(frame_buffer->data, 0, frame_buffer->size);
		
		memcpy(frame_buffer->data, TR_HEAD_UB, 3);
		incode_xtocs(frame_buffer->data+3, p_ub->zidentify_no, 8);
		incode_xtocs(frame_buffer->data+19, p_ub->cidentify_no, 8);
		frame_buffer->data[35] = get_trtrans_to_ch(p_ub->trans_type);
		frame_buffer->data[36] = get_trinfo_to_ch(p_ub->tr_info);
		memcpy(frame_buffer->data+37, p_ub->data, p_ub->data_len);
		memcpy(frame_buffer->data+frame_buffer->size-4, TR_TAIL, 4);

		return frame_buffer;
	
	default: goto  tr_package_err;
	}

tr_package_err:
	return NULL;
}


void get_trbuffer_free(tr_buffer_t *p)
{
	osal_mem_free(p->data);
	osal_mem_free(p);
}
#endif