/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring / Peter Mandrella / Georg Horn
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 *  
 * File Name: md5.h
 * Description: this module contains header file for md5.c
 * 
 * This file is part of FreeAdhocUDF.
 * FreeAdhocUDF is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * FreeAdhocUDF is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with FreeAdhocUDF.  If not, see <http://www.gnu.org/licenses/>
 * 
 * Additional license of md5.h   
 * $Id: md5.c,v 1.3 2006-05-01 16:57:31 quentin Exp $ 
 * Implementation of the md5 algorithm as described in RFC1321
 * Copyright (C) 2005 Quentin Carbonneaux <qcarbonneaux@gmail.com>
 *
 * This file is originally part of md5sum.
 * md5sum is a free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreeAdhocUDF.  If not, see <http://www.gnu.org/licenses/>. 
 ***************************************************************************/

#ifndef MD5_H
#define MD5_H

#include <assert.h>
#include <stdlib.h>

// WARNING :
// This implementation is using 32 bits long values for sizes
typedef unsigned int md5_size;

// MD5 context 
struct md5_ctx {
	struct {
		unsigned int A, B, C, D; // registers 
	} regs;
	unsigned char *buf;
	md5_size size;
	md5_size bits;
};

// Size of the MD5 buffer 
#define MD5_BUFFER 1024

// Basic md5 functions 
#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (~z & y))
#define H(x,y,z) (x ^ y ^ z)
#define I(x,y,z) (y ^ (x | ~z))

// Rotate left 32 bits values (words)
#define ROTATE_LEFT(w,s) ((w << s) | ((w & 0xFFFFFFFF) >> (32 - s)))

#define FF(a,b,c,d,x,s,t) (a = b + ROTATE_LEFT((a + F(b,c,d) + x + t), s))
#define GG(a,b,c,d,x,s,t) (a = b + ROTATE_LEFT((a + G(b,c,d) + x + t), s))
#define HH(a,b,c,d,x,s,t) (a = b + ROTATE_LEFT((a + H(b,c,d) + x + t), s))
#define II(a,b,c,d,x,s,t) (a = b + ROTATE_LEFT((a + I(b,c,d) + x + t), s))

unsigned char *md5 (unsigned char *, md5_size, unsigned char *);
void md5_init (struct md5_ctx *);
void md5_update (struct md5_ctx *context);
void md5_final (unsigned char *digest, struct md5_ctx *context);

#endif 
// MD5_H 
