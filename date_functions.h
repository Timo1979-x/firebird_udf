/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring / Peter Mandrella / Georg Horn
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 * some code from Robert Loipfinger rl copyright (c) 2000 ADITO Software GmbH
 *
 * File Name:	date_functions.h
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

#ifndef isleap
#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)
#endif

// internal functions
ISC_QUAD * gen_ib_date(struct tm *t);
int        decode_language(char *);
char     * intern_getlang();
// exported functions
// calculate
ISC_QUAD * addyear (ISC_QUAD *, int *);
ISC_QUAD * addmonth (ISC_QUAD *, int *);
ISC_QUAD * addweek (ISC_QUAD *, int *);
ISC_QUAD * addday (ISC_QUAD *, int *);
ISC_QUAD * addhour (ISC_QUAD *, int *);
ISC_QUAD * addminute (ISC_QUAD *, int *);
ISC_QUAD * addsecond (ISC_QUAD *, int *);
ISC_QUAD * incdate (ISC_QUAD *, int *, int *, int *);
ISC_QUAD * incdatetime (ISC_QUAD *, int *, int *, int *, int *, int *, int *);
char     * addperiod(char *, char *);
char     * subperiod(char *, char *);
// determin
int      * ageinyears (ISC_QUAD *, ISC_QUAD *);
int      * ageinyearsnull (ISC_QUAD *, ISC_QUAD *);
int      * ageinyearsthreshold (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinyearsthresholdnull (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinmonths (ISC_QUAD *, ISC_QUAD *);
int      * ageinmonthsnull (ISC_QUAD *, ISC_QUAD *);
int      * ageinmonthsthreshold (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinmonthsthresholdnull (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinweeks (ISC_QUAD *, ISC_QUAD *);
int      * ageinweeksiso (ISC_QUAD *, ISC_QUAD *);
int      * ageinweeksnull (ISC_QUAD *, ISC_QUAD *);
int      * ageinweeksnulliso (ISC_QUAD *, ISC_QUAD *);
int      * ageinweeksthreshold (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinweeksthresholdiso (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinweeksthresholdnull (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinweeksthresholdnulliso (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageindays (ISC_QUAD *, ISC_QUAD *);
int      * ageindaysnull (ISC_QUAD *, ISC_QUAD *);
int      * ageindaysthreshold (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageindaysthresholdnull (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinworkingdays (ISC_QUAD *, ISC_QUAD *, int *);
int      * ageinworkingdaysthreshold(ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *, int *);
int      * ageinhours (ISC_QUAD *, ISC_QUAD *);
int      * ageinhoursnull (ISC_QUAD *, ISC_QUAD *);
int      * ageinhoursthreshold (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinhoursthresholdnull (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinminutes (ISC_QUAD *, ISC_QUAD *);
int      * ageinminutesnull (ISC_QUAD *, ISC_QUAD *);
int      * ageinminutesthreshold (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinminutesthresholdnull (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinseconds (ISC_QUAD *, ISC_QUAD *);
int      * ageinsecondsnull (ISC_QUAD *, ISC_QUAD *);
int      * ageinsecondsthreshold (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * ageinsecondsthresholdnull (ISC_QUAD *, ISC_QUAD *, int *, int *, int *, int *);
int      * yearsbetween (ISC_QUAD *, ISC_QUAD *);
int      * monthsbetween (ISC_QUAD *, ISC_QUAD *);
int      * weeksbetween (ISC_QUAD *, ISC_QUAD *);
int      * weeksbetweeniso (ISC_QUAD *, ISC_QUAD *);
int      * daysbetween (ISC_QUAD *, ISC_QUAD *);
int      * workingdaysbetween (ISC_QUAD *, ISC_QUAD *, int *);
int      * hoursbetween (ISC_QUAD *, ISC_QUAD *);
int      * minutesbetween (ISC_QUAD *, ISC_QUAD *);
int      * secondsbetween (ISC_QUAD *, ISC_QUAD *);
int      * indminutesbetween(ISC_QUAD *, ISC_QUAD *);
int      * indminutesbetweenthr(ISC_QUAD *, ISC_QUAD *, ISC_TIME *, int *, ISC_TIME *, int *);
int      * indminutesbetweenthrib(ISC_QUAD *, ISC_QUAD *, ISC_QUAD *, int *, ISC_QUAD *, int *);
int      * dayofyear (ISC_QUAD *);
int      * r_yearday(ISC_QUAD *);
int      * dayofmonth (ISC_QUAD *);
int      * dayofweek (ISC_QUAD *);
int      * r_weekday(ISC_QUAD *);
int      * r_dow(ISC_QUAD *);
int      * daysofmonth (int *, int *);
int      * lastday (int *, int *);
ISC_QUAD * firstdayquarter(ISC_QUAD *);
ISC_QUAD * lastdayquarter(ISC_QUAD *);
ISC_QUAD * firstdaymonth(ISC_QUAD *);
ISC_QUAD * lastdaymonth(ISC_QUAD *);
ISC_QUAD * firstdayweek(ISC_QUAD *);
ISC_QUAD * lastdayweek(ISC_QUAD *);
ISC_QUAD * firstdayweekiso(ISC_QUAD *);
ISC_QUAD * lastdayweekiso(ISC_QUAD *);
ISC_QUAD * firstdaywoy(int *, int *);
ISC_QUAD * lastdaywoy(int *, int *);
ISC_QUAD * firstdaykw(int *, int *);
ISC_QUAD * lastdaykw(int *, int *);
ISC_QUAD * firstdaylastquarter(ISC_QUAD *);
ISC_QUAD * lastdaylastquarter(ISC_QUAD *);
ISC_QUAD * firstdaynextquarter(ISC_QUAD *);
ISC_QUAD * lastdaynextquarter(ISC_QUAD *);
ISC_QUAD * firstdaylastmonth(ISC_QUAD *);
ISC_QUAD * lastdaylastmonth(ISC_QUAD *);
ISC_QUAD * firstdaynextmonth(ISC_QUAD *);
ISC_QUAD * lastdaynextmonth(ISC_QUAD *);
ISC_QUAD * firstdaylastweek(ISC_QUAD *);
ISC_QUAD * firstdaylastweekiso(ISC_QUAD *);
ISC_QUAD * lastdaylastweek(ISC_QUAD *);
ISC_QUAD * lastdaylastweekiso(ISC_QUAD *);
ISC_QUAD * firstdaynextweek(ISC_QUAD *);
ISC_QUAD * firstdaynextweekiso(ISC_QUAD *);
ISC_QUAD * lastdaynextweek(ISC_QUAD *);
ISC_QUAD * lastdaynextweekiso(ISC_QUAD *);
int      * dtime (ISC_QUAD *);
int      * isleapyear (ISC_QUAD *);
int      * r_isleapyear(int *);
double   * zeitdifferenz (ISC_QUAD *, ISC_QUAD *, char *);
int      * countweekdays(ISC_QUAD *, ISC_QUAD *, int *);
int      * year (ISC_QUAD *);
int      * yearofyear (ISC_QUAD *);
int      * quarter (ISC_QUAD *);
int      * month (ISC_QUAD *);
int      * week (ISC_QUAD *);
int      * weekofyear (ISC_QUAD *);
char     * woy (ISC_QUAD *);
char     * woyiso(ISC_QUAD *);
int      * kalenderwoche(ISC_QUAD *);
int      * hour (ISC_QUAD *);
int      * minute (ISC_QUAD *);
int      * second (ISC_QUAD *);
int      * millisecond (ISC_QUAD *);
double   * tzvalue_to_utc();
ISC_QUAD * local2utc(ISC_QUAD *);
ISC_QUAD * utc2local(ISC_QUAD *);
// format
char     * cmonthlong (ISC_QUAD *);
char     * cmonthlonglang (ISC_QUAD *, char *);
char     * cmonthshort (ISC_QUAD *);
char     * cmonthshortlang (ISC_QUAD *, char *);
char     * cdowlong (ISC_QUAD *);
char     * cdowlonglang (ISC_QUAD *, char *);
char     * cdowshort (ISC_QUAD *);
char     * cdowshortlang (ISC_QUAD *, char *);
char     * gformatd (char *, ISC_QUAD *);
char     * datetostr(ISC_QUAD *, char *);
char     * datetostrlang(ISC_QUAD *, char *, char *);
ISC_DATE * encodedate (int *, int *, int *);
ISC_QUAD * r_encodedate(int *, int *, int *);
ISC_DATE * encodedatenull (int *, int *, int *);
ISC_TIME * encodetime (int *, int *, int *);
ISC_TIME * encodetimenull (int *, int *, int *);
ISC_QUAD * encodetimestamp (int *, int *, int *, int *, int *, int *);
ISC_QUAD * r_encodedatetime(int *, int *, int *, int *, int *, int *);
ISC_QUAD * stripdate (ISC_QUAD *);
ISC_QUAD * striptime (ISC_QUAD *);
// compare
ISC_QUAD * maxdate (ISC_QUAD *, ISC_QUAD *);
ISC_QUAD * mindate (ISC_QUAD *, ISC_QUAD *);
int      * equaldate (ISC_QUAD *, ISC_QUAD *);
int      * equaldatetime (ISC_QUAD *, ISC_QUAD *);
// convert
ISC_TIME * strtotime (char *);
double   * datetodouble(ISC_QUAD *);
double   * timetodouble(ISC_QUAD *);
ISC_QUAD * doubletodate(double *);
ISC_QUAD * doubletotime(double *);
int      * period2seconds(char *);
char     * seconds2period(int *);
// movable feasts
ISC_QUAD * osterdatum (int *);
ISC_QUAD * weiberfastnacht(int *);
ISC_QUAD * rosenmontag(int *);
ISC_QUAD * karnevalsdienstag(int *);
ISC_QUAD * aschermittwoch(int *);
ISC_QUAD * palmsonntag(int *);
ISC_QUAD * gruendonnerstag(int *);
ISC_QUAD * karfreitag(int *);
ISC_QUAD * ostersamstag(int *);
ISC_QUAD * ostermontag(int *);
ISC_QUAD * himmelfahrt(int *);
ISC_QUAD * pfingstsonntag(int *);
ISC_QUAD * pfingstmontag(int *);
ISC_QUAD * fronleichnam(int *);

