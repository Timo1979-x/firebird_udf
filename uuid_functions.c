/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 *
 * File Name:	uuid_functions.c
 * Description: this module contains UDFs to generate and manipulate uuids.
 * Some concepts and code from uuidlib by Ian Newby 
 * for exported functions list see uuid_functions.h
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
*****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include "global.h"

#include "sysdep.h"
#include "uuid.h"
#include "date_functions.h"
#include "uuid_functions.h"

/*******************************************************************************
 * Note.
 * Throughout this code the terms _uuid, uuid, and guid are:
 * _uuid - 128bit (16 byte) raw value (internal only)
 * guid - 36 characters (eg. e902893a-9d22-3c7e-a7b8-d6e313b71d9f)
 * uuid - interbase uuid is 19 char value (see fn_i_uuid_create)
 *******************************************************************************
 * Note from adhoc dataservice:
 * in this code the terms UUID and GUID are not as descriped in the RFC4122
 * the term GUID in this code is called UUID in RFC4122
 * and the UUID in this code is not part of the RFC4122
 * it is made by the former programmers and is a decoded UUID for quicker reading
 * so it is better called a UUIDCOMPRESSED
 * We only changed the names of the exported functions for a better understanding
 ******************************************************************************/

static char urlchars[64] =
    "!*0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

// gh 2006-10-20
// Check if a RFC uuid is valid 

int check_uuid(char *uuid)
{
    if (!uuid) {
	return 0;
    } else {
	int l = 0;
	char *c = uuid;

	while (*c) {
	    if (isxdigit(*c) || (*c == '-')) {
		l++;
		c++;
	    } else {
		break;
	    }
	}
	return (l == 36);
    }
}

// gh 2006-10-20
// ct 2006-11-21 implementation of NULL for FB 2.0
// Check if a compressed uuid is valid 

int check_uuidcompr(char *uuidcompr)
{
    if (!uuidcompr) {
	return 0;
    } else {
	int l = 0;
	char *c = uuidcompr;

	while (*c) {
	    if (isalnum(*c) || (*c == '!') || (*c == '*')) {
		l++;
		c++;
	    } else {
		break;
	    }
	}
	return (l == 22);
    }
}

// gh 2006-10-20
// return the passed in _uuid as a 36 char guid

char *_uuid_to_guid(uuid_t * _uuid, char *guid)
{
    int i, j;

    sprintf(guid, "%8.8lx-%4.4x-%4.4x-%2.2x%2.2x-", _uuid->time_low,
	    _uuid->time_mid, _uuid->time_hi_and_version,
	    _uuid->clock_seq_hi_and_reserved, _uuid->clock_seq_low);
    for (i = 0, j = 24; i < 6; i++, j += 2) {
	sprintf(guid + j, "%2.2x", _uuid->node[i]);
    }

    return guid;
}

// gh 2006-10-20
// ct 2008-02-22 replaced MALLOC with malloc
// ct 2009-08-10 bugfix - thanks to Aleksey Avdeev
// tt 2010-12-06 add some implicit assignements
// returns the passed in guid as a _uuid 

uuid_t *guid_to__uuid(char *guid, uuid_t * _uuid)
{
    int i, j;

    char *time_low, *time_mid, *time_hi_and_version;
    char *clock_seq_hi_and_reserved, *clock_seq_low;
    char *node[6];
    char *eptr;			// dummy for strtol
#ifdef UUID_WINDOWS		// Nasty hack to fix lack of strtoll() 
    char *hi_time_low, *lo_time_low;	// see UUID_WINDOWS below. UGH 
#endif

    time_low = (char *) malloc(9);
    time_mid = (char *) malloc(5);
    time_hi_and_version = (char *) malloc(5);
    clock_seq_hi_and_reserved = (char *) malloc(3);
    clock_seq_low = (char *) malloc(3);
    memset(time_low, 0, 9);
    memset(time_mid, 0, 5);
    memset(time_hi_and_version, 0, 5);
    memset(clock_seq_hi_and_reserved, 0, 3);
    memset(clock_seq_low, 0, 3);
    for (i = 0; i < 6; i++) {
	node[i] = (char *) malloc(3);
	memset(node[i], 0, 3);
    }
    strncpy(time_low, guid, 8);
    strncpy(time_mid, guid + 9, 4);
    strncpy(time_hi_and_version, guid + 14, 4);
    strncpy(clock_seq_hi_and_reserved, guid + 19, 2);
    strncpy(clock_seq_low, guid + 21, 2);
    for (i = 0, j = 24; i < 6; i++, j += 2) {
	strncpy(node[i], guid + j, 2);
    }
#ifdef UUID_WINDOWS		// Nasty hack to fix lack of strtoll() 
    hi_time_low = (char *) malloc(5);
    lo_time_low = (char *) malloc(5);
    memset(hi_time_low, 0, 5);
    memset(lo_time_low, 0, 5);
    strncpy(hi_time_low, guid, 4);
    strncpy(lo_time_low, guid + 4, 4);
    _uuid->time_low = strtol(hi_time_low, &eptr, 16);
    _uuid->time_low <<= 16;
    _uuid->time_low |= strtol(lo_time_low, &eptr, 16);
    free(hi_time_low);
    free(lo_time_low);
#else
    _uuid->time_low = strtoll(time_low, &eptr, 16);
#endif
    _uuid->time_mid = (unsigned16)strtol(time_mid, &eptr, 16);
    _uuid->time_hi_and_version = (unsigned16)strtol(time_hi_and_version, &eptr, 16);
    _uuid->clock_seq_hi_and_reserved = (unsigned8)strtol(clock_seq_hi_and_reserved, &eptr, 16);
    _uuid->clock_seq_low = (unsigned8)strtol(clock_seq_low, &eptr, 16);
    for (i = 0; i < 6; i++) {
		_uuid->node[i] = (byte)strtol(node[i], &eptr, 16);
    }

    free(time_low);
    free(time_mid);
    free(time_hi_and_version);
    free(clock_seq_hi_and_reserved);
    free(clock_seq_low);
    for (i = 0; i < 6; i++) {
	free(node[i]);
    }
    return _uuid;
}

#define MASK_1_2	0x03
#define MASK_1_4	0x0f
#define MASK_1_6	0x3f
#define MASK_3_6	0x3c
#define MASK_5_6	0x30
#define MASK_3_8	0xfc
#define MASK_5_8	0xf0
#define MASK_7_8	0xc0
#define MASK_5_10	0x03f0
#define MASK_7_12	0x0fc0
#define MASK_13_16	0xf000
#define MASK_11_16	0xfc00
#define MASK_13_18	0x0003f000
#define MASK_19_24	0x00fc0000
#define MASK_25_30	0x3f000000
#define MASK_31_32	0xc0000000

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// returns the 22char string from the passed in _uuid 

char *_uuid_to_uuid(uuid_t *_uuid, char *uuid)
{
    char sq_u[22];
    int i;

    sq_u[21] = (char)(_uuid->time_low & MASK_1_6);
    sq_u[20] = (char)((_uuid->time_low & MASK_7_12) >> 6);
    sq_u[19] = (char)((_uuid->time_low & MASK_13_18) >> 12);
    sq_u[18] = (char)((_uuid->time_low & MASK_19_24) >> 18);
    sq_u[17] = (char)((_uuid->time_low & MASK_25_30) >> 24);
    sq_u[16] = (char)((_uuid->time_low & MASK_31_32) >> 30);

    sq_u[16] |= (_uuid->time_mid & MASK_1_4) << 2;
    sq_u[15] = (_uuid->time_mid & MASK_5_10) >> 4;
    sq_u[14] = (_uuid->time_mid & MASK_11_16) >> 10;

    sq_u[13] = _uuid->time_hi_and_version & MASK_1_6;
    sq_u[12] = (_uuid->time_hi_and_version & MASK_7_12) >> 6;
    sq_u[11] = (_uuid->time_hi_and_version & MASK_13_16) >> 12;

    sq_u[11] |= (_uuid->clock_seq_hi_and_reserved & MASK_1_2) << 4;
    sq_u[10] = (_uuid->clock_seq_hi_and_reserved & MASK_3_8) >> 2;

    sq_u[9] = (_uuid->clock_seq_low & MASK_1_6);
    sq_u[8] = (_uuid->clock_seq_low & MASK_7_8) >> 6;

    sq_u[8] |= (_uuid->node[5] & MASK_1_4) << 2;
    sq_u[7] = ((_uuid->node[5] & MASK_5_8) >> 4) | ((_uuid->node[4] & MASK_1_2) << 4);
    sq_u[6] = (_uuid->node[4] & MASK_3_8) >> 2;
    sq_u[5] = (_uuid->node[3] & MASK_1_6);
    sq_u[4] = ((_uuid->node[3] & MASK_7_8) >> 6) | ((_uuid->node[2] & MASK_1_4) << 2);
    sq_u[3] = ((_uuid->node[2] & MASK_5_8) >> 4) | ((_uuid->node[1] & MASK_1_2) << 4);
    sq_u[2] = ((_uuid->node[1] & MASK_3_8) >> 2);
    sq_u[1] = (_uuid->node[0] & MASK_1_6);
    sq_u[0] = (_uuid->node[0] & MASK_7_8) >> 6;	// +4 unused bits

    for (i = 0; i < 22; i++) {
	uuid[i] = urlchars[(int) sq_u[i]];
    }
    uuid[22] = '\0';
    return uuid;
}

int sq_to_int(char i)
{
    if (i == '!')
	return 0;
    if (i == '*')
	return 1;
    if (i >= '0' && i <= '9')
	return (int) i - 46;  // 0 is 2
    if (i >= 'A' && i <= 'Z')
	return (int) i - 53;  // A is 12
    if (i >= 'a' && i <= 'z')
	return (int) i - 59;  // a is 38
    return -1;
}

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// returns the _uuid from the passed in uuid 
uuid_t *uuid_to__uuid(char *uuid, uuid_t * _uuid)
{
    _uuid->time_low = sq_to_int(uuid[16]);
    _uuid->time_low <<= 6;
    _uuid->time_low |= sq_to_int(uuid[17]);
    _uuid->time_low <<= 6;
    _uuid->time_low |= sq_to_int(uuid[18]);
    _uuid->time_low <<= 6;
    _uuid->time_low |= sq_to_int(uuid[19]);
    _uuid->time_low <<= 6;
    _uuid->time_low |= sq_to_int(uuid[20]);
    _uuid->time_low <<= 6;
    _uuid->time_low |= sq_to_int(uuid[21]);

    _uuid->time_mid = sq_to_int(uuid[14]);
    _uuid->time_mid <<= 6;
    _uuid->time_mid |= sq_to_int(uuid[15]);
    _uuid->time_mid <<= 4;
    _uuid->time_mid |= ((sq_to_int(uuid[16]) & MASK_3_6) >> 2);

    _uuid->time_hi_and_version = sq_to_int(uuid[11]);
    _uuid->time_hi_and_version <<= 6;
    _uuid->time_hi_and_version |= sq_to_int(uuid[12]);
    _uuid->time_hi_and_version <<= 6;
    _uuid->time_hi_and_version |= sq_to_int(uuid[13]);

    _uuid->clock_seq_hi_and_reserved = sq_to_int(uuid[10]);
    _uuid->clock_seq_hi_and_reserved <<= 2;
    _uuid->clock_seq_hi_and_reserved |= ((sq_to_int(uuid[11]) & MASK_5_6) >> 4);

    _uuid->clock_seq_low = sq_to_int(uuid[8]);
    _uuid->clock_seq_low <<= 6;
    _uuid->clock_seq_low |= sq_to_int(uuid[9]);

    _uuid->node[0] = (sq_to_int(uuid[0]) << 6);
    _uuid->node[0] |= sq_to_int(uuid[1]);

    _uuid->node[1] = (sq_to_int(uuid[2]) << 2);
    _uuid->node[1] |= (sq_to_int(uuid[3]) >> 4);

    _uuid->node[2] = (sq_to_int(uuid[3]) << 4);
    _uuid->node[2] |= (sq_to_int(uuid[4]) >> 2);

    _uuid->node[3] = (sq_to_int(uuid[4]) << 6);
    _uuid->node[3] |= sq_to_int(uuid[5]);

    _uuid->node[4] = (sq_to_int(uuid[6]) << 2);
    _uuid->node[4] |= (sq_to_int(uuid[7]) >> 4);

    _uuid->node[5] = (sq_to_int(uuid[7]) << 4);
    _uuid->node[5] |= (sq_to_int(uuid[8]) >> 2);

    return _uuid;
}


// gh 2006-10-20
// ct 2008-02-22 replaced MALLOC with malloc
// return timestamp from a _uuid. returned pointer must bee free()d 

struct tm *get__uuid_timestamp(uuid_t *_uuid)
{
    uuid_time_t timestamp;
    time_t t;
#ifndef UUID_WINDOWS
    struct tm *tp = malloc(sizeof(struct tm));
#endif

    timestamp = ((uuid_time_t)(_uuid->time_hi_and_version ^ 0x1000) << 48)
	| ((uuid_time_t)_uuid->time_mid << 32) | (uuid_time_t)_uuid->time_low;

    timestamp -= I64(0x01B21DD213814000);
    t = timestamp / 10000000;
    if (t < 0) {
	t = 0;
#ifdef UUID_WINDOWS
	return gmtime(&t);
#else
	gmtime_r(&t, tp);
#endif
    } else {
#ifdef UUID_WINDOWS
	return localtime(&t);
#else
	localtime_r(&t, tp);
#endif
    }
#ifndef UUID_WINDOWS
    return tp;
#endif
}

// Exported UDFs start here... 

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// return the created _uuid as a 36 char RFC uuid version 1a

char *uuid1mac()
{
    char *uuid = (char *) MALLOC (37);
    uuid_t _uuid;

    uuid_create_v1(&_uuid, 0);
    return _uuid_to_guid(&_uuid, uuid);
}

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// return the created _uuid as a 22 character, URL compatible,
// humanly readable string, with the entropy in the low order
// bits for interbase to efficiently hash it.
// Valid URL chars A-Z|a-z|0-9|"-"|"_"|"."|"!"|"~"|"*"|"'"|"("|")"
// and we use a subset of 63 of these, *A-Za-z0-9 

char *uuid1maccompr()
{
    char *uuid = (char *) MALLOC (23);
    uuid_t _uuid;

    uuid_create_v1(&_uuid, 0);
    return _uuid_to_uuid(&_uuid, uuid);
}

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// return the created _uuid as a 36 char RFC uuid version 1b 

char *uuid1rand()
{
    char *uuid = (char *) MALLOC (37);
    uuid_t _uuid;

    uuid_create_v1(&_uuid, 1);
    return _uuid_to_guid(&_uuid, uuid);
}

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// return the created _uuid as a 22 character, URL compatible, humanly readable
//   string.

char *uuid1randcompr()
{
    char *uuid = (char *) MALLOC (23);
    uuid_t _uuid;

    uuid_create_v1(&_uuid, 1);
    return _uuid_to_uuid(&_uuid, uuid);
}

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// return the created _uuid as a 36 char RFC uuid version 4 

char *uuid4()
{
    char *uuid = (char *) MALLOC (37);
    uuid_t _uuid;

    uuid_create_v4(&_uuid);
    return _uuid_to_guid(&_uuid, uuid);
}

// based on uuidlib by Ian Newby 
// gh 2006-10-20
// return the created _uuid as a 22 character, URL compatible, humanly readable
// string. 

char *uuid4compr()
{
    char *uuidcompr = (char *) MALLOC (23);
    uuid_t _uuid;

    uuid_create_v4(&_uuid);
    return _uuid_to_uuid(&_uuid, uuidcompr);
}


// based on uuidlib by Ian Newby 
// gh 2006-10-20
// returns the passed in RFC uuid to a compressed uuid 

char *compressuuid(char *uuid)
{
    if (!uuid) {
	return NULL;
    } else {
	char *uuidcompr = (char *) MALLOC(23);
	uuid_t _uuid;

	if (check_uuid(uuid)) {
	    guid_to__uuid(uuid, &_uuid);
	    uuidcompr = _uuid_to_uuid(&_uuid, uuidcompr);
	} else {
	    strcpy(uuidcompr, "Input no RFC4122 UUID");
	}
	return uuidcompr;
    }
}

// gh 2006-10-20
// return the passed in compressed uuid expanded to a RFC uuid 

char *expanduuidcompr(char *uuidcompr)
{
    if (!uuidcompr) {
	return NULL;
    } else {
	char *uuid = (char *) MALLOC(37);
	uuid_t _uuid;

	if (check_uuidcompr(uuidcompr)) {
	    uuid_to__uuid(uuidcompr, &_uuid);
	    uuid = _uuid_to_guid(&_uuid, uuid);
	} else {
	    strcpy(uuid, "Input no compressed UUID");
	}
	return uuid;
    }
}

// gh 2006-10-20
// ct 2008-02-22 replaced MALLOC with malloc
// ct 2010-12-04 replaced malloc with MALLOC
// return the timestamp from a compressed uuid 

ISC_QUAD *uuid1comprtimestamp(char *uuidcompr)
{
    if (!uuidcompr)
	return NULL;
    else {
	uuid_t _uuid;
	struct tm *tp;
#ifdef UUID_WINDOWS
#else
	ISC_QUAD *date;
#endif
	if (check_uuidcompr(uuidcompr)) {
	    uuid_to__uuid(uuidcompr, &_uuid);
	    if (_uuid.time_hi_and_version & 0x1000) {
		tp = get__uuid_timestamp(&_uuid);
#ifdef UUID_WINDOWS
		return (ISC_QUAD *) gen_ib_date(tp);
#else
                date = MALLOC(sizeof(ISC_QUAD));
                isc_encode_date(tp, date);
		free(tp);
		return date;
#endif
	    }
	}
        return NULL;
    }
}

// gh 2006-10-20
// gh 2006-12-06 implementation of NULL for FB 2.0 and testing on version 1
// return the timestamp from a RFC uuid 

ISC_QUAD *uuid1timestamp(char *uuid)
{
    if (!uuid)
	return NULL;
    else {
	uuid_t _uuid;
	struct tm *tp;
#ifndef UUID_WINDOWS
	ISC_QUAD *date;
#endif
	if (check_uuid(uuid) && (uuid[14] == '1')) {
	    guid_to__uuid(uuid, &_uuid);
	    if (_uuid.time_hi_and_version & 0x1000) {
		tp = get__uuid_timestamp(&_uuid);
#ifdef UUID_WINDOWS
		return (ISC_QUAD *) gen_ib_date(tp);
#else
		date = gen_ib_date(tp);
		free(tp);
		return date;
#endif
	    }
	}
	return NULL;
    }
}


// gh 2006-10-20
// ct 2006-11-21 implementation of NULL for FB 2.0 
// return the nodeid from a compressed uuid 

char *uuid1maccomprmac(char *uuidcompr)
{
    if (!uuidcompr) {
	return NULL;
    } else {
	if (check_uuidcompr(uuidcompr)) {
	    uuid_t _uuid;
	    uuid_to__uuid(uuidcompr, &_uuid);
	    if ((_uuid.time_hi_and_version & 0x1000) && !(_uuid.node[0] & 0x80)) {
		char *nodeid = (char *) MALLOC(19);
		sprintf(nodeid, "%02x:%02x:%02x:%02x:%02x:%02x", _uuid.node[0],
		    _uuid.node[1], _uuid.node[2], _uuid.node[3],
		    _uuid.node[4], _uuid.node[5]);
		return nodeid;
	    }
	}
	return NULL;
    }
}


// gh 2006-10-20
// ct 2006-11-21 implementation of NULL for FB 2.0 
// return the nodeid from a RFC uuid 

char *uuid1macmac(char *uuid)
{
    if (!uuid) {
	return NULL;
    } else {
	int v2;
	
	sscanf(&uuid[24], "%2x", &v2);
	if (check_uuid(uuid) && (uuid[14] == '1') && !(v2 & 0x80)) {
	    char *nodeid = (char *) MALLOC(18);
	    uuid_t _uuid;

	    guid_to__uuid(uuid, &_uuid);
	    sprintf(nodeid, "%02x:%02x:%02x:%02x:%02x:%02x", _uuid.node[0],
		    _uuid.node[1], _uuid.node[2], _uuid.node[3],
		    _uuid.node[4], _uuid.node[5]);
	    return nodeid;
	} else {
	    return NULL;
	}
    }
}


// gh 2006-10-20
// ct 2006-11-21 implementation of NULL for FB 2.0 
// ct 2010-03-15 check of legal values for output
// ct 2010-11-30 check of legal input 
/* Return the variant and the version from a RFC uuid
* variants are:
*  0 NCS (reserved for backward compatibility)
* 10 The current variant, described used and produced on these pages.
* 110 Microsoft (reserved for backward compatibility)
* 111 reserved for further use
* versions are:
* 1  Time-based
*    a with real existing host identifier (IEEE 802 MAC-adress)
*    b with a random host identifier
* 2 DCE Security version (with POSIX UIDs)
* 3 Name-based (MD5 hash)
* 4 Random
* 5 Name-based (SHA-1 hash)
****************************************************************/

char *uuidversion(char *uuid)
{
    if (!uuid) {
	return NULL;
    } else {
	char *version = (char *) MALLOC(254);
	int v1 = uuid[14];
	int v2;

        if (strlen(uuid) != 36) {
           sprintf(version, "no RFC4122 UUID");        
        } else {
	   if (v1 == '1') {
	      sscanf(&uuid[24], "%2x", &v2);
	      if (v2 & 0x80)
	         strcpy(version, "V1b");
	      else
	         strcpy(version, "V1a");
	   } else {
	      if (v1 < '6') {
	         sprintf(version, "V%c", v1);
	       } else {
	          sprintf(version, "no RFC4122 UUID");
	       }  
	   }
        }
	return version;
    }
}
