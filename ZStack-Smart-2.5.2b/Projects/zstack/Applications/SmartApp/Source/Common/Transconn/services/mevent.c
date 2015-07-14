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

#if defined(TRANSCONN_BOARD_GATEWAY) && defined(SSA_CONNECTOR)

extern byte TransconnApp_TaskID;

void upload_event( void *params, uint16 *duration, uint8 *count)
{
	pi_t pi;
	memcpy(pi.sn, get_gateway_info()->gw_no, sizeof(zidentify_no_t));
	pi.trans_type = TRTYPE_UDP_NORMAL;
	pi.fr_type = TRFRAME_CON;
	pi.data = NULL;
	pi.data_len = 0;
	
	send_frame_udp_request(TRHEAD_PI, &pi);
}
#endif
