/*
 * trrequest.h
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

#ifndef __TRREQUEST_H__
#define __TRREQUEST_H__

#include "globals.h"
#include "trframelysis.h"

void pi_handler(pi_t *pi);
void bi_handler(bi_t *bi);

void dc_handler(dc_t *dc);
void ub_handler(ub_t *ub);

void send_frame_udp_request(tr_head_type_t htype, void *frame);

#endif  //__TRREQUEST_H__
