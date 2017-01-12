/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * Copyright (c) 1990- 1993, 1996 Open Software Foundation, Inc.
 * Copyright (c) 1989 by Hewlett-Packard Company, Palo Alto, Ca. &
 * Digital Equipment Corporation, Maynard, Mass.
 * Copyright (c) 1998 Microsoft.
 * 
 * File Name:	uuid.c
 * Description: this module contains basic routines for the UUID functions
 * and was originally written by the Open Software Foundation, Inc.  
 * adapted for FreeAdhoc UDF by Georg Horn 2006-10-20 
 * some concept from uuidlib by Jan Newby  
 * for exported functions list see uuid.h
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
 * Additional license for uuid.c 
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
******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sysdep.h"
#include "uuid.h"

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// various forward declarations 

static int read_state(unsigned16 * clockseq, uuid_time_t * timestamp, uuid_node_t * node);
static void write_state(unsigned16 clockseq, uuid_time_t timestamp, uuid_node_t node);
static void get_current_time(uuid_time_t * timestamp);
static unsigned16 true_random(void);

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// generate a V1 UUID 

int uuid_create_v1(uuid_t *uuid, int rand_nodeid)
{
    uuid_time_t timestamp, last_time;
    unsigned16 clockseq;
    uuid_node_t node;
    uuid_node_t last_node;
    int f;

    // acquire system wide lock so we're alone 
    LOCK;

    // get current time 
    get_current_time(&timestamp);

    // get node ID 
    get_ieee_node_identifier(&node, rand_nodeid);

    // get saved state from NV storage 
    f = read_state(&clockseq, &last_time, &last_node);

    // if no NV state, or if clock went backwards, or node ID changed (e.g.,
    //   net card swap) change clockseq 
    if (!f || memcmp(&node, &last_node, sizeof(uuid_node_t)))
	clockseq = true_random();
    else if (timestamp < last_time)
    	clockseq++;

    // stuff fields into the UUID 
    uuid->time_low = (unsigned long) (timestamp & 0xFFFFFFFF);
    uuid->time_mid = (unsigned short) ((timestamp >> 32) & 0xFFFF);
    uuid->time_hi_and_version = (unsigned short) ((timestamp >> 48) & 0x0FFF);
    uuid->time_hi_and_version |= (1 << 12);
    uuid->clock_seq_low = clockseq & 0xFF;
    uuid->clock_seq_hi_and_reserved = (clockseq & 0x3F00) >> 8;
    uuid->clock_seq_hi_and_reserved |= 0x80;
    memcpy(&uuid->node, &node, sizeof uuid->node);

    // save the state for next time 
    write_state(clockseq, timestamp, node);
    UNLOCK;
    return 1;
}

// gh 2006-10-20
// tt 2010-12-06 add some implicit assignements
// generate a V4 UUID 

int uuid_create_v4(uuid_t *uuid)
{
    int i;

    uuid->time_low = (true_random() << 16) | true_random();
    uuid->time_mid = true_random();
    uuid->time_hi_and_version = true_random();
    uuid->clock_seq_low = (unsigned8)true_random();
    uuid->clock_seq_hi_and_reserved = (unsigned8)true_random();
    for (i = 0; i < 6; i++) {
		uuid->node[i] = (byte)true_random();
    }
    uuid->time_hi_and_version &= 0x0fff;
    uuid->time_hi_and_version |= 0x4000;
    uuid->clock_seq_hi_and_reserved &= 0x3f;
    uuid->clock_seq_hi_and_reserved |= 0x80;
    return 1;
}

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// data type for UUID generator persistent state 

typedef struct {
    uuid_time_t ts;		// saved timestamp 
    uuid_node_t node;		// saved node ID 
    unsigned16 cs;		// saved clock sequence 
} uuid_state;
static uuid_state st;

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// read UUID generator state from non-volatile store 

int read_state(unsigned16 * clockseq, uuid_time_t * timestamp,
    uuid_node_t * node)
{
    FILE *fd;
    static int inited = 0;

    // only need to read state once per boot 
    if (!inited) {
	char *tmpStr;
	char *tmp = getenv("TMP");
	if (!tmp) {
	    tmp = getenv("TEMP");
	    if (!tmp) {
		tmp = "/tmp";
	    }
	}
	tmpStr = (char *) malloc(strlen(tmp) + 20);
	strcpy(tmpStr, tmp);
	strcat(tmpStr, "/uuid.state");
	fd = fopen(tmpStr, "rb");
	if (!fd)
	    return 0;
	fread(&st, sizeof(uuid_state), 1, fd);
	fclose(fd);
	free(tmpStr);
	inited = 1;
    }
    *clockseq = st.cs;
    *timestamp = st.ts;
    *node = st.node;
    return 1;
}

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// save UUID generator state back to non-volatile storage 

void write_state(unsigned16 clockseq, uuid_time_t timestamp, uuid_node_t node)
{
    FILE *fd;
    static int inited = 0;
    static uuid_time_t next_save;

    if (!inited) {
	next_save = timestamp;
	inited = 1;
    }

    // always save state to volatile shared state 
    st.cs = clockseq;
    st.ts = timestamp;
    st.node = node;
    if (timestamp >= next_save) {
	char *tmpStr;
	char *tmp = getenv("TMP");
	if (!tmp) {
	    tmp = getenv("TEMP");
	    if (!tmp) {
		tmp = "/tmp";
	    }
	}
	tmpStr = (char *) malloc(strlen(tmp) + 20);
	strcpy(tmpStr, tmp);
	strcat(tmpStr, "/uuid.state");
	fd = fopen(tmpStr, "wb");
	if (fd) {
	    fwrite(&st, sizeof(uuid_state), 1, fd);
	    fclose(fd);
	}

	// schedule next save for 10 seconds from now 
	free(tmpStr);
	next_save = timestamp + (10 * 10 * 1000 * 1000);
    }
}

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// get time as 60 bit 100ns ticks since whenever.
// Compensate for the fact that real clock resolution is less than 100ns. 

void get_current_time(uuid_time_t * timestamp)
{
    uuid_time_t time_now;
    static uuid_time_t time_last;
    static unsigned16 uuids_this_tick;
    static int inited = 0;

    if (!inited) {
	// ivar: is this line not surplus? maybe it should call time_last as it
	// has not been set yet? 
	get_system_time(&time_now);
	uuids_this_tick = UUIDS_PER_TICK;
	inited = 1;
    }
    while (1) {
	get_system_time(&time_now);

	// if clock reading changed since last UUID generated... 
	if (time_last != time_now) {
	    // reset count of uuids gen'd with this clock reading 
	    uuids_this_tick = 0;
	    break;
	}
	if (uuids_this_tick < UUIDS_PER_TICK) {
	    uuids_this_tick++;
	    break;
	}

    // going too fast for our clock; spin 
    }

    // add the count of uuids to low order bits of the clock reading 
    *timestamp = time_now + uuids_this_tick;

    // Buggered up by ian and ivar. maybe, fudge. need to rem time last 
    time_last = time_now;
}

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// generate a crypto-quality random number. This sample doesn't do that. 

static unsigned16 true_random(void)
{
    static int inited = 0;
    uuid_time_t time_now;

    if (!inited) {
	get_system_time(&time_now);
	time_now = time_now / UUIDS_PER_TICK;
	srand((unsigned int) (((time_now >> 32) ^ time_now) & 0xffffffff));
	inited = 1;
    }
    return rand();
}
