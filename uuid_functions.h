/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring / Peter Mandrella / Georg Horn
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 *
 * File Name:	uuid_functions.h
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
int        check_uuidcompr(char *);
int        check_uuid(char *);
// exported functions
// create
char     * uuid1mac();
char     * uuid1rand();
char     * uuid4();
char     * uuid1maccompr();
char     * uuid1randcompr();
char     * uuid4compr();
// transform
char     * compressuuid(char *);
char     * expanduuidcompr(char *);
// read
char     * uuidversion(char *);
ISC_QUAD * uuid1timestamp(char *);
ISC_QUAD * uuid1comprtimestamp(char *);
char     * uuid1macmac(char *);
char     * uuid1maccomprmac(char *);
