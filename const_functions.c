/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 *  
 * File Name:	const_functions.c
 * Description: this module contains UDFs of technical constants
 * for exported functions list see const_functions.h
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

#include <math.h>
#include "global.h"
#include "const_functions.h"

// mathematical constants

// pi
// ct 2006-11-16
// ct 2010-01-12 changed to pointer
double *pi ()
{
    double *result = MALLOC(sizeof(double));
    *result = M_PI;
    return result;
}

// e Euler
// ct 2006-11-16
// ct 2010-01-12 changed to pointer
double *euler ()
{
    double *result = MALLOC(sizeof(double));
    *result = M_E;
    return result;
}

// Epsilon
// ct 2010-11-29
double *epsilon ()
{
    double *result = MALLOC(sizeof(double));
    *result = DBL_EPSILON;
    return result;
}

// physical constants

// c (speed of light) [m/s]
// ct 2006-11-16
// ct 2010-01-12 changed to pointer
int *c ()
{
    int *result = MALLOC(sizeof(int));
    *result = M_C;
    return result;
}

// g (acceleration of gravity) [m/s^2]
// ct 2006-11-16
// ct 2010-01-12 changed to pointer
double *g ()
{
    double *result = MALLOC(sizeof(double));
    *result = M_G;
    return result;
}

// K (absolute zero point) = 0 Grad Kelvin
// ct 2006-11-16
// ct 2010-01-12 changed to pointer
double *k ()
{
    double *result = MALLOC(sizeof(double));
    *result = M_K;
    return result;
}
