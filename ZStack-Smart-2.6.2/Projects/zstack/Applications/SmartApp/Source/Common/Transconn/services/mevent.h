/*
 * mevent.h
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
#ifndef __MEVENT_H__
#define __MEVENT_H__

#include "globals.h"
#include "protocol.h"
#include <stdio.h>
 
extern void upload_event( void *params, uint16 *duration, uint8 *count);

extern void set_zdev_check(uint16 net_addr);
 
#endif  //__MEVENT_H__