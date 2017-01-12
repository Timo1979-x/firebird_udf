/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring / Peter Mandrella / Georg Horn
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * Copyright (c) 1990- 1993, 1996 Open Software Foundation, Inc.
 * Copyright (c) 1989 by Hewlett-Packard Company, Palo Alto, Ca. &
 * Digital Equipment Corporation, Maynard, Mass.
 * Copyright (c) 1998 Microsoft.
 * 
 * File Name:   sysdep.h
 * Description: This header contains the prototype definitions for sysdep.c
 * and was originally written by the Open Software Foundation, Inc.
 * adapted for FreeAdhoc UDF by Georg Horn 2006-10-20 
 * some changes made 2006 to integrate to the FreeAdhocUDF project 
 * 
 * This file is part of FreeAdhocUDF.
 * FreeAdhocUDF is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * FreeAdhocUDF is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with FreeAdhocUDF.  If not, see <http://www.gnu.org/licenses/>. 
 *
 * Additional license for sysdep.h 
 * To anyone who acknowledges that this file is provided "AS IS"
 * without any express or implied warranty: permission to use, copy,
 * modify, and distribute this file for any purpose is hereby
 * granted without fee, provided that the above copyright notices and
 * this notice appears in all source code copies, and that none of
 * the names of Open Software Foundation, Inc., Hewlett-Packard
 * Company, or Digital Equipment Corporation be used in advertising
 * or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Neither Open Software
 * Foundation, Inc., Hewlett-Packard Company, Microsoft, nor Digital
 * Eqiupment Corporation makes any representations about the suitability
 * of this software for any purpose.
*****************************************************************************/

/* Borland definiert __BORLANDC__, fuer MS Visual C evtl. anpassen...
   _WINDOWS_ darf NICHT definiert sein, es wird in windows.h definiert.
   Defines für BC5 waren: _RTLDLL; */

#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__WATCOMC__)
#define UUID_WINDOWS
#include <windows.h>
#include <wincon.h>
#else
/* Set name of your ethernet-interface here: */
#define INTERFACE_NAME "eth0"
#include <unistd.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h> 
#include <arpa/inet.h>
#endif

/* change to point to where MD5 .h's live */
/* get MD5 sample implementation from RFC 1321 */
// lh #include "md5_global.h"
#include "md5.h"

/* set the following to the number of 100ns ticks of the actual
   resolution of your system's clock */
#define UUIDS_PER_TICK 1024

/* Set the following to a call to acquire a system wide global lock */
#define LOCK
#define UNLOCK

typedef unsigned long unsigned32;
typedef unsigned short unsigned16;
typedef unsigned char unsigned8;
typedef unsigned char byte;

/* Set this to what your compiler uses for 64 bit data type */
#ifdef UUID_WINDOWS
#define unsigned64_t unsigned __int64
#define I64(C) C
#else
#define unsigned64_t unsigned long long
#define I64(C) C##LL
#endif

typedef unsigned64_t uuid_time_t;

typedef struct {
    unsigned char nodeID[6];
} uuid_node_t;

void get_ieee_node_identifier(uuid_node_t *node, int do_rand);
void get_system_time(uuid_time_t *uuid_time);
void get_random_info(unsigned char seed[16]);
