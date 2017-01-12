/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring / Peter Mandrella / Georg Horn
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 *
 * File Name:	checksum_functions.h
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

// internal functions

// exported functions
int    * crosssum(int *);
int    * crossdiff(int *);
int    * weightcrosssum(int *);
int    * iteratecrosssum(int *);
char   * encryptmd5 (unsigned char *);
int    * gtin8check(char *);
int    * upc12check(char *);
int    * gtin13check(char *);
int    * gtin14check(char *);
int    * nve18check(char *);
char   * gtin8(char *);
char   * upc12(char *);
char   * gtin13(char *);
char   * gtin14(char *);
char   * nve18(char *);
int    * isbn10check(char *);
char   * isbn10(char *);
int    * isbn13check(char *);
char   * isbn13(char *);
int    * issn8check(char *);
char   * issn8(char *);
int    * upstncheck(char *);
char   * upstn(char *);
int    * twooffivecheck(char *);
char   * twooffive(char *);
char   * code39check(char *);
char   * code39(char *);
