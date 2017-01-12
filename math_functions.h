/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring / Peter Mandrella / Georg Horn
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 *
 * File Name:	math_functions.h
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
double   intern_zahlrunden(double, int);
// exported functions
// ceate
double * f_rand();
int    * intrandom (int *, int *);
int    * getrandom(int *);
double * doublerandom (int *, int *);
// format
int    * f_round (double *);
double * roundfloat(double *, double *);
double * zahlrunden (double *, int *);
double * roundtoeven (double *, int *);
double * softround (double *, int *);
double * rappenrundung(double *);
char   * fixedpoint (double *, int *);
char   * fixedpointlang (double *, int *, char *, char *);
int    * f_truncate (double *);
char   * dollarval (double *);
char   * euroval (double *);
char   * numinwords(double *, int *, char *);
char   * inputval(double *, char *);
// calculate
double * doubleabs (double *);
int    * integerabs (int *);
double * f_power( double *, double *);
double * f_exp( double *);
double * f_sqrt( double *);
double * fact (int *);
int    * modulo (int *, int *);
int    * r_div(int *, int *);
double * f_floor( double *);
double * f_ceiling( double *);
double * f_frac( double *);
double * f_ln( double *);
double * f_log( double *, double *);
double * f_log10( double *);
int    * isdivisibleby (int*, int*);
int    * evenodd (int *);
int    * f_sign( double *);
double * prozente (double *, double *);
double * r_z(double *);
double * r_dividezero(double *, double *, double *);
// compare
double * maxnum (double *, double *);
double * minnum (double *, double *);
int    * equalfloat (double *, double *);
int    * equalinteger (int *, int *);
// binary
int    * f_bin_and(int *, int *);
int    * f_bin_or(int *, int *);
int    * f_bin_xor(int *, int *);
int    * f_bin_not(int *);
int    * f_bin_shl(int *, int *);
int    * f_bin_shr(int *, int *);
int    * r_getbit(int *, int *);
int    * r_setbit(int *, int *, int *);
// trigonometry 
double * sin_rad (double *);
double * cos_rad (double *);
double * tan_rad (double *);
double * cot_rad (double *);
double * arcsin_rad (double *);
double * arccos_rad (double *);
double * arctan_rad (double *);
double * arctan2_rad (double *, double *);
double * arccot_rad (double *);
double * sin_deg (double *);
double * cos_deg (double *);
double * tan_deg (double *);
double * cot_deg (double *);
double * arcsin_deg (double *);
double * arccos_deg (double *);
double * arctan_deg (double *);
double * arccot_deg (double *);
double * sinhyp (double *);
double * coshyp (double *);
double * tanhyp (double *);
double * cothyp (double *);
double * arsinh (double *);
double * arcosh (double *);
double * artanh (double *);
double * arcoth (double *);
