/* globals.c
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
#include "globals.h"
#include "mevent.h"
#include "protocol.h"
#include "mincode.h"

#include "AF.h"
#include "NLMEDE.h"

#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)

extern nwkIB_t _NIB;
extern uint8 EXT_ADDR_G[16];

uint8 _common_no[8] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF};

void mach_init(void)
{
	gw_info_t *p_gw_info = get_gateway_info();

	memset(p_gw_info->gw_no, 0, sizeof(p_gw_info->gw_no));
	p_gw_info->zapp_type = FRAPP_NONE;
	p_gw_info->zpanid = 0;
	p_gw_info->zchannel = 0;
	p_gw_info->ip_len = 0;
	p_gw_info->p_dev = NULL;
	p_gw_info->next = NULL;
}

void mach_load(void)
{
	gw_info_t *p_gw_info = get_gateway_info();

	incode_ctoxs(p_gw_info->gw_no, EXT_ADDR_G, 16);
	p_gw_info->zapp_type = FRAPP_CONNECTOR;
	p_gw_info->zpanid = _NIB.nwkPanId;
	p_gw_info->zchannel = _NIB.nwkLogicalChannel;;
}
#endif
