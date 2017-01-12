/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 *
 * File Name:	file_functions.c
 * Description: this module contains UDFs to generate and manipulate files
 * for exported functions list see file_functions.h
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
#include <ctype.h>
#include <stdio.h>

#include "global.h"
#include "blob_functions.h"
#include "file_functions.h"

#ifndef FB_API_VER
/* define ISC_UCHAR for interbase */
typedef unsigned char ISC_UCHAR;
#endif

// file functions diverse

// ct 2009-05-08

char *fileversion()
{
    char *sz_return = (char *) MALLOC(255);
    strcpy(sz_return, "adhoc 20091010");
    return sz_return;
}

// file functions with BLObs

// fsg 2008-10-20 
// gh 2009-01-27 adapted to FreeAdhocUDF
// ct 2009-07-15 moved from blob_functions
// Martens & Prahl Versicherungskontor GmbH & Co. KG load blob from file 
// thanks for code from Frank Schlottmann-Gödde

BLOBCALLBACK blob_load(char *s, BLOBCALLBACK b)
{
    if (b->blob_handle) {
    	 unsigned char *buffer;
	     int buffsize = 4096;
	     FILE *file;
	     file = fopen(s, "rb");
	     if (file) {
	       buffer = (unsigned char *) malloc(buffsize);
	       if (buffer) {
		        int readSize;
		        for (;;) {
		          readSize = fread(buffer, 1, buffsize, file);
		          b->blob_put_segment(b->blob_handle, buffer, readSize);
		          if (readSize != buffsize) {
			           break;
		          }
		        }
		        free(buffer);
	       }
	       fclose(file);
	     }
    }
    return b;
}

// fsg 2009-03-05
// gh 2009-09-15 adapted to FreeAdhocUDF
// Martens & Prahl Versicherungskontor GmbH & Co. KG save blob to file 
// thanks for code from Frank Schlottmann-Gödde

ISC_LONG *blob_save(char *s, BLOBCALLBACK b)
{
    if (!s || !b || !b->blob_handle) {
	return NULL;
    } else {
	ISC_LONG *res = MALLOC(sizeof(ISC_LONG));
	FILE *file = fopen(s, "wb");
	*res = -2;

	if (file) {
	    unsigned char *buffer = malloc(b->blob_max_segment);
	    *res = -1;
	    if (buffer) {
		unsigned short len;
		*res = 0;
		while (b->blob_get_segment(b->blob_handle, buffer,
			b->blob_max_segment, &len)) {
		    *res += fwrite(buffer, 1, len, file);
		}
		free(buffer);
	    }
	    fclose(file);
	}
	return res;
    }
}

// file functions with strings

// ct 2009-10-09
// load string from file

char *string_load(char *s, BLOBCALLBACK b)
{
    if (b->blob_handle) {
    	 unsigned char *buffer;
	     int buffsize = 4096;
	     FILE *file;
	     file = fopen(s, "rb");
	     if (file) {
	       buffer = (unsigned char *) malloc(buffsize);
	       if (buffer) {
		        int readSize;
		        for (;;) {
		          readSize = fread(buffer, 1, buffsize, file);
		          b->blob_put_segment(b->blob_handle, buffer, readSize);
		          if (readSize != buffsize) {
			           break;
		          }
		        }
		        free(buffer);
	       }
	       fclose(file);
	     }
    }
    return b;
}

// ct 2009-10-09
// save string to file

int *string_save(char *s, char b)
{
    if (!s || !b) {
	return NULL;
    } else {
	ISC_LONG *res = MALLOC(sizeof(ISC_LONG));
	FILE *file = fopen(s, "wb");
	*res = -2;

	if (file) {
	    unsigned char *buffer = malloc(b->blob_max_segment);
	    *res = -1;
	    if (buffer) {
		unsigned short len;
		*res = 0;
		while (b->blob_get_segment(b->blob_handle, buffer,
			b->blob_max_segment, &len)) {
		    *res += fwrite(buffer, 1, len, file);
		}
		free(buffer);
	    }
	    fclose(file);
	}
	return res;
    }
}
