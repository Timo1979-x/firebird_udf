/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring / Peter Mandrella / Georg Horn
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * Copyright (c) 1990- 1993, 1996 Open Software Foundation, Inc.
 * Copyright (c) 1989 by Hewlett-Packard Company, Palo Alto, Ca. &
 * Digital Equipment Corporation, Maynard, Mass.
 * Copyright (c) 1998 Microsoft.
 * 
 * File Name:	uuid.h
 * Description: This header contains the prototype definitions for uuid.c
 * and was originally written by the Open Software Foundation, Inc.
 * adapted for FreeAdhocUDF by Georg Horn 2006-10-20 
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
 * Additional license for uuid.h 
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

#undef uuid_t

typedef struct _uuid_t {
    unsigned32 time_low;
    unsigned16 time_mid;
    unsigned16 time_hi_and_version;
    unsigned8 clock_seq_hi_and_reserved;
    unsigned8 clock_seq_low;
    byte node[6];
} uuid_t;

/* generate a V1 UUID */
int uuid_create_v1(uuid_t *uuid, int rand_nodeid);

/* generate a V4 UUID */
int uuid_create_v4(uuid_t *uuid);

/* Compare two UUID's "lexically" and return
   -1   u1 is lexically before u2
    0   u1 is equal to u2
    1   u1 is lexically after u2
   Note:   lexical ordering is not temporal ordering!  */
//int uuid_compare(uuid_t * u1, uuid_t * u2);
