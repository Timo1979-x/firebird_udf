/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 * some code from Robert Loipfinger rl copyright (c) 2000 ADITO Software GmbH
 * some code from rFunc UDF library copyright (c) 1998-2003 Polaris Software
 *
 * File Name:   string_functions.c
 * Description: this module contains UDFs to generate and manipulate strings
 * for exported functions list see string_functions.h
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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "string_functions.h"

// ct 2008-12-03 function version has moved to diverse_functions.c
// ct 2008-12-03 function aif has moved to diverse_functions.c
// ct 2009-09-02 function encryptmd5 has moved to checksum_functions.c

// intern

// ISO-8859-1 to upper case table
static unsigned char StringUpTab [256] =
           {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
             16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
             32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
             48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
             64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
             80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
             96,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
            'P','Q','R','S','T','U','V','W','X','Y','Z',123,124,125,126,127,
            128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
            144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
            160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
            176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
            192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
            208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
            192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
            240,209,210,211,212,213,214,247,216,217,218,219,220,221,222,255 };

// ISO-8859-1 to lower case table
static unsigned char StringLoTab [256] =
           {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
             16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
             32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
             48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
             64,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
            'p','q','r','s','t','u','v','w','x','y','z', 91, 92, 93, 94, 95,
             96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
            112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
            128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
            144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
            160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
            176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
            224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
            208,241,242,243,244,245,246,215,248,249,250,251,252,253,254,223,
            224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
            240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255 };

// rFunc 1999-11-27
// WIN-1251 upper case table
static unsigned char upper_win1251[256] =
       {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
        0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
        0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
        0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
        0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
        0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
        0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x7b,0x7c,0x7d,0x7e,0x7f,
        0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
        0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
        0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
        0xb0,0xb1,0xb2,0xb2,0xb4,0xb5,0xb6,0xb7,0xa8,0xb9,0xaa,0xbb,0xbc,0xbd,0xbe,0xaf,
        0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
        0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
        0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
        0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf };

// rFunc 1999-11-27
// WIN-1251 lower case table
static unsigned char lower_win1251[256] =
       {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
        0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
        0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
        0x40,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
        0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x5b,0x5c,0x5d,0x5e,0x5f,
        0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
        0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
        0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
        0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
        0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xb8,0xa9,0xba,0xab,0xac,0xad,0xae,0xbf,
        0xb0,0xb1,0xb3,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
        0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
        0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff,
        0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
        0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff };

// rFunc 2001-05-31
// WIN-1251 latin table
static unsigned char latin_win1251[256] =
       {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
        0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
        0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
        0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
        0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
        0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
        0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
        0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
        0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
        0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
        0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0x45,0xa9,0x45,0xab,0xac,0xad,0xae,0x49,
        0xb0,0xb1,0x49,0x69,0xb4,0xb5,0xb6,0xb7,0x65,0xb9,0x65,0xbb,0xbc,0xbd,0xbe,0x69,
        0x41,0x42,0x56,0x47,0x44,0x45,0x4a,0x5a,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,0x50,
        0x52,0x53,0x54,0x55,0x46,0x48,0x43,0x43,0x53,0x53,0x5f,0x49,0x5f,0x45,0x55,0x4a,
        0x61,0x62,0x76,0x67,0x64,0x65,0x6a,0x7a,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,
        0x72,0x73,0x74,0x75,0x66,0x68,0x63,0x63,0x73,0x73,0x5f,0x69,0x5f,0x65,0x75,0x6a };       

// used in gsoundex
static char * TabGruppenVon    = "SCSZCZTZTSDSKSQUPFPHUEAEOEEIEYEUAUOU";
static char * TabGruppenNach   = "C C C C C C X KVV V Y E ÷ AYAYOYA@@ ";
static char * TabErsVon        = "ZKGQƒ‹IJﬂFWPT@¡¿»…⁄Ÿ“”ÕÃ";
static char * TabErsNach       = "CCCCEYYYSVVBDUAAEEUUOOYY";
static char * GueltigeZeichen1 = "ABCDLMNORSUVWXY÷";
static char * GueltigeZeichen2 = "ABCDEFGHIJKLMNOPQRSTUVWXYZƒ÷‹";

// pm 2005-10-03
// used in stripstring and stripstringhold
int findchar (char c, char *sz)
{
  int i;
  int len = strlen(sz);

  for (i = 0 ;
     (i < len) && (c != sz[i]) ;
     i++) ;

  if (i == len)
    return 0;
  else
    return -1;
}

// pm 2005-10-03
// used in replacestring
// ct 2010-01-13 replace char * with unsigned char *
//               thanks to Lukas Mai from Synerpy
static int strincomp(unsigned char *s1, unsigned char *s2, int nmax)
{
  while (*s1 && StringUpTab[(int) *s1] == StringUpTab[(int) *s2] && nmax)
    s1++, s2++, nmax--;

  if (nmax == 0)
    return 0;
  else
    return (StringUpTab[(int) *s1] - StringUpTab[(int) *s2]);
}

// Implementations
// ----------

// ct 2006-11-20 implementation of NULL for FB 2.0
char * character (int *c)
{
  if (!c)
    return NULL;
  else
  {
    char *sz = (char *) MALLOC (2);
    sz[0] = (char) *c;
    sz[1] = (char) 0;

    return sz;
  }
}

char * crlf ()
{
  char * sz = (char *) MALLOC (3);
  sz[0] = (char) 13;
  sz[1] = (char) 10;
  sz[2] = (char) 0;
  return sz;
}

// ct 2006-02-22
char * lf ()
{
  char * sz = (char *) MALLOC (2);
  sz[0] = (char) 10;
  sz[1] = (char) 0;
  return sz;
}

// ct 2006-10-05
char * tab ()
{
  char * sz = (char *) MALLOC (2);
  sz[0] = (char) 9;
  sz[1] = (char) 0;
  return sz;
}

// ct 2006-10-05
char * nbsp ()
{
  char * sz = (char *) MALLOC (2);
  sz[0] = (char) 160;
  sz[1] = (char) 0;
  return sz;
}

// ct 2006-10-05
char * dqm ()
{
  char * sz = (char *) MALLOC (2);
  sz[0] = (char) 34;
  sz[1] = (char) 0;
  return sz;
}

// ct 2006-10-05
char * sqm ()
{
  char * sz = (char *) MALLOC (2);
  sz[0] = (char) 39;
  sz[1] = (char) 0;
  return sz;
}

// ct 2010-11-06
char * dsqm ()
{
  char * sz = (char *) MALLOC (3);
  sz[0] = (char) 39;
  sz[1] = (char) 39;
  sz[2] = (char) 0;
  return sz;
}

// ct 2006-11-20 implementation of NULL for FB 2.0
// gh 2007-01-17 intercept negativ values for count
// tt 2010-12-06 set int to unsigned int
char * left (char *sz, int *count)
{
  if(!sz || !count)
    return NULL;
  else
  {
    int i;
    unsigned int c = (*count > 0) ? *count : 0;
    int len = (strlen(sz) < c) ? strlen(sz) : c;
    char * sz_result = (char *) MALLOC (len + 1);

    for (i = 0 ; i < len ; i++)
      sz_result[i] = sz[i];

    sz_result[len] = (char) 0;
    return sz_result;
  }
}

// ct 2006-11-20 implementation of NULL for FB 2.0 
// ct 2007-01-22 intercept negativ values for count
char * right (char *sz, int *count)
{
  if(!sz || !count)
    return NULL;
  else
  {
    int c = (*count > 0) ? *count : 0;
    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (c + 1);
    int i, j;

    for (j = 0, i = (c > len) ? 0 : len - c ;
        i < len ; i++, j++)
      sz_result[j] = sz[i];

    sz_result[j] = (char) 0;
    return sz_result;
  }
}

// ct 2006-11-20 implementation of NULL for FB 2.0
// gh 2007-01-17 intercept negativ values for count 
char * mid (char *sz, int *index, int *count)
{
  if(!sz || !index || !count)
    return NULL;
  else
  {
    int ind = (*index > 0) ? *index : 0;
    int c = (*count > 0 && *index >= 0) ? *count : 0;
    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (c + 1);
    int i;

    for (i = ind ; (i < len) && (i - ind < c); i++)
      sz_result[i - ind] = sz[i];
    sz_result[i - ind] = (char) 0;

    return sz_result;
  }
}

// pm 2005-10-03
// f_mid1 = f_copy = f_mid with turned parameters and starting param 2 with 1
char * mid1 (char *sz, int *from, int *count)
{
  int f = *from - 1;
  return mid(sz, &f, count);
}

// ct 2006-11-20 implementation of NULL for FB 2.0 
// ct 2007-01-17 intercept negativ values for anz 
// ct 2009-05-04 now use code form mid
char * strcopy (char *sz, int *from, int *count)
{
  return mid(sz, from, count);
}

// pm 2005-10-03
char * space (int *n)
{
  return strofchar(" ", n);
}

// ct 2006-11-20 implementation of NULL for FB 2.0
// line_wrap algorithm
/*
 * 1. Get the length of the string.
 * 2. If "the rest" of the string is smaller than col_width, just return
 *    it.
 * 3. Otherwise, let 'j' be the end of the column, and decrement j to
 *    a word boundary.
 *    a. Find a word boundary.
 *    b. If j = i (meaning the word is too long), then the word has
 *       has to be chopped.
 */
char * linewrap (char *sz, int *index, int *col_width)
{
  if(!sz || !index || !col_width)
    return NULL;
  else
  {
    int len = strlen(sz); /* (1) */
    int i = *index;
    int j;
    char * sz_result;

    if ((len - i) <= *col_width) /* (2) */
      j = len - i;
    else {
      j = i + *col_width - 1;
      while (j > i && !isspace(sz[j]))
        j--; /* (2a) */
      if (j == i)
        j = *col_width;
      else
        j = j - i + 1;
    }

    sz_result = (char *) MALLOC(j + 1);
    for (i = 0 ; i < j ; i++)
      sz_result[i] = sz[i + *index];
    sz_result[j] = '\0';
    return sz_result;
  }
}

// ct 2006-11-20 implementation of NULL for FB 2.0
char * ltrim (char *sz)
{
  if(!sz)
    return NULL;
  else
  {
    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);
    int i;

    for (i = 0 ;
       (i < len) && isspace(sz[i]) ;
       i++) ;

    sprintf(sz_result, "%s", &sz[i]);
    return sz_result;
  }
}

// ct 2006-11-20 implementation of NULL for FB 2.0 
char * rtrim (char *sz)
{
  if(!sz)
    return NULL;
  else
  {

    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);
    int i, j;

    for (i = len - 1 ;
       (i >= 0) && isspace(sz[i]) ;
       i--) ;

    for (j = 0 ; j <= i ; j++)
      sz_result[j] = sz[j];
    sz_result[j] = (char) 0;

    return sz_result;
  }
}

// ct 2006-11-20 implementation of NULL for FB 2.0
char * lrtrim (char *sz)
{
  if(!sz)
    return NULL;
  else
  {
    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);
    int i, j, k;

    for (i = 0 ;
       (i < len) && isspace(sz[i]) ;
       i++) ;

    for (j = len - 1 ;
       (j >= 0) && isspace(sz[j]) ;
       j--) ;

    for (k = i ; k <= j ; k++)
      sz_result[k - i] = sz[k];
    sz_result[k - i] = (char) 0;

    return sz_result;
  }
}

// ct 2006-11-20 implementation of NULL for FB 2.0
char * padleft (char *sz, char *sz_pad, int *size)
{
  if(!sz || !sz_pad || !size)
    return NULL;
  else
  {
    int len_sz = strlen(sz);
    int len_sz_pad = strlen(sz_pad);
    int len_sz_result = *size > len_sz ? *size : len_sz;
    char * sz_result = (char *) MALLOC (len_sz_result + 1);
    int i, j;

    sz_result[len_sz_result] = (char) 0;

    for (i = len_sz - 1 ; i >= 0 ; i--)
      sz_result[len_sz_result - (len_sz - i - 1) - 1] = sz[i];

    for (i = len_sz_result - (len_sz - i - 1) - 1 ;
       i >= 0 ;
       ) {
      for (j = len_sz_pad - 1 ;
         (j >= 0) && (i >= 0) ;
         j--, i--)
      sz_result[i] = sz_pad[j];
    }

    return sz_result;
  }
}

// ct 2007-02-03 version for rfunc with swapped parameter 2 and 3
char * r_padleft (char *sz, int *size, char *sz_pad)
{
  if(!sz || !sz_pad || !size)
    return NULL;
  else
  {
    int len_sz = strlen(sz);
    int len_sz_pad = strlen(sz_pad);
    int len_sz_result = *size > len_sz ? *size : len_sz;
    char * sz_result = (char *) MALLOC (len_sz_result + 1);
    int i, j;

    sz_result[len_sz_result] = (char) 0;

    for (i = len_sz - 1 ; i >= 0 ; i--)
      sz_result[len_sz_result - (len_sz - i - 1) - 1] = sz[i];

    for (i = len_sz_result - (len_sz - i - 1) - 1 ;
       i >= 0 ;
       ) {
      for (j = len_sz_pad - 1 ;
         (j >= 0) && (i >= 0) ;
         j--, i--)
      sz_result[i] = sz_pad[j];
    }

    return sz_result;
  }
}

// ct 2006-11-20 implementation of NULL for FB 2.0 
char * padright (char *sz, char *sz_pad, int *size)
{
  if(!sz || !sz_pad || !size)
    return NULL;
  else
  {
    int len_sz = strlen(sz);
    int len_sz_pad = strlen(sz_pad);
    int len_sz_result = *size > len_sz ? *size : len_sz;
    char * sz_result = (char *) MALLOC (len_sz_result + 1);
    int i, j;

    sz_result[len_sz_result] = (char) 0;

    for (i = 0 ; i < len_sz ; i++)
      sz_result[i] = sz[i];

    while (i < len_sz_result)
      for (j = 0 ;
         (j < len_sz_pad) && (i < len_sz_result) ;
         j++, i++)
        sz_result[i] = sz_pad[j];

    return sz_result;
  }
}

// ct 2007-02-03 version for rfunc with swapped parameter 2 and 3
char * r_padright (char *sz, int *size, char *sz_pad)
{
  if(!sz || !sz_pad || !size)
    return NULL;
  else
  {
    int len_sz = strlen(sz);
    int len_sz_pad = strlen(sz_pad);
    int len_sz_result = *size > len_sz ? *size : len_sz;
    char * sz_result = (char *) MALLOC (len_sz_result + 1);
    int i, j;

    sz_result[len_sz_result] = (char) 0;

    for (i = 0 ; i < len_sz ; i++)
      sz_result[i] = sz[i];

    while (i < len_sz_result)
      for (j = 0 ;
         (j < len_sz_pad) && (i < len_sz_result) ;
         j++, i++)
        sz_result[i] = sz_pad[j];

    return sz_result;
  }
}

// gh 2006-11-23
// gh 2006-11-23 implementation of NULL for FB 2.0 
char *padcenter(char *sz, char *sz_pad, int *size)
{
    if (!sz || !sz_pad || !size) {
        return NULL;
    } else {
        int len_sz = strlen(sz);
        int len_sz_pad = strlen(sz_pad);
        int len_sz_result = *size > len_sz ? *size : len_sz;
        char *sz_result = (char *) MALLOC(len_sz_result + 1);
        char *p = sz_result;
        int left, right;

        if (len_sz >= *size) {
            /* nothing to pad at all */
            strcpy(sz_result, sz);
            return sz_result;
        }

        if (((*size - len_sz) % 2) == 0) {
            /* even number of chars to pad */
            left = (*size - len_sz) / 2;
            right = left;
        } else {
            /* odd: one more char on the left side */
            left = (*size - len_sz + 1) / 2;
            right = left - 1;
        }

        /* pad left */
        while (left > 0) {
            int l = (len_sz_pad < left) ? len_sz_pad : left;
            strncpy(p, sz_pad, l);
            left -= l;
            p += l;
        }

        /* copy string */
        strcpy(p, sz);
        p += len_sz;

        /* pad right */
        while (right > 0) {
            int l = (len_sz_pad < right) ? len_sz_pad : right;
            strncpy(p, sz_pad, l);
            right -= l;
            p += l;
        }
        *p = '\0';

        /* faedisch */
        return sz_result;
    }
}

// gh 2006-04-26
// ct 2006-11-20 implementation of NULL for FB 2.0
// gh 2010-10-12 fixed crash when sz_replace is empty
//               thanks to Alexandr Morozov
char *replace(char *sz, char *sz_replace, char *sz_with)
{
    if (!sz || !sz_replace || !sz_with) {
	return NULL;
    } else {
	int len_sz = strlen(sz);
	int len_sz_replace = strlen(sz_replace);
	int len_sz_with = strlen(sz_with);
	int i, j;
	int k = 0;
	char sz_res[MAX_STRINGBUFFER_SIZE];
	char *sz_result;

	for (i = 0; i < len_sz;) {
	    if ((len_sz_replace > 0) &&
		    (strncmp(&sz[i], sz_replace, len_sz_replace) == 0)) {
		for (j = 0; j < len_sz_with; j++, k++) {
		    sz_res[k] = sz_with[j];
		}
		i += len_sz_replace;
	    } else {
		sz_res[k++] = sz[i];
		i++;
	    }
	}
	sz_res[k] = (char) 0;
	sz_result = (char *) MALLOC(strlen(sz_res) + 1);
	sprintf(sz_result, "%s", sz_res);
	return sz_result;
    }
}

// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2010-01-13 replace char * with unsigned char *
//               thanks to Lukas Mai from Synerpy
char * soundex (unsigned char * sz)
{
  if(!sz)
    return NULL;
  else
  {

    int i, j;
    char code;
    int len = strlen((const char *) sz);
    char * sz_result = (char *) MALLOC (6);
    sz_result[0] = ' ';
    sz_result[1] = '0';
    sz_result[2] = '0';
    sz_result[3] = '0';
    sz_result[4] = '0';
    sz_result[5] = (char) 0;

    if (strlen((const char *) sz) != 0) {
      sz_result[0] = toupper(sz[0]);

      j = 1;
      i = 1;
      while (i < len && j < 5) {
        switch(sz[i]) {
        case 'B':
        case 'F':
        case 'P':
        case 'V':
        case 'b':
        case 'f':
        case 'p':
        case 'v':
          code = '1'; break;
        case 'C':
        case 'G':
        case 'J':
        case 'K':
        case 'Q':
        case 'S':
        case 'X':
        case 'Z':
        case 'c':
        case 'g':
        case 'j':
        case 'k':
        case 'q':
        case 's':
        case 'x':
        case 'z':
          code = '2'; break;
        case 'D':
        case 'T':
        case 'd':
        case 't':
          code = '3'; break;
        case 'L':
        case 'l':
          code = '4'; break;
        case 'M':
        case 'N':
        case 'm':
        case 'n':
          code = '5'; break;
        case 'R':
        case 'r':
          code = '6'; break;
        default:
          code = '0';
        }

        if (code != '0' && code != sz_result[j - 1])
          sz_result[j++] = code;
        i++;
      }
    }
    return sz_result;
  }
}

// rl 2000
// pm 2005-10-22 fixed memory leaks
// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2007-11-13 bug in upper: missed 0-byte
// gh 2007-11-23 fixed more memory leaks
char *gsoundex(char *sz)
{
    if (!sz) {
        return NULL;
    } else {
        int i, j, len;
        char *ps, *pd;
        char sz_subs[3];
        char *sz_result = upper(sz);

        // ---> folgende Zeilen ersetzen stripstringhold:
        ps = sz_result;
        pd = sz_result;
        while (*ps) {
            if (strchr(GueltigeZeichen2, *ps) != NULL) {
                *pd++ = *ps;
            }
            ps++;
        }
        *pd = 0;
        // <---

        len = strlen(sz_result);
        for (i = 0; i < len; i++) {
            strncpy(sz_subs, sz_result + i, 2);
            sz_subs[2] = 0;
            ps = strstr(TabGruppenVon, sz_subs);
            j = ps ? (ps - TabGruppenVon) : -1;
            if (j % 2 == 0 && strlen(sz_subs) == 2) {
                sz_result[i] = TabGruppenNach[j];
                sz_result[i + 1] = TabGruppenNach[j + 1];
            }
        }

        len = strlen(sz_result);
        for (i = 0; i < len; i++) {
            sz_subs[0] = sz_result[i];
            sz_subs[1] = 0;
            ps = strstr(TabErsVon, sz_subs);
            j = ps ? (ps - TabErsVon) : -1;
            if (j != -1) {
                sz_result[i] = TabErsNach[j];
            }
        }

        // ---> folgende Zeilen ersetzen stripstringhold:
        ps = sz_result;
        pd = sz_result;
        while (*ps) {
            if (strchr(GueltigeZeichen1, *ps) != NULL) {
                *pd++ = *ps;
            }
            ps++;
        }
        *pd = 0;
        // <---

        // ---> folgende Zeilen ersetzen die nachfolgende Schleife
        ps = sz_result;
        pd = sz_result;
        while (*ps) {
            if (*ps != *(ps + 1))
                *pd++ = *ps;
            ps++;
        }
        *pd = 0;
        // <---

        /* for (i = 0; i < strlen(sz_result) - 1; i++) {
            if (sz_result[i] == sz_result[i + 1]) {
                sz_result = strrm(sz_result, &i);
            }
        } */

        return sz_result;
    }
}

// ct 2006-11-20 implementation of NULL for FB 2.0
int * stringlength (char *sz)
{
  if(!sz)
    return NULL;
  else
  {
    int *result = MALLOC(sizeof(int));
    *result = strlen(sz);
    return result;
  }
}

// ct 2006-11-20 implementation of NULL for FB 2.0
char * stripstring (char *sz, char *sz_strip_chars)
{
  if(!sz || !sz_strip_chars)
    return NULL;
  else
  {

    int i;
    int j = 0;
    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);

    for (i = 0 ; i < len ; i++)
      if (!findchar(sz[i], sz_strip_chars))
      {
        sz_result[j] = sz[i];
        j++;
      }
    sz_result[j] = (char) 0;
    return sz_result;
  }
}

// rl 2000
// ct 2006-11-20 implementation of NULL for FB 2.0 
char * stripstringhold (char *sz, char *sz_strip_chars)
{
  if(!sz || !sz_strip_chars)
    return NULL;
  else
  {

    int i;
    int j = 0;
    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);

    for (i = 0 ; i < len ; i++)
      if (findchar(sz[i], sz_strip_chars))
      {
        sz_result[j] = sz[i];
        j++;
      }
    sz_result[j] = (char) 0;
    return sz_result;
  }
}

// pm 2005-10-16
char * digits(char *sz)
{
  return stripstringhold(sz,"0123456789");
}

// pm 2004-08-21 rewritten; swapped parameters
// ct 2006-11-20 implementation of NULL for FB 2.0
int * substr (char *sz_search, char *sz)
{
  if(!sz_search || !sz)
    return NULL;
  else
  {
    int *result = MALLOC(sizeof(int));
    char * pc = strstr(sz_search,sz);
    if (pc == NULL)
      *result = -1;
    else
      *result = pc - sz_search;
    return result;
  }
}

// ct 2006-11-30 version with NULL instead of -1
int *substrnull(char *sz_search, char *sz)
{
    if (!sz_search || !sz) {
        return NULL;
    } else {
        char *pc = strstr(sz_search, sz);
        if (pc == NULL) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = pc - sz_search;
            return result;
        }
    }
}

// gh 2006-03-06 the same as above only swapped parameters
// ct 2006-11-20 implementation of NULL for FB 2.0 
int *strsub(char *sz, char *sz_search)
{
    if (!sz || !sz_search) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        char *pc = strstr(sz_search, sz);
        if (pc == NULL) {
            *result = -1;
        } else {
            *result = pc - sz_search;
        }
        return result;
    }
}

// ct 2007-02-03
int *strpos(char *sz, char *sz_search)
{
    if (!sz || !sz_search) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        char *pc = strstr(sz_search, sz);
        if (pc == NULL) {
            *result = 0;
        } else {
            *result = pc - sz_search + 1;
        }
        return result;
    }
}

// ct 2006-11-30 version with NULL instead of -1
int *strsubnull(char *sz, char *sz_search)
{
    if (!sz || !sz_search) {
        return NULL;
    } else {
        char *pc = strstr(sz_search, sz);
        if (pc == NULL) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = pc - sz_search;
            return result;
        }
    }
}

// rl 2000
// pm 2004-08-23 rewritten
// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2007-11-13 bug: missed 0-byte
char * lower (char *sz)
{
  if(!sz)
    return NULL;
  else
  {

    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);
    int i;

    for (i=0; i <= len; i++)
      sz_result[i] = StringLoTab[(unsigned char)sz[i]];

    sz_result[len] = (char) 0;
    return sz_result;
  }
}

// gh 2010-11-04
// used in upper and b_textpos
char *intern_upper(char *sz, int do_MALLOC)
{
    int len = strlen(sz);
    char *sz_result = do_MALLOC ? MALLOC(len + 1) : malloc(len + 1);
    int i;

    for (i = 0; i <= len; i++) {
	sz_result[i] = StringUpTab[(unsigned char) sz[i]];
    }

    sz_result[len] = (char) 0;
    return sz_result;
}

// rl 2000
// pm 2004-08-23 rewritten
// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2007-11-13 bug: missed 0-byte
// gh 2010-11-04 use of intern_upper
char *upper(char *sz)
{
    if (!sz) {
	return NULL;
    } else {
	return intern_upper(sz, 1);
    }
}

// ct 2007-11-28
// only to use with win1251
// similar to rFunc fn_rupper
char * rupper (char *sz)
{
  if(!sz)
    return NULL;
  else
  {
    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);
    int i;

    for (i=0; i <= len; i++)
      sz_result[i] = upper_win1251[(unsigned char)sz[i]];

    sz_result[len] = (char) 0;
    return sz_result;
  }
}

// ct 2007-11-28
// only to use with win1251
// similar to rFunc fn_rlower
char * rlower (char *sz)
{
  if(!sz)
    return NULL;
  else
  {

    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);
    int i;

    for (i=0; i <= len; i++)
      sz_result[i] = lower_win1251[(unsigned char)sz[i]];

    sz_result[len] = (char) 0;
    return sz_result;
  }
}

// ct 2007-11-28
// only to use with win1251
// similar to rFunc fn_rlatin
char * rlatin (char *sz)
{
  if(!sz)
    return NULL;
  else
  {

    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);
    int i;

    for (i=0; i <= len; i++)
      sz_result[i] = latin_win1251[(unsigned char)sz[i]];

    sz_result[len] = (char) 0;
    return sz_result;
  }
}

// pm 2004-08-21 special characters enabled
// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2007-01-18 bug with input upper characters
char * propercase (char *sz)
{
  if(!sz)
    return NULL;
  else
  {

    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);
    int i;
    int word_begin = 1;

    for (i = 0 ; i < len ; i++)
    {
      if ((isalnum(sz[i]) || (unsigned char)(sz[i])>=192) && word_begin)
      {
        word_begin = 0;
        sz_result[i] = StringUpTab [(unsigned char)sz[i]];
      }
      else
      if (!isalnum(sz[i]) && (unsigned char)(sz[i])<192)
      {
        word_begin = 1;
        sz_result[i] = sz[i];
      }
      else
      {
        sz_result[i] = StringLoTab [(unsigned char)sz[i]];
      }
    }
    sz_result[len] = (char) 0;
    return sz_result;
  }
}

// rl 2000
// pm 2004-08-21 changed
// ct 2006-11-20 implementation of NULL for FB 2.0 
char * telefonnr (char *sz, int *asterisk)
{
  if(!sz || !asterisk)
    return NULL;
  else
  {
    int len = strlen(sz);
    char * sz_result = (char *) MALLOC (len + 1);
    int m = 0;
    int i;
    int a = *asterisk;
    for (i = 0; i < len; i++)
    {
      if ((sz[i] >= '0' && sz[i] <= '9') || (sz[i] == '+' && m==0))
      {
        sz_result[m] = sz[i];
        m++;
      }
    }

    if (m < a) m = a;
    while (a)
    {
      sz_result[m-a] = '*';
      a--;
    }

    sz_result[m] = (char) 0;
    return sz_result;
  }
}

// ct 2006-11-20 implementation of NULL for FB 2.0
// tt 2010-12-06 set int to unsigned int
char * strrm(char *sz, int *indx)
{
  if(!sz || !indx || *sz == 0)
    return NULL;
  else
  {
        unsigned int i, j;
        char * sz_result = (char *) MALLOC (strlen(sz));
        j=0;

        for (i=0; i<strlen(sz);i++){
                if (i != *indx)
                {
                        sz_result[j] = sz[i];
                        j++;
                }
        }

    sz_result[strlen(sz)-1] = (char) 0;

    return sz_result;
  }
}


// pm 2005-10-03
// ct 2006-11-20 implementation of NULL for FB 2.0
int * equalstring (char *sz1, char *sz2)
{
  if(!sz1 || !sz2)
    return NULL;
  else
  {
    int *result = MALLOC(sizeof(int));
    *result = (strcmp(sz1,sz2) == 0);
    return result;
  }
}


// pm 2005-10-03
// gh 2006-03-02 edit for ‰ˆ¸
// ct 2006-11-20 implementation of NULL for FB 2.0
// tt 2010-12-06 add implicit assignements
char * findword (char *sz, int *n)
{
  if(!sz || !n)
    return NULL;
  else {
    char *sz_result;
    char *last;

    if (*n < 0 || *n >= (int)strlen(sz))
      return NULL;
    else {
      sz += *n;
      while (*sz != 0 && *sz == ' ')
        sz++;
      last = sz;
      while (*last != 0 && *last != ' ')
        last++;

      sz_result = (char*) MALLOC(last - sz + 1);
      strncpy(sz_result, sz, last-sz);
      sz_result[last-sz] = 0;
    }

    return sz_result;
  }
}

// pm 2005-10-03
// gh 2006-03-01 added do_malloc
// ct 2006-11-20 implementation of NULL for FB 2.0
// gh 2007-08-22 removed do_malloc in call intern_replacestring
// ct 2007-12-03 (re)added do_malloc 
char *replacestring(char *s, char *subs, char *replby, int *all, int *case_insensitive)
{
  if(!s || !subs || !replby || !all || !case_insensitive)
    return NULL;
  else
    return intern_replacestring(s, subs, replby, all, case_insensitive, 1);
}

// gh 2007-08-22 removed do_malloc
// ct 2007-12-03 (re)added do_malloc 
// ct 2010-03-10 changed calculation of len
//               thanks to Lukas Mai from Synerpy
char *intern_replacestring (char *s, char *subs, char *replby, int *all, int *case_insensitive, int do_MALLOC)
{
  char *sz_result;
  char *pd;

  int sl = strlen(s);
  int ssl = strlen(subs);
  int rl = strlen(replby);
  int done = 0;

  if (ssl == 0) {
    sz_result = do_MALLOC ? MALLOC(1) : malloc(1);
    *sz_result = 0;
  } else {
// old    int len = (sl / ssl + 1) * (rl + 1) + 1;
    int len = sl + 1;
    if (rl > ssl) {
       len += (sl / ssl) * (rl - ssl);
    } 
    sz_result = do_MALLOC ? MALLOC(len) : malloc(len);

    pd = sz_result;
    while (*s)
      if ( (!done || *all) &&
           ((!*case_insensitive && strncmp(s, subs, ssl) == 0) ||
            (*case_insensitive && strincomp((unsigned char *) s, (unsigned char *) subs, ssl) == 0)) ) {

        strcpy(pd, replby);
        pd += rl;
        s += ssl;
        done = 1;
      } else {
        *pd++ = *s++;
      }
    *pd = 0;
  }
  return sz_result;
}

// pm 2005-10-03
// ct 2006-11-20 implementation of NULL for FB 2.0 
// ct 2008-12-26 moved to math_functions
//char * dollarval (double *value) 

// pm 2005-10-03
// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2008-12-26 moved to math_functions
//char * euroval (double *value)

// pm 2005-10-03
// gh 2006-03-02 edit for ‰ˆ¸
// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2006-11-30 returns NULL insted of 0 if not found
char *findnthword(char *sz, int *n)
{
    if (!sz || !n) {
        return NULL;
    } else {
        char *pw;
        int len;

        while (*sz && *n >= 0) {
            while (*sz && *sz == ' ')
                sz++;
            if (*n > 0)
                while (*sz && *sz != ' ')
                    sz++;
            (*n)--;
        }

        pw = sz;
        while (*pw && *pw != ' ')
            pw++;

        len = pw - sz;
        if (len == 0) {
            return NULL;
        } else {
            char *sz_result = (char *) MALLOC(len + 1);
            strncpy(sz_result, sz, len);
            sz_result[len] = 0;
            return sz_result;
        }
    }
}

// 1999 Gregory Deatz
// pm 2005-10-03
#define iswordchar(sz, i, start)       \
          (isalnum(sz[i]) ||           \
           ((i > start) &&             \
            !isspace(sz[i-1]) &&       \
            (sz[i] == '\'')))          \

// ct 2006-11-20 implementation of NULL for FB 2.0
int * findwordindex (char *sz, int *index)
{
  if(!sz || !index)
    return NULL;
  else
  {
    int *result = MALLOC(sizeof(int));
    int i = *index;
    int len = strlen(sz);

    while (i < len && !iswordchar(sz, i, *index))
      i++;

    *result = (i >= len) ? -1 : i;
    return result;
  }
}

// ct 2006-11-30 returns null instead of 0 if not found
int *findwordindexnull(char *sz, int *index)
{
    if (!sz || !index) {
        return NULL;
    } else {
        int i = *index;
        int len = strlen(sz);

        while (i < len && !iswordchar(sz, i, *index))
            i++;

        if (i >= len) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = i;
            return result;
        }
    }
}

// pm 2005-10-03
// ct 2006-11-20 implementation of NULL for FB 2.0 
char * collatebr (char *sz)
{
  if(!sz)
    return NULL;
  else
  {

    char *sz_return = (char*) MALLOC (strlen(sz) + 1);
    char *pd = sz_return;

    while (*sz)
    {
      switch (*sz)
      {
        case '·': case '‚': case '„': case '‡': case '‰': case 'Â': case '¡': case '¬': case '√': case '¿': case 'ƒ': case '≈':
          *pd++ = 'A'; break;
        case 'È': case 'Í': case 'Ë': case 'Î': case '…': case ' ': case '»': case 'À':
          *pd++ = 'E'; break;
        case 'Ì': case 'Ó': case 'Ï': case 'Ô': case 'Õ': case 'Œ': case 'Ã': case 'œ':
          *pd++ = 'I'; break;
        case 'Û': case 'Ù': case 'ı': case 'Ú': case 'ˆ': case '”': case '‘': case '’': case '“': case '÷':
          *pd++ = 'O'; break;
        case '˙': case '˚': case '˘': case '¸': case '⁄': case '€': case 'Ÿ': case '‹':
          *pd++ = 'U'; break;
        case 'Á': case '«':
          *pd++ = 'C'; break;
        case 'Ò': case '—':
          *pd++ = 'N'; break;
        case '˝': case 'ˇ': case '›':
          *pd++ = 'Y'; break;
        default:
          *pd++ = *sz;
      }
      sz++;
    }

    *pd = 0;
    return sz_return;
  }
}

// pm 2005-10-03
// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2006-11-30 intercept negativ n
char * strofchar (char *sz, int *n)
{
  if(!sz || !n)
    return NULL;
  else
  {
    int c = (*n > 0) ? *n : 0;
    char * sz_return = (char*) MALLOC (c + 1);
    char * pd = sz_return;

    if(*n < 0) *n = 0;

    while ((*n)--)
      *pd++ = *sz;

    *pd = 0;

    return sz_return;
  }
}

// gh 2006-02-21
// base on GrUDF in Delphi by Torsten Grundke / Gerd Kroll
// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2008-02-22 for *search replaced MALLOC with malloc
char *stringlistitem(char *stringlist, char *item)
{
    if (!stringlist || !item) {
        return NULL;
    } else {
        char *search = malloc(strlen(item) + 3);
        char *pos, *end, *result;

        sprintf(search, "\"%s=", item);
        pos = strstr(stringlist, search);
        if (pos) {
            pos++;
            end = strchr(pos, '"');
            if (end) {
                *end = '\0';
            }
            result = MALLOC(strlen(pos) + 1);
            strcpy(result, pos);
            if (end) {
                *end = '"';
            }
        } else {
            result = NULL;
        }

        free(search);
        return result;
    }
}

// gh 2006-02-21
// gh 2006-02-28 added MAXSTRLEN
// gh 2006-03-01 added do_MALLOC
// ct 2006-11-20 implementation of NULL for FB 2.0 
// ct 2007-11-23 changed MAXSTRLEN from 32758
// ct 2009-01-15 changed MAXSTRLEN to 31997 and *p to unsigned char
#define MAXSTRLEN 31997
char *intern_excelstring(char *string, int do_MALLOC)
{
  if(!string)
    return NULL;
  else {
    unsigned char *p = (unsigned char *) string;
    char *q, *result;
    int len = 0;
    /* Count length of string, double count quotes */
    while (*p && len < MAXSTRLEN) {
        if (*p == '"') {
            len++;
        }
        if (len < MAXSTRLEN) {
            len++;
            p++;
        }
    }
    result = do_MALLOC ? MALLOC(len + 3) : malloc(len + 3);
    p = (unsigned char *) string;
    q = result;
    /* Start with double quote */
    *q++ = '"';
    len = 0;
    /* Copy the string, omitting CRs and doubling double quotes */
    while (*p && len < MAXSTRLEN) {
        if (*p != 13) {
            if (*p == '"') {
                *q++ = '"';
                len++;
            }
            if (len < MAXSTRLEN) {
                *q++ = *p++;
                len++;
            }
        } else {
            p++;
        }
    }
    *q++ = '"';
    *q = '\0';
    return result;
  }
}

// gh 2006-02-21
char *excelstring(char *string)
{
    return intern_excelstring(string, 1);
}

// ct 2006-11-15
// ct 2006-11-20 implementation of NULL for FB 2.0
// gh 2009-09-04 predecessor and successor swapped (function wrong to name)  
char * predecessor (char *sz, int *n)
{
  if(!sz || !n)
    return NULL;
  else
  {

    char * sz_return = (char*) MALLOC (2);
    sz_return[0] = *sz - *n;
    sz_return[1] = '\0';
    return sz_return;
  }
}

// ct 2006-11-15
// ct 2006-11-20 implementation of NULL for FB 2.0
// gh 2009-09-04 predecessor and successor swapped (function wrong to name)  
char * successor (char *sz, int *n)
{
  if(!sz || !n)
    return NULL;
  else
  {
    char * sz_return = (char*) MALLOC (2);
    sz_return[0] = *sz + *n;
    sz_return[1] = '\0';
    return sz_return;
  }
}

// ct 2006-11-16
// ct 2006-11-20 implementation of NULL for FB 2.0 
char * reverse (char *sz)
{
  if(!sz)
    return NULL;
  else
  {
    int i;
    int j;
    int len = strlen(sz);
    char * sz_return = (char *) MALLOC (len + 1);

    for (i = len - 1, j = 0; i >= 0 ; i--, j++)
      sz_return[j] = sz[i];

    sz_return[len] = '\0';

    return sz_return;
  }
}

// ct 2006-11-17
// ct 2006-11-20 implementation of NULL for FB 2.0 
int * wordcount (char *sz)
{
  if(!sz)
    return NULL;
  else
  {
    int *result = MALLOC(sizeof(int));
    int wort = 0;
    int count = 0;
    while (*sz)
    {
      if(isspace(*sz))
        wort = 0;
      else
      {
        if(wort == 0)
        {
          wort = 1;
          count++;
        }
      }
      sz++;
    }
    *result = count;
    return result;
  }
}

// ct 2006-11-17
// ct 2006-11-20 implementation of NULL for FB 2.0 
short * ord(unsigned char *sz)
{
  if(!sz)
    return NULL;
  else
  {
    short *result = MALLOC(sizeof(short));
    *result = sz[0];
    return result;
  }
}

// ct 2006-11-17
// ct 2006-11-20 implementation of NULL for FB 2.0 
int * stringcount(char *sz, char *sz_in)
{
  if(!sz || !sz_in || *sz == 0 || *sz_in == 0)
    return NULL;
  else
  {
    int *result = MALLOC(sizeof(int));
    char *str = sz;
    int len = strlen(sz_in);
    int count = 0;
    while ((str = strstr(str, sz_in)) != NULL )
    {
      str +=len;
      count++;
    }
    *result = count;
    return result;
  }
}

// ct 2007-02-03 version with swapped parameters for rFunc
int * r_stringcount(char *sz_in, char *sz)
{
  if(!sz || !sz_in || *sz == 0 || *sz_in == 0)
    return NULL;
  else
  {
    int *result = MALLOC(sizeof(int));
    char *str = sz;
    int len = strlen(sz_in);
    int count = 0;
    while ((str = strstr(str, sz_in)) != NULL )
    {
      str +=len;
      count++;
    }
    *result = count;
    return result;
  }
}

// ct 2007-02-03
int * r_strcmp(char *s1, char *s2)
{
  if(!s1 || !s2)
    return NULL;
  else
  {
    int *result = MALLOC(sizeof(int));
    *result = strcmp(s1, s2);
    return result;
  }
}

// ct 2007-02-03
int * r_wordcount(char *s, char *delim, int *flag)
{
  if(!s || !delim || !flag)
    return NULL;
  else
  {
      int *result = MALLOC(sizeof(int));

      char *c = s;
      int r = 0;
      int d = 1;

      while (*c)
         if (strchr(delim, *c++))
            d++; // counting words
         else {
            while (*c && !strchr(delim, *c)) c++;
               r++; // counting full words
         }
        if (*flag)
                *result = d;
        else
                *result = r;
      return result;
  }
}

// ct 2007-02-03
char * wordnum(char *s, int *n, char *delim, int *flag)
{
  if(!s || !n || !delim || !flag)
    return NULL;
  else
  {
      int len = strlen(s);
      char * result = (char*) MALLOC (len + 1);

        char *c = s, *ptr;
        int r = 1;

        if (!*c) return c;
        while (*c && r!=*n)
                if (strchr(delim, *c++)) {
                        if (*flag)
                                r++;
                        else
                                while (*c && strchr(delim, *c)) c++;
                } else {
                        while (*c && !strchr(delim, *c)) c++;
                        if (!*flag) r++;
                }
        if (!*flag)
                while (*c && strchr(delim, *c)) c++;
        ptr = c;
        while (*ptr && !strchr(delim, *ptr)) ptr++;
        *ptr = '\0';

        strcpy(result, c);
        return result;
  }      
}

// gh 2007-02-05
// Convert to uppercase, but copy only alphanumeric characters,
// german umlauts and characters with accents. Convert umlauts and accents
// into their accordant "normal" character, for example ˆ into O. 
char *keyup(char *key, int *len)
{
    char *res, *pc1, *pc2;
    int i;
    char c;

    res = MALLOC(*len + 1);
    pc1 = key;
    pc2 = res;
    i = 0;
    while (*pc1 && i < *len) {
        c = StringUpTab[(unsigned char)*pc1++];
        if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
            *pc2++ = c;
            i++;
        } else {
            switch (c) {
            case '»': case '…': case ' ': case 'À':
                *pc2++ = 'E';
                i++;
                break;
            case '¿': case '¡': case '¬': case 'ƒ': case '√': case '≈':
                *pc2++ = 'A';
                i++;
                break;
            case 'Ÿ': case '⁄': case '€': case '‹':
                *pc2++ = 'U';
                i++;
                break;
            case '“': case '”': case '‘': case '’': case '÷':
                *pc2++ = 'O';
                i++;
                break;
            case 'Ã': case 'Õ': case 'Œ': case 'œ':
                *pc2++ = 'I';
                i++;
                break;
            case '«':
                *pc2++ = 'C';
                i++;
                break;
            case 'ÿ':
                *pc2++ = 'O';
                i++;
                if (i < *len) {
                    *pc2++ = 'E';
                    i++;
                }
                break;
            case 'ﬂ':
                *pc2++ = 'S';
                i++;
                if (i < *len) {
                    *pc2++ = 'S';
                    i++;
                }
                break;
            case '∆':
                *pc2++ = 'A';
                i++;
                if (i < *len) {
                    *pc2++ = 'E';
                    i++;
                }
                break;
            case '–':
                *pc2++ = 'D';
                i++;
                break;
            case '›' :
                *pc2++ = 'Y';
                i++;
                break;
            case '—':
                *pc2++ = 'N';
                i++;
                break;
            }
        }
    }
    *pc2 = 0;
    return res;
}

// ct 2007-02-05
char *r_c(char * s)
{
  return s;
}

// gh 2007-02-06
char *convertsymbols(char *s, char *source, char *target)
{
    if (!s || !source || !target || (strlen(source) > strlen(target))) {
        return NULL;
    } else {
        int len = strlen(s);
        char *result = (char *) MALLOC(len + 1);
        char *p1 = s, *p2 = result, *p;

        while (*p1) {
            if ((p = strchr(source, *p1)) != NULL) {
                *p2++ = target[p - source];
                p1++;
            } else {
                *p2++ = *p1++;
            }
        }
        *p2 = '\0';
        return result;
    }
}

// gh 2007-07-04: repeattrim
// Output is a trimmed string of all repeats 
// only the first character of trim is used
char *repeattrim(char *string, char *trim)
{
    if (!string) {
        return NULL;
    } else {
        char *result = MALLOC(strlen(string) + 1);
        char *s1 = string, *s2 = result;
        if (!trim || !trim[0]) {
            strcpy(result, string);
        } else {
            while (*s1) {
                while (*s1 && (*s1 != trim[0])) {
                    *s2++ = *s1++;
                }
                *s2++ = *s1++;
                while (*s1 == trim[0]) {
                    s1++;
                }
            }
            *s2 = '\0';
        }
        return result;
    }
}

// gh 2007-07-04
// Output ist string, count mal wiederholt mit maximaler L‰nge maxlen.
char *strrepeat(char *string, int *count)
{
    if (!string || !count) {
        return NULL;
    } else {
        int i = 0, j = 0;
        int len = *count * strlen(string) + 1;
        char *result = MALLOC(len);

        if (*string) {
            while (i < len - 1) {
                result[i++] = string[j++];
                if (!string[j]) {
                    j = 0;
                }
            }
        }
        result[i] = '\0';
        return result;
    }
}

// gh 2007-07-04
// replaces parts of strings: form pos count characters are replaced
// example:
/*
SELECT F_STRSTUFF('1234567890', 3, 2, 'abc') FROM RDB$DATABASE
result: 12abc567890 */

char *strstuff(char *string, int *pos, int *count, char *repl)
{
    if (!string || !pos || !count || !repl) {
        return NULL;
    } else {
        int i = 0, j = 0, l = 0, n;
        int slen = strlen(string);
        int len = slen + strlen(repl) + 1;
        char *result = MALLOC(len);

        n = (*pos > slen) ? slen : ((*pos > 0) ? *pos - 1 : 0);
        while (l < n) {
            result[l++] = string[j++];
        }
        while (repl[i]) {
            result[l++] = repl[i++];
        }
        j = (j + *count < slen) ? (j + *count) : slen;
        while (string[j] && l < len) {
            result[l++] = string[j++];
        }
        result[l] = '\0';
        return result;
    }
}

// gh 2007-07-04
// Conversion of double d to string using format fmt.
/*
Formatting strings (as in c-function sprintf):
% [flags] [width] [.prec] type_char
flags:
-       left justify
+       begin with plus or minus sign
blank   blank instead of a plus
#       alternate conversion:
0x (0X) before for x,X type_char
always a decimal point for e,E,f
width:
n       at least n chars, padded by blanks
0n      at least n chars, padded by zeros
prec:
(none)  1 for d,i,o,u,x,X types
6 for e,E,f types
all significant digits for g,G
.0      for d,i,o,u,x types, precision set to default
for e,E,f types, no decimal point
.n      for d,i,o,u,x types, at least n digits
for e,E,f types, n digits after the decimal point
for g,G, at most n significant digits
type_char:
d,i     signed decimal integer
o       unsigned octal integer
u       unsigned decimal integer
x,X     unsigned hexadecimal int
f       signed value of the form [-]dddd.dddd
e       signed value of the form [-]d.dddd or e[+/-]ddd
g       signed value in either e or f form, based on given value and precision
E,G     same as e,g, but with E for exponent
examples:
FloatToStr(13.5, '%7.3e') = '1.350e+01'
FloatToStr(13.5, '%+7.3f') = ' +13.500'
FloatToStr(13.5345, '%.5g') = '13.534'
FloatToStr(13.5345, '%.5f') = '13.53450'
FloatToStr(13, '%05X') = '0000D'
FloatToStr(13, '%#04x') = '0x0d' */
char *floattostr(double *d, char *fmt)
{
    if (!d || !fmt) {
        return NULL;
    } else {
        char *result = MALLOC(64);
        char *s, slong[32];
        long i;

        result[0] = '\0';

        if ((s = strstr(fmt, "%")) != NULL) {
            i = 1;
            while (strchr("+- #", s[i]))
                i++;
            while (strchr("0123456789", s[i]))
                i++;
            while (strchr(".", s[i]))
                i++;
            while (strchr("0123456789", s[i]))
                i++;
            if (strchr("diouxX", s[i])) {
                sprintf(slong, "%.0f", *d);
                i = atoi(slong);
                sprintf(result, fmt, i);
            } else {
                sprintf(result, fmt, *d);
            }
        } else {
            sprintf(result, fmt, *d);
        }
        return result;
    }
}

// gh 2007-07-04
// ct 2010-11-16 bug with NULL
// Conversion of integer d to string using format fmt (See FloatToStr)
char *inttostr(int *i, char *fmt)
{
    if (!i || !fmt) {
        return NULL;
    } else {   
        char *buffer = MALLOC(32);

        sprintf(buffer, fmt, *i);
        return buffer;
    }    
}

// gh 2008-02-12
// extracts domain from email-adress
char *domainfromemail (char * email)
{
    if (!email) {
        return NULL;
    } else {
        char *result = MALLOC(strlen(email));
        char *p = strchr(email, '@');
        if (p) {
            int l;
            strcpy(result, p + 1);
            l = strlen(result);
            while (l && isspace(result[l - 1])) {
                l--;
            }
            result[l] = '\0';
        } else {
            strcpy(result, "");
        }
        return result;
    }
}

// gh 2008-02-14
// gh 2008-02-22
// ct 2009-01-15 changed to unsigned char
char *rot13(char *string)
{
    if (!string) {
        return NULL;
    } else {
        char *result = MALLOC(strlen(string) + 1);
        unsigned char *r = (unsigned char *) result, *s = (unsigned char *) string;
        while ((*r = *s++) != '\0') {
            if (*r >= 'a' && *r <= 'z') {
                *r += 13;
                if (*r > 'z') {
                    *r = 'a' + (*r - 'z') - 1;
                }
            }
            if (*r >= 'A' && *r <= 'Z') {
                *r += 13;
                if (*r > 'Z') {
                    *r = 'A' + (*r - 'Z') - 1;
                }
            }
            r++;
        }
        return result;
    }
}

