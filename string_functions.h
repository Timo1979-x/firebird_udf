/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 * some code from Robert Loipfinger rl copyright (c) 2000 ADITO Software GmbH
 * some code from rFunc UDF library copyright (c) 1998-2003 Polaris Software
 *
 * File Name:	string_functions.h
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
int     find_char (char, char *);
char  * intern_excelstring (char *, int);
char  * intern_replacestring (char *, char *, char *, int *, int *, int);
// exported functions
// create
char  * crlf ();
char  * lf ();
char  * space (int *);
char  * nbsp ();
char  * dqm ();
char  * sqm ();
char  * dsqm ();
char  * tab ();
char  * strofchar (char *, int *);
// edit
char  * left (char *, int *);
char  * mid (char *, int *, int *);
char  * mid1 (char *, int *, int *);
char  * strcopy (char *, int *, int *);
char  * right (char *, int *);
int   * substr (char *, char *);
int   * substrnull (char *, char *);
int   * strsub (char *, char *);
int   * strsubnull (char *, char *);
int   * strpos (char *, char *);
char  * strrm (char *, int *);
char  * ltrim (char *);
char  * rtrim (char *);
char  * lrtrim (char *);
char  * padleft (char *, char *, int *);
char  * r_padleft (char *, int *, char *);
char  * padright (char *, char *, int *);
char  * r_padright (char *, int *, char *);
char  * padcenter (char *, char *, int *);
char  * linewrap (char *, int *, int *);
char  * replace (char *, char *, char *);
char  * replacestring (char *, char *, char *, int *, int *);
char  * convertsymbols(char *, char *, char *);
char  * reverse (char *);
char  * stripstring (char *, char *);
char  * stripstringhold (char *, char *);
char  * repeattrim(char *, char *);
char  * strrepeat(char *, int *);
char  * strstuff(char *, int *, int *, char *);
char  * collatebr (char *);
char  * keyup(char *, int *);
char  * predecessor (char *, int *);
char  * successor (char *, int *);
// convert
char  * lower (char *);
char  * intern_upper (char *, int);
char  * upper (char *);
char  * rlower (char *);
char  * rupper (char *);
char  * rlatin (char *);
char  * propercase (char *);
char  * character (int *);
short * ord(unsigned char *);
char  * rot13(char *);
char  * soundex (unsigned char *);
char  * gsoundex (char *);
char  * telefonnr (char *, int *);
char  * digits (char *);
char  * excelstring (char *);
char  * r_c(char *);
char  * floattostr(double *, char *);
char  * inttostr(int *, char *);
// compare
int   * equalstring (char *, char *);
int   * r_strcmp(char *, char *);
// find
char  * findword (char *, int *);
char  * findnthword (char *, int *);
char  * wordnum(char *, int *, char *, int *);
int   * findwordindex (char *, int *);
int   * findwordindexnull (char *, int *);
int   * stringlength (char *);
char  * stringlistitem (char *, char *);
int   * wordcount (char *);
int   * r_wordcount(char *, char *, int *);
int   * stringcount(char *, char *);
int   * r_stringcount(char *, char *);
char  * domainfromemail (char *);

