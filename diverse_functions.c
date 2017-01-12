/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 * some code from Robert Loipfinger rl copyright (c) 2000 ADITO Software GmbH
 *
 * File Name:   diverse_functions.c
 * Description: this module contains UDFs to generate and manipulate strings
 * for exported functions list see diverse_functions.h
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
#include <time.h>
#include "global.h"
#include "date_functions.h"
#include "diverse_functions.h"

// pm 2005-10-03
// ct 2008-12-03 function version moved from string_functions.c
// gh 2010-11-23 set max length to 30
char *version()
{
    char *sz_return = (char *) MALLOC(31);
    strncpy(sz_return, "adhoc 20101206", 30);
    sz_return[30] = '\0';
    return sz_return;
}

// ct 2010-11-23
// no commend
char *versionname()
{
    char *sz_return = (char *) MALLOC(255);
    strcpy(sz_return, "St.Nicholas edition");
    return sz_return;
}

// ct 2010-12-04
// give the language of the OS
char *oslanguage ()
{
    char *lc_return = (char *) MALLOC(3);
    strncpy(lc_return, intern_getlang(), 2);
    lc_return[2] = '\0';
    return lc_return;
}

// rl 2001
// ct 2006-11-20 implementation of NULL for FB 2.0 
// ct 2008-12-03 function aif moved from string_functions.c
// tt 2010-12-06 add implicit assignements
char *aif(char *p1, char *o, char *p2, char *t, char *e)
{
    if (!p1 || !o || !p2 || !t || !e) {
	return NULL;
    } else {
	int len = (strlen(t) > strlen(e)) ? strlen(t) : strlen(e);
	char *sz_result = (char *) MALLOC(len + 1);
	float w1, w2;
	int erg;
	int ergebnis = 0;

	if (o[0] == 'n') {
	    w1 = (float)atof(p1);
	    w2 = (float)atof(p2);

	    if (strcmp(o, "n=") == 0) {
		if (w1 == w2)
		    ergebnis = 1;
	    } else if (strcmp(o, "n<>") == 0) {
		if (w1 != w2)
		    ergebnis = 1;
	    } else if (strcmp(o, "n<") == 0) {
		if (w1 < w2)
		    ergebnis = 1;
	    } else if (strcmp(o, "n>") == 0) {
		if (w1 > w2)
		    ergebnis = 1;
	    } else if (strcmp(o, "n<=") == 0) {
		if (w1 <= w2)
		    ergebnis = 1;
	    } else if (strcmp(o, "n>=") == 0) {
		if (w1 >= w2)
		    ergebnis = 1;
	    }
	} else {
	    erg = strcmp(p1, p2);

	    if (strcmp(o, "=") == 0) {
		if (erg == 0)
		    ergebnis = 1;
	    } else if (strcmp(o, "<>") == 0) {
		if (erg != 0)
		    ergebnis = 1;
	    } else if (strcmp(o, ">") == 0) {
		if (erg > 0)
		    ergebnis = 1;
	    } else if (strcmp(o, "<") == 0) {
		if (erg < 0)
		    ergebnis = 1;
	    } else if (strcmp(o, ">=") == 0) {
		if (erg >= 0)
		    ergebnis = 1;
	    } else if (strcmp(o, "<=") == 0) {
		if (erg <= 0)
		    ergebnis = 1;
	    }
	}

	if (ergebnis == 1) {
	    strcpy(sz_result, t);
	    len = strlen(t);
	} else {
	    strcpy(sz_result, e);
	    len = strlen(e);
	}

	sz_result[len] = (char) 0;

	return sz_result;
    }
}

// gh 2008-12-03
char *snvl(char *s, char *r)
{
    char *result;

    if (!s) {
	if (!r) {
	    result = (char *) MALLOC(1);
	    *result = '\0';
	} else {
	    result = (char *) MALLOC(strlen(r) + 1);
	    strcpy(result, r);
	}
    } else {
	result = (char *) MALLOC(strlen(s) + 1);
	strcpy(result, s);
    }

    return result;
}

// ct 2008-12-03
char *snvl2(char *s, char *r, char *q)
{
    char *result;

    if (!s) {
	if (!q) {
	    result = (char *) MALLOC(1);
	    *result = '\0';
	} else {
	    result = (char *) MALLOC(strlen(q) + 1);
	    strcpy(result, q);
	}
    } else {
	if (!r) {
	    result = (char *) MALLOC(1);
	    *result = '\0';
	} else {
	    result = (char *) MALLOC(strlen(r) + 1);
	    strcpy(result, r);
	}
    }

    return result;
}

// ct 2008-12-03
int *invl(int *s, int *r)
{
    int *result = MALLOC(sizeof(int));

    if (!s) {
	if (!r) {
	    *result = 0;
	} else {
	    *result = *r;
	}
    } else {
	*result = *s;
    }

    return result;
}

// ct 2008-12-03
int *invl2(int *s, int *r, int *q)
{
    int *result = MALLOC(sizeof(int));

    if (!s) {
	if (!q) {
	    *result = 0;
	} else {
	    *result = *q;
	}
    } else {
	if (!r) {
	    *result = 0;
	} else {
	    *result = *r;
	}
    }

    return result;
}

// ct 2008-12-03
double *dnvl(double *s, double *r)
{
    double *result = MALLOC(sizeof(double));

    if (!s) {
	if (!r) {
	    *result = 0;
	} else {
	    *result = *r;
	}
    } else {
	*result = *s;
    }

    return result;
}

// ct 2008-12-03
double *dnvl2(double *s, double *r, double *q)
{
    double *result = MALLOC(sizeof(double));

    if (!s) {
	if (!q) {
	    *result = 0;
	} else {
	    *result = *q;
	}
    } else {
	if (!r) {
	    *result = 0;
	} else {
	    *result = *r;
	}
    }

    return result;
}

