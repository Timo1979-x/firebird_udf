/*****************************************************************************
 * FreeAdhocUDF - copyright(c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright(c) 1999 Gregory Deatz
 * some code from Robert Loipfinger rl copyright(c) 2000 ADITO Software GmbH
 *
 * File Name:	utf8_functions.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FreeAdhocUDF. If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

// string functions
char         * u_left(char *, int *);
char         * u_right(char *, int *);
char         * u_mid(char *, int *, int *);
char         * u_padleft(char *, char *, int *);
char         * u_padright(char *, char *, int *);
char         * u_padcenter(char *, char *, int *);
int          * u_substr(char *, char *);
char         * u_strrm(char *, int *);
char         * u_replacestring(char *, char *, char *, int *, int *);
char         * u_reverse(char *);
char         * u_stripstring(char *, char *);
char         * u_stripstringhold(char *, char *);
char         * u_repeattrim(char *, char *);
char         * u_strstuff(char *, int *, int *, char *);
char         * u_predecessor(char *, int *);
char         * u_successor(char *, int *);
char         * u_lower(char *);
char         * u_upper(char *);
char         * u_propercase(char *);
char         * u_cyrillic_latin(char *);
char         * u_character(int *);
int          * u_ord(char *);
int          * u_rstrcmp(char *, char *);
int          * u_stringlength(char *);
char         * u_linewrap(char *, int *, int *);
char         * u_convertsymbols(char *, char *, char *);
char         * u_findword(char *, int *);
char         * u_nbsp();
char         * u_nnbsp();
// BLOb functions
BLOBCALLBACK   u_blobreplacestring(BLOBCALLBACK, char *, char *, int *, int *, BLOBCALLBACK);
int          * u_blobsubstr(BLOBCALLBACK, char *);
// date/time functions
static char  * intern_u_format_date(ISC_QUAD *, char *, char *);
char         * u_monthlong(ISC_QUAD *);
char         * u_monthshort(ISC_QUAD *);
char         * u_dowlong(ISC_QUAD *);
char         * u_dowshort(ISC_QUAD *);
char         * u_date2strlang(ISC_QUAD *, char *, char *);
char         * u_monthlonglang(ISC_QUAD *, char *);
char         * u_monthshortlang(ISC_QUAD *, char *);
char         * u_dowlonglang(ISC_QUAD *, char *);
char         * u_dowshortlang(ISC_QUAD *, char *);
// numeric functions
