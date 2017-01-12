/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring / Peter Mandrella / Georg Horn
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 *
 * File Name:   blob_functions.h
 * Description: This header contains the prototype definitions
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

/* Blob passing structure */
// InterBase uses follows, FireBird uses from ibase.h
#ifndef FB_API_VER
typedef struct blobcallback {
// ct 20081030 changed for InterBase from 
//  short (*blob_get_segment) (void *hnd, void *buffer, long buf_size, long *result_len);
// to
    short (*blob_get_segment) (void* hnd, void* buffer, ISC_USHORT buf_size, ISC_USHORT* result_len);
    isc_blob_handle *blob_handle;
    ISC_LONG blob_number_segments;
    ISC_LONG blob_max_segment;
    ISC_LONG blob_total_length;
    void (*blob_put_segment) (void *hnd, const void *buffer, ISC_USHORT buf_size);
    ISC_LONG(*blob_lseek) (void *hnd, ISC_USHORT mode, ISC_LONG offset);
} *BLOBCALLBACK;
#endif

// internal functions
char         * intern_blobaspchar (BLOBCALLBACK, int);
// exported functions
// transform
char         * blobaspchar (BLOBCALLBACK);
BLOBCALLBACK   strblob (char *, BLOBCALLBACK);
char         * excelblob (BLOBCALLBACK);
// edit
char         * blobleft (BLOBCALLBACK, int *);
char         * blobmid (BLOBCALLBACK, ISC_LONG *, int *);
char         * r_blobmid (BLOBCALLBACK, int *, int *);
char         * blobright (BLOBCALLBACK, int *);
BLOBCALLBACK   blobcat (BLOBCALLBACK, BLOBCALLBACK, BLOBCALLBACK);
BLOBCALLBACK   blobcatstr (BLOBCALLBACK, char *, BLOBCALLBACK);
BLOBCALLBACK   blobreplacestring (BLOBCALLBACK, char *, char *, int *, int *, BLOBCALLBACK);
int          * blobsubstr (BLOBCALLBACK,char *);
char         * blobline (BLOBCALLBACK, int*);
// calculate
ISC_LONG     * blobsize (BLOBCALLBACK);
ISC_LONG     * blobmaxsegmentlength (BLOBCALLBACK);
ISC_LONG     * blobsegmentcount (BLOBCALLBACK);
int          * bloblinecount (BLOBCALLBACK);
int          * blobwordcount (BLOBCALLBACK);
// compare
int          * blobcompare (BLOBCALLBACK,BLOBCALLBACK);
int          * b_strcmp (BLOBCALLBACK,BLOBCALLBACK);
//find
int          * b_strpos (char *, BLOBCALLBACK);
int          * b_textpos (char *, BLOBCALLBACK);

