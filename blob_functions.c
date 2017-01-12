/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 *
 * File Name:	blob_functions.c
 * Description: this module contains UDFs to generate and manipulate BLObs
 * for exported functions list see blob_functions.h
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
#include "string_functions.h"
#include "blob_functions.h"

#ifndef FB_API_VER
/* define ISC_UCHAR for interbase */
typedef unsigned char ISC_UCHAR;
#endif

// intern
// pm 2004-08-21
// pm 2004-08-25 rtrim eingebaut
// gh 2006-03-01 added do_MALLOC
// pm 2006-09-18 added evaluation of blob_max_segment and blob_get_segment return value
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK
// ct 2007-12-03 (re)added do_malloc 
// ct 2009-01-20 changed to ISC_UCHAR *
char *intern_blobaspchar(BLOBCALLBACK blob, int do_malloc)
{
  char *result;
  ISC_USHORT bytes_read;
  ISC_LONG bytes_left, total_bytes_read;

  bytes_read = 0;
  total_bytes_read = 0;

  if (blob->blob_handle == NULL) {
    result = do_malloc ? MALLOC(1) : malloc(1);
  } else {
    result = do_malloc ? MALLOC(blob->blob_total_length + 1) : malloc(blob->blob_total_length + 1);

    memset(result,0,blob->blob_total_length + 1);

    bytes_left = blob->blob_total_length;
    while (bytes_left > 0) {
      if (!blob->blob_get_segment(blob->blob_handle, (ISC_UCHAR *) result + total_bytes_read,
                             blob->blob_total_length, &bytes_read))
        break;

      total_bytes_read += bytes_read;
      bytes_left -= bytes_read;
    }
  }

  while (total_bytes_read>0 && isspace(result[total_bytes_read-1]))
    --total_bytes_read;

  result[total_bytes_read] = '\0';

  return result;
}

// ct 2006-12-04 implementation of NULL for FB 2.0
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK 
// ct 2007-12-03 (re)added do_malloc 
char *blobaspchar(BLOBCALLBACK blob)
{
  if (blob->blob_handle == NULL)
    return NULL;
  else
    return intern_blobaspchar(blob, 1);
}

// pm 2004-08-21
// ct 2006-12-04 implementation of NULL for FB 2.0
// ct 2007-08-13 change int to ISC_LONG
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK 
// ct 2007-12-03 (re)added do_malloc 
char * blobmid (BLOBCALLBACK blob, ISC_LONG *from, int *count)
{
  if (blob->blob_handle == NULL || !from || !count)
    return NULL;
  else
  {
    ISC_LONG len = blob->blob_total_length;
    char * result = intern_blobaspchar(blob, 1);
    int f = *from, c = *count;

    if (c < 0) c = 0;
    if (f < 0) f = 0;
    else 
      if (f > len) f = len;
    if (f + c > len) c = len - f;

    memmove(result, result + f, c);
    result[c] = '\0';

    return result;
  }
}

// ct 2007-01-30
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK 
// ct 2007-12-03 (re)added do_malloc 
char * r_blobmid (BLOBCALLBACK blob, int *from, int *count)
{
  if (blob->blob_handle == NULL || !from || !count)
    return NULL;
  else
  {
    ISC_LONG len = blob->blob_total_length;
    char * result = intern_blobaspchar(blob, 1);
    int f = *from, c = *count;

    if (c < 0) c = 0;
    if (f < 1) f = 1;
    else
      if (f > len) f = len;
    if (f + c > len) c = len - f;

    memmove(result, result + f - 1, c);
    result[c] = '\0';

    return result;
  }
}

// pm 2004-08-21
// pm 2004-08-25 rtrim eingebaut
// pm 2006-09-18 added evaluation of blob_max_segment and blob_get_segment return value
// ct 2006-12-04 implementation of NULL for FB 2.0
// gh 2007-08-22 changed some types and struct blob to BLOBCALLBACK 
char * blobleft (BLOBCALLBACK blob, int *count)
{
  if (blob->blob_handle == NULL || !count)
    return NULL;
  else
  {
    ISC_LONG from = 0;
    return blobmid(blob,&from,count);
  }
}

// pm 2004-08-21
// ct 2006-12-04 implementation of NULL for FB 2.0
// gh 2007-08-22 changed some types and struct blob to BLOBCALLBACK 
char * blobright (BLOBCALLBACK blob, int * count)
{
  if (blob->blob_handle == NULL || !count)
    return NULL;
  else
  {
    ISC_LONG f = blob->blob_total_length - *count;
    return blobmid(blob, &f, count);
  }
}

// pm 2004-08-21
// ct 2006-12-04 implementation of NULL for FB 2.0
// gh 2007-08-22 changed some types and struct blob to BLOBCALLBACK 
ISC_LONG * blobsize (BLOBCALLBACK blob)
{
  if (blob->blob_handle == NULL)
    return NULL;
  else
  {
    ISC_LONG * result = MALLOC(sizeof(ISC_LONG));
    *result = blob->blob_total_length;
    return result;
  }
}

// pm 2004-08-21
// pm 2004-08-25 rtrim eingebaut
// gh 2007-08-22 changed some types and struct blob to BLOBCALLBACK 
// ct 2007-08-22 implementation of NULL for FB 2.0
// ct 2009-01-20 changed to ISC_UCHAR *
BLOBCALLBACK strblob(char *str, BLOBCALLBACK result)
{
/*
  if (!str)
    return NULL;
  else
  {
*/
    char *pc;

    if (str) {
	pc = str + strlen(str) - 1;
	while (pc >= str && isspace(*pc))
	    --pc;
    } else {
	str = "";
	pc = str - 1;
    }

    result->blob_put_segment(result->blob_handle, (ISC_UCHAR*) str, pc + 1 - str);
    return result;
//  }
}

// gh 2006-02-22
// pm 2006-09-18 added evaluation of blob_max_segment and blob_get_segment return value
// gh 2007-08-22 changed some types and struct blob to BLOBCALLBACK
// gh 2008-02-28 changed MALLOC to malloc
// ct 2008-02-28 null for FB2
// ct 2009-01-20 changed to ISC_UCHAR *
BLOBCALLBACK blobcat(BLOBCALLBACK b1, BLOBCALLBACK b2, BLOBCALLBACK result)
{
    ISC_USHORT bytes_read;
    char *buffer;
    ISC_LONG bytes_left, buflen = 0;

    if (b1 && b1->blob_handle && b1->blob_total_length > buflen) {
	buflen = b1->blob_total_length;
    }
    if (b2 && b2->blob_handle && b2->blob_total_length > buflen) {
	buflen = b2->blob_total_length;
    }
    buffer = malloc(buflen);

    if (b1 && b1->blob_handle) {
	bytes_left = b1->blob_total_length;
	while (bytes_left > 0) {
	    if (!b1->blob_get_segment(b1->blob_handle, (ISC_UCHAR *) buffer, b1->blob_max_segment,
              		&bytes_read))
              break;
	    result->blob_put_segment(result->blob_handle, (ISC_UCHAR *) buffer, bytes_read);
	    bytes_left -= bytes_read;
	}
    }

    result->blob_put_segment(result->blob_handle, (ISC_UCHAR *) "\r\n", 2);

    if (b2 && b2->blob_handle) {
	bytes_left = b2->blob_total_length;
	while (bytes_left > 0) {
	    if (!b2->blob_get_segment(b2->blob_handle, (ISC_UCHAR *) buffer, b2->blob_max_segment,
		&bytes_read))
              break;
	    result->blob_put_segment(result->blob_handle, (ISC_UCHAR *) buffer, bytes_read);
	    bytes_left -= bytes_read;
	}
    }

    free(buffer);
    return result;
}

// gh 2006-02-22
// pm 2006-09-18 added evaluation of blob_max_segment and blob_get_segment return value
// gh 2007-08-22 changed some types and struct blob to BLOBCALLBACK
// gh 2008-02-28 changed MALLOC to malloc
// ct 2009-01-20 changed to ISC_UCHAR *
BLOBCALLBACK blobcatstr(BLOBCALLBACK b1, char *str, BLOBCALLBACK result)
{
    ISC_USHORT bytes_read;
    ISC_LONG bytes_left;
    char *buffer, *p;

    if (b1 && b1->blob_handle) {
	buffer = malloc(b1->blob_total_length);
	bytes_left = b1->blob_total_length;
	while (bytes_left > 0) {
	    if (!b1->blob_get_segment(b1->blob_handle, (ISC_UCHAR *) buffer, b1->blob_max_segment,
		&bytes_read))
              break;
	    result->blob_put_segment(result->blob_handle, (ISC_UCHAR *) buffer, bytes_read);
	    bytes_left -= bytes_read;
	}
	free(buffer);
    }

    result->blob_put_segment(result->blob_handle, (ISC_UCHAR *) "\r\n", 2);

    p = str + strlen(str) - 1;
    while (p >= str && isspace(*p))
	--p;
    result->blob_put_segment(result->blob_handle, (ISC_UCHAR *) str, p + 1 - str);

    return result;
}

// gh 2006-02-28
// ct 2006-12-04 implementation of NULL for FB 2.0
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK
// ct 2007-12-03 (re)added do_malloc
// ct 2009-01-15 changed to unsigned char
char *excelblob(BLOBCALLBACK blob)
{
  if (blob->blob_handle == NULL)
    return NULL;
  else
  {
    char *string = intern_blobaspchar(blob, 0);
    char *result = intern_excelstring(string, 1);
    free(string);
    return result;
  }
}

// gh 2006-02-28
// gh 2007-08-22 changed some types and struct blob to BLOBCALLBACK
// gh 2007-08-22 removed do_malloc in call intern_blobaspchar and intern_replacestring
// ct 2007-12-03 (re)added do_malloc
BLOBCALLBACK blobreplacestring(BLOBCALLBACK b, char *subs, char *replby, int *all, int *case_insensitive, BLOBCALLBACK result)
{
    char *string1 = intern_blobaspchar(b, 0);
    char *string2 = intern_replacestring(string1, subs, replby, all, case_insensitive, 0);
    free(string1);
    result = strblob(string2, result);
    free(string2);
    return result;
}

// pm 2006-09-24
// ct 2006-12-04 implementation of NULL for FB 2.0
// gh 2007-08-22 changed some types and struct blob to BLOBCALLBACK 
ISC_LONG * blobmaxsegmentlength (BLOBCALLBACK  blob)
{
  if (blob->blob_handle == NULL)
    return NULL;
  else
  {
    ISC_LONG * result = MALLOC(sizeof(ISC_LONG));
    *result = blob->blob_max_segment;
    return result;
  }
}

// pm 2006-09-24
// ct 2006-12-04 rewritten as pointer
// gh 2007-08-22 changed some types and struct blob to BLOBCALLBACK 
ISC_LONG * blobsegmentcount (BLOBCALLBACK  blob)
{
  if (blob->blob_handle == NULL)
    return NULL;
  else
  {
    ISC_LONG * result = MALLOC(sizeof(ISC_LONG));
    *result = blob->blob_number_segments;
    return result;
  }
}

// pm 2006-09-24
// ct 2006-12-04 implementation of NULL for FB 2.0
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK
// ct 2007-12-03 (re)added do_malloc 
// ct 2010-03-10 fixed bug in calculating endpos (thanks to Lukas Mai from
// Synerpy)
char * blobline (BLOBCALLBACK  blob, int* line)
{
  if (blob->blob_handle == NULL || !line)
    return NULL;
  else
  {
    char *data = intern_blobaspchar(blob, 0);
    char *result = MALLOC(MAX_STRINGBUFFER_SIZE);
    char *p = data;
    char *endpos;

    while ((*line)-- > 1 && p != NULL)
    {
      p = strchr(p,'\n');
      if (p != NULL) p++;
    }

    if (p == NULL)
      *result = 0;
    else
    {
      endpos = strchr(p,'\n');
      if (endpos != NULL) {
         if (endpos> p &&  *(endpos-1) == '\r')
           *(endpos-1) = 0;
         else
           *endpos = 0;
      }               
      strcpy(result,p);
    }
    free(data);
    return result;
  }
}

// pm 2006-09-24
// ct 2006-12-04 implementation of NULL for FB 2.0
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK
// ct 2007-12-03 (re)added do_malloc  
int * blobcompare (BLOBCALLBACK b1, BLOBCALLBACK b2)
{
  if (b1->blob_handle == NULL || b2->blob_handle == NULL)
    return NULL;
  else
  {
    char *data1 = intern_blobaspchar(b1, 0);
    char *data2 = intern_blobaspchar(b2, 0);
    int * result = MALLOC(sizeof(int));
    *result = !strcmp(data1,data2);
    free(data1);
    free(data2);
    return result;
  }
}

// ct 2007-07-06 version for rFunc similar to blobcompare
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK
// ct 2007-12-03 (re)added do_malloc  
int * b_strcmp (BLOBCALLBACK b1, BLOBCALLBACK b2)
{
  if (b1->blob_handle == NULL || b2->blob_handle == NULL) {
    return NULL;
  } else {
    char *data1 = intern_blobaspchar(b1, 0);
    char *data2 = intern_blobaspchar(b2, 0);
    int * result = MALLOC(sizeof(int));
    *result = strcmp(data2,data1);
    free(data2);
    free(data1);
    return result;
  }
}

// pm 2006-09-24
// ct 2006-12-04 implementation of NULL for FB 2.0
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK
// ct 2007-12-03 (re)added do_malloc  
int * blobsubstr (BLOBCALLBACK blob, char *s)
{
  if (blob->blob_handle == NULL || !s)
    return NULL;
  else
  {
    char *data = intern_blobaspchar(blob, 0);
    char *pos = strstr(data,s);
    int * result = MALLOC(sizeof(int));
    *result = (pos==NULL ? -1 : pos-data);
    free(data);
    return result;
  }
}

// pm 2006-09-24
// ct 2006-12-04 implementation of NULL for FB 2.0
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK
// ct 2007-12-03 (re)added do_malloc  
int * bloblinecount (BLOBCALLBACK blob)
{
  if (blob->blob_handle == NULL)
    return NULL;
  else
  {
    char *data = intern_blobaspchar(blob, 0);
    int *result = MALLOC(sizeof(int));
    char *p = data;
    int count = 0;

    while (*p)
    {
      if (*p == '\n')
        count++;
      p++;
    }
    if (p > data && p[-1] != '\n')
      count++;
    free(data);
    *result = count;
    return result;
  }
}

// ct 2006-12-04
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK
// ct 2007-12-03 (re)added do_malloc  
int * blobwordcount (BLOBCALLBACK blob)
{
  if (blob->blob_handle == NULL)
    return NULL;
  else
  {
    char *data = intern_blobaspchar(blob, 0);
    int *result = MALLOC(sizeof(int));
    char *p = data;
    int wort = 0;
    int count = 0;

    while (*p)
    {
      if(*p == ' ' || *p == '\n' || *p == '\t')
        wort = 0;
      else
      {
        if(wort == 0)
        {
          wort = 1;
          count++;
        }
      }
      p++;
    }
    free(data);
    *result = count;
    return result;
  }
}

// ct 2007-05-21 version for rFunc similar to blobsubstr
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK
// ct 2007-12-03 (re)added do_malloc  
int * b_strpos (char *s, BLOBCALLBACK blob)
{
  if (blob->blob_handle == NULL || !s)
    return NULL;
  else
  {
    char *data = intern_blobaspchar(blob, 0);
    char *pos = strstr(data,s);
    int * result = MALLOC(sizeof(int));
    *result = (pos==NULL ? 0 : pos-data + 1);
    free(data);
    return result;
  }
}

// gh 2007-07-04 version for rFunc similar to blobsubstr
// gh 2007-08-22 removed do_malloc, changed some types and struct blob to BLOBCALLBACK
// ct 2007-12-03 (re)added do_malloc 
// gh 2010-11-04 use of intern_upper 
int *b_textpos(char *s, BLOBCALLBACK blob)
{
    if (blob->blob_handle == NULL || !s) {
	return NULL;
    } else {
	char *data = intern_blobaspchar(blob, 0);
	char *data2 = intern_upper(data, 0);
	char *s2 = intern_upper(s, 0);
	char *pos = strstr(data2, s2);
	int *result = MALLOC(sizeof(int));
	*result = (pos == NULL ? 0 : pos - data2 + 1);
	free(data);
	free(data2);
	free(s2);
	return result;
    }
}

// ct 2009-07-15 moved to file_functions
//BLOBCALLBACK blob_load(char *s, BLOBCALLBACK b)
