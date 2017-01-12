/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz gd
 * some code from Robert Loipfinger rl copyright (c) 2000 ADITO Software GmbH
 *
 * File Name:   date_functions.c
 * Description: this module contains UDFs to generate and manipulate date/time
 * for exported functions list see date_functions.h
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
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <locale.h>

#if defined(__BORLANDC__) || defined(_MSC_VER) || defined(__WATCOMC__)
#include <windows.h>
#include <winnt.h>
#endif

#include "global.h"
#include "math_functions.h"
#include "date_functions.h"
#include "string_functions.h"

static void i_week_of_year (ISC_QUAD *, int *, int *);

#define SUNDAY 0
#define MONDAY 1
#define TUESDAY 2
#define WEDNESDAY 3
#define THURSDAY 4
#define FRIDAY 5
#define SATURDAY 6

#define JANUARY 0
#define FEBRUARY 1
#define MARCH 2
#define APRIL 3
#define MAY 4
#define JUNE 5
#define JULY 6
#define AUGUST 7
#define SEPTEMBER 8
#define OCTOBER 9
#define NOVEMBER 10
#define DECEMBER 11

// 2007-07-22 ct add more languages
// uk de fr it es pt nl no se dk fi hu ie ee is al va v1 c1 s1 s2 w1 b1 b2 n1 za fo lu w2
static char *DOW[] =
{
 "Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday",
 "Sonntag","Montag","Dienstag","Mittwoch","Donnerstag","Freitag","Samstag",
 "Dimanche","Lundi","Mardi","Mercredi","Jeudi","Vendredi","Samedi",
 "Domenica","Lunedì","Martedì","Mercoledì","Giovedì","Venerdì","Sabato",
 "Domingo","Lunes","Martes","Miércoles","Jueves","Viernes","Sábado",
 "Segunda-feira", "Terça-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "Sábado", "Domingo",
 "Zondag", "Maandag", "Dinsdag", "Woensdag", "Donderdag", "Vrijdag", "Zaterdag",
 "Søndag", "Mandag", "Tirsdag", "Onsdag", "Torsdag", "Fredag", "Lørdag",
 "Söndag", "Måndag", "Tisdag", "Onsdag", "Torsdag", "Fredag", "Lördag",
 "Søndag", "Mandag", "Tirsdag", "Onsdag", "Torsdag", "Fredag", "Lørdag",
 "Sunnuntai", "Maanantai", "Tiistai", "Keskiviikko", "Torstai", "Perjantai", "Lauantai",
 "Vasárnap", "Hétfõ", "Kedd", "Szerda", "Csütörtök", "Péntek", "Szombat",
 "Dé Domhnaigh", "Dé Luain", "Dé Máirt", "Dé Céadaoin", "Déardaoin", "Dé hAoine", "Dé Sathairn",
 "Pühapäev", "Esmaspäev", "Teisipäev", "Kolmapäev", "Neljapäev", "Reede", "Laupäev",
 "Sunnudagur", "Mánudagur", "Þriðjudagur", "Miðvikudagur", "Fimmtudagur", "Föstudagur", "Laugardagur",
 "e diel", "e hënë", "e martë", "e mërkurë", "e enjte", "e premte", "e shtunë",
 "Dies Solis", "Dies Lunae", "Dies Martis", "Dies Mercurii", "Dies Jovis", "Dies Veneris", "Dies Saturni",
 "Dominica", "Feria Secunda", "Feria Tertia", "Feria Quarta", "Feria Quinta", "Feria Sexta", "Sabbatum",
 "Diumenge", "Dilluns", "Dimarts", "Dimecres", "Dijous", "Divendres", "Dissabte",
 "Sunday", "Monanday", "Tysday", "Wadensday", "Fuirsday", "Friday", "Seturday",
 "Di-Dòmhnaich", "Di-Luain", "Di-Màirt", "Di-Ciadain", "Di-Ardaoin", "Di-Haoine", "Di-Sàthairne",
 "Dydd Sul", "Dydd Llun", "Dydd Mawrth", "Dydd Mercher", "Dydd Iau", "Dydd Gwener", "Dydd Sadwrn",
 "Disul", "Dilun", "Dimeurzh", "Dimerc’her", "Diriaou", "Digwener", "Disadorn",
 "igandea", "astelehena", "asteartea", "asteazkena", "osteguna", "ostirala", "larunbata",
 "Sundag", "Mandag", "Tysdag", "Onsdag", "Forsdag", "Fredag", "Laurdag",
 "Sondag", "Maandag", "Dinsdag", "Woensdag", "Donderdag", "Vrydag", "Saterdag",
 "sunnudagur", "mánadagur", "týsdagur", "mikudagur", "hósdagur", "fríggjadagur", "leygardagur",
 "Sonndeg", "Méindeg", "Dënschdeg", "Mëttwoch", "Donneschdeg", "Freideg", "Samschdeg",
 "dimenge", "londi", "mårdi", "mierkidi", "djudi", "vénrdi", "semdi"
};

// 2007-07-22 ct
// 2008-02-05 ct corrects se and fi
// uk de fr it es pt nl no se dk fi hu ie ee is al va v1 c1 s1 s2 w1 b1 b2 n1 za fo lu w2
static char *DOWSHORT[] =
{
 "Sun","Mon","Tue","Wed","Thu","Fri","Sat",
 "So.","Mo.","Di.","Mi.","Do.","Fr.","Sa.",
 "Dim","Lun","Mar","Mer","Jeu","Ven","Sam",
 "Dom","Lun","Mar","Mer","Gio","Ven","Sab",
 "Dom","Lun","Mar","Miér","Jue","Viér","Sáb",
 "Dom","2ª", "3ª", "4ª", "5ª", "6ª", "Sáb",
 "Zondag", "Maandag", "Dinsdag", "Woensdag", "Donderdag", "Vrijdag", "Zaterdag",
 "Søndag", "Mandag", "Tirsdag", "Onsdag", "Torsdag", "Fredag", "Lørdag",
 "Sö", "Må", "Ti", "On", "To", "Fr", "Lö",
 "Søn", "Man", "Tir", "Ons", "Tor", "Fre", "Lør",
 "Su", "Ma", "Ti", "Ke", "To", "Pe", "La",
 "Vasárnap", "Hétfõ", "Kedd", "Szerda", "Csütörtök", "Péntek", "Szombat",
 "Dé Domhnaigh", "Dé Luain", "Dé Máirt", "Dé Céadaoin", "Déardaoin", "Dé hAoine", "Dé Sathairn",
 "Pühapäev", "Esmaspäev", "Teisipäev", "Kolmapäev", "Neljapäev", "Reede", "Laupäev",
 "Sunnudagur", "Mánudagur", "Þriðjudagur", "Miðvikudagur", "Fimmtudagur", "Föstudagur", "Laugardagur",
 "e diel", "e hënë", "e martë", "e mërkurë", "e enjte", "e premte", "e shtunë",
 "Dies Solis", "Dies Lunae", "Dies Martis", "Dies Mercurii", "Dies Jovis", "Dies Veneris", "Dies Saturni",
 "Dominica", "Feria Secunda", "Feria Tertia", "Feria Quarta", "Feria Quinta", "Feria Sexta", "Sabbatum",
 "Diumenge", "Dilluns", "Dimarts", "Dimecres", "Dijous", "Divendres", "Dissabte",
 "Sunday", "Monanday", "Tysday", "Wadensday", "Fuirsday", "Friday", "Seturday",
 "Di-Dòmhnaich", "Di-Luain", "Di-Màirt", "Di-Ciadain", "Di-Ardaoin", "Di-Haoine", "Di-Sàthairne",
 "Dydd Sul", "Dydd Llun", "Dydd Mawrth", "Dydd Mercher", "Dydd Iau", "Dydd Gwener", "Dydd Sadwrn",
 "Disul", "Dilun", "Dimeurzh", "Dimerc’her", "Diriaou", "Digwener", "Disadorn",
 "igandea", "astelehena", "asteartea", "asteazkena", "osteguna", "ostirala", "larunbata",
 "Sundag", "Mandag", "Tysdag", "Onsdag", "Forsdag", "Fredag", "Laurdag",
 "Sondag", "Maandag", "Dinsdag", "Woensdag", "Donderdag", "Vrydag", "Saterdag",
 "sunnudagur", "mánadagur", "týsdagur", "mikudagur", "hósdagur", "fríggjadagur", "leygardagur",
 "Sonndeg", "Méindeg", "Dënschdeg", "Mëttwoch", "Donneschdeg", "Freideg", "Samschdeg",
 "dimenge", "londi", "mårdi", "mierkidi", "djudi", "vénrdi", "semdi"
};

// 2007-07-22 ct add more languages
// uk de fr it es pt nl no se dk fi hu ie ee is al va v1 c1 s1 s2 w1 b1 b2 n1 za fo lu w2
static char *MONTH[] =
{
 "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December",
 "Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember",
 "Janvier", "Février", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre",
 "Gennaio", "Febbraio", "Marzo", "Aprile", "Maggio", "Guigno", "Luglio", "Agosto", "Settembre", "Ottobre", "Novembre", "Dicembre",
 "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Setiembre", "Octubre", "Noviembre", "Diciembre",
 "Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro",
 "Januari", "Februari", "Maart", "April", "Mei", "Juni", "Juli", "Augustus", "September", "October", "November", "December",
 "Januar", "Februar", "Mars", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Desember",
 "Januari", "Februari", "Mars", "April", "Maj", "Juni", "Juli", "Augusti", "September", "Oktober", "November", "December",
 "Januar", "Februar", "Marts", "April", "Maj", "Juni", "Juli", "August", "September", "Oktober", "November", "December",
 "Tammikuu", "Helmikuu", "Maaliskuu", "Huhtikuu", "Toukokuu", "Kesäkuu", "Heinäkuu", "Elokuu", "Syyskuu", "Lokakuu", "Marraskuu", "Joulukuu",
 "Január", "Február", "Március", "Április", "Május", "Június", "Július", "Augusztus", "Szeptember", "Október", "November", "December",
 "Enáir", "Feabhra", "An Márta", "An t-Aibreán", "An Bhealtaine", "An Meitheamh", "Iúil", "Lúnasa", "Meán Fómhair", "Deireadh Fómhair", "Samhain", "Nollaig",
 "Jaanuar", "Veebruar", "Märts", "Aprill", "Mai", "Juuni", "Juuli", "August", "September", "Oktoober", "November", "Detsember",
 "janúar", "febrúar", "mars", "apríl", "maí", "júní", "júlí", "ágúst", "september", "október", "nóvember", "desember",
 "janar", "shkurt", "mars", "prill", "maj", "qershor", "korrik", "gusht", "shtator", "tetor", "nëntor", "dhjetor",
 "Ianuarius", "Februarius", "Martius", "Aprilis", "Maius", "Iunius", "Iulius", "Augustus", "September", "October", "November", "December",
 "Ianuarius", "Februarius", "Martius", "Aprilis", "Maius", "Iunius", "Iulius", "Augustus", "September", "October", "November", "December",
 "Gener", "Febrer", "Març", "Abril", "Maig", "Juny", "Juliol", "Agost", "Setembre", "Octubre", "Novembre", "Desembre",
 "Januar", "Februar", "Mairch", "Apryle", "Mey", "Juin", "Julie", "Augist", "September", "October", "November", "Dizember",
 "an Faoilteach", "an Gearran", "an Márt", "an Giblean", "an Ceitean", "an t-Òg-mhios", "an t-luchar", "an Lúnasdal", "an t-Sultain", "an Dàmhair", "an t-Samhain", "an Dúbhlachd",
 "Ionawr", "Chwefror", "Mawrth", "Ebrill", "Mai", "Mehefin", "Gorffennaf", "Awst", "Medi", "Hydref", "Tachwedd", "Rhagfyr",
 "Genver", "C'hwevrer", "Meurzh", "Ebrel", "Mae", "Mezheven", "Gouere", "Eost", "Gwengolo", "Here", "Miz Du", "Kerzu",
 "urtarrila", "otsaila", "martxoa", "apirila", "maiatza", "ekaina", "uztaila", "abuztua", "iraila", "urria", "azaroa", "abendua",
 "januar", "februar", "mars", "april", "mai", "juni", "juli", "august", "september", "oktober", "november", "desember",
 "Januarie", "Februarie", "Maart", "April", "Mei", "Junie", "Julie", "Augustus", "September", "Oktober", "November", "Desember",
 "januar", "februar", "mars", "apríl", "mai", "juni", "juli", "august", "september", "oktober", "november", "desember",
 "Januar", "Februar", "Mäerz", "Abrëll", "Mee", "Juni", "Juli", "August", "September", "Oktouber", "November", "Dezember",
 "djanvî", "fevrî", "måss", "avri", "may", "djun", "djulete", "awousse", "setimbe", "octôbe", "nôvimbe", "decimbe"
};

// 2007-07-22 ct add more languages and delimiter
const char* LANG_INDEX = "uk-de-fr-it-es-pt-nl-no-se-dk-fi-hu-ie-ee-is-al-va-v1-c1-s1-s2-w1-b1-b2-n1-za-fo-lu-w2-";

// ct 2006-11-23 implementation of NULL for FB 2.0
// ct 2008-02-12 correction
ISC_QUAD * addyear (ISC_QUAD *date, int *years_to_add)
{
  if (!date || !years_to_add)
    return NULL;
  else
  {
    struct tm t;
    isc_decode_date(date, &t);
    t.tm_year += *years_to_add;
    // If it's February, and the day of the month is 29, and
    // it's *not* a leap year, then adjust the day to 1st march
    if (t.tm_mon == FEBRUARY && t.tm_mday > 28 && !isleap(t.tm_year+1900)) {
      t.tm_mday = 1;
      t.tm_mon = MARCH;
    }
    return (ISC_QUAD *) gen_ib_date(&t);
  }
}

// pm 2006-05-16 Korrektur
// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_QUAD * addmonth (ISC_QUAD * date, int * months_to_add)
{
  if (!date || !months_to_add)
    return NULL;
  else
  {
    struct tm t;
    int tm_mon, tm_year;
    isc_decode_date (date, &t);
    tm_mon = t.tm_mon + 12 * t.tm_year + *months_to_add;
    tm_year = tm_mon / 12;
    tm_mon %= 12;
    // Now, check that the month day is correctly adjusted.
    // 30 days has Sep, Apr, Jun, Nov all the rest have 31 except Feb
    if (tm_mon != FEBRUARY) {
      if (t.tm_mday > 30) {
        switch (tm_mon) {
        case APRIL:
        case JUNE:
        case SEPTEMBER:
        case NOVEMBER:
          t.tm_mday = 30;
        }
      }
    }
    else if (t.tm_mday > 28)
      t.tm_mday = isleap(tm_year+1900) ? 29 : 28;

    t.tm_mon = tm_mon;
    t.tm_year = tm_year;

    return (ISC_QUAD *) gen_ib_date(&t);
  }
}

// intern: return days of month m in year y
static int intern_daysofmonth(int m, int y)
{
    if (m == 2) {
        if (!isleap(y)) {
            return 28;
        } else {
            return 29;
        }
    }
    if ((m == 4) || (m == 6) || (m == 9) || (m == 11)) {
        return 30;
    }
    return 31;
}

// pm 2005-10-16
// return days of month stored in t
static int daysoftsmonth(struct tm *t)
{
  int month = t->tm_mon + 1;
  int year = t->tm_year + 1900;
  return intern_daysofmonth(month, year);
}

// intern
// pm 2005-10-16
// pm 2006-02-24 bugfix
// ct 2007-11-28
// ct 2008-03-03 bugfix for negativ input-values
static ISC_QUAD * addseconds (ISC_QUAD *date, int seconds)
{
    struct tm t;
    isc_decode_date(date,&t);
    t.tm_sec += seconds;
    if (t.tm_sec >= 60) {
      t.tm_min  += (t.tm_sec / 60);
      t.tm_sec %= 60;
      t.tm_hour += (t.tm_min / 60);
      t.tm_min %= 60;
      t.tm_mday += (t.tm_hour / 24);
      t.tm_hour %= 24;
      while (t.tm_mday > daysoftsmonth(&t)) {
        t.tm_mday -= daysoftsmonth(&t);
        t.tm_mon++;
        if (t.tm_mon > 11) {
          t.tm_mon=0;
          t.tm_year++;
        }
      }
    }
    else if (t.tm_sec < 0) {
      t.tm_min += ((t.tm_sec - 59) / 60);
      if (t.tm_sec % 60 == 0) {
        t.tm_sec = (t.tm_sec % 60);
      }
      else {
        t.tm_sec = (t.tm_sec % 60) + 60;
      }
      if (t.tm_min < 0) {
        t.tm_hour += ((t.tm_min - 59) / 60);
        if (t.tm_min % 60 == 0) {
          t.tm_min = (t.tm_min % 60);
        }
        else {
          t.tm_min = (t.tm_min % 60) + 60;
        }
        if (t.tm_hour < 0) {
          t.tm_mday += ((t.tm_hour - 23) / 24);
          if (t.tm_hour % 24 == 0) {
            t.tm_hour = (t.tm_hour % 24);
          }
          else {
            t.tm_hour = (t.tm_hour % 24) + 24;
          }
          while (t.tm_mday < 1) {
            t.tm_mon--;
            if (t.tm_mon < 0) {
              t.tm_mon = 11;
              t.tm_year--;
            }
            t.tm_mday += daysoftsmonth(&t);
          }
        }
      }
    }
    return (ISC_QUAD *) gen_ib_date(&t);
}

// pm 2005-10-16
// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_QUAD * addweek (ISC_QUAD *date, int *weeks)
{
  if (!date || !weeks)
    return NULL;
  else
    return addseconds(date, 7 * 24 * 60 * 60 * *weeks);
}

// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_QUAD * addday (ISC_QUAD *date, int *days)
{
  if (!date || !days)
    return NULL;
  else
    return addseconds(date, 24 * 60 * 60 * *days);
}

// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_QUAD * addhour (ISC_QUAD *date, int *hours)
{
  if (!date || !hours)
    return NULL;
  else
    return addseconds(date, 60 * 60 * *hours);
}

// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_QUAD * addminute (ISC_QUAD *date, int *minutes)
{
  if (!date || !minutes)
    return NULL;
  else
    return addseconds(date, 60 * *minutes);
}

// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_QUAD * addsecond (ISC_QUAD *date, int *seconds)
{
  if (!date || !seconds)
    return NULL;
  else
    return addseconds(date, *seconds);
}

// intern: calculate age in years between date1 and date2
// pm 2005-10-xx
// gh 2006-xx-xx made from ageinyears
// ct 2009-03-13 check of full year
static int intern_ageinyears(ISC_QUAD * date1, ISC_QUAD * date2)
{
    struct tm t1, t2;
    int res;

    isc_decode_date(date2, &t2);
    isc_decode_date(date1, &t1);
    res = t2.tm_year - t1.tm_year;
    if (res < 0) {
	if (t1.tm_mon < t2.tm_mon
	    || (t1.tm_mon == t2.tm_mon && t1.tm_mday < t2.tm_mday)
	    || (t1.tm_mon == t2.tm_mon && t1.tm_mday == t2.tm_mday
		&& (t1.tm_hour * 3600 + t1.tm_min * 60 + t1.tm_sec) <
		(t2.tm_hour * 3600 + t2.tm_min * 60 + t2.tm_sec)))
	    /* GH: Muss hier nicht res + 1 hin? */
	    return res - 1;
	else
	    return res;
    } else {
	if (t2.tm_mon < t1.tm_mon
	    || (t2.tm_mon == t1.tm_mon && t2.tm_mday < t1.tm_mday)
	    || (t2.tm_mon == t1.tm_mon && t2.tm_mday == t1.tm_mday
		&& (t2.tm_hour * 3600 + t2.tm_min * 60 + t2.tm_sec) <
		(t1.tm_hour * 3600 + t1.tm_min * 60 + t1.tm_sec)))
	    return res - 1;
	else
	    return res;
    }
}

// return also negative ages
// pm 2005-10-xx
int *ageinyears(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = intern_ageinyears(date1, date2);
        return result;
    }
}

// return null instead of negativ ages
int *ageinyearsnull(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int res = intern_ageinyears(date1, date2);
        if (res < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = res;
            return result;
        }
    }
}

// intern: calculate age in years with min and max threshold
static int intern_ageinyearsthreshold(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    int age = intern_ageinyears(date1, date2);
    if (*usemin == 1 && age < *min) {
        age = *min;
    } else if (*usemax == 1 && age > *max) {
        age = *max;
    }
    return age;
}

//  return age in years with min and max threshold
int *ageinyearsthreshold(ISC_QUAD * date1, ISC_QUAD * date2, int *min,
    int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = intern_ageinyearsthreshold(date1, date2, min, usemin, max, usemax);
        return result;
    }
}

//  return null instead of negativ age
int *ageinyearsthresholdnull (ISC_QUAD *date1, ISC_QUAD *date2, int *min,
    int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int res = intern_ageinyearsthreshold(date1, date2, min, usemin, max, usemax);
        if (res < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = res;
            return result;
        }
    }
}

// intern: calculate age in months
// gd 2000-01-10
// pm 2004-08-21 swapped parameters
// gh 2006-xx-xx made from ageinmonth
// ct 2009-03-13 check of full month
static int intern_ageinmonths(ISC_QUAD * date1, ISC_QUAD * date2)
{
    struct tm t1, t2;
    int res;
    isc_decode_date(date2, &t2);
    isc_decode_date(date1, &t1);
    res = ((t2.tm_year - t1.tm_year) * 12) + t2.tm_mon - t1.tm_mon;
    if (res < 0) {
	if (t1.tm_mday < t2.tm_mday
	    || (t1.tm_mday == t2.tm_mday
		&& (t1.tm_hour * 3600 + t1.tm_min * 60 + t1.tm_sec) <
		(t2.tm_hour * 3600 + t2.tm_min * 60 + t2.tm_sec)))
	    return res + 1;
	else
	    return res;
    } else {
	if (t2.tm_mday < t1.tm_mday
	    || (t2.tm_mday == t1.tm_mday
		&& (t2.tm_hour * 3600 + t2.tm_min * 60 + t2.tm_sec) <
		(t1.tm_hour * 3600 + t1.tm_min * 60 + t1.tm_sec)))
	    return res - 1;
	else
	    return res;
    }
}

// return age in months
// gh 2006-xx-xx made with intern_ageinmonths
int *ageinmonths(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = intern_ageinmonths(date1, date2);
        return result;
    }
}

// return null instead of negativ ages
// gh 2006-xx-xx made with intern_ageinmonths
int *ageinmonthsnull(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int res = intern_ageinmonths(date1, date2);
        if (res < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = res;
            return result;
        }
   }
}

// return age in months with threshold
// pm 2004-08-21
// ct 2006-05-15 fixed
// gh 2006-xx-xx made with intern_ageinmonths
int *ageinmonthsthreshold(ISC_QUAD * date1, ISC_QUAD * date2, int *min,
    int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        int age = intern_ageinmonths(date1, date2);
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        *result = age;
        return result;
    }
}

// return null instead of negativ age
// gh 2006-xx-xx made with intern_ageinmonths
int *ageinmonthsthresholdnull(ISC_QUAD * date1, ISC_QUAD * date2, int *min,
    int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int age = intern_ageinmonths(date1, date2);
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        if (age < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = age;
            return result;
        }
    }
}

// Returns days since 31.12.1899 - (c) 2000 Robert Loipfinger
// rl 2000-xx-xx written as dtime
//    2006-xx-xx made from dtime
static int intern_dtime(ISC_QUAD * date)
{
    struct tm t;
    int day = 0, i, year;
    isc_decode_date(date, &t);
    for (i = 0; i < t.tm_year; i++) {
        day += isleap(i + 1900) ? 366 : 365;
    }
    // um 1 versetzt, wg. daysofmonth-Logik (Monat 1-basiert)
    for (i = 1; i <= t.tm_mon; i++) {
        year = t.tm_year + 1900;
        day += intern_daysofmonth(i, year);
    }
    day += t.tm_mday - 1;
    return day;
}

// Returns days since 31.12.1899 - (c) 2000 Robert Loipfinger
// rl 2000-xx-xx
//    2006-xx-xx made with intern_dtime
int *dtime(ISC_QUAD * date)
{
    if (!date) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = intern_dtime(date);
        return result;
    }
}

// intern: return number of occurrences of a certain weekday between date1 and date2
// gh 2008-02-14 made as countweekdays
// ct 2009-01-13 made from countweekdays, bringing timestamps in right order and new algorithm
static int intern_countweekdays(ISC_QUAD * timestamp1, ISC_QUAD * timestamp2, int * weekday)
{
    // only valid weekdays 1 = Monday to 7 = Sunday
    if(*weekday < 1 || *weekday > 7) {
        return 0;    
    } else {
        int m, days = 0;
        ISC_QUAD *tst1, *tst2;
        struct tm t1, t2;
        // bring timestamps in right order: 1st earlier timestamp, 2nd later timestamp
        if ((timestamp1->isc_quad_high > timestamp2->isc_quad_high) ||
         ((timestamp1->isc_quad_high == timestamp2->isc_quad_high) &&
          (timestamp1->isc_quad_low > timestamp2->isc_quad_low))) {
            tst1 = timestamp2;
            tst2 = timestamp1;
        } else {
            tst1 = timestamp1;
            tst2 = timestamp2;
        }
        isc_decode_date(tst1, &t1);
        isc_decode_date(tst2, &t2);
        // number of occurrences is number of full weeks
        days = (intern_dtime(tst2) - intern_dtime(tst1)) / 7;
        // bring tm_wday to same system than weekday
        // weekday is:             1 = Monday ..., 6 = Saturday, 7 = Sunday
        // tm_wday is: 0 = Sunday, 1 = Monday ..., 6 = Saturday 
        if (t1.tm_wday == 0) {
            t1.tm_wday = 7;
        }
        if (t2.tm_wday == 0) {
            t2.tm_wday = 7;
        }
        // count days more than a full week
        m = (intern_dtime(tst2) - intern_dtime(tst1)) % 7;
        // if more days than a full week we had to add one day if ...
        if (m > 0) {
           // ... "same" week (1st tm_weekday <= 2nd tm_weekday) 
           if (t1.tm_wday <= t2.tm_wday) {
             // if weekday is between 1st und 2nd tm_weekday
             if(*weekday >= t1.tm_wday && *weekday <= t2.tm_wday) {
               days++;
             }
           }
           else {
           // ... not "same" week
             // if part of the week with weekday < 1st tm_weekday 
             if (*weekday < t1.tm_wday) {
               if (*weekday + 7 >= t1.tm_wday && *weekday + 7 <= t2.tm_wday + 7) {
                 days++;             
               }
             }
             else {
             // if part of the week with weekday >= 1st tm_weekday
               if (*weekday >= t1.tm_wday && *weekday <= t2.tm_wday + 7) {
                 days++;             
               }             
             }           
           }
        }
        else {
        // full week
          // if weekday = 1st tm_weekday
          if (*weekday == t1.tm_wday) {
            days++;
          }  
        }
        return days;
    }
}

// return number of occurrences of a certain weekday between date1 and date2
// gh 2008-02-14
// ct 2009-01-13 use of intern_countweekdays
int *countweekdays(ISC_QUAD * timestamp1, ISC_QUAD * timestamp2, int *weekday)
{
    if (!timestamp1 || !timestamp2 || !weekday) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = intern_countweekdays(timestamp1, timestamp2, weekday);
        return result;
    }
}    
    
// intern: calculate age in weeks
// sunday = 1st day of week
// If the two dates are in the same week, the return value is 0.
// gd 2000-01-10
static int intern_ageinweeks(ISC_QUAD * date1, ISC_QUAD * date2)
{
    struct tm t1, t2;
    int days1, days2;
    isc_decode_date(date1, &t1);
    isc_decode_date(date2, &t2);
    days1 = intern_dtime(date1) - t1.tm_wday;
    days2 = intern_dtime(date2) - t2.tm_wday;
    return (days2 - days1) / 7;
}

// intern: calculate age in weeks ISO standard
// monday = 1st day of week
// If the two dates are in the same week, the return value is 0.
// ct 2008-02-18
static int intern_ageinweeksiso(ISC_QUAD * date1, ISC_QUAD * date2)
{
    struct tm t1, t2;
    int days1, days2;
    isc_decode_date(date1, &t1);
    isc_decode_date(date2, &t2);
    days1 = intern_dtime(date1);
    days2 = intern_dtime(date2);
    return (days2 - days1) / 7;
}

//  return age in weeks
int *ageinweeks(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = intern_ageinweeks(date1, date2);
        return result;
    }
}

// ct 2008-02-18
//  return age in weeks ISO standard
int *ageinweeksiso(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = intern_ageinweeksiso(date1, date2);
        return result;
    }
}

//  return return age in weeks and null instead of negativ age
int *ageinweeksnull(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int res = intern_ageinweeks(date1, date2);
        if (res < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = res;
            return result;
        }
    }
}

// ct 2008-02-18
// return age in weeks ISO standard and null instead of negativ age
int *ageinweeksnulliso(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int res = intern_ageinweeksiso(date1, date2);
        if (res < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = res;
            return result;
        }
    }
}

//  return age in weeks with threshold
int *ageinweeksthreshold(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        int age = intern_ageinweeks(date1, date2);
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        *result = age;
        return result;
    }
}

// ct 2008-02-18
//  return age in weeks ISO standard with threshold
int *ageinweeksthresholdiso(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        int age = intern_ageinweeksiso(date1, date2);
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        *result = age;
        return result;
    }
}

//  returns age in weeks with threshold null instead of negativ ages
int *ageinweeksthresholdnull(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int age = intern_ageinweeks(date1, date2);
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        if (age < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = age;
            return result;
        }
    }
}

// ct 2008-02-18
// returns age in weeks with threshold ISO standard null instead of negativ ages
int *ageinweeksthresholdnulliso(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int age = intern_ageinweeksiso(date1, date2);
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        if (age < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = age;
            return result;
        }
    }
}

// return age in days
// pm 2005-10-xx
int *ageindays(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = intern_dtime(date2) - intern_dtime(date1);
        return result;
    }
}

//  return null instead of negativ age
int *ageindaysnull(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int res = intern_dtime(date2) - intern_dtime(date1);
        if (res < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = res;
            return result;
        }
    }
}

//  return age in days with threshold
int *ageindaysthreshold(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        int age = intern_dtime(date2) - intern_dtime(date1);
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        *result = age;
        return result;
    }
}

//  return null instead of negativ age
int *ageindaysthresholdnull(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int age = intern_dtime(date2) - intern_dtime(date1);
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        if (age < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = age;
            return result;
        }
    }
}

// ct 2009-01-08
// similar to ageindays but without the weekday/s from param 3
int *ageinworkingdays(ISC_QUAD * timestamp1, ISC_QUAD * timestamp2, int *weekdays)
{
    if (!timestamp1 || !timestamp2 || !weekdays) {
        return NULL;
    } else {
        int z = *weekdays, y;
        int *result = MALLOC(sizeof(int));
        *result = intern_dtime(timestamp2) - intern_dtime(timestamp1);
        while (z > 0) {
          y = z % 10;
          if (*result > 0) {
            *result -= intern_countweekdays(timestamp1, timestamp2, &y);
          } else {
            *result += intern_countweekdays(timestamp1, timestamp2, &y);          
          }            
          z /= 10;
        }
        return result;
    }
}

// ct 2009-01-09
// similar to ageindaysthreshold but without the weekday/s from param 7
int *ageinworkingdaysthreshold(ISC_QUAD * timestamp1, ISC_QUAD * timestamp2,
    int *min, int *usemin, int *max, int *usemax, int *weekdays)
{
    if (!timestamp1 || !timestamp2 || !min || !usemin || !max || !usemax || !weekdays) {
        return NULL;
    } else {
        int z = *weekdays, y;
        int *result = MALLOC(sizeof(int));
        int age = intern_dtime(timestamp2) - intern_dtime(timestamp1);
        while (z > 0) {
          y = z % 10;
          if (age > 0) {
            age -= intern_countweekdays(timestamp1, timestamp2, &y);
          } else {
            age += intern_countweekdays(timestamp1, timestamp2, &y);          
          }            
          z /= 10;
        }
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        *result = age;        
        return result;
    }
}

// intern: calculate difference between date1 and date 2
static double intern_zeitdifferenz(ISC_QUAD * date1, ISC_QUAD * date2, char *sz)
{
    struct tm d1, d2;
    int aid;
    int secdiff; 
    double res = 0;
    isc_decode_date(date1, &d1);
    isc_decode_date(date2, &d2);
    aid = intern_dtime(date1) - intern_dtime(date2);
    secdiff = ((d1.tm_hour * 3600) + (d1.tm_min * 60) + d1.tm_sec) - ((d2.tm_hour * 3600) + (d2.tm_min * 60) + d2.tm_sec);
    if (strlen(sz) == 1) {
        switch (sz[0]) {
        case 't':
            res = aid + (double) secdiff / 86400.0;
            break;
        case 'h':
            res = aid * 24 + (double) secdiff / 3600.0;
            break;
        case 'm':
            res = (double) aid *1440.0 + (double) secdiff / 60.0;
            break;
        case 's':
            res = (double) aid *86400.0 + secdiff;
            break;
        }
    }   
    return res;
}

//  return difference between date1 and date 2
double *zeitdifferenz(ISC_QUAD * date1, ISC_QUAD * date2, char *sz)
{
    if (!date1 || !date2 || !sz) {
        return NULL;
    } else {
        double *result = MALLOC(sizeof(double));
        *result = intern_zeitdifferenz(date1, date2, sz);
        return result;
    }
}

// return age in hours
// pm 2005-10-xx
int *ageinhours(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = (int) intern_zeitdifferenz(date2, date1, "h");
        return result;
    }
}

//  return null instead of negativ age
int *ageinhoursnull(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int res = (int) intern_zeitdifferenz(date2, date1, "h");
        if (res < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = res;
            return result;
        }
   }
}

//  return age in hours with threshold
int *ageinhoursthreshold(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        int age = (int) intern_zeitdifferenz(date2, date1, "h");
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        *result = age;
        return result;
    }
}

//  return null instead of negativ age
int *ageinhoursthresholdnull(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int age = (int) intern_zeitdifferenz(date2, date1, "h");
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        if (age < 0) {
             return NULL;
        } else {
              int *result = MALLOC(sizeof(int));
              *result = age;
            return result;
        }
    }
}

// return age in minutes
// pm 2005-10-xx
int *ageinminutes(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = (int) intern_zeitdifferenz(date2, date1, "m");
        return result;
    }
}

//  return null instead of negativ age
int *ageinminutesnull(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int res = (int) intern_zeitdifferenz(date2, date1, "m");
        if (res < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = res;
            return result;
        }
    }
}

//  return age in minutes with threshold
int *ageinminutesthreshold(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        int age = (int) intern_zeitdifferenz(date2, date1, "m");
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        *result = age;
        return result;
    }
}

//  return null instead of negativ age
int *ageinminutesthresholdnull(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int age = (int) intern_zeitdifferenz(date2, date1, "m");
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        if (age < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = age;
            return result;
        }
    }
}

// return age in seconds
// pm 2005-10-xx
int *ageinseconds(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = (int) intern_zeitdifferenz(date2, date1, "s");
        return result;
    }
}

//  return null instead of negativ age
int *ageinsecondsnull(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2) {
        return NULL;
    } else {
        int res = (int) intern_zeitdifferenz(date2, date1, "s");
        if (res < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = res;
            return result;
        }
    }
}

//  return age in seconds with threshold
int *ageinsecondsthreshold(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        int age = (int) intern_zeitdifferenz(date2, date1, "s");
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        *result = age;
        return result;
    }
}

//  return null instead of negativ age
int *ageinsecondsthresholdnull(ISC_QUAD * date1, ISC_QUAD * date2,
    int *min, int *usemin, int *max, int *usemax)
{
    if (!date1 || !date2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int age = (int) intern_zeitdifferenz(date2, date1, "s");
        if (*usemin == 1 && age < *min) {
            age = *min;
        } else if (*usemax == 1 && age > *max) {
            age = *max;
        }
        if (age < 0) {
            return NULL;
        } else {
            int *result = MALLOC(sizeof(int));
            *result = age;
            return result;
        }
    }
}

// intern
// pm 2004-08-21
int decode_language(char *language)
{
    char cl[3];
    cl[0] = language[0] | 0x20;
    cl[1] = language[1] | 0x20;
    cl[2] = '\0';
    if (!strcmp(cl, "us")) {
        return 0;
    } else {
        const char *pc = strstr(LANG_INDEX, cl);
        if (pc == NULL)
            return -1;
        else
            return (pc - LANG_INDEX) / 3;
    }
}

// gh 2010-03-11
// get language form locale
static char *language = NULL;

char *intern_getlang()
{
    if (!language) {
#ifdef WINDOWS
	DWORD LangID = PRIMARYLANGID(GetSystemDefaultLangID());
    // uk de fr it es pt nl no se dk fi hu ie ee is al va v1 c1 s1 s2 w1 b1 b2 n1 za fo lu w2
	switch (LangID) {  
	case 0x09:
	    language = "uk";
	    break;
	case 0x07:
	    language = "de";
	    break;
	case 0x0C:
	    language = "fr";
	    break;
	case 0x10:
	    language = "it";
	    break;
	case 0x0A:
	    language = "es";
	    break;
	case 0x16:
	    language = "pt";
	    break;
	case 0x13:
	    language = "nl";
	    break;
	case 0x14:
	    language = "no";
	    break;
	case 0x1D:
	    language = "se";
	    break;
	case 0x06:
	    language = "dk";
	    break;
	case 0x0B:
	    language = "fi";
	    break;
	case 0x0E:
	    language = "hu";
	    break;
	case 0x3C:
	    language = "ie";
	    break;
	default:
	    language = "us";
	}
#else
	char *locale = getenv("LC_ALL");
	if (!locale || !*locale) locale = getenv("LC_TIME");
	if (!locale || !*locale) locale = getenv("LANG");
	if (locale && *locale) {
	    language = malloc(3);
	    strncpy(language, locale, 2);
	    language[2] = '\0';
	    if (!strstr(LANG_INDEX, language)) {
		free(language);
		language = "us";
	    }
	} else {
	    language = "us";
	}
#endif        
    }
    return language;
}

// pm 2004-08-21
// ct 2006-11-23 implementation of NULL for FB 2.0
char *cdowlonglang(ISC_QUAD * ib_date, char *language)
{
    if (!ib_date || !language)
        return NULL;
    else {
        struct tm t;
        char *result = (char *) MALLOC(LOC_STRING_SIZE);
        int lang;
        isc_decode_date(ib_date, &t);
        lang = decode_language(language);
        if (lang < 0)
            strcpy(result, "???");
        else
            strcpy(result, DOW[lang * 7 + t.tm_wday]);
        return result;
    }
}

// pm 2004-08-21
// ct 2006-11-23 implementation of NULL for FB 2.0
// ct 2010-03-11 setting output to locale language
// gh 2010-03-20 optimized
char *cdowlong(ISC_QUAD * ib_date)
{
    if (!ib_date) {
        return NULL;
    } else {
        return cdowlonglang(ib_date, intern_getlang());
    }
}

// pm 2004-08-21
// ct 2006-11-23 implementation of NULL for FB 2.0
// ct 2007-07-22 own table for shortnames
// old code takes only first 3 leters from longname
char *cdowshortlang(ISC_QUAD * ib_date, char *language)
{
    if (!ib_date || !language)
        return NULL;
    else {
        struct tm t;
        char *result = (char *) MALLOC(LOC_STRING_SIZE);
        int lang;
        isc_decode_date(ib_date, &t);
        lang = decode_language(language);
        if (lang < 0)
            strcpy(result, "???");
        else
            strcpy(result, DOWSHORT[lang * 7 + t.tm_wday]);
        result[4] = '\0';
        return result;
    }
}

// pm 2004-08-21
// ct 2006-11-23 implementation of NULL for FB 2.0
// ct 2010-03-11 setting output to locale language
// gh 2010-03-20 optimized
char *cdowshort(ISC_QUAD * ib_date)
{
    if (!ib_date) {
        return NULL;
    } else {
        return cdowshortlang(ib_date, intern_getlang());
    }
}

// pm 2004-08-21 optimized
// ct 2006-11-23 implementation of NULL for FB 2.0
char *cmonthlonglang(ISC_QUAD * ib_date, char *language)
{
    if (!ib_date || !language)
        return NULL;
    else {
        struct tm t;
        char *result = (char *) MALLOC(LOC_STRING_SIZE);
        int lang;
        isc_decode_date(ib_date, &t);
        lang = decode_language(language);
        if (lang < 0)
            strcpy(result, "???");
        else
            strcpy(result, MONTH[12 * lang + t.tm_mon]);
        return result;
    }
}

// pm 2004-08-21 optimized
// ct 2006-11-23 implementation of NULL for FB 2.0
// ct 2010-03-11 setting output to locale language
// gh 2010-03-20 optimized
char *cmonthlong(ISC_QUAD * ib_date)
{
    if (!ib_date) {
        return NULL;
    } else {
        return cmonthlonglang(ib_date, intern_getlang());
    }    
}

// pm 2004-08-21 optimized
// ct 2006-11-23 implementation of NULL for FB 2.0
char *cmonthshortlang(ISC_QUAD * ib_date, char *language)
{
    if (!ib_date || !language)
        return NULL;
    else {
        char *result = cmonthlonglang(ib_date, language);
        if (!strcmp(language, "fr") && result[1] == 'u')
            result[2] = result[3];      // Juin -> Jun,   Juillet -> Jul
        result[3] = '\0';
        return result;
    }
}

// pm 2004-08-21 optimized
// ct 2006-11-23 implementation of NULL for FB 2.0
// ct 2010-03-11 setting output to locale language
// gh 2010-03-20 optimized
char *cmonthshort(ISC_QUAD * ib_date)
{
    if (!ib_date) {
        return NULL;
    } else {
        return cmonthshortlang(ib_date, intern_getlang());
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *dayofmonth(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm t;
        isc_decode_date(date, &t);
        *result = t.tm_mday;
        return result;
    }
}

// intern
static int intern_dayofweek(ISC_QUAD * date)
{
        struct tm t;
        isc_decode_date(date, &t);
        return t.tm_wday + 1;
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *dayofweek(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = intern_dayofweek(date);
        return result;
    }
}

// ct 2007-01-27
int *r_weekday(ISC_QUAD * date)
{
    if (!date) {
        return NULL;
    }
    else
    {
        int *result = MALLOC(sizeof(int));
        *result = intern_dayofweek(date) - 1;
        return result;
    }
}

// ct 2007-01-27
int *r_dow(ISC_QUAD * date)
{
    if (!date) {
        return NULL;
    }
    else
    {
        int *result = MALLOC(sizeof(int));
        *result = intern_dayofweek(date) - 1 == 0 ? 7 : intern_dayofweek(date) - 1;
        return result;
    }
}

// intern
static int intern_dayofyear(ISC_QUAD * date)
{
        struct tm t;
        isc_decode_date(date, &t);
        return t.tm_yday + 1;
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *dayofyear(ISC_QUAD * date)
{
    if (!date) {
        return NULL;
    }
    else
    {
        int *result = MALLOC(sizeof(int));
        *result = intern_dayofyear(date);
        return result;
    }
}

// ct 2007-01-27
int *r_yearday(ISC_QUAD * date)
{
    if (!date) {
        return NULL;
    }
    else
    {
        int *result = MALLOC(sizeof(int));
        *result = intern_dayofyear(date) - 1;
        return result;
    }
}

// rl 2001
// pm 2004-08-21
// ct 2006-11-23 implementation of NULL for FB 2.0
char *gformatd(char *format, ISC_QUAD * ib_date)
{
    if (!format || !ib_date)
        return NULL;
    else {
        struct tm t;
        char *result = (char *) MALLOC(LOC_STRING_SIZE);
        char *pr = result;
        char c;
        int count;
        const char *pcmonth, *pcdow;
        isc_decode_date(ib_date, &t);
        while (*format) {
            c = *format++;
            count = 1;
            while (*format == c)
                count++, format++;
            pcdow = DOW[7 + t.tm_wday]; // german
            pcmonth = MONTH[12 + t.tm_mon];     // german
            switch (c) {
            case 'd':
                if (count == 1)
                    sprintf(pr, "%d", t.tm_mday);
                else if (count == 2)
                    sprintf(pr, "%02d", t.tm_mday);
                else if (count == 3)
                    sprintf(pr, "%c%c%c", pcdow[0], pcdow[1], pcdow[2]);
                else if (count == 4)
                    strcpy(pr, pcdow);
                break;
            case 'm':
                if (count == 1)
                    sprintf(pr, "%d", t.tm_mon + 1);
                else if (count == 2)
                    sprintf(pr, "%02d", t.tm_mon + 1);
                else if (count == 3)
                    sprintf(pr, "%c%c%c",
                            pcmonth[0], pcmonth[1], pcmonth[2]);
                else if (count == 4)
                    strcpy(pr, pcmonth);
                break;
            case 'y':
                if (count == 2)
                    sprintf(pr, "%02d", t.tm_year % 100);
                else if (count == 4)
                    sprintf(pr, "%04d", t.tm_year + 1900);
                break;
            case 'h':
                if (count == 1)
                    sprintf(pr, "%d", t.tm_hour);
                else if (count == 2)
                    sprintf(pr, "%02d", t.tm_hour);
                break;
            case 'n':
                if (count == 1)
                    sprintf(pr, "%d", t.tm_min);
                else if (count == 2)
                    sprintf(pr, "%02d", t.tm_min);
                break;
            case 's':
                if (count == 1)
                    sprintf(pr, "%d", t.tm_sec);
                else if (count == 2)
                    sprintf(pr, "%02d", t.tm_sec);
                break;
            default:
                while (count--)
                    *pr++ = c;
                *pr = '\0';
            }
            while (*pr)
                pr++;
        }
        *pr = '\0';
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *isleapyear(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm t;
        isc_decode_date(date, &t);
        *result = isleap(t.tm_year + 1900);
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *r_isleapyear(int * year)
{
    if (!year)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        *result = isleap(* year);
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_QUAD *maxdate(ISC_QUAD * ib_date_1, ISC_QUAD * ib_date_2)
{
    if (!ib_date_1 || !ib_date_2)
        return NULL;
    else {
        ISC_QUAD *result = (ISC_QUAD *) MALLOC(sizeof(ISC_QUAD));
        if ((ib_date_1->isc_quad_high > ib_date_2->isc_quad_high) ||
            ((ib_date_1->isc_quad_high == ib_date_2->isc_quad_high) &&
             (ib_date_1->isc_quad_low > ib_date_2->isc_quad_low)))
            *result = *ib_date_1;
        else
            *result = *ib_date_2;
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_QUAD *mindate(ISC_QUAD * ib_date_1, ISC_QUAD * ib_date_2)
{
    if (!ib_date_1 || !ib_date_2)
        return NULL;
    else {
        ISC_QUAD *result = (ISC_QUAD *) MALLOC(sizeof(ISC_QUAD));
        if ((ib_date_1->isc_quad_high < ib_date_2->isc_quad_high) ||
            ((ib_date_1->isc_quad_high == ib_date_2->isc_quad_high) &&
             (ib_date_1->isc_quad_low < ib_date_2->isc_quad_low)))
            *result = *ib_date_1;
        else
            *result = *ib_date_2;
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *month(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm t;
        isc_decode_date(date, &t);
        *result = t.tm_mon + 1;
        return result;
    }
}

// pm 2004-08-21 bug fixed
// ct 2006-11-23 implementation of NULL for FB 2.0
int *quarter(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm t;
        isc_decode_date(date, &t);
        *result = t.tm_mon / 3 + 1;
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_QUAD *stripdate(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = 0;
        t.tm_mon = 0;
        t.tm_year = 0;
        t.tm_wday = 0;
        t.tm_yday = 0;
    return (ISC_QUAD *) gen_ib_date(&t);
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_QUAD *striptime(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
    return (ISC_QUAD *) gen_ib_date(&t);
    }
}

// pm 2005-10-16 returntyp TIME instead of TIMESTAMP
// ct 2006-11-23 implementation of NULL for FB 2.0
ISC_TIME *strtotime(char *sz)
{
    if (!sz)
        return NULL;
    else {
        struct tm t;
        int len = strlen(sz);
        int h, m, s, pm, i;
        ISC_TIME *result = (ISC_TIME *) MALLOC(sizeof(ISC_TIME));
        i = 0;
        h = strtol(sz, NULL, 10);
        while (i < len && (sz[i] != ':'))
            i++;
        i++;
        if (i < len) {
            m = strtol(&sz[i], NULL, 10);
            while (i < len && (sz[i] != ':'))
                i++;
            i++;
        } else
            m = 0;
        if (i < len)
            s = strtol(&sz[i], NULL, 10);
        else
            s = 0;
        pm = len >= 2 ? (strcmp(&sz[len - 2], "AM")) : 0;
        t.tm_sec = s;
        t.tm_min = m;
        if (pm)
            t.tm_hour = (h + ((h == 12) ? 0 : 12)) % 24;
        else
            t.tm_hour = (h + ((h == 12) ? 12 : 0)) % 24;
        t.tm_mday = 0;
        t.tm_mon = 0;
        t.tm_year = 0;
        t.tm_wday = 0;
        t.tm_yday = 0;
        t.tm_isdst = 0;
        isc_encode_sql_time(&t, result);
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *week(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm t;
        int dow_ybeg, wk;
        isc_decode_date(date, &t);
        // On what day of the week did the year begin?
        dow_ybeg = ((t.tm_wday + 7) - (t.tm_yday % 7)) % 7;
        wk = ((t.tm_yday - t.tm_wday + 7) / 7) + 1;
        if (dow_ybeg == SUNDAY)
            *result = wk - 1;
        else
            *result = wk;
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *weekofyear(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        int week, year;
        i_week_of_year(ib_date, &week, &year);
        *result = week;
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
// ct 2007-01-18 single figures with leading 0
char *woy(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        char *result = (char *) MALLOC(LOC_STRING_SIZE + 1);
        int week, year;
        i_week_of_year(ib_date, &week, &year);
        sprintf(result, "%d%02d", year, week);
        return result;
    }
}

static int intern_kalenderwoche(ISC_QUAD * date);

// gh 2008-02-12
// ct 2009-01-30
// week of year with year according to ISO
char *woyiso(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        char *result = MALLOC(7);
        struct tm t;
        int kw = intern_kalenderwoche(date);
        isc_decode_date(date, &t);
        if (kw > 51 && t.tm_mon == 0) {
            t.tm_year -= 1;
        }    
        if (kw == 1 && t.tm_mon == 11) {
            t.tm_year += 1;
        }
        sprintf(result, "%04d%02d", t.tm_year + 1900, kw);
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *year(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm t;
        isc_decode_date(date, &t);
        *result = t.tm_year + 1900;
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *yearofyear(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        int week, year;
        i_week_of_year(date, &week, &year);
        *result = year;
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *hour(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm t;
        isc_decode_date(date, &t);
        *result = t.tm_hour;
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *minute(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm t;
        isc_decode_date(date, &t);
        *result = t.tm_min;
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *second(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm t;
        isc_decode_date(date, &t);
        *result = t.tm_sec;
        return result;
    }
}

// ct 2007-02-01
int *millisecond(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        div_t x;
        x = div(date->gds_quad_low, ISC_TIME_SECONDS_PRECISION);
        *result = x.rem;
        return result;
    }
}

//  return days of month m in year y
int *daysofmonth(int *m, int *y)
{
    if (!m || !y) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        *result = intern_daysofmonth(*m, *y);
        return result;
    }
}

//  same as daysofmonth() but with swapped paramaters
int *lastday(int *y, int *m)
{
    return daysofmonth(m, y);
}


// generate interbase date from struct tm
ISC_QUAD * gen_ib_date(struct tm * t)
{
    ISC_QUAD *ibd = (ISC_QUAD *) MALLOC(sizeof(ISC_QUAD));
    isc_encode_date(t, ibd);
    return ibd;
}

// Simple observations:
//   7 * 52 = 364 = a common year + 1 (or 2 if it is a leap year)
//   There are 365-366 days in the year.
//   A week is Sunday - Saturday.
// So....
// if the year begins on a Sun - Wed, then Jan 1 is considered to begin
//   Week #1.
// if the year begins on Thu - Sat, then Jan 1 is considered to still
//   be part of the last week of the previous year.
// if the year begins on a Wed OR
//   the year begins on a Tue or Wed and its a leap year THEN
//   the year has 53 weeks in it...
static void i_week_of_year(ISC_QUAD * ib_date, int *week, int *year)
{
    struct tm t;
    int dow_ybeg, yr, wk;
    isc_decode_date(ib_date, &t);
    yr = t.tm_year + 1900;
    // When did the year begin?
    dow_ybeg = ((t.tm_wday + 7) - (t.tm_yday % 7)) % 7;
    // Get the Sunday beginning this week.
    wk = (t.tm_yday - t.tm_wday);
    // If the Sunday beginning this week was last year, then
    //   if this year begins on a Wednesday or previous, then
    //   this is most certainly the first week of the year.
    //   if this year begins on a thursday or
    //     last year was a leap year and this year begins on a friday, then
    //   this week is 53 of last year.
    //   Otherwise this week is 52 of last year.
    if (wk < 0) {
        if (dow_ybeg <= WEDNESDAY)
            wk = 1;
        else if ((dow_ybeg == THURSDAY) ||
                 (isleap((yr - 1)) && (dow_ybeg == FRIDAY))) {
            wk = 53;
            yr--;
        } else {
            wk = 52;
            yr--;
        }
    } else {
        // If the Sunday beginning this week falls in this year
        //   if the year begins on a Sun, Mon, Tue or Wed then
        //     This week # is (Week + 7) div 7
        //   otherwise this week is
        //     Week div 7 + 1.
        //   if the week is > 52 then
        //     if this year began on a wed or this year is leap year and it
        //       began on a tuesday, then set the week to 53.
        //     otherwise set the week to 1 of *next* year.
        if (dow_ybeg <= WEDNESDAY)
            wk = (wk + 7 + dow_ybeg) / 7;
        else
            wk = (wk / 7) + 1;
        if (wk > 52) {
            if ((dow_ybeg == WEDNESDAY) ||
                (isleap(yr) && (dow_ybeg == TUESDAY)))
                wk = 53;
            else {
                wk = 1;
                yr++;
            }
        }
    }
    *week = wk;
    *year = yr;
}

// pm 2005-10-03
// ct 2006-11-23 implementation of NULL for FB 2.0
int *equaldate(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm d1, d2;
        isc_decode_date(date1, &d1);
        isc_decode_date(date2, &d2);
        *result = (d1.tm_year == d2.tm_year && d1.tm_mon == d2.tm_mon
                   && d1.tm_mday == d2.tm_mday);
        return result;
    }
}

// pm 2005-10-03
// ct 2006-11-23 implementation of NULL for FB 2.0
int *equaldatetime(ISC_QUAD * date1, ISC_QUAD * date2)
{
    if (!date1 || !date2)
        return NULL;
    else {
        int *result = MALLOC(sizeof(int));
        struct tm d1, d2;
        isc_decode_date(date1, &d1);
        isc_decode_date(date2, &d2);
        *result = (d1.tm_year == d2.tm_year && d1.tm_mon == d2.tm_mon
                   && d1.tm_mday == d2.tm_mday && d1.tm_hour == d2.tm_hour
                   && d1.tm_min == d2.tm_min && d1.tm_sec == d2.tm_sec);
        return result;
    }
}

// gh 2008-02-23
// made from old osterdatum origin by pm 2005-10-03
static ISC_QUAD *intern_osterdatum(int *year)
{
    ISC_QUAD *result;
    struct tm date;
    int m = 0, n = 0;
    int a, b, c, d, e;

    if (*year < 100) {
        if (*year <= 80) {
            *year += 2000;
        } else {
            *year += 1900;
        }
    }
    // Easteralgorithm (Pascal) from Michael Heydekamp
    date.tm_year = *year - 1900;
    if (*year < 1700)
        m = 22, n = 2;
    else if (*year < 1800)
        m = 23, n = 3;
    else if (*year < 1900)
        m = 23, n = 4;
    else if (*year < 2100)
        m = 24, n = 5;
    else if (*year < 2200)
        m = 24, n = 6;
    else if (*year < 2300)
        m = 25, n = 0;
    a = *year % 19;
    b = *year % 4;
    c = *year % 7;
    d = (19 * a + m) % 30;
    e = (2 * b + 4 * c + 6 * d + n) % 7;
    if (22 + d + e <= 31)
        date.tm_mday = 22 + d + e, date.tm_mon = 3;
    else
        date.tm_mday = d + e - 9, date.tm_mon = 4;
    if (date.tm_mday == 26 && date.tm_mon == 4)
        date.tm_mday = 19;
    else if (date.tm_mday == 25 && date.tm_mon == 4 && d == 28
             && e == 6 && a > 10)
        date.tm_mday = 18;
    date.tm_mon--;
    date.tm_sec = 0;
    date.tm_min = 0;
    date.tm_hour = 0;
    date.tm_wday = 0;
    date.tm_yday = 0;
    result = malloc(sizeof(ISC_QUAD));
    isc_encode_date(&date, result);
    return result;
}

// pm 2005-10-03
// ct 2006-11-23 implementation of NULL for FB 2.0
// gh 2008-02-23 changed to intern_osterdatum
ISC_QUAD *osterdatum(int *year)
{
    if (!year) {
        return NULL;
    } else {
        ISC_QUAD *result = (ISC_QUAD *) MALLOC(sizeof(ISC_QUAD));
        ISC_QUAD *ostern = intern_osterdatum(year);
        memcpy(result, ostern, sizeof(ISC_QUAD));
        free(ostern);
        return result;
    }
}

// pm 2005-10-16
// ct 2006-11-23 implementation of NULL for FB 2.0
int *yearsbetween(ISC_QUAD * d1, ISC_QUAD * d2)
{
    if (!d1 || !d2) {
        return NULL;
    } else {
        int *result = ageinyears(d1, d2);
        *result = abs(*result);
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *monthsbetween(ISC_QUAD * d1, ISC_QUAD * d2)
{
    if (!d1 || !d2) {
        return NULL;
    } else {
        int *result = ageinmonths(d1, d2);
        *result = abs(*result);
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *weeksbetween(ISC_QUAD * d1, ISC_QUAD * d2)
{
    if (!d1 || !d2) {
        return NULL;
    } else {
        int *result = ageinweeks(d1, d2);
        *result = abs(*result);
        return result;
    }
}

// ct 2008-02-18
// ISO-version of weeksbetween
int *weeksbetweeniso(ISC_QUAD * d1, ISC_QUAD * d2)
{
    if (!d1 || !d2) {
        return NULL;
    } else {
        int *result = ageinweeksiso(d1, d2);
        *result = abs(*result);
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *daysbetween(ISC_QUAD * d1, ISC_QUAD * d2)
{
    if (!d1 || !d2) {
        return NULL;
    } else {
        int *result = ageindays(d1, d2);
        *result = abs(*result);
        return result;
    }
}

// ct 2009-01-09
int *workingdaysbetween(ISC_QUAD * d1, ISC_QUAD * d2, int * weekdays)
{
    if (!d1 || !d2 || !weekdays) {
        return NULL;
    } else {
        int *result = ageinworkingdays(d1, d2, weekdays);
        *result = abs(*result);
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *hoursbetween(ISC_QUAD * d1, ISC_QUAD * d2)
{
    if (!d1 || !d2) {
        return NULL;
    } else {
        int *result = ageinhours(d1, d2);
        *result = abs(*result);
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *minutesbetween(ISC_QUAD * d1, ISC_QUAD * d2)
{
    if (!d1 || !d2) {
        return NULL;
    } else {
        int *result = ageinminutes(d1, d2);
        *result = abs(*result);
        return result;
    }
}

// ct 2006-11-23 implementation of NULL for FB 2.0
int *secondsbetween(ISC_QUAD * d1, ISC_QUAD * d2)
{
    if (!d1 || !d2) {
        return NULL;
    } else {
        int *result = ageinseconds(d1, d2);
        *result = abs(*result);
        return result;
    }
}

// pm 2005-10-16
// ct 2006-11-23 implementation of NULL for FB 2.0
// ct 2009-01-15 add check of inputs
ISC_DATE *encodedate(int *year, int *month, int *day)
{
    if (!year || !month || !day || *month < 1 || *month > 12 || *day < 1 || *day > 31) {
        return NULL;
    } else {
        ISC_DATE *result = (ISC_DATE *) MALLOC(sizeof(ISC_DATE));
        struct tm t;
        t.tm_year = *year - 1900;
        t.tm_mon = *month - 1;
        t.tm_mday = *day;
        t.tm_wday = 0;
        t.tm_yday = 0;
        isc_encode_sql_date(&t, result);
        return result;
    }
}

// gh 2006-12-01
// ct 2009-01-15 add check of inputs
ISC_DATE *encodedatenull(int *year, int *month, int *day)
{
    if (!year || !month || !day || *month < 1 || *month > 12 || *day < 1 || *day > 31) {
        return NULL;
    } else {
        ISC_DATE dummy;
        struct tm t1, t2;
        t1.tm_year = *year - 1900;
        t1.tm_mon = *month - 1;
        t1.tm_mday = *day;
        t1.tm_wday = 0;
        t1.tm_yday = 0;
        isc_encode_sql_date(&t1, &dummy);
        isc_decode_sql_date(&dummy, &t2);
        if (t1.tm_mon != t2.tm_mon) {
            return NULL;
        } else {
            ISC_DATE *result = (ISC_DATE *) MALLOC(sizeof(ISC_DATE));
            memcpy(result, &dummy, sizeof(ISC_DATE));
            return result;
        }
    }
}

// ct 2007-01-27
// ct 2009-01-15 add check of inputs
ISC_QUAD *r_encodedate(int *day, int *month, int *year)
{
    if (!year || !month || !day || *month < 1 || *month > 12 || *day < 1 || *day > 31) {
        return NULL;
    } else {
        struct tm t;
        t.tm_year = *year - 1900;
        t.tm_mon = *month - 1;
        t.tm_mday = *day;
        t.tm_hour = 0;
        t.tm_min = 0;
        t.tm_sec = 0;
        t.tm_wday = 0;
        t.tm_yday = 0;
        return gen_ib_date(&t);
    }
}

// pm 2005-10-16
// ct 2006-11-23 implementation of NULL for FB 2.0
// ct 2009-01-15 add check of inputs
ISC_TIME *encodetime(int *hour, int *minute, int *second)
{
    if (!hour || !minute || !second || *hour < 0 || *hour > 23 || *minute < 0 || *minute > 59 || *second < 0 || *second > 59) {
        return NULL;
    }    
    else {
        ISC_TIME *result = (ISC_TIME *) MALLOC(sizeof(ISC_TIME));
        struct tm t;
        t.tm_hour = *hour;
        t.tm_min = *minute;
        t.tm_sec = *second;
        isc_encode_sql_time(&t, result);
        return result;
    }
}

// ct 2006-12-01
// ct 2009-01-15 add check of inputs
ISC_TIME *encodetimenull(int *hour, int *minute, int *second)
{
    if (!hour || !minute || !second || *hour < 0 || *hour > 23 || *minute < 0 || *minute > 59 || *second < 0 || *second > 59) {
        return NULL;
    } else {
        ISC_TIME dummy;
        struct tm t1, t2;
        t1.tm_hour = *hour;
        t1.tm_min = *minute;
        t1.tm_sec = *second;
        isc_encode_sql_time(&t1, &dummy);
        isc_decode_sql_time(&dummy, &t2);
        // absurd if - but necessary !? to avoid compiler error
        if (t1.tm_hour != t2.tm_hour) {
            return NULL;
        } else {
            ISC_TIME *result = (ISC_TIME *) MALLOC(sizeof(ISC_TIME));
            memcpy(result, &dummy, sizeof(ISC_TIME));
            return result;
        }
    }
}

// pm 2005-10-16
// ct 2006-11-23 implementation of NULL for FB 2.0
// ct 2009-01-15 add check of inputs
ISC_QUAD *encodetimestamp(int *year, int *month, int *day, int *hour,
    int *minute, int *second)
{
    if (!year || !month || !day || !hour || !minute || !second || *month < 1 || *month > 12 || *day < 1 || *day > 31 || *hour < 0 || *hour > 23 || *minute < 0 || *minute > 59 || *second < 0 || *second > 59) {
        return NULL;
    } else {
        struct tm t;
        t.tm_year = *year - 1900;
        t.tm_mon = *month - 1;
        t.tm_mday = *day;
        t.tm_hour = *hour;
        t.tm_min = *minute;
        t.tm_sec = *second;
        t.tm_wday = 0;
        t.tm_yday = 0;
        return gen_ib_date(&t);
    }
}

// ct 2007-01-27
// ct 2009-01-15 add check of inputs 
ISC_QUAD *r_encodedatetime(int *day, int *month, int *year, int *hour,
    int *minute, int *second)
{
    if (!year || !month || !day || !hour || !minute || !second || *month < 1 || *month > 12 || *day < 1 || *day > 31 || *hour < 0 || *hour > 23 || *minute < 0 || *minute > 59 || *second < 0 || *second > 59) {
        return NULL;
    } else {
        struct tm t;
        t.tm_year = *year - 1900;
        t.tm_mon = *month - 1;
        t.tm_mday = *day;
        t.tm_hour = *hour;
        t.tm_min = *minute;
        t.tm_sec = *second;
        t.tm_wday = 0;
        t.tm_yday = 0;
        return gen_ib_date(&t);
    }
}

// intern
// gh 2007-01-30
// convert a period in format "[-][d:]hh:mm:ss" into seconds.
// return 0 on success, -1 on error. 
static int period_to_time(char *p, int *t)
{
    int neg, n, d, h, m, s;
    char *pc;
    // check if negative 
    if (*p == '-') {
        p++;
        neg = 1;
    } else {
        neg = 0;
    }
    // count number of elements
    pc = p;
    n = 1;
    while (*pc) {
        if (*pc++ == ':') {
            n++;
        }
    }
    // scan elements and check if all are positive 
    if (n == 3) {
        d = 0;
        n = sscanf(p, "%d:%d:%d", &h, &m, &s);
        if (n != 3 || h < 0 || m < 0 || s < 0) {
            return -1;
        }
    } else if (n == 4) {
        n = sscanf(p, "%d:%d:%d:%d", &d, &h, &m, &s);
        if (n != 4 || d < 0 || h < 0 || m < 0 || s < 0) {
            return -1;
        }
    } else {
        return -1;
    }
    // calculate seconds 
    *t = d * 24 * 3600 + h * 3600 + m * 60 + s;
    if (neg) {
        *t = -*t;
    }
    return 0;
}

// intern
// gh 2007-01-30
// convert seconds into a period
static char *time_to_period(int t)
{
    int d, h, m, s, neg;
    char *p = MALLOC(LOC_STRING_SIZE);
    if (t < 0) {
        t = -t;
        neg = 1;
    } else {
        neg = 0;
    }
    d = t / (24 * 3600);
    t -= d * 24 * 3600;
    h = t / 3600;
    t -= h * 3600;
    m = t / 60;
    s = t - m * 60;
    if (d) {
        sprintf(p, "%s%02d:%02d:%02d:%02d", neg ? "-" : "", d, h, m, s);
    } else {
        sprintf(p, "%s%02d:%02d:%02d", neg ? "-" : "", h, m, s);
    }
    return p;
}

// gh 2007-01-30
// return p1 + p2
char *addperiod(char *p1, char *p2)
{
    int t1, t2;
    if (!p1 || !p2 || period_to_time(p1, &t1) < 0 || period_to_time(p2, &t2) < 0) {
        return NULL;
    } else {
        return time_to_period(t1 + t2);
    }
}

// gh 2007-01-30
// p1 - p2
char *subperiod(char *p1, char *p2)
{
    int t1, t2;
    if (!p1 || !p2 || period_to_time(p1, &t1) < 0 || period_to_time(p2, &t2) < 0) {
        return NULL;
    } else {
        return time_to_period(t1 - t2);
    }
}

// gh 2007-01-30
// convert a period into seconds
int *period2seconds(char *p)
{
    int t;
    if (!p || period_to_time(p, &t) < 0) {
        return NULL;
    } else {
        int *i = MALLOC(sizeof(int));
        *i = t;
        return i;
    }
}

// gh 2007-01-30
// convert seconds into a period
char *seconds2period(int *t)
{
    if (!t) {
        return NULL;
    } else {
        return time_to_period(*t);
    }
}

// ct 2007-01-28
char* datetostr(ISC_QUAD *date, char *pat)
{
    if (!date || !pat)
        return NULL;
    else {
        char *result = (char *) MALLOC (LOC_STRING_SIZE);
        struct tm t;
        isc_decode_date(date, &t);
        if (!strftime(result, LOC_STRING_SIZE - 1, pat, &t)) result[0] = '\0';
        return result;
    }
}

// gh 2008-02-19
static char *replstr(char *s, char *subs, char *replby)
{
    char *sz_result;
    char *pd;
    int sl = strlen(subs);
    int rl = strlen(replby);
    sz_result = malloc(LOC_STRING_SIZE);
    pd = sz_result;
    while (*s) {
        if (strncmp(s, subs, sl) == 0) {
            strcpy(pd, replby);
            pd += rl;
            s += sl;
        } else {
            *pd++ = *s++;
        }
    }
    *pd = 0;
    return sz_result;
}

// gh 2008-02-19
// uk de fr it es pt nl no se dk fi hu ie ee is al va v1 c1 s1 s2 w1 b1 b2 n1 za fo lu w2
char *datetostrlang(ISC_QUAD *date, char *pat, char *language)
{
    if (!date || !pat || !language) {
        return NULL;
    } else {
        char *result = (char *) MALLOC (LOC_STRING_SIZE);
        struct tm t;
        int lang;
        isc_decode_date(date, &t);
        lang = decode_language(language);
        if (lang < 0) {
            strcpy(result, "???");
        } else {
            char *r1 = malloc(LOC_STRING_SIZE), *r2, *p;
            int i;
            char monthshort1[4], monthshort2[4];
            if (!strftime(r1, LOC_STRING_SIZE - 1, pat, &t)) {
                r1[0] = '\0';
            }
            for (i = 0; i < 7; i++) {
                if (strstr(r1, DOW[i])) {
                    r2 = replstr(r1, DOW[i], DOW[lang * 7 + i]);
                    free(r1);
                    r1 = r2;
                }
                p = strstr(r1, DOWSHORT[i]);
                if (p && strncmp(p, DOW[lang * 7 + i], strlen(DOW[lang * 7 + i]))) {
                    r2 = replstr(r1, DOWSHORT[i], DOWSHORT[lang * 7 + i]);
                    free(r1);
                    r1 = r2;
                }
            }
            for (i = 0; i < 12; i++) {
                if (strstr(r1, MONTH[i])) {
                    r2 = replstr(r1, MONTH[i], MONTH[lang * 12 + i]);
                    free(r1);
                    r1 = r2;
                }
                strncpy(monthshort1, MONTH[i], 3);
                monthshort1[3] = '\0';
                if (!strcmp(language, "fr") && monthshort1[1] == 'u') {
                    // Juin -> Jun, Juillet -> Jul 
                    monthshort1[2] = MONTH[i][3];
                }
                p = strstr(r1, monthshort1);
                if (p && strncmp(p, MONTH[lang * 12 + i], strlen(MONTH[lang * 12 + i]))) {
                    strncpy(monthshort2, MONTH[lang * 12 + i], 3);
                    monthshort2[3] = '\0';
                    if (!strcmp(language, "fr") && monthshort2[1] == 'u') {
                        // Juin -> Jun, Juillet -> Jul
                        monthshort2[2] = MONTH[lang * 12 + i][3];
                    }
                    r2 = replstr(r1, monthshort1, monthshort2);
                    free(r1);
                    r1 = r2;
                }
            }
            strcpy(result, r1);
            free(r1);
        }
        return result;
    }
}

// ct 2007-01-28
ISC_QUAD *firstdaymonth(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = 1;
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2007-01-28
ISC_QUAD *lastdaymonth(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = intern_daysofmonth(t.tm_mon + 1, t.tm_year + 1900);
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// gh
const double msec_in_day = 24. * 60. * 60. * ISC_TIME_SECONDS_PRECISION;

// ct 2007-02-01
double *timetodouble(ISC_QUAD * time)
{
    if (!time)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = (double) time->gds_quad_low / msec_in_day;
        return result;
    }
}

// ct 2007-02-01
double *datetodouble(ISC_QUAD * date)
{
    if (!date)
        return NULL;
    else {
        double *result = MALLOC(sizeof(double));
        *result = (double) date->gds_quad_high + (double) date->gds_quad_low / msec_in_day;
        return result;
    }
}

// ct 2007-02-01
ISC_QUAD * doubletodate(double * d)
{
    if (!d)
        return NULL;
    else {
        ISC_QUAD *result = (ISC_QUAD *) MALLOC (sizeof(ISC_QUAD));
        result->gds_quad_high = *d;
        *d = (*d - result->gds_quad_high) * msec_in_day;
        result->gds_quad_low = intern_zahlrunden(*d, 0);
        return result;
    }
}

// ct 2007-02-01
ISC_QUAD * doubletotime(double * d)
{
    if (!d)
        return NULL;
    else {
        ISC_QUAD *result = (ISC_QUAD *) MALLOC(sizeof(ISC_QUAD));
        result->gds_quad_high = *d;
        *d = (*d - result->gds_quad_high) * msec_in_day;
        result->gds_quad_high = 0;
        result->gds_quad_low = intern_zahlrunden(*d, 0);
        return result;
    }
}

// ct 2007-02-02
// gh 2008-02-22
ISC_QUAD * incdate (ISC_QUAD *date, int *d, int *m, int *y)
{
  if (!date || !d || !m || !y)
    return NULL;
  else
  {
    ISC_QUAD *result, *result1;
    struct tm t;
    int tm_mon, tm_year;
    isc_decode_date(date, &t);
    tm_mon = t.tm_mon + 12 * t.tm_year + *m;
    tm_year = tm_mon / 12;
    tm_mon %= 12;
    // Now, check that the month day is correctly adjusted.
    // 30 days has Sep, Apr, Jun, Nov all the rest have 31 except Feb
    if (tm_mon != FEBRUARY) {
      if (t.tm_mday > 30) {
        switch (tm_mon) {
        case APRIL:
        case JUNE:
        case SEPTEMBER:
        case NOVEMBER:
          t.tm_mday = 30;
        }
      }
    }
    else if (t.tm_mday > 28)
      t.tm_mday = isleap(tm_year+1900) ? 29 : 28;
    t.tm_mon = tm_mon;
    t.tm_year = tm_year;
    t.tm_year += *y;
    // If it's February, and the day of the month is 29, and
    // it's *not* a leap year, then adjust the day
    if (t.tm_mon == FEBRUARY && t.tm_mday > 28 && !isleap(t.tm_year+1900))
      t.tm_mday = 28;
    result = malloc(sizeof(ISC_QUAD));
    isc_encode_date(&t, result);
    result1 = addseconds(result, 24 * 60 * 60 * *d);
    free(result);
    return result1;
  }
}

// ct 2007-02-02
// gh 2008-02-22
ISC_QUAD * incdatetime (ISC_QUAD *date, int *d, int *m, int *y, int *h, int *min, int *s)
{
  if (!date || !d || !m || !y || !h || !min || !s)
    return NULL;
  else
  {
    ISC_QUAD *result, *result1;
    struct tm t;
    int tm_mon, tm_year;
    isc_decode_date(date, &t);
    tm_mon = t.tm_mon + 12 * t.tm_year + *m;
    tm_year = tm_mon / 12;
    tm_mon %= 12;
    // Now, check that the month day is correctly adjusted.
    // 30 days has Sep, Apr, Jun, Nov all the rest have 31 except Feb
    if (tm_mon != FEBRUARY) {
      if (t.tm_mday > 30) {
        switch (tm_mon) {
        case APRIL:
        case JUNE:
        case SEPTEMBER:
        case NOVEMBER:
          t.tm_mday = 30;
        }
      }
    }
    else if (t.tm_mday > 28)
      t.tm_mday = isleap(tm_year+1900) ? 29 : 28;
    t.tm_mon = tm_mon;
    t.tm_year = tm_year;
    t.tm_year += *y;
    // If it's February, and the day of the month is 29, and
    // it's *not* a leap year, then adjust the day
    if (t.tm_mon == FEBRUARY && t.tm_mday > 28 && !isleap(t.tm_year+1900))
      t.tm_mday = 28;
    result = malloc(sizeof(ISC_QUAD));
    isc_encode_date(&t, result);
    result1 = addseconds(result, (24 * 60 * 60 * *d) + (3600 * *h) + (60 * *min) + *s);
    free(result);
    return result1;
  }
}

// gh 2007-08-09
// intern
// weekday of 1st january
static int intern_wd11(ISC_QUAD *date, int k)
{
    struct tm t;
    ISC_QUAD d;
    isc_decode_date(date, &t);
    t.tm_mday = 1;
    t.tm_mon = 0;
    t.tm_year = t.tm_year + k;
    isc_encode_date(&t, &d);
    isc_decode_date(&d, &t);
    return t.tm_wday + 1;
}

// ct 2007-08-09
// gh 2008-02-12 made from old kalenderwoche
// intern
// calc week of year (german KW) ISO 8601
static int intern_kalenderwoche(ISC_QUAD * date)
{
        int dayofyear, weekday11, weekday11ly, weekday11ny, week;
        // calc day of year, 1st Jan = 1 ... 
        dayofyear = intern_dayofyear(date) - 1;
        // calc weekday of 1st Jan of this year, 1 = Sunday, 2 = Monday  ... 
        weekday11 = intern_wd11(date, 0);
        // change to 0 = Monday, 1 = Thuesday ... 6 = Sunday
        weekday11 = weekday11 == 1 ? 6 : weekday11 - 2;
        // calc weekday of 1st Jan last year, 1 = Sunday, 2 = Monday  ... 
        weekday11ly = intern_wd11(date, -1);
        // change to 0 = Monday, 1 = Thuesday ... 6 = Sunday
        weekday11ly = weekday11ly == 1 ? 6 : weekday11ly - 2;
        // calc weekday of 1st Jan next year, 1 = Sunday, 2 = Monday  ... 
        weekday11ny = intern_wd11(date, 1);
        // change to 0 = Monday, 1 = Thuesday ... 6 = Sunday
        weekday11ny = weekday11ny == 1 ? 6 : weekday11ny - 2;
        if (weekday11 > 3)
        {
           dayofyear -= (7 - weekday11);
        }
        else
        {
           dayofyear += weekday11;
        }
        if (dayofyear < 0)
        {
          if (   (weekday11 == 4) || (weekday11ly == 3))
          {
             week = 53;
          }
          else
          {
             week = 52;
          }
        }
        else
        {
             week = (dayofyear / 7) + 1;
        }
        if ((dayofyear > 360) && (week > 52))
        {
          if (weekday11 == 3)
          {
             week = 53;
          }
          else
          {
              if (weekday11ny == 4)
              {
                week = 53;
              }
              else
              {
                week = 1;
              }
          }
        }
        return week;
}

// gh 2008-02-12
int *kalenderwoche(ISC_QUAD * date)
{
    if (!date)
    {
        return NULL;
    }
    else
    {
        int *result = MALLOC(sizeof(int));
        *result = intern_kalenderwoche(date);
        return result;
    }
}

// christian floating holidays

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
// Weiberfastnacht /       = 52 days before easter sunday
ISC_QUAD *weiberfastnacht(int *year)
{
     if (!year) {
        return NULL;
     } else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * -52);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
// Carnival Monday / Rosenmontag / Veille du Mardi Gras = 48 days before easter sunday
ISC_QUAD *rosenmontag(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * -48);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Mardi Gras / Karnevalsdienstag / Mardi Gras            =  -47 days before easter sunday
ISC_QUAD *karnevalsdienstag(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * -47);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Ash Wednesday / Aschermittwoch / Mercredi des Cendres  =  -46 days before easter sunday
ISC_QUAD *aschermittwoch(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * -46);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Palm Sunday / Palmsonntag / Dimanche des Rameaux       =   -7 days before easter sunday
ISC_QUAD *palmsonntag(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * -7);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Easter Thursday / Gründonnerstag /             =   -3 days before easter sunday
ISC_QUAD *gruendonnerstag(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * -3);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Easter Friday / Karfreitag / Vendredi Saint            =   -2 days before easter sunday
ISC_QUAD *karfreitag(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * -2);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Easter Saturday / Ostersamstag / Samedi de Paques      =   -1 day before easter sunday
ISC_QUAD *ostersamstag(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * -1);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Easter Monday / Ostermontag / Lundi de Paques          =   +1 day after easter sunday
ISC_QUAD *ostermontag(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * 1);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Ascension of Christ / Christi Himmelfahrt / Ascension  =  +39 days after easter sunday
ISC_QUAD *himmelfahrt(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * 39);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Whitsunday / Pfingstsonntag / Dimanche de Pentecote    =  +49 days after easter sunday
ISC_QUAD *pfingstsonntag(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * 49);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Whitmonday / Pfingstmontag / Lundi de Pentecote        =  +50 days after easter sunday
ISC_QUAD *pfingstmontag(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * 50);
        free(ostern);
        return result;
     }
}

// ct 2007-07-22
// ct 2008-02-23 changed to intern_osterdatum
//  Feast of Corpus Christi / Fronleichnam / Fete-Dieu = +60 days after easter sunday
ISC_QUAD *fronleichnam(int *year)
{
     if (!year)
        return NULL;
     else {
        ISC_QUAD *ostern = intern_osterdatum(year);
        ISC_QUAD *result = addseconds(ostern, 24 * 60 * 60 * 60);
        free(ostern);
        return result;
     }
}

// ct 2008-11-08
// industrial minutes (round seconds to minute) between 2 timestamps
int *indminutesbetween(ISC_QUAD *timestamp1, ISC_QUAD *timestamp2)
{
     if (!timestamp1 || !timestamp2) 
         return NULL;
     else {
       int *result = MALLOC(sizeof(int));
       int seconds;
       double minutes;
       seconds = (int) intern_zeitdifferenz(timestamp2, timestamp1, "s");      
       minutes = (double) seconds / 60.0;
       // round seconds and made industry minutes from minutes
       if (minutes < 0) 
          minutes = intern_zahlrunden(-minutes, 0) / 0.6;
       else 
          minutes = intern_zahlrunden(minutes, 0) / 0.6;
       *result = intern_zahlrunden(minutes, 0);
       return result;
     }
} 

// ct/gh 2008-11-11
int *indminutesbetweenthr(ISC_QUAD *timestamp1, ISC_QUAD *timestamp2,
    ISC_TIME *min, int *usemin, ISC_TIME *max, int *usemax)
{
    if (!timestamp1 || !timestamp2 || !min || !usemin || !max || !usemax) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        int seconds;
        double minutes;
        int m1=0,m2=0,m3=0,m4=0;
        ISC_QUAD *tst1, *tst2;
        struct tm t1, t2, t3, t4;
        // bring timestamps in right order: 1st starttime, 2nd stoptime
        if ((timestamp1->isc_quad_high > timestamp2->isc_quad_high) ||
         ((timestamp1->isc_quad_high == timestamp2->isc_quad_high) &&
          (timestamp1->isc_quad_low > timestamp2->isc_quad_low))) {
            tst1 = timestamp2;
            tst2 = timestamp1;
        } else {
            tst1 = timestamp1;
            tst2 = timestamp2;
        }
        // decode param 1 and 2
        isc_decode_date(tst1, &t1);
        isc_decode_date(tst2, &t2);

        // use threshold or not ...
        // ... for starttime      
        if(!min) {
          t3.tm_hour = 0;
          t3.tm_min = 0;
        }
        else {
          // this doesn't run with InterBase
          isc_decode_sql_time(min, &t3);    
        }
        m1 = (t1.tm_hour * 60) + t1.tm_min;
        m3 = (t3.tm_hour * 60) + t3.tm_min;
        if (*usemin == 1 && m1 < m3) {
            t1.tm_hour = t3.tm_hour;
            t1.tm_min = t3.tm_min;
        }
        // ... for stoptime
        if(!max) {
          t4.tm_hour = 0;
          t4.tm_min = 0;
        }
        else {
          // this doesn't run with InterBase
          isc_decode_sql_time((ISC_TIME *) max, &t4); 
        }        
        m2 = (t2.tm_hour * 60) + t2.tm_min;
        m4 = (t4.tm_hour * 60) + t4.tm_min;      
        if (*usemax == 1 && m2 > m4) {
            t2.tm_hour = t4.tm_hour;
            t2.tm_min = t4.tm_min;
        }
        isc_encode_date(&t1, tst1);
        isc_encode_date(&t2, tst2);
        seconds = (int) intern_zeitdifferenz(tst2, tst1, "s");      
        minutes = (double) seconds / 60.0;
        // round seconds and made industry minutes from minutes
        if (minutes < 0) {
          minutes = intern_zahlrunden(-minutes, 0) / 0.6;
        } 
        else { 
          minutes = intern_zahlrunden(minutes, 0) / 0.6;
        }
        *result = intern_zahlrunden(minutes, 0);
        return result;
  }
}

// ct/gh 2009-01-20
// ugly version of indminutesbetweenthr made for InterBase
// because there's a bug in InterBase for ISC_TIME and isc_decode_sql_time
// so we use here ISC_QUAD and isc_decode_date
int *indminutesbetweenthrib(ISC_QUAD *timestamp1, ISC_QUAD *timestamp2,
    ISC_QUAD *min, int *usemin, ISC_QUAD *max, int *usemax)
{
    if (!timestamp1 || !timestamp2) {
        return NULL;
    } else {
        int *result = MALLOC(sizeof(int));
        int seconds;
        double minutes;
        int m1=0,m2=0,m3=0,m4=0;
        ISC_QUAD *tst1, *tst2;
        struct tm t1, t2, t3, t4;
        // bring timestamps in right order: 1st starttime, 2nd stoptime
        if ((timestamp1->isc_quad_high > timestamp2->isc_quad_high) ||
         ((timestamp1->isc_quad_high == timestamp2->isc_quad_high) &&
          (timestamp1->isc_quad_low > timestamp2->isc_quad_low))) {
            tst1 = timestamp2;
            tst2 = timestamp1;
        } else {
            tst1 = timestamp1;
            tst2 = timestamp2;
        }
        // decode param 1 and 2
        isc_decode_date(tst1, &t1);
        isc_decode_date(tst2, &t2);

        // use threshold or not ...
        // ... for starttime      
        if(!min) {
          t3.tm_hour = 0;
          t3.tm_min = 0;
        }
        else {
          isc_decode_date(min, &t3); 
          // cut everything with date
          t3.tm_year = 0;
          t3.tm_yday = 0;
          t3.tm_mon = 0;
          t3.tm_mday = 0;
          t3.tm_wday = 0;
          t3.tm_isdst = 0;
        }
        m1 = (t1.tm_hour * 60) + t1.tm_min;
        m3 = (t3.tm_hour * 60) + t3.tm_min;
        if (*usemin == 1 && m1 < m3) {
            t1.tm_hour = t3.tm_hour;
            t1.tm_min = t3.tm_min;
        }
        // ... for stoptime
        if(!max) {
          t4.tm_hour = 0;
          t4.tm_min = 0;
        }
        else {
          isc_decode_date(max, &t4); 
          // cut everything with date
          t4.tm_year = 0;
          t4.tm_yday = 0;
          t4.tm_mon = 0;
          t4.tm_mday = 0;
          t4.tm_wday = 0;
          t4.tm_isdst = 0;
        }        
        m2 = (t2.tm_hour * 60) + t2.tm_min;
        m4 = (t4.tm_hour * 60) + t4.tm_min;      
        if (*usemax == 1 && m2 > m4) {
            t2.tm_hour = t4.tm_hour;
            t2.tm_min = t4.tm_min;
        }
        isc_encode_date(&t1, tst1);
        isc_encode_date(&t2, tst2);
        seconds = (int) intern_zeitdifferenz(tst2, tst1, "s");      
        minutes = (double) seconds / 60.0;
        // round seconds and made industry minutes from minutes
        if (minutes < 0) {
          minutes = intern_zahlrunden(-minutes, 0) / 0.6;
        } 
        else { 
          minutes = intern_zahlrunden(minutes, 0) / 0.6;
        }
        *result = intern_zahlrunden(minutes, 0);
        return result;
  }
}

// ct 2008-12-22
ISC_QUAD *firstdayweek(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday + 1 - intern_dayofweek(ib_date);        
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2008-12-22
ISC_QUAD *firstdayweekiso(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday + 2 - intern_dayofweek(ib_date);        
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2008-12-22
ISC_QUAD *lastdayweek(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday + 7 - intern_dayofweek(ib_date);  
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2008-12-22
ISC_QUAD *lastdayweekiso(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday + 8 - intern_dayofweek(ib_date);  
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2008-12-23
ISC_QUAD *firstdayquarter(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = 1;
        switch (t.tm_mon / 3 + 1) {
          case 1:
            t.tm_mon = JANUARY;
            break;
          case 2:
            t.tm_mon = APRIL;
            break;
          case 3:
            t.tm_mon = JULY;
            break;
          case 4:
            t.tm_mon = OCTOBER;
            break;
        }    
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2008-12-23
ISC_QUAD *lastdayquarter(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        switch (t.tm_mon / 3 + 1) {
          case 1:
            t.tm_mday = 31;
            t.tm_mon = MARCH;
            break;
          case 2:
            t.tm_mday = 30;
            t.tm_mon = JUNE;
            break;
          case 3:
            t.tm_mday = 30;
            t.tm_mon = SEPTEMBER;
            break;
          case 4:
            t.tm_mday = 31;
            t.tm_mon = DECEMBER;
            break;
        }    
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2008-12-26
// ct 2010-04-11 bug in calculating ISO if 1st January is a Friday
// intern
static ISC_QUAD *intern_firstdaykw(int * kw, int * year, int iso)
{
     struct tm t;
     ISC_QUAD d;        
     // set to 1st January of input year
     t.tm_year = *year - 1900;
     t.tm_mon = JANUARY;   // JANUARY == 0
     t.tm_mday = 1;
     t.tm_hour = 0;
     t.tm_min = 0;
     t.tm_sec = 0;
     t.tm_yday = 0;
     // generate 1.1.year 
     isc_encode_date(&t, &d); 
     isc_decode_date(&d, &t);
     if (iso == 1) {
       // calculate ISO with 1st January not always KW 1 and 1st dayofweek is Monday
       switch (t.tm_wday) {
         case 0:
         case 1:
         case 2:
         case 3:
         case 4: 
           return addseconds(&d, ((24 * 3600 * (*kw - 1) * 7) - ((t.tm_wday - 1) * 3600 * 24)));
         case 5:
           return addseconds(&d, ((24 * 3600 * (*kw - 1) * 7) + (3 * 3600 * 24)));         
         case 6:
           return addseconds(&d, ((24 * 3600 * (*kw - 1) * 7) + (2 * 3600 * 24)));
	 default:
	   return 0;
       }
     } 
     else {
       // calculate US = 1st January always in KW 1 and 1st dayofweek is Sunday
       return addseconds(&d, ((24 * 3600 * (*kw - 1) * 7) - ((t.tm_wday) * 3600 * 24)));
     }
}

// ct 2008-12-26
ISC_QUAD *firstdaykw(int * kw, int * year)
{
    if (!kw || !year)
        return NULL;
    else {
        ISC_QUAD *result = malloc(sizeof(ISC_QUAD));
        result = intern_firstdaykw(kw, year, 1);
        return result;    
    }
}

// ct 2008-12-26
ISC_QUAD *lastdaykw(int * kw, int * year)
{
    if (!kw || !year)
        return NULL;
    else {
        ISC_QUAD *result = malloc(sizeof(ISC_QUAD));
        result = addseconds(intern_firstdaykw(kw, year, 1), 6 * 24 * 3600);
        return result;
    }
}

// ct 2008-12-26
ISC_QUAD *firstdaywoy(int * kw, int * year)
{
    if (!kw || !year)
        return NULL;
    else {
        ISC_QUAD *result = malloc(sizeof(ISC_QUAD));
        result = intern_firstdaykw(kw, year, 0);
        return result;    
    }
}

// ct 2008-12-26
ISC_QUAD *lastdaywoy(int * kw, int * year)
{
    if (!kw || !year)
        return NULL;
    else {
        ISC_QUAD *result = malloc(sizeof(ISC_QUAD));
        result = addseconds(intern_firstdaykw(kw, year, 0), 6 * 24 * 3600);
        return result;
    }
}

// ct 2009-03-11
ISC_QUAD *firstdaylastquarter(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = 1;
        switch (t.tm_mon / 3) {
          case 0:
            t.tm_mon = OCTOBER;
            t.tm_year = t.tm_year - 1;
            break;
          case 1:
            t.tm_mon = JANUARY;
            break;
          case 2:
            t.tm_mon = APRIL;
            break;
          case 3:
            t.tm_mon = JULY;
            break;
        }           
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-03-11
ISC_QUAD *lastdaylastquarter(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        switch (t.tm_mon / 3) {
          case 0:
            t.tm_mday = 31;
            t.tm_mon = DECEMBER;
            t.tm_year = t.tm_year - 1;
            break;
          case 1:
            t.tm_mday = 31;
            t.tm_mon = MARCH;
            break;
          case 2:
            t.tm_mday = 30;
            t.tm_mon = JUNE;
            break;
          case 3:
            t.tm_mday = 30;
            t.tm_mon = SEPTEMBER;
            break;
        }    
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-03-11
ISC_QUAD *firstdaynextquarter(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = 1;
        switch (t.tm_mon / 3 + 4) {          
          case 4:
            t.tm_mon = APRIL;
            break;
          case 5:
            t.tm_mon = JULY;
            break;
          case 6:
            t.tm_mon = OCTOBER;
            break;
          case 7:
            t.tm_mon = JANUARY;
            t.tm_year = t.tm_year + 1;
            break;            
        }    
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-03-11
ISC_QUAD *lastdaynextquarter(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        switch (t.tm_mon / 3 + 4) {
          case 4:
            t.tm_mday = 30;
            t.tm_mon = JUNE;
            break;
          case 5:
            t.tm_mday = 30;
            t.tm_mon = SEPTEMBER;
            break;
          case 6:
            t.tm_mday = 31;
            t.tm_mon = DECEMBER;
            break;
          case 7:
            t.tm_mday = 31;
            t.tm_mon = MARCH;
            t.tm_year = t.tm_year + 1;
            break;
        }    
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *firstdaylastweek(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday - 6 - intern_dayofweek(ib_date);        
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *firstdaylastweekiso(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday - 5 - intern_dayofweek(ib_date);        
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *lastdaylastweek(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday - intern_dayofweek(ib_date);  
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *lastdaylastweekiso(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday + 1 - intern_dayofweek(ib_date);  
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *firstdaynextweek(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday + 8 - intern_dayofweek(ib_date);        
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *firstdaynextweekiso(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday + 9 - intern_dayofweek(ib_date);        
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *lastdaynextweek(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday + 14 - intern_dayofweek(ib_date);  
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *lastdaynextweekiso(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        t.tm_mday = t.tm_mday + 15 - intern_dayofweek(ib_date);  
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *firstdaylastmonth(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        if (t.tm_mon == 0) {
          t.tm_mon = 11;
          t.tm_year = t.tm_year - 1;
        } else {  
          t.tm_mon = t.tm_mon - 1;
        }          
        t.tm_mday = 1;
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *lastdaylastmonth(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        if (t.tm_mon == 0) {
          t.tm_mon = 11;
          t.tm_year = t.tm_year - 1;
        } else {  
          t.tm_mon = t.tm_mon - 1;
        }          
        t.tm_mday = intern_daysofmonth(t.tm_mon + 1, t.tm_year + 1900);
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *firstdaynextmonth(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        if (t.tm_mon > 11) {
          t.tm_mon = 0;
          t.tm_year = t.tm_year + 1;
        } else {  
          t.tm_mon = t.tm_mon + 1;
        }          
        t.tm_mday = 1;
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// ct 2009-05-08
ISC_QUAD *lastdaynextmonth(ISC_QUAD * ib_date)
{
    if (!ib_date)
        return NULL;
    else {
        struct tm t;
        isc_decode_date(ib_date, &t);
        if (t.tm_mon > 11) {
          t.tm_mon = 0;
          t.tm_year = t.tm_year + 1;
        } else {  
          t.tm_mon = t.tm_mon + 1;
        }          
        t.tm_mday = intern_daysofmonth(t.tm_mon + 1, t.tm_year + 1900);
        t.tm_sec = 0;
        t.tm_min = 0;
        t.tm_hour = 0;
        t.tm_isdst = 0;
        return gen_ib_date(&t);
    }
}

// gh 2010-12-03
// determin deviation in hours to UTC WITHOUT regard of summertime
double *tzvalue_to_utc()
{
    struct tm *tm;
    time_t t;
    double *res = MALLOC(sizeof(double));

    t = time(NULL);
    tm = localtime(&t);

    *res = (double)timezone / 3600.0;
    return res;
}

// due to code resons this function is placed in date_functions insteade of conv_functions
// tt 2010-12-02 Timo Tegtmeier
// convert a given local timestamp to UTC time
ISC_QUAD *local2utc(ISC_QUAD *ib_date) {
    if (!ib_date)
        return NULL;
    else {
        struct tm local, *utc;
		time_t tmp;

        isc_decode_date(ib_date, &local);
		tmp = mktime(&local);
		utc = gmtime(&tmp);
		return gen_ib_date(utc);
	}
}

// due to code resons this function is placed in date_functions insteade of conv_functions
// tt 2010-12-02 Timo Tegtmeier
// convert a UTC timestamp to local time
ISC_QUAD *utc2local(ISC_QUAD *ib_date) {
    if (!ib_date)
        return NULL;
    else {
        struct tm utc, *local;
		time_t tmp;

        isc_decode_date(ib_date, &utc);
		tmp = mktime(&utc) - timezone;
		local = localtime(&tmp);
		return gen_ib_date(local);
	}
}
