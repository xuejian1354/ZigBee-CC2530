/*
 * framelysis.c
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
#include "framelysis.h"

#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)

frHeadType_t get_frhead_from_str(char *head)
{
	if(!strncmp(FR_HEAD_UC, head, 3))
	{
		return HEAD_UC;
	}
	else if(!strncmp(FR_HEAD_UO, head, 3))
	{
		return HEAD_UO;
	}
	else if(!strncmp(FR_HEAD_UH, head, 3))
	{
		return HEAD_UH;
	}
	else if(!strncmp(FR_HEAD_UR, head, 3))
	{
		return HEAD_UR;
	}
	else if(!strncmp(FR_HEAD_DE, head, 2))
	{
		return HEAD_DE;
	}
	
	return HEAD_NONE;
}

int get_frhead_to_str(char *dst, frHeadType_t head_type)
{
	switch(head_type)
	{
	case HEAD_UC:
		strcpy(dst, FR_HEAD_UC);
		break;

	case HEAD_UO:
		strcpy(dst, FR_HEAD_UO);
		break;

	case HEAD_UH:
		strcpy(dst, FR_HEAD_UH);
		break;

	case HEAD_UR:
		strcpy(dst, FR_HEAD_UR);
		break;

	case HEAD_DE:
		strcpy(dst, FR_HEAD_DE);
		break;

	default: return -1;
	}

	return 0;
}
fr_app_type_t get_frapp_type_from_str(char *app_type)
{
	if(!strncmp(FR_APP_CONNECTOR, app_type, 2))
	{
		return FRAPP_CONNECTOR;
	}
	else if(!strncmp(FR_APP_LIGHTSWITCH_ONE, app_type, 2))
	{
		return FRAPP_LIGHTSWITCH_ONE;
	}
	else if(!strncmp(FR_APP_LIGHTSWITCH_TWO, app_type, 2))
	{
		return FRAPP_LIGHTSWITCH_TWO;
	}
	else if(!strncmp(FR_APP_LIGHTSWITCH_THREE, app_type, 2))
	{
		return  FRAPP_LIGHTSWITCH_THREE;
	}
	else if(!strncmp(FR_APP_LIGHTSWITCH_FOUR, app_type, 2))
	{
		return FRAPP_LIGHTSWITCH_FOUR;
	}
	else if(!strncmp(FR_APP_HUELIGHT, app_type, 2))
	{
		return FRAPP_HUELIGHT;
	}
	else if(!strncmp(FR_APP_ALARM, app_type, 2))
	{
		return FRAPP_ALARM;
	}
	else if(!strncmp(FR_APP_IR_DETECTION, app_type, 2))
	{
		return FRAPP_IR_DETECTION;
	}
	else if(!strncmp(FR_APP_SUPER_BUTTON, app_type, 2))
	{
		return FRAPP_SUPER_BUTTON;
	}
	else if(!strncmp(FR_APP_DOOR_SENSOR, app_type, 2))
	{
		return FRAPP_DOOR_SENSOR;
	}
	else if(!strncmp(FR_APP_ENVDETECTION, app_type, 2))
	{
		return FRAPP_ENVDETECTION;
	}
	else if(!strncmp(FR_APP_IR_RELAY, app_type, 2))
	{
		return FRAPP_IR_RELAY;
	}
	else if(!strncmp(FR_APP_AIRCONTROLLER, app_type, 2))
	{
		return FRAPP_AIRCONTROLLER;
	}
	else if(!strncmp(FR_APP_HUMITURE_DETECTION, app_type, 2))
	{
		return FRAPP_HUMITURE_DETECTION;
	}
	else if(!strncmp(FR_APP_SOLENOID_VALVE, app_type, 2))
	{
		return FRAPP_SOLENOID_VALVE;
	}

	return FRAPP_NONE;
}

int get_frapp_type_to_str(char *dst, fr_app_type_t app_type)
{
	switch(app_type)
	{
	case FRAPP_CONNECTOR:
		strcpy(dst, FR_APP_CONNECTOR);
		break;

	case FRAPP_LIGHTSWITCH_ONE:
		strcpy(dst, FR_APP_LIGHTSWITCH_ONE);
		break;

	case FRAPP_LIGHTSWITCH_TWO:
		strcpy(dst, FR_APP_LIGHTSWITCH_TWO);
		break;

	case FRAPP_LIGHTSWITCH_THREE:
		strcpy(dst, FR_APP_LIGHTSWITCH_THREE);
		break;

	case FRAPP_LIGHTSWITCH_FOUR:
		strcpy(dst, FR_APP_LIGHTSWITCH_FOUR);
		break;

	case FRAPP_HUELIGHT:
		strcpy(dst, FR_APP_HUELIGHT);
		break;

	case FRAPP_ALARM:
		strcpy(dst, FR_APP_ALARM);
		break;

	case FRAPP_IR_DETECTION:
		strcpy(dst, FR_APP_IR_DETECTION);
		break;

	case FRAPP_DOOR_SENSOR:
		strcpy(dst, FR_APP_DOOR_SENSOR);
		break;

	case FRAPP_ENVDETECTION:
		strcpy(dst, FR_APP_ENVDETECTION);
		break;

	case FRAPP_IR_RELAY:
		strcpy(dst, FR_APP_IR_RELAY);
		break;

	case FRAPP_SUPER_BUTTON:
		strcpy(dst, FR_APP_SUPER_BUTTON);
		break;

	case FRAPP_AIRCONTROLLER:
		strcpy(dst, FR_APP_AIRCONTROLLER);
		break;

	case FRAPP_HUMITURE_DETECTION:
		strcpy(dst, FR_APP_HUMITURE_DETECTION);
		break;

	case FRAPP_SOLENOID_VALVE:
		strcpy(dst, FR_APP_SOLENOID_VALVE);
		break;

	default:
		strcpy(dst, "FF");
		return -1;
	}

	return 0;
}

fr_net_type_t get_frnet_type_from_str(char net_type)
{
	if(FR_DEV_ROUTER == net_type)
	{
		return FRNET_ROUTER;
	}
	else if(FR_DEV_ENDDEV == net_type)
	{
		return FRNET_ENDDEV;
	}

	return FRNET_NONE;
}

char get_frnet_type_to_ch(fr_net_type_t net_type)
{
	switch(net_type)
	{
	case FRNET_ROUTER:
		return FR_DEV_ROUTER;

	case FRNET_ENDDEV:
		return FR_DEV_ENDDEV;
	}

	return 'F';
}

void *get_frame_alloc(frHeadType_t htype, uint8 buffer[], int length)
{ 
	if(length > FRAME_BUFFER_SIZE)
	{
		goto fr_analysis_err;
	}
	
 	switch(htype)
	{
	case HEAD_UC: 
		if(length>=FR_UC_DATA_FIX_LEN && !memcmp(buffer, FR_HEAD_UC, 3)
			&& !memcmp(buffer+length-4, FR_TAIL, 4))
		{
			UC_t *uc = osal_mem_alloc(sizeof(UC_t));
			memset(uc, 0, sizeof(UC_t));
			memcpy(uc->head, buffer, 3);
			uc->type = buffer[3];
			memcpy(uc->ed_type, buffer+4, 2);
			memcpy(uc->short_addr, buffer+6, 4);
			memcpy(uc->ext_addr, buffer+10, 16);
			memcpy(uc->panid, buffer+26, 4);
			memcpy(uc->channel, buffer+30, 4);
	
			if(length-FR_UC_DATA_FIX_LEN > 0)
			{
				uint8 *data_buffer = osal_mem_alloc(length-FR_UC_DATA_FIX_LEN);
				memset(data_buffer, 0, length-FR_UC_DATA_FIX_LEN);
				memcpy(data_buffer, buffer+34, length-FR_UC_DATA_FIX_LEN);
				uc->data_len = length-FR_UC_DATA_FIX_LEN;
				uc->data = data_buffer;
			}
			else
			{
				uc->data_len = 0;
				uc->data = NULL;
			}
			
			memcpy(uc->tail, buffer+length-4, 4);

			return (void *)uc;
		}
		else { goto  fr_analysis_err;}
		
	case HEAD_UO: 
		if(length>=FR_UO_DATA_FIX_LEN && !memcmp(buffer, FR_HEAD_UO, 3)
			&& !memcmp(buffer+length-4, FR_TAIL, 4))
		{
			UO_t *uo = osal_mem_alloc(sizeof(UO_t));
			memset(uo, 0, sizeof(UO_t));
			memcpy(uo->head, buffer, 3);
			uo->type = buffer[3];
			memcpy(uo->ed_type, buffer+4, 2);
			memcpy(uo->short_addr, buffer+6, 4);
			memcpy(uo->ext_addr, buffer+10, 16);

			if(length-FR_UO_DATA_FIX_LEN > 0)
			{
				uint8 *data_buffer = osal_mem_alloc(length-FR_UO_DATA_FIX_LEN);
				memset(data_buffer, 0, length-FR_UO_DATA_FIX_LEN);
				memcpy(data_buffer, buffer+26, length-FR_UO_DATA_FIX_LEN);
				uo->data_len = length-FR_UO_DATA_FIX_LEN;
				uo->data = data_buffer;
			}
			else
			{
				uo->data_len = 0;
				uo->data = NULL;
			}
			
			memcpy(uo->tail, buffer+length-4, 4);

			return (void *)uo;
		}
		else { goto  fr_analysis_err;}
		
	case HEAD_UH: 
		if(length>=FR_UH_DATA_FIX_LEN && !memcmp(buffer, FR_HEAD_UH, 3)
			&& !memcmp(buffer+7, FR_TAIL, 4))
		{
			UH_t *uh = osal_mem_alloc(sizeof(UH_t));
			memset(uh, 0, sizeof(UH_t));
			memcpy(uh->head, buffer, 3);
			memcpy(uh->short_addr, buffer+3, 4);
			memcpy(uh->tail, buffer+7, 4);;

			return (void *)uh;
		}
		else { goto  fr_analysis_err;}
		
	case HEAD_UR: 
		if(length>=FR_UR_DATA_FIX_LEN && !memcmp(buffer, FR_HEAD_UR, 3)
			&& !memcmp(buffer+length-4, FR_TAIL, 4))
		{
			UR_t *ur = osal_mem_alloc(sizeof(UR_t));
			memset(ur, 0, sizeof(UR_t));
			memcpy(ur->head, buffer, 3);
			ur->type = buffer[3];
			memcpy(ur->ed_type, buffer+4, 2);
			memcpy(ur->short_addr, buffer+6, 4);

			if(length-FR_UR_DATA_FIX_LEN > 0)
			{
				uint8 *data_buffer = osal_mem_alloc(length-FR_UR_DATA_FIX_LEN);
				memset(data_buffer, 0, length-FR_UR_DATA_FIX_LEN);
				memcpy(data_buffer, buffer+10, length-FR_UR_DATA_FIX_LEN);
				ur->data_len = length-FR_UR_DATA_FIX_LEN;
				ur->data = data_buffer;
			}
			else
			{
				ur->data_len = 0;
				ur->data = NULL;
			}
			
			memcpy(ur->tail, buffer+length-4, 4);

			return (void *)ur;
		}
		else { goto  fr_analysis_err;}
	
	case HEAD_DE: 
		if(length>=FR_DE_DATA_FIX_LEN && !memcmp(buffer, FR_HEAD_DE, 2)
			&& !memcmp(buffer+length-4, FR_TAIL, 4))
		{
			DE_t *de = osal_mem_alloc(sizeof(DE_t));
			memset(de, 0, sizeof(DE_t));
			memcpy(de->head, buffer, 2);
			memcpy(de->cmd, buffer+2, 4);
			memcpy(de->short_addr, buffer+6, 4);

			if(length-FR_DE_DATA_FIX_LEN > 0)
			{
				uint8 *data_buffer = osal_mem_alloc(length-FR_DE_DATA_FIX_LEN);
				memset(data_buffer, 0, length-FR_DE_DATA_FIX_LEN);
				memcpy(data_buffer, buffer+10, length-FR_DE_DATA_FIX_LEN);
				de->data_len = length-FR_DE_DATA_FIX_LEN;
				de->data = data_buffer;
			}
			else
			{
				de->data_len = 0;
				de->data = NULL;
			}
			
			memcpy(de->tail, buffer+length-4, 4);

			return (void *)de;
		}
		else { goto  fr_analysis_err;}

	default: goto  fr_analysis_err;
	}

fr_analysis_err:
	return NULL;
}


void get_frame_free(frHeadType_t htype, void *p)
{
	if(p == NULL)
	{
		return;
	}
	
	switch(htype)
	{
	case HEAD_UC: 
	{
		UC_t *p_uc = (UC_t *)p;
		if(p_uc != NULL && p_uc->data != NULL)
		{
			osal_mem_free(p_uc->data);
		}
		osal_mem_free(p);
	}
	break;
		
	case HEAD_UO: 
	{
		UO_t *p_uo = (UO_t *)p;
		if(p_uo != NULL && p_uo->data != NULL)
		{
			osal_mem_free(p_uo->data);
		}
		osal_mem_free(p);
	}
	break;
		
	case HEAD_UH: 
	{
		osal_mem_free(p);
	}
	break;
		
	case HEAD_UR: 
	{
		UR_t *p_ur = (UR_t *)p;
		if(p_ur != NULL && p_ur->data != NULL)
		{
			osal_mem_free(p_ur->data);
		}
		osal_mem_free(p);
	}
	break;
	
	case HEAD_DE: 
	{
		DE_t *p_de = (DE_t *)p;
		if(p_de != NULL && p_de->data != NULL)
		{
			osal_mem_free(p_de->data);
		}
		osal_mem_free(p);
	}
	break;

	default: break;
	}
}
#endif