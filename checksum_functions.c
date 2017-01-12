/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 *
 * File Name:   checksum_functions.c
 * Description: this module contains UDFs to generate checksums
 * for exported functions list see checksum_functions.h
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
#include <string.h>
#include "global.h"
#include "md5.h"
#include "checksum_functions.h"

// ct 2009-09-07 the following functions has moved from math_functions
// crosssum, crossdiff, weightcrosssum, iteratecrosssum,
// gtin8check, gtin13check, gtin14check, nve18check, gtin8, upc12check, upc12,
// gtin13, gtin14, nve18, isbn10check, isbn10, isbn13check, isbn13, issn8check,
// issn8, upstncheck, upstn, twooffivecheck, twooffive, code39check, code39
// ct 2009-09-07 the function encryptmd5 moved from string_functions

// ct 2008-11-03
// crosssum - german: quersumme
// addition of digits of a number
int * crosssum(int *zahl)
{
    if (!zahl) {
        return NULL;
    }
    else {
        int *result = MALLOC(sizeof(int));
        int quer = 0, z = *zahl;
        while (z > 0) {
          quer += z % 10;
          z /= 10;
        }
        *result = quer;
        return result;
    }
}

// ct 2008-11-03
// crossdiff - german: querdifferenz oder alternierende Quersumme
// alternate add and diff of the digits of a number starting right
int * crossdiff(int *zahl)
{
    if (!zahl) {
        return NULL;
    }
    else {
        int *result = MALLOC(sizeof(int));
        int quer = 0, i = 1, z = *zahl;
        while (z > 0) {
          if (i == 1) {
             quer += z % 10;
             i = 0;
          } else {
             quer -= z % 10;
             i = 1;
          }   
          z /= 10;
        }
        *result = quer;
        return result;
    }
}

// ct 2008-11-03
// weightend crosssum - german: gewichtete quersumme
// add all multiplications of each digit with his position starting right
int * weightcrosssum(int *zahl)
{
    if (!zahl) {
        return NULL;
    }
    else {
        int *result = MALLOC(sizeof(int));
        int quer = 0, i = 1, z = *zahl;
        while (z > 0) {
          quer += z % 10 * i;
          i += 1;
          z /= 10;
        }
        *result = quer;
        return result;
    }
}

// ct 2008-11-03
// iterate crosssum - german: einstellige oder iterierte quersumme
// addition of digits of a number as a loop (1. crosssum = new number ...) until crosssum < 10 
int * iteratecrosssum(int *zahl)
{
    if (!zahl) {
        return NULL;
    }
    else {
        int *result = MALLOC(sizeof(int));
        int quer = 10, z = *zahl;
        while (quer > 9) {
          quer = 0;
          while (z > 0) {
          quer += z % 10;
          z /= 10;
          }
          z = quer;
        }  
        *result = quer;
        return result;
    }
}

// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2009-09-02 function encryptmd5 moved from string_functions.c
// gh 2009-09-02 using the md5 algorithm from Quentin Carbonneaux <qcarbonneaux@gmail.com>
char * encryptmd5 (unsigned char *sz)
{
    if (!sz) {
	return NULL;
    } else {
	unsigned char digest[16];
	int i;

	char *sz_return = (char *) MALLOC(33);
	md5(sz, strlen((char *) sz), digest);
	for (i = 0; i < 16; i++) {
	    sprintf(&sz_return[i * 2], "%02x", digest[i]);
	}
	return sz_return;
    }
}

// gh 2009-01-15
// internal function for all gtin checksums
static int intern_gtincheck(char *string1, int length, char *string2)
{
    int crosssum, i;
    char *p1, *p2;
    // to use this function for checksums with variable length 
    if (length == 0) {
	length = strlen(string1) + 1;
    }
    // strip all non-numeric characters
    p1 = string1;
    p2 = string2;
    i = 0;
    while (*p1 && i < length - 1) {
	if (*p1 >= '0' && *p1 <= '9') {
	    *p2++ = *p1;
	    i++;
	}
	p1++;
    }
    *p2 = '\0';
    // build checksum for each digit
    crosssum = i = 0;
    p2--;
    while (p2 >= string2) {
	if (i == 0) {
	    crosssum += (*p2 - '0') * 3;
	    i = 1;
	} else {
	    crosssum += (*p2 - '0');
	    i = 0;
	}
	p2--;
    }
    if (crosssum % 10 == 0) {
	return 0;
    } else {
	return (crosssum / 10 + 1) * 10 - crosssum;
    }
}

// gh 2009-01-15
// GTIN-8 checking number (ex EAN-8)
int *gtin8check(char *string)
{
    if (!string) {
	return NULL;
    } else {
	int *result = MALLOC(sizeof(int));
	char *string2 = malloc(9);
	*result = intern_gtincheck(string, 8, string2);
	free(string2);
	return result;
    }
}

// gh 2009-01-15
// GTIN-8 complete number (ex EAN-8)
char *gtin8(char *string)
{
    if (!string) {
	return NULL;
    } else {
	char *result = MALLOC(9);
	char *string2 = malloc(9);
	int sum;
	sum = intern_gtincheck(string, 8, string2);
	sprintf(result, "%s%d", string2, sum);
	free(string2);
	return result;
    }
}

// ct 2009-01-15
// UPC-12 checking number
int *upc12check(char *string)
{
    if (!string) {
	return NULL;
    } else {
	int *result = MALLOC(sizeof(int));
	char *string2 = malloc(13);
	*result = intern_gtincheck(string, 12, string2);
	free(string2);
	return result;
    }
}

// ct 2009-01-15
// UPC-12 complete number
char *upc12(char *string)
{
    if (!string) {
	return NULL;
    } else {
	char *result = MALLOC(13);
	char *string2 = malloc(13);
	int sum;
	sum = intern_gtincheck(string, 12, string2);
	sprintf(result, "%s%d", string2, sum);
	free(string2);
	return result;
    }
}

// ct 2009-01-15
// GTIN-13 or GLN checking number (ex EAN-13 - International Article Number, former European Article Number)
int *gtin13check(char *string)
{
    if (!string) {
	return NULL;
    } else {
	int *result = MALLOC(sizeof(int));
	char *string2 = malloc(14);
	*result = intern_gtincheck(string, 13, string2);
	free(string2);
	return result;
    }
}

// ct 2009-01-15
// GTIN-13 complete number (ex EAN-13)
char *gtin13(char *string)
{
    if (!string) {
	return NULL;
    } else {
	char *result = MALLOC(14);
	char *string2 = malloc(14);
	int sum;
	sum = intern_gtincheck(string, 13, string2);
	sprintf(result, "%s%d", string2, sum);
	free(string2);
	return result;
    }
}

// ct 2009-01-15
// GTIN-14 checking number 
int *gtin14check(char *string)
{
    if (!string) {
	return NULL;
    } else {
	int *result = MALLOC(sizeof(int));
	char *string2 = malloc(15);
	*result = intern_gtincheck(string, 14, string2);
	free(string2);
	return result;
    }
}

// ct 2009-01-15
// GTIN-14 complete number
char *gtin14(char *string)
{
    if (!string) {
	return NULL;
    } else {
	char *result = MALLOC(15);
	char *string2 = malloc(15);
	int sum;
	sum = intern_gtincheck(string, 14, string2);
	sprintf(result, "%s%d", string2, sum);
	free(string2);
	return result;
    }
}

// ct 2009-01-15
// NVE-18 checking number 
int *nve18check(char *string)
{
    if (!string) {
	return NULL;
    } else {
	int *result = MALLOC(sizeof(int));
	char *string2 = malloc(19);
	*result = intern_gtincheck(string, 18, string2);
	free(string2);
	return result;
    }
}

// ct 2009-01-15
// NVE-18 complete number
char *nve18(char *string)
{
    if (!string) {
	return NULL;
    } else {
	char *result = MALLOC(19);
	char *string2 = malloc(19);
	int sum;
	sum = intern_gtincheck(string, 18, string2);
	sprintf(result, "%s%d", string2, sum);
	free(string2);
	return result;
    }
}

// ct 2009-01-16
// internal function for ISBN-10 checksums
static int intern_isbn10(char *string1, int length, char *string2)
{
    int crosssum, i;
    char *p1, *p2;
    // strip all non-numeric characters
    p1 = string1;
    p2 = string2;
    i = 0;
    while (*p1 && i < length - 1) {
	if (*p1 >= '0' && *p1 <= '9') {
	    *p2++ = *p1;
	    i++;
	}
	p1++;
    }
    *p2 = '\0';
    // build checksum for each digit
    crosssum = 0;
    i = 2;
    p2--;
    while (p2 >= string2) {
	crosssum += (*p2 - '0') * i;
	i++;
	p2--;
    }
    return 11 - (crosssum % 11);
}

// ct 2009-01-16
// ISBN-10 checking number
int *isbn10check(char *string)
{
    if (!string) {
	return NULL;
    } else {
	int *result = MALLOC(sizeof(int));
	char *string2 = malloc(11);
	*result = intern_isbn10(string, 10, string2);
	free(string2);
	return result;
    }
}

// ct 2009-01-16
// ISBN-10 complete number
char *isbn10(char *string)
{
    if (!string) {
	return NULL;
    } else {
	char *result = MALLOC(14);
	char *string2 = malloc(11);
	char *string3 = malloc(13);
	int sum, i = 0, j = 0;
	sum = intern_isbn10(string, 10, string2);
	// format as an ISBN-10
	for (j = 0; j < 11;) {
	    if (j == 1 || j == 7) {
		string3[j] = '-';
		j++;
	    } else {
		string3[j] = string2[i];
		i++;
		j++;
	    }
	}
	string3[j] = '\0';
	free(string2);
	sprintf(result, "%s-%d", string3, sum);
	free(string3);
	return result;
    }
}

// ct 2009-01-16
// ISBN-13 checking number (same as GTIN-13)
int *isbn13check(char *string)
{
    if (!string) {
	return NULL;
    } else {
	int *result = MALLOC(sizeof(int));
	char *string2 = malloc(14);
	*result = intern_gtincheck(string, 13, string2);
	free(string2);
	return result;
    }
}

// ct 2009-01-16
// ISBN-13 complete number
char *isbn13(char *string)
{
    if (!string) {
	return NULL;
    } else {
	char *result = MALLOC(18);
	char *string2 = malloc(14);
	char *string3 = malloc(16);
	int sum, i = 0, j = 0;
	sum = intern_gtincheck(string, 13, string2);
	// format as an ISBN-13
	for (j = 0; j < 15;) {
	    if (j == 3 || j == 5 || j == 11) {
		string3[j] = '-';
		j++;
	    } else {
		string3[j] = string2[i];
		i++;
		j++;
	    }
	}
	string3[j] = '\0';
	free(string2);
	sprintf(result, "%s-%d", string3, sum);
	free(string3);
	return result;
    }
}

// ct 2009-01-17
// ISSN checking number
int *issn8check(char *string)
{
    if (!string) {
	return NULL;
    } else {
	int *result = MALLOC(sizeof(int));
	char *string2 = malloc(9);
	*result = intern_isbn10(string, 8, string2);
	free(string2);
	return result;
    }
}

// ct 2009-01-17
// ISSN complete number
char *issn8(char *string)
{
    if (!string) {
	return NULL;
    } else {
	char *result = MALLOC(10);
	char *string2 = malloc(9);
	char *string3 = malloc(10);
	int sum, i = 0, j = 0;
	sum = intern_isbn10(string, 8, string2);
	// format as an ISSN
	for (j = 0; j < 8;) {
	    if (j == 4) {
		string3[j] = '-';
		j++;
	    } else {
		string3[j] = string2[i];
		i++;
		j++;
	    }
	}
	string3[j] = '\0';
	free(string2);
	sprintf(result, "%s%d", string3, sum);
	free(string3);
	return result;
    }
}

// necessary for UPS TN
static char *UPSDigitFrom = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char *UPSDigitTo = "23456789012345678901234567";

// gh 2009-01-19
// internal function for UPS trackingnumber checksums
static int intern_upstn(char *string1, char *string2)
{
    int crosssum, i, j;
    char *p1 = string1, *p2 = string2, *ps;

    // strip "1Z" from beginning of string1
    if (*p1 == '1' && *(p1 + 1) == 'Z') {
	p1 += 2;
    }
    // strip all non numeric and non alphabetic chars, copy max 15 chars.
    crosssum = i = 0;
    while (*p1 && i < 15) {
	if (*p1 >= '0' && *p1 <= '9') {
	    if (i % 2 == 0) {
		crosssum += (*p1 - '0');
	    } else {
		crosssum += (*p1 - '0') * 2;
	    }
	    *p2++ = *p1;
	    i++;
	} else if (*p1 >= 'A' && *p1 <= 'Z') {
	    /* replace alphabetic characters with digits */
	    ps = strchr(UPSDigitFrom, *p1);
	    j = ps - UPSDigitFrom;
	    if (i % 2 == 0) {
		crosssum += (UPSDigitTo[j] - '0');
	    } else {
		crosssum += (UPSDigitTo[j] - '0') * 2;
	    }
	    *p2++ = *p1;
	    i++;
	}
	p1++;
    }
    *p2 = '\0';

    if (crosssum % 10 == 0) {
	return 0;
    } else {
	return 10 - crosssum % 10;
    }
}

// ct 2009-01-17
// UPS tracking No checking number
int *upstncheck(char *string)
{
    if (!string) {
	return NULL;
    } else {
	int *result = MALLOC(sizeof(int));
	char *string2 = malloc(21);
	*result = intern_upstn(string, string2);
	free(string2);
	return result;
    }
}

// ct 2009-01-17
// UPS tracking No complete number
char *upstn(char *string)
{
    if (!string) {
	return NULL;
    } else {
	char *result = MALLOC(22);
	char *string2 = malloc(22);
	char *string3 = malloc(22);
	int sum, i = 0, j = 0;
	sum = intern_upstn(string, string2);
	// format as an UPS TrackingNo.
	for (j = 0; j < 17;) {
	    if (j == 6 || j == 9) {
		string3[j] = ' ';
		j++;
	    } else {
		string3[j] = string2[i];
		i++;
		j++;
	    }
	}
	string3[j] = '\0';
	free(string2);
	sprintf(result, "1Z %s%d", string3, sum);
	free(string3);
	return result;
    }
}

// ct 2009-01-17
// 2 of 5 checking number
int *twooffivecheck(char *string)
{
    if (!string) {
	return NULL;
    } else {
	int *result = MALLOC(sizeof(int));
	char *string2 = malloc(256);
	*result = intern_gtincheck(string, 0, string2);
	free(string2);
	return result;
    }
}

// ct 2009-01-17
// 2 of 5 complete number
char *twooffive(char *string)
{
    if (!string) {
	return NULL;
    } else {
	char *result = MALLOC(256);
	char *string2 = malloc(256);
	int sum;
	sum = intern_gtincheck(string, 0, string2);
	sprintf(result, "%s%d", string2, sum);
	free(string2);
	return result;
    }
}

// necessary for code-39
static char *code39DigitFrom = "ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%";
// replace: A=10 ... Z=35 -=36 .=37 ' '=38 $=39 /=40 +=41 %=42 

// gh 2009-01-27
// internal function for code-39 checksums
static void intern_code39(char *string1, char *string2, char *result)
{
    int crossum, i;
    char *p1 = string1, *p2 = string2, *ps;

    /* copy string, omitting all unallowed chars and calculate checksum: */
    crossum = 0;
    while (*p1) {
	if (*p1 >= '0' && *p1 <= '9') {
	    crossum += (*p1 - '0');
	    *p2++ = *p1;
	} else if ((ps = strchr(code39DigitFrom, *p1)) != NULL) {
	    /* replace characters with numbers */
	    i = ps - code39DigitFrom;
	    crossum += (i + 10);
	    *p2++ = *p1;
	}
	p1++;
    }
    *p2 = '\0';

    if (crossum > 9) {
	*result = code39DigitFrom[(crossum % 43) - 10];
    } else {
	*result = crossum + '0';
    }

    *(result + 1) = '\0';
}

// ct 2009-01-27
// code-39 checking number
char *code39check(char *string1)
{
    if (!string1) {
	return NULL;
    } else {
	char *result = MALLOC(2);
	char *string2 = malloc(strlen(string1) + 1);

	intern_code39(string1, string2, result);
	free(string2);
	return result;
    }
}

// ct 2009-01-27
// code-39 complete number
char *code39(char *string1)
{
    if (!string1) {
	return NULL;
    } else {
	char *result = MALLOC(strlen(string1) + 2);
	char *string2 = malloc(2);

	intern_code39(string1, result, string2);
	strcat(result, string2);
	free(string2);
	return result;
    }
}
