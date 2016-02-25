/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 * some code from Robert Loipfinger rl copyright (c) 2000 ADITO Software GmbH
 *
 * File Name:	diverse_functions.h
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
 * along with FreeAdhocUDF.  If not, see <http://www.gnu.org/licenses/>. 
*****************************************************************************/

// exported functions
char   * version ();
char   * versionname ();
char   * oslanguage ();
char   * aif (char *, char *, char *, char *, char *);
// nvl
char   * snvl(char *, char *);
char   * snvl2(char *, char *, char *);
int    * invl(int *, int *);
int    * invl2(int *, int *, int *);
double * dnvl(double *, double *);
double * dnvl2(double *, double *, double *);
