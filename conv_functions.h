/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring / Peter Mandrella / Georg Horn
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 *
 * File Name:	conv_functions.h
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
 
// exported functions
// longitudinal
double   * p2mm (double *);
double   * in2m (double *);
double   * li2m(double *);
double   * ft2m (double *);
double   * yd2m (double *);
double   * fm2m (double *);
double   * rd2m(double *);
double   * ch2m(double *);
double   * fur2m(double *);
double   * lea2km(double *);
double   * mile2km (double *);
double   * mm2p (double *);
double   * m2in (double *);
double   * m2li(double *);
double   * m2ft (double *);
double   * m2yd (double *);
double   * m2fm (double *);
double   * m2rd(double *);
double   * m2ch(double *);
double   * m2fur(double *);
double   * km2lea(double *);
double   * km2mile (double *);
// area
double   * ar2sqm (double *);
double   * ha2sqm (double *);
double   * morgen2sqm (double *);
double   * sqft2sqm (double *);
double   * ac2sqm (double *);
double   * sqyd2sqm (double *);
double   * sqm2ar (double *);
double   * sqm2ha (double *);
double   * sqm2morgen (double *);
double   * sqm2sqft (double *);
double   * sqm2ac (double *);
double   * sqm2sqyd (double *);
// volume
double   * cuin2cum(double *);
double   * cuft2cum(double *);
double   * cuyd2cum(double *);
double   * cup2ml(double *);
double   * gbfloz2ml (double *);
double   * usfloz2ml (double *);
double   * gbpt2l (double *);
double   * uspt2l (double *);
double   * gbqt2l (double *);
double   * usqt2l (double *);
double   * gbgal2l (double *);
double   * usgal2l (double *);
double   * bbl2l (double *);
double   * gbbu2l(double *);
double   * usbu2l(double *);
double   * cum2cuin(double *);
double   * cum2cuft(double *);
double   * cum2cuyd(double *);
double   * ml2cup(double *);
double   * ml2gbfloz (double *);
double   * ml2usfloz (double *);
double   * l2gbpt (double *);
double   * l2uspt (double *);
double   * l2gbqt (double *);
double   * l2usqt (double *);
double   * l2gbgal (double *);
double   * l2usgal (double *);
double   * l2bbl (double *);
double   * l2gbbu(double *);
double   * l2usbu(double *);
// weight
double   * ct2g (double *);
double   * oz2g (double *);
double   * oztr2g (double *);
double   * cwt2g(double *);
double   * st2g(double *);
double   * obu2g(double *);
double   * dra2g(double *);
double   * lb2kg (double *);
double   * g2ct (double *);
double   * g2oz (double *);
double   * g2oztr (double *);
double   * g2cwt(double *);
double   * g2st(double *);
double   * g2obu(double *);
double   * g2dra(double *);
double   * kg2lb (double *);
// temperature
double   * c2f (double *);
double   * f2c (double *);
// date/time
double   * time2dectime (ISC_QUAD *);
ISC_TIME * dectime2time(double *);
double   * time2indtime (ISC_QUAD *);
ISC_TIME * indtime2time(double *);
// technical
double   * g2t (double *);
double   * t2g (double *);
double   * bar2torr (double *);
double   * bar2pa (double *);
double   * psi2pa (double *);
double   * torr2pa (double *);
double   * torr2bar (double *);
double   * pa2bar (double *);
double   * pa2psi (double *);
double   * pa2torr (double *);
double   * cal2j (double *);
double   * j2cal (double *);
double   * lbft2nm(double *);
double   * nm2lbft(double *);
double   * ps2kw (double *);
double   * bhp2kw (double *);
double   * kw2ps (double *);
double   * kw2bhp (double *);
double   * mph2kmph(double *);
double   * kmph2mph(double *);
double   * fps2mps(double *);
double   * mps2fps(double *);
double   * mpusgal2lp100km(double *);
double   * mpgbgal2lp100km(double *);
double   * kmpl2lp100km(double *);
double   * lp100km2kmpl(double *);
double   * mpl2lp100km(double *);
double   * lp100km2mpl(double *);
// nautical
double   * shot2m(double *);
double   * m2shot(double *);
double   * cbl2m(double *);
double   * m2cbl(double *);
double   * nl2m(double *);
double   * m2nl(double *);
double   * nm2m(double *);
double   * m2nm(double *);
double   * sm2km (double *);
double   * km2sm (double *);
double   * smph2kmph(double *);
double   * kmph2smph(double *);
double   * rt2cbm (double *);
double   * cbm2rt (double *);
// number systems
int      * roman2arabic(char *);
char     * arabic2roman (int *);
int      * convertfrom33 (char *);
char     * convertto33 (int *);
int      * convertfrombase (char *, int *, char *);
char     * converttobase (int *, int *, char *);
char     * inttohex (int *, int *);
int      * hextoint (char *);
// geometrical
double   * degtorad (double *);
double   * radtodeg (double *);
