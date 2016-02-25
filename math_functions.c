/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 *
 * File Name:   math_functions.c
 * Description: this module contains UDFs to generate/manipulate math functions
 * for exported functions list see math_functions.h
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
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__WATCOMC__)
#include <process.h>
#else
pid_t getpid(void);
#endif

#include "global.h"
#include "math_functions.h"
#include "date_functions.h"

#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__WATCOMC__)
#define HAVE_MATH_FLOOR
#endif

#ifndef HAVE_MATH_FLOOR
double trunc(double x);
#endif

// ct 2009-09-07 the following functions has moved to checksum_functions
// crosssum, crossdiff, weightcrosssum, iteratecrosssum,
// gtin8check, gtin13check, gtin14check, nve18check, gtin8, upc12check, upc12,
// gtin13, gtin14, nve18, isbn10check, isbn10, isbn13check, isbn13, issn8check,
// issn8, upstncheck, upstn, twooffivecheck, twooffive, code39check, code39

// 2007-07-22 ct add more languages
// uk de fr it es pt nl no se dk fi hu ie ee is al va v1 c1 s1 s2 w1 b1 b2 n1 za fo lu w2
static const char * NUM[] =
{
 "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ".",
 "zero", "one","two","three","four","five","six","seven", "eight", "nine", "dot",
 "null","eins","zwei","drei","vier","fünf","sechs", "sieben", "acht", "neun", "Komma",
 "zéro", "un", "deux", "trois", "quatre", "cinq", "six", "sept", "huit", "neuf", "virgule",
 "zero", "uno", "due", "tre", "quattro", "cinque", "sei", "sette", "otto", "nove", "virgola",
 "cero", "uno", "dos", "tres", "cuatro", "cinco", "seis", "siete", "ocho", "nueve", "coma",
 "zero", "um", "duas", "três", "quatro", "cinco", "seis", "sete", "oito", "nove", ",",
 "nul", "een", "twee", "drie", "vier", "vijf", "zes", "zeven", "acht", "negen", ",",
 "null", "en", "to", "tre", "fire", "fem", "seks", "syv", "åtte", "ni", ",",
 "noll", "en", "två", "tre", "fyra", "fem", "sex", "sju", "åtta", "nio", ",",
 "nul", "en", "to", "tre", "fire", "fem", "seks", "syv", "otte", "ni", ",",
 "nolla", "yksi", "kaksi", "kolme", "neljä", "viisi", "kuusi", "seitsemän", "kahdeksan", "yhdeksän", ",",
 "nulla", "egy", "ketto~", "három", "négy", "öt", "hat", "hét", "nyolc", "kilenc", ",",
 "náid", "a haon", "a dó", "a trí", "a ceathair", "a cúig", "a sé", "a seacht", "a hocht", "a naoi", ",",
 "null", "üks", "kaks", "kolm", "neli", "viis", "kuus", "seitse", "kaheksa", "üheksa", ",",
 "núll", "einn", "tveir", "þrír", "fjórir", "fimm", "sex", "sjö", "átta", "níu", ",",
 "zero", "një", "dy", "tre", "katër", "pesë", "gjashtë", "shtatë", "tetë", "nëntë", ",",
 "NVLLVS", "VNVS", "DVO", "TRES", "QVATTVOR", "QVINQVE", "SEX", "SEPTEM", "OCTO", "NOVEM", ",",
 "nullus", "unus", "duo", "tres", "quattuor", "quinque", "sex", "septem", "octo", "novem", ",",
 "zero", "un", "dos", "tres", "quatre", "cinc", "sis", "set", "vuit", "nou", ",",
 "nocht", "ane", "twa", "three", "fower", "five", "sax", "sieven", "aicht", "nine", ",",
 "neoni", "aon", "dhà", "trì", "ceithir", "còig", "sia", "seachd", "ochd", "naoi", ",",
 "dim", "un", "dau", "tri", "pedwar", "pump", "chwech", "saith", "wyth", "naw", ",",
 "mann", "unan", "daou", "tri", "pevar", "pemp", "c’hwec’h", "seizh", "eizh", "nav", ",",
 "zero", "bat", "bi", "hiru", "lau", "bost", "sei", "zazpi", "zortzi", "bederatzi", ",",
 "null", "ein", "to", "tri", "fire", "fem", "seks", "syv", "åtte", "ni", ",",
 "nul", "een", "twee", "drie", "vier", "vyf", "ses", "sewe", "agt", "nege", ",",
 "null", "ein", "tveir", "tríggir", "fýra", "fimm", "seks", "sjey", "átta", "níggju", ",",
 "eent", "zwee", "dräi", "véier", "fënnef", "sechs", "siwen", "aacht", "néng", "zéng", ",",
 "zérô", "onk", "deus", "troes", "cwate", "cénk", "shîjh", "set", "ût", "noûv", ","
};

// ct 2006-11-26 implementation of NULL for FB 2.0
// ct 2008-12-26 delete because the same as dollarval
// char *converttodollar(double *value)

// pm 2005-10-03
// ct 2006-11-20 implementation of NULL for FB 2.0 
// ct 2008-12-26 moved vom string_functions
char * dollarval (double *value)
{
  if(!value)
    return NULL;
  else
  {
    char * sz_result = (char *) MALLOC (35);
    sprintf(sz_result, "$%.2f", *value);
    return sz_result;
  }
}

// pm 2005-10-03
// ct 2006-11-20 implementation of NULL for FB 2.0
// ct 2008-12-26 moved vom string_functions
char * euroval (double *value)
{
  if(!value)
    return NULL;
  else
  {
    char * sz_result = (char *) MALLOC (35);
    sprintf(sz_result, "%.2f EUR", *value);
    return sz_result;
  }
}

// ct 2006-11-26 implementation of NULL for FB 2.0
double *doubleabs(double *value)
{
    if (!value)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        if (*value < 0)
            *result = -*value;
        else
            *result = *value;
        return result;
    }
}


// ct 2006-11-26 implementation of NULL for FB 2.0
char *fixedpoint(double *value, int *decimals)
{
    if (!value || !decimals)
        return NULL;
    else {
        char *sz_result = MALLOC(LOC_STRING_SIZE + 1);
        sprintf(sz_result, "%.*f", *decimals, *value);
        return sz_result;
    }
}

// ct 2009-09-23 fixes incorrect round of some numbers
// ct 2010-02-25 replaced a dirty workaround with epsilonics
// ct 2010-03-10 replaced while with pow
// gh 2010-10-14 replace pow own algorithm
// intern: round d1 with precision nk
/*double intern_zahlrunden(double d1, int nk)
{
    int sign = (d1 < 0.0);
    int n;
    if (sign) {
        d1 *= -1.0;
    }
    // workaround with the method of epsilonics
    // for more explanation see global.h
    d1 += DBL_EPSILON;
    n = nk;
    // for exact values uses pow instead of while because pow uses the math coprocessor
    d1 *= pow(10.0, n); 
#ifdef HAVE_MATH_FLOOR
    d1 = floor(d1 + 0.5);
#else
    d1 = trunc(d1 + 0.5);
#endif   
    n = nk;
    // for exact values uses pow instead of while because pow uses the math coprocessor
    d1 /= pow(10.0, n); 
    if (sign) {
        d1 *= -1.0;
    }
    return d1;
}
*/
double intern_zahlrunden(double d1, int nk)
{
    int sign = (d1 < 0.0);
    int i;
    double n = 1.0;

    if (sign) {
        d1 *= -1.0;
    }

    // workaround with the method of epsilonics
    // for more explanation see global.h
    d1 += DBL_EPSILON;
    
    // calculate 10 to the power of nk
    if (nk < 0) {
	for (i = 0; i > nk; i--) {
	    n /= 10.0;
	}
    } else {
	for (i = 0; i < nk; i++) {
	    n *= 10.0;
	}
    }
    d1 *= n;

#ifdef HAVE_MATH_FLOOR
    d1 = floor(d1 + 0.5);
#else
    d1 = trunc(d1 + 0.5);
#endif   

    d1 /= n;

    if (sign) {
        d1 *= -1.0;
    }

    return d1;
}

// pm 2005-10-xx
// gh 2008-01-
// gh 2010-10-21 bug malloc instead of MALLOC
// ct 2010-11-04 now also negativ values for number of digits to be round allowed
// round value with precision nk, using comma as decimal separator and tsep for thousands grouping.
char *fixedpointlang(double *value, int *nk, char *comma, char *tsep)
{
    if (!value || !nk || !comma || !tsep) {
        return NULL;
    } else {
        char buffer[128];
        double val;
        char *sz_result = MALLOC(LOC_STRING_SIZE + 1);
        char *resp = sz_result;
        char *pc;
        int i, bl, offset;
        
        val = intern_zahlrunden(*value, *nk);
        if (*nk < 0) {
          *nk = 0;
        }
        offset = (*nk > 0) ? 2 : 1;

        bl = sprintf(buffer, "%.*f", *nk, val);
        for (i = 0; i < bl; i++) {
            if (buffer[i] == '.' || buffer[i] == ',') {
                // Dezimalpunkt
                pc = comma;
                while (*pc) {
                    *resp++ = *pc++;
                }
            } else {
                *resp++ = buffer[i];
            }
            if (*tsep && (i < bl - offset - *nk) && (((bl - offset - *nk - i) % 3) == 0)) {
                // Tausender-Separator
                pc = tsep;
                while (*pc) {
                    *resp++ = *pc++;
                }
            }
        }
        *resp = '\0';
        return sz_result;
    }
}

// ct 2006-11-26 implementation of NULL for FB 2.0
int *integerabs(int *value)
{
    if (!value)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        if (*value < 0)
            *result = -*value;
        else
            *result = *value;
        return result;
    }
}


// pm 21.08.04
// ct 2006-11-26 implementation of NULL for FB 2.0
double *maxnum(double *d1, double *d2)
{
    if (!d1 || !d2)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        if (*d1 >= *d2)
            *result = *d1;
        else
            *result = *d2;
        return result;
    }
}


// pm 21.08.04
// ct 2006-11-26 implementation of NULL for FB 2.0
double *minnum(double *d1, double *d2)
{
    if (!d1 || !d2)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        if (*d1 <= *d2)
            *result = *d1;
        else
            *result = *d2;
        return result;
    }
}


// ct 2006-11-26 implementation of NULL for FB 2.0
int *modulo(int *numerator, int *denominator)
{
    if (!numerator || !denominator)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        if (*denominator == 0)
            *result = 0;
        else
            *result = *numerator % *denominator;
        return result;
    }
}


// pm 21.08.04
// ct 2006-11-26 implementation of NULL for FB 2.0
int *isdivisibleby(int *numerator, int *denominator)
{
    if (!numerator || !denominator)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        if (*denominator == 0)
            *result = 0;
        else
            *result = ((*numerator % *denominator) == 0);
        return result;
    }
}


// ct 2006-11-14
// ct 2006-11-26 implementation of NULL for FB 2.0
int *evenodd(int *numerator)
{
    if (!numerator)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = ((*numerator % 2) == 0);
        return result;
    }
}


// ct 2006-11-26 implementation of NULL for FB 2.0
// ct 2006-12-15 changed name from round to f_round because of function name in c
int *f_round(double *value)
{
    if (!value)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        if (*value < 0)
            *result = (int) (*value - 0.5);
        else
            *result = (int) (*value + 0.5);
        return result;
    }
}


// ct 2006-11-26 implementation of NULL for FB 2.0
// ct 2006-12-15 changed name from truncate to f_truncate because of function name in c
int * f_truncate(double *value)
{
    if (!value)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = (int) (*value);
        return result;
    }
}


// pm 21.08.04
// ct 2006-11-26 implementation of NULL for FB 2.0
double *fact(int *n)
{
    if (!n)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        if (*n < 1 || *n > 100)
            *result = 0.0;
        else {
            int i;
            double f = 1.0;
            for (i = 2; i <= *n; i++)
                f *= i;
            *result = f;
        }
        return result;
    }
}


// pm 03.10.05
// ct 2006-11-26 implementation of NULL for FB 2.0
int *equalfloat(double *d1, double *d2)
{
    if (!d1 || !d2)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = (*d1 == *d2);
        return result;
    }
}


// pm 03.10.05
// ct 2006-11-26 implementation of NULL for FB 2.0
int *equalinteger(int *i1, int *i2)
{
    if (!i1 || !i2)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = (*i1 == *i2);
        return result;
    }
}


// udf: round d1 with precision nk.
double *zahlrunden(double *d1, int *nk)
{
    if (!d1 || !nk) {
        return NULL;
    } else {
        double *result = MALLOC(sizeof(double));
        *result = intern_zahlrunden(*d1, *nk);
        return result;
    }
}

// ct 2007-01-24
// ct 2010-02-25 add workaround with epsilonics
// ct 2010-03-10 replaced while with pow
double *roundtoeven(double *d1, int *nk)
{
    if (!d1 || !nk) {
        return NULL;
    } 
    else {
        double *result = MALLOC(sizeof(double));
        double res = *d1;
        int n = *nk;
        int sign = (res < 0.0);
        if (sign) {
            res *= -1.0;
        }
        // for exact values uses pow instead of while because pow uses the math coprocessor
        res *= pow(10.0, n); 
        // workaround with the method of epsilonics
        // for more explanation see global.h
        if(res - floor(res) - 0.5 < DBL_EPSILON) {
           if((int) res & 1) {
              res = floor(res + 0.5);
	   } 
	   else {
              res = floor(res);
           }   
        }
        else {
           if(res - floor(res) < 0.5) {
             res = floor(res);
	   } 
	   else {
             res = floor(res + 0.5);
           }  
        }
        n = *nk;
        // for exact values uses pow instead of while because pow uses the math coprocessor
        res /= pow(10.0, n); 
        if (sign) {
            res *= -1.0;
        }
        *result = res;
        return result;
    }
}

// ct 2006-12-21 
// ct 2010-02-26 changed to intern_zahlrunden with epsilonics
// gh 2010-10-14 now also negativ values for number of digits to be round allowed
// similar to softround from rfunc
// ... but we don't understand the scence ...
double *softround(double *d1, int *nk)
{
    if (!d1 || !nk) {
        return NULL;
    } else {
        double *result = MALLOC(sizeof(double));
        if(*d1 * pow(10, *nk) >= 1) 
          *result = intern_zahlrunden(*d1, *nk);
        else
          *result = *d1;        
        return result;
    }
}

// pm 2005-10-22
// ct 2006-11-26 implementation of NULL for FB 2.0
double *roundfloat(double *d1, double *d2)
{
    if (!d1 || !d2)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        if (*d2 == 0)
            *result = 0;
        else
            *result = floor((*d1 / *d2) + 0.5) * *d2;
        return result;
    }
}

// pm 2005-10-03
// ct 2006-11-26 implementation of NULL for FB 2.0
// ct 2006-12-10 bug
// ct 2008-10-17 now compatible with GrUDF
double *prozente(double *d1, double *d2)
{
    if (!d1 || !d2)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = ( 100.0 / *d2 * *d1 ) - 100.0;
        return result;
    }
}


// gh 14.10.06
/* integer random number in range a <= x <= e */
// ct 2006-11-26 implementation of NULL for FB 2.0
int *intrandom(int *a, int *e)
{
    if (!a || !e)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        static time_t t = 0;
        double r = *e - *a + 1;

        if (t == 0)
            srand((unsigned) time(&t) + getpid());
        *result = *a + (int) (r * rand() / (RAND_MAX + 1.0));
        return result;
    }
}

// ct 2007-02-05
int *getrandom(int *e)
{
    if (!e)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        static time_t t = 0;
        double r = *e + 1;

        if (t == 0)
            srand((unsigned) time(&t) + getpid());
        *result = (int) (r * rand() / (RAND_MAX + 1.0));
        return result;
    }
}

// ct 2006-12-15
double *doublerandom(int *a, int *e)
{
    if (!a || !e)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        static time_t t = 0;
        double r = *e - *a + 1;

        if (t == 0)
            srand((unsigned) time(&t) + getpid());
        *result = *a + (double) (r * rand() / (RAND_MAX + 1.0));
        return result;
    }
}

// ct 2006-12-15
double * sin_rad(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = sin(*a);
        return result;
    }
}

// ct 2006-12-15
double * cos_rad(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = cos(*a);
        return result;
    }
}

// ct 2006-12-15
double * tan_rad(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = tan(*a);
        return result;
    }
}

// ct 2006-12-15
double *cot_rad(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = 1.0 / (tan(*a));
        return result;
    }
}

// ct 2006-12-15
double * arcsin_rad(double *a)
{
    if (!a || *a < -1 || *a > 1)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = asin(*a);
        return result;
    }
}

// ct 2006-12-15
double * arccos_rad(double *a)
{
    if (!a || *a < -1 || *a > 1)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = acos(*a);
        return result;
    }
}

// ct 2006-12-15
double * arctan_rad(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = atan(*a);
        return result;
    }
}

// ct 2006-12-15
double * arctan2_rad(double *a, double *b)
{
    if (!a || !b)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = atan2(*a, *b);
        return result;
    }
}

// ct 2006-12-15
double * arccot_rad(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = 1.0 / atan(*a);
        return result;
    }
}

// ct 2006-12-15
double * sin_deg(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = sin(*a / 180.0 * M_PI);
        return result;
    }
}

// ct 2006-12-15
double * cos_deg(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = cos(*a / 180.0 * M_PI);
        return result;
    }
}

// ct 2006-12-15
double * tan_deg(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = tan(*a / 180.0 * M_PI);
        return result;
    }
}

// ct 2006-12-15
double * cot_deg(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = 1.0 / (tan(*a / 180.0 * M_PI));
        return result;
    }
}

// ct 2006-12-15
double * arcsin_deg(double *a)
{
    if (!a || *a < -1 || *a > 1)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = asin(*a) / M_PI * 180.0;
        return result;
    }
}

// ct 2006-12-15
double * arccos_deg(double *a)
{
    if (!a || *a < -1 || *a > 1)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = acos(*a) / M_PI * 180.0;
        return result;
    }
}

// ct 2006-12-15
double * arctan_deg(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = atan(*a) / M_PI * 180.0;
        return result;
    }
}

// ct 2006-12-15
double * arccot_deg(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = (1.0 / atan(*a)) / M_PI * 180.0;
        return result;
    }
}

// ct 2006-12-15
double * sinhyp(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = sinh(*a);
        return result;
    }
}

// ct 2006-12-15
double * coshyp(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = cosh(*a);
        return result;
    }
}

// ct 2006-12-15
double * tanhyp(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = tanh(*a);
        return result;
    }
}

// ct 2006-12-15
double *cothyp(double *a)
{
    if (!a || *a == 0)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = 1.0 / (tanh(*a));
        return result;
    }
}

// ct 2006-12-15
double * arsinh(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = log(*a + sqrt((*a * *a) + 1));
        return result;
    }
}

// ct 2006-12-15
double * arcosh(double *a)
{
    if (!a || *a < 1)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = log(*a + sqrt((*a * *a) - 1));
        return result;
    }
}

// ct 2006-12-15
double * artanh(double *a)
{
    if (!a || *a <= -1 || *a >= 1)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = (log((1 + *a) / (1 - *a))) / 2;
        return result;
    }
}

// ct 2006-12-15
double * arcoth(double *a)
{
    if (!a || (*a >= -1 && *a <= 1))
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));

        *result = (log((*a + 1) / (*a - 1))) / 2;
        return result;
    }
}

// ct 2006-12-15
double * f_rand()
{
    double *result = MALLOC(sizeof(double));
    static time_t t = 0;

    if (t == 0) {
        srand((unsigned) time(&t) + getpid());
    }
    *result = (double) rand() / (double) RAND_MAX;
    return result;
}

// ct 2006-12-15
int * f_sign( double *a)
{
    if (!a)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        if (*a == 0) {
            *result = 0;
        }
        if (*a > 0) {
            *result = 1;
        }
        if (*a < 0) {
            *result = -1;
        }
        return result;
    }
}

// ct 2006-12-15
double * f_sqrt( double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = sqrt(*a);
        return result;
    }
}

// ct 2006-12-15
double * f_ln( double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = log(*a);
        return result;
    }
}

// ct 2006-12-15
double * f_log( double *a, double *b)
{
    if (!a || !b)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = log(*a) / log(*b);
        return result;
    }
}

// ct 2006-12-15
double * f_log10( double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = log10(*a);
        return result;
    }
}

// ct 2006-12-15
double * f_floor( double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = floor(*a);
        return result;
    }
}

// ct 2006-12-15
double * f_ceiling( double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = ceil(*a);
        return result;
    }
}

// ct 2006-12-15
double * f_frac( double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        if (*a > 0)
            *result = *a - floor(*a);
        if (*a < 0)
            *result = *a - ceil(*a);
        if (*a == 0)
            *result = 0;
        return result;
    }
}

// ct 2006-12-15
int * f_bin_and(int *a, int *b)
{
    if (!a || !b)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = (*a & *b);
        return result;
    }
}

// ct 2006-12-15
int * f_bin_or(int *a, int *b)
{
    if (!a || !b)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = (*a | *b);
        return result;
    }
}

// ct 2006-12-15
int * f_bin_xor(int *a, int *b)
{
    if (!a || !b)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = (*a ^ *b);
        return result;
    }
}

// ct 2006-12-15
int * f_bin_not(int *a)
{
    if (!a)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = ~(*a);
        return result;
    }
}

// ct 2006-12-15
int * f_bin_shl(int *a, int *b)
{
    if (!a || !b)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = (*a << *b);
        return result;
    }
}

// ct 2006-12-15
int * f_bin_shr(int *a, int *b)
{
    if (!a || !b)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = (*a >> *b);
        return result;
    }
}

// ct 2007-02-05
int * r_getbit(int * num, int * shift)
{
    if (!num || !shift)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        if ((*shift<0) || (*shift>31))
                *result = -1;
        if(((1<<*shift)&(*num))==0)
                *result = 0;
        else
                *result = 1;
        return result;
    }
}

// ct 2007-02-05
int * r_setbit(int * num, int * shift, int * val)
{
    if (!num || !shift || !val)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        if((*shift<0) || (*shift>31) || (*val<0) || (*val>1))
                *result = *num;
        else
        {
                if(*val==1)
                        *result = (*num) | (1<<*shift);
                else
                        *result = (*num) & (~(1<<*shift));
        }
        return result;
    }
}

// ct 2006-12-15
double * f_power(double *a, double *b)
{
    if (!a || !b)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = pow(*a, *b);
        return result;
    }
}

// ct 2006-12-15
double * f_exp(double *a)
{
    if (!a)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = exp(*a);
        return result;
    }
}

// gh 2007-01-22
char *numinwords(double *value, int *precision, char *language)
{
    if (!value || !precision || !language) {
        return NULL;
    } else {
        char *result = (char *) MALLOC(MAX_STRINGBUFFER_SIZE);
        char num[32], *p;
        int lang, i;

        if (*precision > 10) *precision = 10;
        if (*precision < 0) *precision = 0;
        sprintf(num, "%.10f", *value);
        p = strchr(num, '.');
        if (*precision > 0) {
            p += (*precision + 1);
        }
        *p = '\0';

        lang = decode_language(language);
        if (lang < 0) {
            strcpy(result, "???");
        } else {
            strcpy(result, "***");
            for (i = 0; num[i]; i++) {
                if (num[i] == '.' || num[i] == ',') {
                    strcat(result, NUM[(11 * (lang + 1)) + 10]);
                } else {
                    strcat(result, NUM[(11 * (lang + 1)) + num[i] - '0']);
                }
                if (num[i + 1]) {
                    strcat(result, "-");
                }
            }
            strcat(result, "***");
        }
        return result;
    }
}

// ct 2007-01-28
int * r_div(int *q, int *r)
{
    if (!q || !r) {
        return NULL;
    }
    else {
        int *result = MALLOC(sizeof(int));
        div_t x;
        if (*r == 0)
          *result = 0;
        else {
           x = div(*q, *r);
           *result = x.quot;
        }
        return result;
    }
}

// ct 2007-02-05
// ct 2010-01-12 changed to pointer
double *r_z(double * a)
{
    double *result = MALLOC(sizeof(double));
    *result = *a;
    return result;
}

// ct 2007-02-05
// ct 2010-01-12 changed to pointer
double *r_dividezero(double * a, double * b, double * def)
{
    double *result = MALLOC(sizeof(double));
    *result = (*b) ? (*a / *b) : *def;
    return result;
}

// gh 2009-09-15
// format a double value with 2 decimal places and an currency sign from input
char *inputval(double *value, char *currency)
{
    if (!value || !currency) {
	return NULL;
    } else {
	char *res = MALLOC(35);
	snprintf(res, 34, "%.2f %s", *value, currency);
	return res;
    }
}

// gh 2010-10-14
double *rappenrundung(double *d)
{
    if (!d)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));

        int sign = (*d < 0.0);
        if (sign) {
            *d = -*d;
        }
        // workaround with the method of epsilonics
        // for more explanation see global.h
        *d += DBL_EPSILON;  
        *d *= 20.0;
#ifdef HAVE_MATH_FLOOR
        *d = floor(*d + 0.5);
#else
        *d = trunc(*d + 0.5);
#endif   
        *d /= 20.0;
        if (sign) {
            *d = -*d;
        }
        *result = *d;
	return result;
    }    
}
