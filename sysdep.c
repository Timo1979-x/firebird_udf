/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * Copyright (c) 1990- 1993, 1996 Open Software Foundation, Inc.
 * Copyright (c) 1989 by Hewlett-Packard Company, Palo Alto, Ca. &
 * Digital Equipment Corporation, Maynard, Mass.
 * Copyright (c) 1998 Microsoft.
 * 
 * File Name:	sysdep.c
 * Description: this module contains system routines for the UUID functions
 * and was originally written by the Open Software Foundation, Inc.
 * adapted for FreeAdhoc UDF by Georg Horn 2006-10-20
 * for exported functions list see sysdep.h
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
 * Additional license for sysdep.c 
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sysdep.h"

// get a mac address if available 
#ifdef UUID_WINDOWS
typedef struct _ASTAT_ {
    ADAPTER_STATUS adapt;
    NAME_BUFFER NameBuff[30];

} ASTAT;

ASTAT Adapter;

int get_mac_address(uuid_node_t *node)
{
    NCB Ncb;
    LANA_ENUM lenum;

    memset(&Ncb, 0, sizeof(Ncb));
    Ncb.ncb_command = NCBENUM;
    Ncb.ncb_buffer = (UCHAR *) &lenum;
    Ncb.ncb_length = sizeof(lenum);
    Netbios(&Ncb);
    if ((Netbios(&Ncb) != 0) || (lenum.length < 1)) {
	return -1;
    }

    memset(&Ncb, 0, sizeof(Ncb));
    Ncb.ncb_command = NCBRESET;
    Ncb.ncb_lana_num = lenum.lana[0];
    if (Netbios(&Ncb) != 0) {
	return -1;
    }

    memset(&Ncb, 0, sizeof(Ncb));
    Ncb.ncb_command = NCBASTAT;
    Ncb.ncb_lana_num = lenum.lana[0];
    strcpy((char *) Ncb.ncb_callname, "*");
    Ncb.ncb_buffer = (PUCHAR) & Adapter;
    Ncb.ncb_length = sizeof(Adapter);
    if (Netbios(&Ncb) != 0) {
	return -1;
    }

    node->nodeID[0] = Adapter.adapt.adapter_address[0];
    node->nodeID[1] = Adapter.adapt.adapter_address[1];
    node->nodeID[2] = Adapter.adapt.adapter_address[2];
    node->nodeID[3] = Adapter.adapt.adapter_address[3];
    node->nodeID[4] = Adapter.adapt.adapter_address[4];
    node->nodeID[5] = Adapter.adapt.adapter_address[5];

    return 0;
}
#else  // ! UUID_WINDOWS 
int get_mac_address(uuid_node_t *node)
{
    int sfd;
    struct ifreq ifr;
    unsigned char *u;

    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	return -1;
    }

    memset(&ifr, 0, sizeof ifr);
    strcpy(ifr.ifr_name, INTERFACE_NAME);
    if (ioctl(sfd, SIOCGIFHWADDR, &ifr) < 0) {
	return -1;
    }

    u = (unsigned char *) &ifr.ifr_addr.sa_data;
    node->nodeID[0] = u[0];
    node->nodeID[1] = u[1];
    node->nodeID[2] = u[2];
    node->nodeID[3] = u[3];
    node->nodeID[4] = u[4];
    node->nodeID[5] = u[5];

    close(sfd);
    return 0;
}
#endif  // ! UUID_WINDOWS 

// system dependent call to get IEEE node ID.
//   This implementation generates a random node ID if rand_nodeid is 1
//   or if no real nodeid can be detected. 
void get_ieee_node_identifier(uuid_node_t *node, int rand_nodeid)
{
    static int real_inited = 0;
    static int rand_inited = 0;
    static uuid_node_t real_node;
    static uuid_node_t rand_node;

    if (rand_nodeid) {
	if (!rand_inited) {
	    unsigned char seed[16];
	    get_random_info(seed);
	    seed[0] |= 0x80;
	    memcpy(&rand_node, seed, sizeof(uuid_node_t));
	    rand_inited = 1;
	}
	*node = rand_node;
    } else {
	if (!real_inited) {
	    if (get_mac_address(&real_node) < 0) {
		get_ieee_node_identifier(node, 1);
		memcpy(&real_node, &rand_node, sizeof(uuid_node_t));
	    }
	    real_inited = 1;
	}
	*node = real_node;
    }
}

// system dependent call to get the current system time.
//   returned as 100ns ticks since Oct 15, 1582, but resolution may be less
//   than 100ns. 
#ifdef UUID_WINDOWS
void get_system_time(uuid_time_t * uuid_time)
{
    unsigned __int64 time;

    GetSystemTimeAsFileTime((FILETIME *) & time);

    // NT keeps time in FILETIME format which is 100ns ticks since
    //   Jan 1, 1601.  UUIDs use time in 100ns ticks since Oct 15, 1582.
    //   The difference is 17 Days in Oct + 30 (Nov) + 31 (Dec)
    //   + 18 years and 5 leap days. 
    time += (unsigned __int64) (1000 * 1000 * 10)	// seconds
	* (unsigned __int64) (60 * 60 * 24)	// days
	* (unsigned __int64) (17 + 30 + 31 + 365 * 18 + 5);	// # of days

    *uuid_time = time;
}

void get_random_info(unsigned char seed[16])
{
    typedef struct {
	MEMORYSTATUS m;
	SYSTEM_INFO s;
	FILETIME t;
	LARGE_INTEGER pc;
	DWORD tc;
	DWORD l;
	char hostname[MAX_COMPUTERNAME_LENGTH + 1];
    } randomness;
    randomness r;

    // memory usage stats 
    GlobalMemoryStatus(&r.m);
    // random system stats 
    GetSystemInfo(&r.s);
    // 100ns resolution (nominally) time of day 
    GetSystemTimeAsFileTime(&r.t);
    // high resolution performance counter 
    QueryPerformanceCounter(&r.pc);
    // milliseconds since last boot 
    r.tc = GetTickCount();
    r.l = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerName(r.hostname, &r.l);
    md5((unsigned char *)&r, sizeof(randomness), seed);    
}
#else // ! UUID_WINDOWS 
void get_system_time(uuid_time_t * uuid_time)
{
    struct timeval tp;

    gettimeofday(&tp, (struct timezone *) 0);

    // Offset between UUID formatted times and Unix formatted times.
    //   UUID UTC base time is October 15, 1582.
    //   Unix base time is January 1, 1970. 
    *uuid_time = ((uuid_time_t)tp.tv_sec * 10000000)
	+ ((uuid_time_t)tp.tv_usec * 10) + I64(0x01B21DD213814000);
}

void get_random_info(unsigned char seed[16])
{
  typedef struct {
	struct sysinfo s;
	struct timeval t;
	char hostname[257];
    } randomness;
    randomness r;
    sysinfo(&r.s);
    gettimeofday(&r.t, (struct timezone *) 0);
    gethostname(r.hostname, 256);
    md5((unsigned char *)&r, sizeof(randomness), seed);    
}
#endif // ! UUID_WINDOWS 
