/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 *
 * File Name:   conv_functions.c
 * Description: this module contains UDFs to convert units
 * for exported functions list see conv_functions.h
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

#include <time.h>  
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "global.h"
#include "conv_functions.h"
#include "math_functions.h"

#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__WATCOMC__)
#define HAVE_MATH_FLOOR
#endif

/* *** number systems *** */

// pm
// ct 2006-11-25 implementation of NULL for FB 2.0
// number system: number in 33-system to decimal-system
int *convertfrom33(char *sz)
{
    if (!sz)
        return NULL;
    else {
        int base = 33;
        int *result = convertfrombase(sz, &base,
            "123456789ABCDEFGHIJKLMNPQRSTUVWXYZ");
        return result;
    }
}

// pm 
// ct 2006-11-25 implementation of NULL for FB 2.0
// number system: number in decimal-system to 33-system
char *convertto33(int *value)
{
    if (!value)
        return NULL;
    else {
        int base = 33;
        char *result = converttobase(value, &base,
            "123456789ABCDEFGHIJKLMNPQRSTUVWXYZ");
        return result;
    }
}

// pm
static int get_value_of(char c, char *digits)
{
    int i;

    for (i = 0; (digits[i] != (char) 0) && (digits[i] != c); i++);
    return i;
}

// pm
// ct 2006-11-25 implementation of NULL for FB 2.0
// number system: number in anyone-system to decimal-system
int *convertfrombase(char *sz, int *base, char *digits)
{
    if (!sz || !base || !digits)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        int i;
        int res = 0;
        int len = strlen(sz);

        for (i = 0; i < len; i++)
            res = (*base * res) + get_value_of(sz[i], digits);
        *result = res;
        return result;
    }
}

// pm
// ct 2006-11-25 implementation of NULL for FB 2.0
// ct 2009-03-11 intercept of invalid ranges
// number system: number decimal-system to in anyone-system
char *converttobase(int *value, int *base, char *digits)
{
    if (!value || !base || !digits || *value < 0 || *base < 2 || (unsigned int) *value > 2147483647 || *base > 60)
        return NULL;
    else {
        char sz_res[LOC_STRING_SIZE + 1];
        char *sz_result = (char *) MALLOC(LOC_STRING_SIZE + 1);
        int i = 0;
        int j, r, len_sz_res;
        int first_time = 1;

        for (j = *value; (j != 0) || first_time; j = j / *base, first_time = 0) {
            r = j % *base;
            sz_res[i++] = digits[r];
        }
        sz_res[i] = (char) 0;
        sz_result[i] = (char) 0;
        len_sz_res = i;

        for (i = 0; i < len_sz_res; i++)
            sz_result[i] = sz_res[len_sz_res - i - 1];

        return sz_result;
    }
}

// pm 2005-10-03
// ct 2006-11-25 implementation of NULL for FB 2.0
// number system: integer in decimal-system to hexword
char *inttohex(int *n, int *digits)
{
    if (!n || !digits)
        return NULL;
    else {
        char *sz_result = (char *) MALLOC(*digits + 1);
        char *pc = sz_result;

        sprintf(sz_result, "%*x", *digits, *n);

        while (*pc) {
            if (*pc == ' ')
                *pc = '0';
            pc++;
        }

        return sz_result;
    }
}

// pm 2005-10-03
// ct 2006-11-25 implementation of NULL for FB 2.0
// number system: hexword to integer in decimal-system
int *hextoint(char *sz)
{
    if (!sz)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        int res = 0;

        while (*sz) {
            res *= 16;
            if (isdigit(*sz)) {
                res += (*sz - '0');
            } else if (isalpha(*sz)) {
                if (*sz <= 'Z')
                    res += ((*sz - 'A') + 10);
                else
                    res += ((*sz - 'a') + 10);
            }
            sz++;
        }

        *result = res;
        return result;
    }
}

// ct 2006-12-xx
// number system: roman to arabic
int *roman2arabic(char *rz)
{
    if (!rz)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        int len = strlen(rz);
        char ziffer;            /* enthaelt die aktuelle Ziffer der roemischen Zahl */
        int naechst = 0;        /* enthaelt 'ziffer' in arabischer Form             */
        int letzt = 0;          /* enthaelt die vorherige Ziffer in arabischer Form */
        int wert = 0;           /* enthaelt das Ergebnis = arabische Zahl           */

        int i;

    /*----------------------------------------------------
    verarbeite die roemische Zahl von links nach rechts:
    ----------------------------------------------------*/
        for (i = 0; i < len; i++) {

            ziffer = toupper(rz[i]);

      /*-------------------------------------------
      Die aktuelle roemische Ziffer wird in ihren
      arabischen Gegenwert uebersetzt.
      -------------------------------------------*/
            switch (ziffer) {

            case 'M':
                naechst = 1000;
                break;

            case 'D':
                naechst = 500;
                break;

            case 'C':
                naechst = 100;
                break;

            case 'L':
                naechst = 50;
                break;

            case 'X':
                naechst = 10;
                break;

            case 'V':
                naechst = 5;
                break;

            case 'I':
                naechst = 1;
                break;

            default:
                *result = -1;   /* ungueltige Ziffer => Abbruch */
                return result;

            }                   /* switch */

      /*--------------------------------------------------------
      Wenn die vorherige roemische Ziffer einen kleineren Wert
      hat als die akutelle, so wird der Wert der vorherigen
      subtrahiert, ansonsten wird er addiert.
      --------------------------------------------------------*/
            if (letzt < naechst) {
                wert -= letzt;
            } else {
                wert += letzt;
            }

            letzt = naechst;    /* eine Ziffer weiterruecken */

        }                       /* for */

        wert += letzt;

        *result = wert;
        return result;
    }
}

// ct 2006-12-xx
// number system: arabic to roman
static int ArabicTab[13] =
    { 1, 4, 5, 9, 10, 40, 50, 90, 100, 400, 500, 900, 1000 };

static char *RomanTab[13] =
    { "I", "IV", "V", "IX", "X", "XL", "L", "XC", "C", "CD", "D", "CM", "M" };

char *arabic2roman(int *az)
{
    if (!az)
        return NULL;
    else {
        int i, a = *az;
        char *rz = (char *) MALLOC(250);

        *rz = '\0';
        if (a <= 200000) {
            for (i = 12; i >= 0; i--) {
                while (a >= ArabicTab[i]) {
                    a -= ArabicTab[i];
                    strcat(rz, RomanTab[i]);
                }
            }
        }
        return rz;
    }
}

/* *** geometrical system *** */

// pm 2005-10-03
// ct 2006-11-25 implementation of NULL for FB 2.0
// geometrical: angle (Alt-)degree to radian (radian measure)
double *degtorad(double *d)
{
    if (!d)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *d / 180.0 * M_PI;
        return result;
    }
}

// pm 2005-10-03
// ct 2006-11-25 implementation of NULL for FB 2.0
//  geometrical: radian (radian measure) to angle (Alt-)degree
double *radtodeg(double *d)
{
    if (!d)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *d / M_PI * 180.0;
        return result;
    }
}

/* *** length *** */

// ct 2006-12-xx
// length: inch to meter
double *in2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 2.54 / 100.0;
        return result;
    }
}

// ct 2006-12-xx
// length: meter to inch
double *m2in(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 2.54 * 100.0;
        return result;
    }
}

// ct 2006-12-xx
// length: feet to meter
double *ft2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.3048;
        return result;
    }
}

// ct 2006-12-xx
// length: meter to feet
double *m2ft(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.3048;
        return result;
    }
}

// ct 2006-12-xx
// length: yard to meter
double *yd2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.9144;
        return result;
    }
}

// ct 2006-12-xx
// length: meter to yard
double *m2yd(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.9144;
        return result;
    }
}

// ct 2006-12-xx
// length: mile to kilometer
double *mile2km(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 1.609344;
        return result;
    }
}

// ct 2006-12-xx
// length: kilometer to mile
double *km2mile(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 1.609344;
        return result;
    }
}

// ct 2006-12-xx
// length: fathom to meter
double *fm2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 1.829;
        return result;
    }
}

// ct 2006-12-xx
// length: meter to fathom
double *m2fm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 1.829;
        return result;
    }
}

// ct 2008-02-12
// length: furlong to meter
double *fur2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 201.168;
        return result;
    }
}

// ct 2008-02-12
// length: meter to furlong
double *m2fur(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 201.168;
        return result;
    }
}

// ct 2008-02-12
// length: chain to meter
double *ch2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 20.1168;
        return result;
    }
}

// ct 2008-02-12
// length: meter to chain
double *m2ch(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 20.1168;
        return result;
    }
}

// ct 2008-02-12
// length: rod to meter
double *rd2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 5.0292;
        return result;
    }
}

// ct 2008-02-12
// length: meter to rod
double *m2rd(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 5.0292;
        return result;
    }
}

// ct 2008-02-12
// length: link to meter
double *li2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.201168;
        return result;
    }
}

// ct 2008-02-12
// length: meter to link
double *m2li(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.201168;
        return result;
    }
}

// ct 2008-02-12
// length: league to kilometer
double *lea2km(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 4.828032;
        return result;
    }
}

// ct 2008-02-12
// length: kilometer to league
double *km2lea(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 4.828032;
        return result;
    }
}

// ct 2006-12-xx
// nautic length: seamile to kilometer
double *sm2km(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 1.8520;
        return result;
    }
}

// ct 2006-12-xx
// nautic length: kilometer to seamile
double *km2sm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 1.8520;
        return result;
    }
}

// ct 2008-02-12
// nautic length: shackle (GB) or shot(US) to meter
double *shot2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 27.432;
        return result;
    }
}

// ct 2008-02-12
// nautic length: meter to shackle (GB) or shot(US)
double *m2shot(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 27.432;
        return result;
    }
}

// ct 2008-02-12
// nautic length: cable to meter
double *cbl2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 185.3184;
        return result;
    }
}

// ct 2008-02-12
// nautic length: meter to cable
double *m2cbl(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 185.3184;
        return result;
    }
}

// ct 2008-02-12
// nautic length: sealeague to meter
double *nl2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 5559.552;
        return result;
    }
}

// ct 2008-02-12
// nautic length: meter to sealeague
double *m2nl(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 5559.552;
        return result;
    }
}

// ct 2008-02-12
// nautic length: admiraltymile to meter
double *nm2m(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 1853.184;
        return result;
    }
}

// ct 2008-02-12
// nautic length: meter to admiraltymile
double *m2nm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 1853.184;
        return result;
    }
}

// ct 2006-12-xx
// typographic dimension: typoPoint to millimeter
double *p2mm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.376065;
        return result;
    }
}

// ct 2006-12-xx
// typographic dimension: millimeter to typoPoint
double *mm2p(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.376065;
        return result;
    }
}

/* *** area *** */

// ct 2006-12-xx
// area: ar to squarmeter
double *ar2sqm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 100.0;
        return result;
    }
}

// ct 2006-12-xx
// area: squarmeter to ar
double *sqm2ar(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 100.0;
        return result;
    }
}

// ct 2006-12-xx
// area: ha to squarmeter
double *ha2sqm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 10000.0;
        return result;
    }
}

// ct 2006-12-xx
// area: squarmeter to ha
double *sqm2ha(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 10000.0;
        return result;
    }
}

// ct 2006-12-xx
// area: german Morgen to squarmeter
double *morgen2sqm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 2500.0;
        return result;
    }
}

// ct 2006-12-xx
// area: squarmeter to german Morgen
double *sqm2morgen(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 2500.0;
        return result;
    }
}

// ct 2006-12-xx
// area: squarfeet to squarmeter
double *sqft2sqm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.09290306;
        return result;
    }
}

// ct 2006-12-xx
// area: squarmeter to squarfeet
double *sqm2sqft(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.09290306;
        return result;
    }
}

// ct 2006-12-xx
// area: acre to squarmeter
double *ac2sqm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 4046.856;
        return result;
    }
}

// ct 2006-12-xx
// area: squarmeter to acre
double *sqm2ac(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 4046.856;
        return result;
    }
}

// ct 2006-12-xx
// area: squaryard to squarmeter
double *sqyd2sqm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.8361;
        return result;
    }
}

// ct 2006-12-xx
// area: squarmeter to squaryard
double *sqm2sqyd(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.8361;
        return result;
    }
}

/* *** volume *** */
// ct 2006-12-xx
// volume: barrel to litre
double *bbl2l(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 158.988;
        return result;
    }
}

// ct 2006-12-xx
// volume: litre to barrel
double *l2bbl(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 158.988;
        return result;
    }
}

// ct 2006-12-xx
// volume: GB fluid ounce to mililitre
double *gbfloz2ml(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 28.4131;
        return result;
    }
}

// ct 2006-12-xx
// volume: mililitre to GB fluid ounce
double *ml2gbfloz(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 28.4131;
        return result;
    }
}

// ct 2006-12-xx
// volume: US fluid ounce to mililitre
double *usfloz2ml(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 29.5735;
        return result;
    }
}

// ct 2006-12-xx
// volume: mililitre to US fluid ounce
double *ml2usfloz(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 29.5735;
        return result;
    }
}

// ct 2006-12-xx
// volume: GB pint to litre
double *gbpt2l(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.568262;
        return result;
    }
}

// ct 2006-12-xx
// volume: litre to GB pint
double *l2gbpt(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.568262;
        return result;
    }
}

// ct 2006-12-xx
// volume: US pint to litre
double *uspt2l(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.473176;
        return result;
    }
}

// ct 2006-12-xx
// volume: litre to US pint
double *l2uspt(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.473176;
        return result;
    }
}

// ct 2006-12-xx
// volume: GB quart to litre
double *gbqt2l(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 1.136521;
        return result;
    }
}

// ct 2006-12-xx
// volume: litre to GB quart
double *l2gbqt(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 1.136521;
        return result;
    }
}

// ct 2006-12-xx
// volume: US quart to litre
double *usqt2l(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.9463531;
        return result;
    }
}

// ct 2006-12-xx
// volume: litre to US quart
double *l2usqt(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.9463531;
        return result;
    }
}

// ct 2006-12-xx
// volume: GB gallon to litre
double *gbgal2l(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 4.546091;
        return result;
    }
}

// ct 2006-12-xx
// volume: litre to GB gallon
double *l2gbgal(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 4.546091;
        return result;
    }
}

// ct 2006-12-xx
// volume: US gallon to litre
double *usgal2l(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 3.785411;
        return result;
    }
}

// ct 2006-12-xx
// volume: litre to US gallon
double *l2usgal(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 3.785411;
        return result;
    }
}

// ct 2008-02-12
// volume: cubic inch to cubic meter
double *cuin2cum(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.000016387064;
        return result;
    }
}

// ct 2008-02-12
// volume: cubic meter to cubic inch
double *cum2cuin(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.000016387064;
        return result;
    }
}

// ct 2008-02-12
// volume: cubic foot to cubic meter
double *cuft2cum(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.028316846592;
        return result;
    }
}

// ct 2008-02-12
// volume: cubic meter to cubic foot
double *cum2cuft(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.028316846592;
        return result;
    }
}

// ct 2008-02-12
// volume: cubic yard to cubic meter
double *cuyd2cum(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.764554857984;
        return result;
    }
}

// ct 2008-02-12
// volume: cubic meter to cubic yard
double *cum2cuyd(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.764554857984;
        return result;
    }
}

// ct 2008-02-12
// volume: cup (8 ounces) to milliliter
double *cup2ml(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 236.56;
        return result;
    }
}

// ct 2008-02-12
// volume: milliliter to cup (8 ounces)
double *ml2cup(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 236.56;
        return result;
    }
}

// ct 2006-12-xx
// nautic volume: register ton to cubic meter
double *rt2cbm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 2.83168;
        return result;
    }
}

// ct 2006-12-xx
// nautic volume: cubic meter to register ton
double *cbm2rt(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 2.83168;
        return result;
    }
}

/* *** weight *** */

// ct 2006-12-xx
// weight: carat to gramm
double *ct2g(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.2;
        return result;
    }
}

// ct 2006-12-xx
// weight: gramm to carat
double *g2ct(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.2;
        return result;
    }
}

// ct 2006-12-xx
// weight: ounce to gramm
double *oz2g(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 28.3495;
        return result;
    }
}

// ct 2006-12-xx
// weight: gramm to ounce
double *g2oz(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 28.3495;
        return result;
    }
}

// ct 2006-12-xx
// weight: troy ounce to gramm
double *oztr2g(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 31.1;
        return result;
    }
}

// ct 2006-12-xx
// weight: gramm to troy ounce
double *g2oztr(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 31.1;
        return result;
    }
}

// ct 2006-12-xx
// weight: pound to kilogramm
double *lb2kg(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.45359237;
        return result;
    }
}

// ct 2006-12-xx
// weight: kilogramm to pound
double *kg2lb(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.45359237;
        return result;
    }
}

// ct 2008-02-12
// weight: stone to gramm
double *st2g(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 6350.293180;
        return result;
    }
}

// ct 2008-02-12
// weight: gramm to stone
double *g2st(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 6350.293180;
        return result;
    }
}

// ct 2008-02-12
// weight: hundredweight (cental) to gramm
double *cwt2g(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 5359.237;
        return result;
    }
}

// ct 2008-02-12
// weight: gramm to hundredweight (cental)
double *g2cwt(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 5359.237;
        return result;
    }
}

/* *** temperature *** */

// ct 2006-12-xx
// temperature: celsius to fahrenheit
double *c2f(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 1.8 + 32.0;
        return result;
    }
}

// ct 2006-12-xx
// temperature: fahrenheit to celsius
double *f2c(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = (*x - 32.0) / 1.8;
        return result;
    }
}

/* *** technical *** */

// ct 2006-12-xx
// magnetic: Gauss to Tesla
double *g2t(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 10000.0;
        return result;
    }
}

// ct 2006-12-xx
// magnetic: Tesla to Gauss
double *t2g(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 10000.0;
        return result;
    }
}

// ct 2006-12-xx
// pressure: bar to torr
double *bar2torr(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 750.0638;
        return result;
    }
}

// ct 2006-12-xx
// pressure: torr to bar
double *torr2bar(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 750.0638;
        return result;
    }
}

// ct 2006-12-xx
// pressure: bar to pascal
double *bar2pa(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 100000.0;
        return result;
    }
}

// ct 2006-12-xx
// pressure: pascal to bar
double *pa2bar(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 100000.0;
        return result;
    }
}

// ct 2006-12-xx
// pressure: psi to pascal
double *psi2pa(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 6894.757;
        return result;
    }
}

// ct 2006-12-xx
// pressure: pascal to psi
double *pa2psi(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 6894.757;
        return result;
    }
}

// ct 2006-12-xx
// pressure: torr to pascal
double *torr2pa(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 133.322;
        return result;
    }
}

// ct 2006-12-xx
// pressure: pascal to torr
double *pa2torr(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 133.322;
        return result;
    }
}

// ct 2006-12-xx
// energie/work: calorie to joule
double *cal2j(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 4.1868;
        return result;
    }
}

// ct 2006-12-xx
// energie/work: joule to calorie
double *j2cal(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 4.1868;
        return result;
    }
}

// ct 2008-02-12
// energie/work: pounds-feet to Newton-meter
double *lbft2nm(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 1.3562811;
        return result;
    }
}

// ct 2008-02-12
// energie/work: Newton-meter to pounds-feet
double *nm2lbft(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 1.3562811;
        return result;
    }
}

// ct 2006-12-xx
// power: PS to kW
double *ps2kw(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.73549875;
        return result;
    }
}

// ct 2006-12-xx
// ct 2010-12-01 changed to div instead of mul
// power: kW to PS
double *kw2ps(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.73549875;
        return result;
    }
}

// ct 2006-12-xx
// power: british hp to kW
double *bhp2kw(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.745699;
        return result;
    }
}

// ct 2006-12-xx
// power: kW to british hp
double *kw2bhp(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.745699;
        return result;
    }
}

// ct 2008-02-12
// consumption: Miles per USGallon to liter per 100km
// consumption: liter per 100km to Miles per USGallon
double *mpusgal2lp100km(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = 235.2145968 / *x;
        return result;
    }
}

// ct 2008-02-12
// consumption: Miles per GBGallon to liter per 100km
// consumption: liter per 100km to Miles per GBGallon
double *mpgbgal2lp100km(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = 282.4809363 / *x;
        return result;
    }
}

// ct 2008-02-12
// speed: miles per hour to kilometer per hour
double *mph2kmph(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 1.609344;
        return result;
    }
}

// ct 2008-02-12
// speed: kilometer per hour to miles per hour
double *kmph2mph(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 1.609344;
        return result;
    }
}

// ct 2008-02-12
// speed: food per seconds to meter per seconds
double *fps2mps(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.3048;
        return result;
    }
}

// ct 2008-02-12
// speed: meter per seconds to food per seconds
double *mps2fps(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.3048;
        return result;
    }
}

// ct 2008-02-12
// nautic speed: seamile per hour to kilometer per hour
double *smph2kmph(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 1.852;
        return result;        
    }
}

// ct 2008-02-12
// nautic speed: kilometer per hour to seamile per hour
double *kmph2smph(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 1.852;
        return result;
    }
}

// ct 2008-10-31
// weight: obulus (greek, ancient world) to gramm
double *obu2g(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 0.72;
        return result;
    }
}

// ct 2008-10-31
// weight: gramm to obulus (greek, ancient world)
double *g2obu(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 0.72;
        return result;
    }
}

// ct 2008-10-31
// weight: drachme (greek, ancient world) to gramm
double *dra2g(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 4.4;
        return result;
    }
}

// ct 2008-10-31
// weight: gramm to drachme (greek, ancient world)
double *g2dra(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 4.4;
        return result;
    }
}

// ct 2008-10-31
// volume: GBbushel to litre
double *gbbu2l(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 36.349;
        return result;
    }
}

// ct 2008-10-31
// volume: litre to GBbushel
double *l2gbbu(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 36.349;
        return result;
    }
}

// ct 2008-10-31
// volume: USbushel to litre
double *usbu2l(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 35.239;
        return result;
    }
}

// ct 2008-10-31
// volume: litre to USbushel
double *l2usbu(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 35.239;
        return result;
    }
}

extern const double msec_in_day; 

// ct 2008-11-02
// timestamp to decimal time (time in decimal unit)
double * time2dectime (ISC_QUAD *timestamp)
{
    if (!timestamp)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = (double) timestamp->gds_quad_low / msec_in_day * 24;
        return result;
    }
}

#ifndef HAVE_MATH_FLOOR
   double trunc(double x);
#endif

// ct/gh 2008-11-07
// decimaltime (time in decimal unit) to timestamp
ISC_TIME *dectime2time(double *dectime)
{
    if (!dectime) {
	     return NULL;
    } 
    else {
       ISC_TIME *result = (ISC_TIME *) MALLOC(sizeof(ISC_TIME));
	     struct tm t;
	     int minutes;

#ifdef HAVE_MATH_FLOOR
	     t.tm_hour = (int) floor(*dectime);
       minutes = (int) floor(60 * (*dectime - t.tm_hour));
#else
	     t.tm_hour = (int) trunc(*dectime);
       minutes = (int) trunc(60 * (*dectime - t.tm_hour));
#endif
	     t.tm_min  = (int) intern_zahlrunden(60 * (*dectime - t.tm_hour),0);
	     t.tm_sec  = (int) intern_zahlrunden(60 *((60 * (*dectime - t.tm_hour)) - minutes),0);
       if(t.tm_sec == 60)
          t.tm_sec = 0;
//       t.tm_mday = t.tm_mon = t.tm_year = t.tm_wday = t.tm_yday = t.tm_isdst = 0;
	     isc_encode_sql_time(&t, result);
  
	     return result;
    }
}

// ct 2008-11-07
// timestamp to industrial time (time in decimal unit round seconds to minute)
double * time2indtime (ISC_QUAD *timestamp)
{
    if (!timestamp)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        double m, r;
        struct tm t;
	      isc_decode_date(timestamp, &t);
//        t.tm_mday = t.tm_mon = t.tm_year = t.tm_wday = t.tm_yday = t.tm_isdst = 0;
        if(t.tm_sec>29) {
          t.tm_min += 1;
        }       
//        t.tm_sec = 0;
#ifdef HAVE_MATH_FLOOR
        m = floor(t.tm_min / 0.6) / 100.0;
#else
        m = trunc(t.tm_min / 0.6) / 100.0;
#endif
        r = (double) intern_zahlrunden((t.tm_hour + m) ,2); 
        *result = r;       
              
        return result;
    }
}

// ct 2008-11-07
// industrialtime (time in decimal unit round seconds to minute) to timestamp
ISC_TIME *indtime2time(double *indtime)
{
    if (!indtime) {
	     return NULL;
    } 
    else {
       ISC_TIME *result = (ISC_TIME *) MALLOC(sizeof(ISC_TIME));
	     struct tm t;
	     int minutes;
    
#ifdef HAVE_MATH_FLOOR
	     t.tm_hour = (int) floor(*indtime);
       minutes = (int) floor(60 * (*indtime - t.tm_hour));
#else
	     t.tm_hour = (int) trunc(*indtime);
       minutes = (int) trunc(60 * (*indtime - t.tm_hour));
#endif
	     t.tm_min  = (int) intern_zahlrunden(60 * (*indtime - t.tm_hour),0);
	     t.tm_sec  = (int) intern_zahlrunden(60 *((60 * (*indtime - t.tm_hour)) - minutes),0);
       if(t.tm_sec > 29)
          t.tm_sec = 60;
       else
          t.tm_sec = 0;          
       t.tm_mday = t.tm_mon = t.tm_year = t.tm_wday = t.tm_yday = t.tm_isdst = 0;
	     isc_encode_sql_time(&t, result);
  
	     return result;
    }
}

// ct 2010-11-22
// consumption: km per 1 litre to litre per 100km
double *kmpl2lp100km(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 100.0;
        return result;
    }
}

// ct 2010-11-22
// consumption: liter per 100km to km per 1 litre
double *lp100km2kmpl(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 100.0;
        return result;
    }
}
// ct 2010-11-22
// consumption: miles per 1 litre to litre per 100km
double *mpl2lp100km(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x * 62.13711922;
        return result;
    }
}

// ct 2010-11-22
// consumption: liter per 100km to miles per 1 litre
double *lp100km2mpl(double *x)
{
    if (!x)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = *x / 62.13711922;
        return result;
    }
}
