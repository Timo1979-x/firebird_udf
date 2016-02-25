/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 * some code from Robert Loipfinger rl copyright (c) 2000 ADITO Software GmbH
 *
 * File Name:   utf8_functions.c
 * Description: this module contains UDFs to generate and manipulate strings,
 * BLObs, numeric and date/time for charset UTF-8 
 * for exported function list see utf8_functions.h
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "global.h"
#include "blob_functions.h"
#include "utf8_functions.h"

// For UTF-8 include header files from the ICU project http://site.icu-project.org/
#include "unicode/ustring.h"
#include "unicode/uchar.h"
#include "unicode/ustdio.h"
#include "unicode/udat.h"

// *** internal functions ***

// gh 2009-09-01
// gh 2010-11-25 check of proper utf8 and avoid potential overflow
// Convert an UTF-8 string handed to an UDF-Function by the Database into an
// UChar string that can be manipulated using the u_... functions from ICU.
// If c is greater than -1, only the first c characters are returned. If retlen
// is not NULL, the length of the returned string is stored into *retlen. The
// returned string is zero terminated and the pointer should be free()ed after
// use.

static UChar *utf82uchar(char *str, int c, int *retlen)
{
    UChar *res;
    UErrorCode err = 0;
    int32_t len = -1;

    u_strFromUTF8(NULL, 0, &len, str, -1, &err);
    if (len < 0) {
	return NULL;
    }
    if (c > -1 && len > c) {
	len = c;
    }
    if (retlen) {
	*retlen = len;
    }
    res = malloc((len + 1) * sizeof(UChar));
    err = 0;
    u_strFromUTF8(res, len, NULL, str, -1, &err);
    *(res + len) = (UChar)0;
    return res;
}

// gh 2009-09-01/-14
// Convert an UChar string back into an UTF-8 string that can be returned to
// the database. The returned string is zero terminated and the pointer should
// be freed by the database (declare UDF with FREE_IT).

static char *uchar2utf8(UChar *str, int do_MALLOC)
{
    char *res;
    UErrorCode err;
    int32_t len;

    err = 0;
    u_strToUTF8(NULL, 0, &len, str, -1, &err);
    res = do_MALLOC ? MALLOC(len + 1) : malloc(len + 1);
    err = 0;
    u_strToUTF8(res, len, NULL, str, -1, &err);
    *(res + len) = (char)0;
    return res;
}

// *** string_functions edit ***

// gh 2009-09-01/-14
// Return the cnt left chars from str.
// utf8-version of left from string_functions

char *u_left(char *str, int *cnt)
{
    if (!str || !cnt) {
	return NULL;
    } else {
	int c = (*cnt > 0) ? *cnt : 0;
	UChar *tmp;
	char *res;

	tmp = utf82uchar(str, c, NULL);
	res = uchar2utf8(tmp, 1);
	free(tmp);
	return res;
    }
}

// gh 2009-09-01/-14
// Return the cnt right chars from str.
// utf8-version of right from string_functions

char *u_right(char *str, int *cnt)
{
    if (!str || !cnt) {
	return NULL;
    } else {
	int c = (*cnt > 0) ? *cnt : 0;
	UChar *tmp, *tmp2;
	char *res;
	int len, i, j;

	tmp = utf82uchar(str, -1, &len);
	tmp2 = malloc((len + 1) * sizeof(UChar));

	for (j = 0, i = (c > len) ? 0 : len - c; i < len; i++, j++) {
	    tmp2[j] = tmp[i];
	}
	tmp2[j] = (UChar)0;

	res = uchar2utf8(tmp2, 1);
	free(tmp);
	free(tmp2);
	return res;
    }
}

// gh 2009-09-01/-14
// Return cnt chars from str, starting at position ind.
// utf8-version of mid from string_functions

char *u_mid(char *str, int *ind, int *cnt)
{
    if (!str || !ind || !cnt) {
	return NULL;
    } else {
	int i = (*ind > 0) ? *ind : 0;
	int c = (*cnt > 0 && *ind >= 0) ? *cnt : 0;
	UChar *tmp, *tmp2;
	char *res;
	int len, j;

	tmp = utf82uchar(str, -1, &len);
	tmp2 = malloc((len + 1) * sizeof(UChar));

	for (j = i; (j < len) && (j - i < c); j++) {
	    tmp2[j - i] = tmp[j];
	}
	tmp2[j - i] = (UChar)0;

	res = uchar2utf8(tmp2, 1);
	free(tmp);
	free(tmp2);
	return res;
    }
}

// gh 2009-09-01/-14
// Left-pad str with pad up to a length of size
// utf8-version of padleft from string_functions

char *u_padleft(char *str, char *pad, int *size)
{
    if (!str || !pad || !size) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_pad, *tmp_res;
	char *res;
	int len_str, len_pad, len_res, diff;

	tmp_str = utf82uchar(str, -1, &len_str);
	tmp_pad = utf82uchar(pad, -1, &len_pad);

	len_res = *size > len_str ? *size : len_str;
	tmp_res = malloc((len_res + 1) * sizeof(UChar));
	*tmp_res = (UChar)0;

	if (len_pad > 0) {
	    diff = len_res - len_str;
	    while (diff > 0) {
		u_strncat(tmp_res, tmp_pad, diff);
		diff -= len_pad;
	    }
	}

	u_strcat(tmp_res, tmp_str);

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_pad);
	free(tmp_res);
	return res;
    }
}

// gh 2009-09-01/-14
// Right-pad str with pad up to a length of size.
// utf8-version of padright from string_functions

char *u_padright(char *str, char *pad, int *size)
{
    if (!str || !pad || !size) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_pad, *tmp_res;
	char *res;
	int len_str, len_pad, len_res, diff;

	tmp_str = utf82uchar(str, -1, &len_str);
	tmp_pad = utf82uchar(pad, -1, &len_pad);

	len_res = *size > len_str ? *size : len_str;
	tmp_res = malloc((len_res + 1) * sizeof(UChar));
	u_strcpy(tmp_res, tmp_str);

	if (len_pad > 0) {
	    diff = len_res - len_str;
	    while (diff > 0) {
		u_strncat(tmp_res, tmp_pad, diff);
		diff -= len_pad;
	    }
	}

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_pad);
	free(tmp_res);
	return res;
    }
}

// gh 2009-09-01/-14
// Left-right-pad str with pad up to a length of size.
// utf8-version of padcenter from string_functions

char *u_padcenter(char *str, char *pad, int *size)
{
    if (!str || !pad || !size) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_pad, *tmp_res;
	char *res;
	int len_str, len_pad, len_res, diff_left, diff_right;

	tmp_str = utf82uchar(str, -1, &len_str);
	tmp_pad = utf82uchar(pad, -1, &len_pad);

	len_res = *size > len_str ? *size : len_str;
	tmp_res = malloc((len_res + 1) * sizeof(UChar));
	*tmp_res = (UChar)0;

        if (((*size - len_str) % 2) == 0) {
            // even number of chars to pad
            diff_left = diff_right = (*size - len_str) / 2;
        } else {
            // odd number of chars: pad one more char on the left side
            diff_left = (*size - len_str + 1) / 2;
            diff_right = diff_left - 1;
        }

	if (len_pad > 0) {
	    while (diff_left > 0) {
		u_strncat(tmp_res, tmp_pad, diff_left);
		diff_left -= len_pad;
	    }
	}

	u_strcat(tmp_res, tmp_str);

	if (len_pad > 0) {
	    while (diff_right > 0) {
		u_strncat(tmp_res, tmp_pad, diff_right);
		diff_right -= len_pad;
	    }
	}

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_pad);
	free(tmp_res);
	return res;
    }
}

// gh 2009-09-03
// return position of sub in str
// utf8-version of substrnull (not substr!) from string_functions

int *u_substr(char *str, char *sub)
{
    if (!sub || !str) {
        return NULL;
    } else {
	UChar *tmp_str, *tmp_sub, *pc;
	int len_str, len_sub, *res;

	tmp_str = utf82uchar(str, -1, &len_str);
	tmp_sub = utf82uchar(sub, -1, &len_sub);

        if (!(pc = u_strstr(tmp_str, tmp_sub))) {
            res = NULL;
        } else {
            res = MALLOC(sizeof(int));
            *res = pc - tmp_str;
        }

	free(tmp_str);
	free(tmp_sub);
	return res;
    }
}

// gh 2009-09-03/-14
// remove character at position ind from str
// utf8-version of strrm from string_functions

char *u_strrm(char *str, int *pos)
{
    if (!str || !pos || *str == 0) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_res;
	int len_str, i, j;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);
	tmp_res = malloc((len_str + 1) * sizeof(UChar));

	for (i = 0, j = 0; i < len_str; i++) {
	    if (i != *pos) {
		tmp_res[j] = tmp_str[i];
		j++;
	    }
	}
	tmp_res[j] = (UChar)0;

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_res);
	return res;
    }
}

// gh 2009-09-14
// ct 2010-03-10 changed calculation of len
// internal: used in u_replacestring and u_blobreplacestring.

UChar *intern_u_replacestring(UChar *str, int len_str, UChar *subs, int len_subs,
    UChar *replby, int len_replby, int all, int case_insensitive)
{
    UChar *tmp_res, *p, *q;
    int done = 0;

    if (len_subs == 0) {
	tmp_res = malloc(sizeof(UChar));
	*tmp_res = (UChar)0;
    } else {
// old	int len = (len_str / len_subs + 1) * (len_replby + 1) + 1;
        int len = len_str + 1;
        if (len_replby > len_subs) {
           len += (len_str / len_subs) * (len_replby - len_subs);
        } 
	tmp_res = malloc(len * sizeof(UChar));
	p = tmp_res;
	q = str;
	while (*q) {
	    if ((!done || all) &&
		    ((!case_insensitive && !u_strncmp(q, subs, len_subs)) ||
		    (case_insensitive && !u_strncasecmp(q, subs, len_subs, 0)))) {
		u_strcpy(p, replby);
		p += len_replby;
		q += len_subs;
		done = 1;
	    } else {
		*p++ = *q++;
	    }
	}
	*p = (UChar)0;
    }

    return tmp_res;
}

// gh 2009-09-03/-14
// replaces string2 with string3 in string1 one/many times case sensitiv/intensitiv
// utf8-version of replacestring from string_functions

char *u_replacestring(char *str, char *subs, char *replby, int *all,
    int *case_insensitive)
{
    if (!str || !subs || !replby || !all || !case_insensitive) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_subs, *tmp_replby, *tmp_res;
	int len_str, len_subs, len_replby;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);
	tmp_subs = utf82uchar(subs, -1, &len_subs);
	tmp_replby = utf82uchar(replby, -1, &len_replby);

	tmp_res = intern_u_replacestring(tmp_str, len_str, tmp_subs, len_subs,
	    tmp_replby, len_replby, *all, *case_insensitive);

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_subs);
	free(tmp_replby);
	free(tmp_res);
	return res;
    }
}

// gh 2009-09-03/-14
// reverse output of input string
// utf8-version of reverse from string_functions

char *u_reverse(char *str)
{
    if (!str) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_res;
	int len_str, i, j;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);
	tmp_res = malloc((len_str + 1) * sizeof(UChar));

	for (i = len_str - 1, j = 0; i >= 0; i--, j++) {
	    tmp_res[j] = tmp_str[i];
	}
	tmp_res[j] = (UChar)0;

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_res);
	return res;
    }
}

// gh 2009-09-03
// check if c occurs in str
// utf8-version of internal findchar from string_functions
// internal: used in u_strip

static int u_findchar(UChar c, UChar *str)
{
    int i = 0, len = u_strlen(str);

    while ((i < len) && (c != str[i])) {
	i++;
    }

    return (i < len) ? 1 : 0;
}

// gh 2009-09-03/-14
// if !reverse, remove all characters in strip from str
// if reverse, remove all chars that are _not_ in strip from string
// internal: used in stripstring and stripstringhold

static char *u_strip(char *str, char *strip, int reverse)
{
    if (!str || !strip) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_strip, *tmp_res;
	int len_str, i, j;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);
	tmp_strip = utf82uchar(strip, -1, NULL);
	tmp_res = malloc((len_str + 1) * sizeof(UChar));

	for (i = 0, j = 0; i < len_str; i++) {
	    if ((!reverse && !u_findchar(tmp_str[i], tmp_strip))
		    || (reverse && u_findchar(tmp_str[i], tmp_strip))) {
		tmp_res[j++] = tmp_str[i];
	    }
	}
	tmp_res[j] = (UChar)0;

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_res);
	return res;
    }
}

// gh 2009-09-03
// strips string from all characters which are content of parameter 2
// utf8-version of stripstring from string_functions

char *u_stripstring(char *str, char *strip)
{
    return u_strip(str, strip, 0);
}

// gh 2009-09-03
// strips string from all characters which are NOT content of parameter 2
// utf8-version of stripstringhold from string_functions

char *u_stripstringhold(char *str, char *strip)
{
    return u_strip(str, strip, 1);
}

// gh 2009-09-04/-14
// remove all multiple occurences of trim from str
// only the first character of trim is used
// utf8-version of repeattrim from string_functions

char *u_repeattrim(char *str, char *trim)
{
    if (!str || !trim || !trim[0]) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_trim, *tmp_res, *s1, *s2;
	int len_str;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);
	if (!tmp_str) {
	    return NULL;
	}
	tmp_trim = utf82uchar(trim, -1, NULL);
	if (!tmp_trim) {
	    free(tmp_str);
	    return NULL;
	}
	tmp_res = malloc((len_str + 1) * sizeof(UChar));

	s1 = tmp_str;
	s2 = tmp_res;
	while (*s1) {
	    while (*s1 && (*s1 != tmp_trim[0])) {
		*s2++ = *s1++;
	    }
	    if (*s1) {
		*s2++ = *s1++;
	    }
	    while (*s1 && (*s1 == tmp_trim[0])) {
		s1++;
	    }
	}
	*s2 = (UChar)0;

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_trim);
	free(tmp_res);
	return res;
    }
}

// gh 2009-09-03/-14
// at position pos, count characters from string are replaced with repl
// utf8-version of strstuff from string_functions

char *u_strstuff(char *str, int *pos, int *count, char *repl)
{
    if (!str || !pos || !count || !repl) {
        return NULL;
    } else {
	UChar *tmp_str, *tmp_repl, *tmp_res;
        int len_str, len_repl, len_res, i = 0, j = 0, l = 0, n;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);
	tmp_repl = utf82uchar(repl, -1, &len_repl);
        len_res = len_str + len_repl + 1;
        tmp_res = malloc(len_res * sizeof(UChar));

        n = (*pos > len_str) ? len_str : ((*pos > 0) ? *pos - 1 : 0);
        while (l < n) {
            tmp_res[l++] = tmp_str[j++];
        }
        while (tmp_repl[i]) {
            tmp_res[l++] = tmp_repl[i++];
        }
        j = (j + *count < len_str) ? (j + *count) : len_str;
        while (tmp_str[j] && l < len_res) {
            tmp_res[l++] = tmp_str[j++];
        }
        tmp_res[l] = (UChar)0;

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_repl);
	free(tmp_res);
        return res;
    }
}

// gh 2009-09-03/-14
// if pred, return nth predecessor in character-set of first character from str
// if !pred, return nth successor in character-set of first character from str
// utf8-version of predecessor from string_functions

static char *u_pred_succ(char *str, int *n, int pred)
{
    if (!str || !n) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_res;
	char *res;

	tmp_str = utf82uchar(str, -1, NULL);
        tmp_res = malloc(2 * sizeof(UChar));

	tmp_res[0] = pred ? (tmp_str[0] - *n) : (tmp_str[0] + *n);
	tmp_res[1] = (UChar)0;

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_res);
        return res;
    }
}

// gh 2009-09-03
// utf8-versions of predecessor from string_functions

char *u_predecessor(char *str, int *n)
{
    return u_pred_succ(str, n, 1);
}

// gh 2009-09-03
// utf8-versions of successor from string_functions

char *u_successor(char *str, int *n)
{
    return u_pred_succ(str, n, 0);
}

// *** string_functions convert ***

// gh 2009-09-03/-14
// return upper or lower case version of str
// intern: used in u_lower and u_upper

static char *u_low_up(char *str, int up)
{
    if (!str) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_res;
	UErrorCode err = 0;
	int len_str;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);
        tmp_res = malloc((len_str + 1) * sizeof(UChar));

	if (up) {
	    u_strToUpper(tmp_res, len_str + 1, tmp_str, -1, "", &err);
	} else {
	    u_strToLower(tmp_res, len_str + 1, tmp_str, -1, "", &err);
	}

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_res);
        return res;
    }
}

// gh 2009-09-03
// utf8-versions of lower from string_functions

char *u_lower(char *str)
{
    return u_low_up(str, 0);
}

// gh 2009-09-03
// utf8-versions of upper from string_functions

char *u_upper(char *str)
{
    return u_low_up(str, 1);
}

// gh 2009-09-03/-14
// propercase str: 1st character of word uppercase, all others lower
// utf8-version of propercase from string_functions

char *u_propercase(char *str)
{
    if (!str) {
	return NULL;
    } else {
	UChar *tmp_str;
	int len_str, i, word_begin;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);

	for (i = 0, word_begin = 1; i < len_str; i++) {
	    if (u_isalnum(tmp_str[i]) && word_begin) {
		word_begin = 0;
		tmp_str[i] = u_toupper(tmp_str[i]);
	    } else if (!u_isalnum(tmp_str[i])) {
		word_begin = 1;
	    } else {
		tmp_str[i] = u_tolower(tmp_str[i]);
	    }
	}

	res = uchar2utf8(tmp_str, 1);
	free(tmp_str);
        return res;
    }
}

// ct 2009-09-08
// ISO 9 transliteration table cyrillic-latin
// transliteration of russian, bulgarian, serbo-croatian, ukrainian and
// belorussian to latin alphabet  from utf8 1024 to 1299 (utf-8 range for
// cyrillic letters) - not all used for transliteration
// used in u_cyrillic_latin

static UChar cyrillic_latin[304] = {
    // 1024 - 1039
    0x0020, 0x00cb, 0x0110, 0x0020, 0x00ca, 0x0020, 0x00cc, 0x00cf, 0x004a, 0x01c8, 0x01cb, 0x0106, 0x0020, 0x0020, 0x0020, 0x01c5,
    // 1040 CYRILLIC CAPITAL LETTER A - 1055
    0x0041, 0x0042, 0x0056, 0x0047, 0x0044, 0x0045, 0x017d, 0x005a, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f, 0x0050,
    // 1056 - 1071 CYRILLIC CAPITAL LETTER YA
    0x0052, 0x0053, 0x0054, 0x0055, 0x0046, 0x0048, 0x0043, 0x010c, 0x0160, 0x015c, 0x02ba, 0x0059, 0x02b9, 0x00c8, 0x00db, 0x00c2,
    // 1072 CYRILLIC SMALL LETTER A - 1087
    0x0061, 0x0062, 0x0076, 0x0067, 0x0064, 0x0065, 0x017e, 0x007a, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f, 0x0070,
    // 1088 - 1103
    0x0072, 0x0073, 0x0074, 0x0075, 0x0066, 0x0068, 0x0063, 0x010d, 0x0161, 0x015d, 0x02ba, 0x0079, 0x02b9, 0x00e8, 0x00fb, 0x00e2,
    // 1104 - 1119
    0x0020, 0x00eb, 0x0111, 0x0020, 0x00ea, 0x0020, 0x00ec, 0x00ef, 0x006a, 0x01c9, 0x01cc, 0x0107, 0x0020, 0x0020, 0x0020, 0x01c6,
    // 1120 - 1135
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    // 1136 - 1151
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    // 1152 - 1167
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    // 1168 - 1183
    0x0047, 0x0067, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    // 1184 -
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020,
    0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020, 0x0020
};

// gh 2009-09-09/-14
// ISO 9 transliteration for cyrillic characters to latin characters
// similar to rlatin from string_functions

char *u_cyrillic_latin(char *str)
{
    if (!str) {
	return NULL;
    } else {
	UChar *tmp_str;
	int len_str, i;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);

	for (i = 0; i < len_str; i++) {
	    if (tmp_str[i] >= 1024 && tmp_str[i] <= 1328) {
		tmp_str[i] = cyrillic_latin[tmp_str[i] - 1024];
	    }
	}

	res = uchar2utf8(tmp_str, 1);
	free(tmp_str);
        return res;
    }
}

// gh 2009-09-03/-14
// return a string with character number c
// utf8-version of character from string_functions

char *u_character(int *c)
{
    if (!c) {
	return NULL;
    } else {
	UChar *tmp_str = malloc(2 * sizeof(UChar));
	char *res;

	tmp_str[0] = (UChar)*c;
	tmp_str[1] = (UChar)0;

	res = uchar2utf8(tmp_str, 1);
	free(tmp_str);
        return res;
    }
}

// gh 2009-09-03
// return UTF8-code-no of first character of str
// utf8-version of ord from string_functions

int *u_ord(char *str)
{
    if (!str) {
	return NULL;
    } else {
	UChar *tmp_str;
	int *res;

	tmp_str = utf82uchar(str, -1, NULL);

	res = MALLOC(sizeof(int));
	*res = tmp_str[0];
	free(tmp_str);
	return res;
    }
}

// *** string_functions compare ***

// gh 2009-09-03
// compares 2 strings
// output < 0 : string 1 smaller than string 2
// output = 0 : string 1 equal to string 2
// output > 0 : string 1 greater than string 2
// utf8-version of r_strcmp from string_functions

int *u_rstrcmp(char *str1, char *str2)
{
    if (!str1 || !str2) {
	return NULL;
    } else {
	UChar *tmp_str1, *tmp_str2;
	int *res;

	tmp_str1 = utf82uchar(str1, -1, NULL);
	tmp_str2 = utf82uchar(str2, -1, NULL);

	res = MALLOC(sizeof(int));
	*res = u_strcmpCodePointOrder(tmp_str1, tmp_str2);

	free(tmp_str1);
	free(tmp_str2);
	return res;
    }
}

// gh 2009-09-03
// return the length of a string
// utf8-version of stringlength from string_functions

int *u_stringlength(char *str)
{
    if (!str) {
	return NULL;
    } else {
	UChar *tmp_str;
	int len_str, *res;

	tmp_str = utf82uchar(str, -1, &len_str);

	res = MALLOC(sizeof(int));
	*res = len_str;
	free(tmp_str);
	return res;
    }
}

// gh 2009-09-09/-14
// Starting at position index in str, chop the string at the last blank before
// position index + col_width. If no blank is found, string is chopped at
// index + col_width.
// utf8-version of linewrap from string_functions

char *u_linewrap(char *str, int *index, int *col_width)
{
    if (!str || !index || !col_width) {
	return NULL;
    } else {
	UChar *tmp_str, *first, *last;
	int len_str, ind, width;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);
	ind = (*index > 0) ? *index : 0;
	width = (*col_width > 0) ? *col_width : 0;

	if (ind > len_str) {
	    ind = len_str;
	}
	if (ind + width > len_str) {
	    width = len_str - ind;
	}
	
	first = tmp_str + ind;
	last = first + width;
	if (*last) {
	    while (last > first && !u_isspace(*last)) {
		last--;
	    }
	}
	if (last == first) {
	    last = first + width;
	}
	*last = (UChar)0;

	res = uchar2utf8(first, 1);
	free(tmp_str);
        return res;
    }
}

// gh 2009-09-09/-14
// In str, replace every source[i] with target[i].
// utf8-version of convertsymbols from string_functions

char *u_convertsymbols(char *str, char *source, char *target)
{
    if (!str || !source || !target) {
	return NULL;
    } else {
	UChar *tmp_str, *tmp_src, *tmp_trg, *tmp_res, *p1, *p2, *p;
	int len_str, len_src, len_trg;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);
	tmp_src = utf82uchar(source, -1, &len_src);
	tmp_trg = utf82uchar(target, -1, &len_trg);

	if (len_src > len_trg) {
	    free(tmp_str);
	    free(tmp_src);
	    free(tmp_trg);
	    return NULL;
	}

	tmp_res = malloc((len_str + 1) * sizeof(UChar));

	p1 = tmp_str;
	p2 = tmp_res;
	while (*p1) {
	    if ((p = u_strchr(tmp_src, *p1))) {
		*p2++ = tmp_trg[p - tmp_src];
		p1++;
	    } else {
		*p2++ = *p1++;
	    }
	}
	*p2 = (UChar)0;

	res = uchar2utf8(tmp_res, 1);
	free(tmp_str);
	free(tmp_src);
	free(tmp_trg);
	free(tmp_res);
	return res;
    }
}

// gh 2009-09-09/-14
// Find next word in str, starting at position n.
// utf8-version of findword from string_functions

char *u_findword(char *str, int *n)
{
    if (!str || !n) {
	return NULL;
    } else {
	UChar *tmp_str, *first, *last;
	int len_str, pos;
	char *res;

	tmp_str = utf82uchar(str, -1, &len_str);
	pos = (*n < 0) ? 0 : *n;

	if (pos >= len_str) {
	    free(tmp_str);
	    return NULL;
	}

	first = tmp_str + pos;
	while (*first && u_isspace(*first)) {
	    first++;
	}
	last = first;
	while (*last && !u_isspace(*last)) {
	    last++;
	}
	*last = (UChar)0;

	res = uchar2utf8(first, 1);
	free(tmp_str);
	return res;
    }
}

// gh 2010-11-23
// utf8-version of nbsp
char *u_nbsp()
{
    char *res = MALLOC(3);

    res[0] = 0xc2;
    res[1] = 0xa0;
    res[2] = 0x00;

    return res;
}

// ct 2010-11-26
// narrow no-break space - ONLY Unicode !
char *u_nnbsp()
{
    char *res = MALLOC(4);

    res[0] = 0xe2;
    res[1] = 0x80;
    res[2] = 0xaf;
    res[3] = 0x00;

    return res;
}

// maybe later or if asked for
//F_COLLATEBR or F_KEYUP
//F_GSOUNDEX or something similar
//F_WORDNUM
//F_FINDWORDINDEX
//F_STR2EXCEL

// *** numeric functions ***

// gh 2009-09-14
// Format a double value with 2 decimal places and an euro sign
char *u_euroval(double *value)
{
    if (!value) {
	return NULL;
    } else {
	UChar *tmp_res = malloc(35 * sizeof(UChar));
	char *res;

	u_snprintf(tmp_res, 34, "%.2f %C", *value, 0x20AC);

	res = uchar2utf8(tmp_res, 1);
	free(tmp_res);
	return res;
    }
}

// maybe later or if asked for
//F_NUMINWORDSM

// *** BLOb functions ***

// gh 2009-09-14
// replace subs with replace in blob, only first occurence or all,
// case sensitive or not.
// utf8-version of blobreplacestring from blob_functions 

BLOBCALLBACK u_blobreplacestring(BLOBCALLBACK b, char *subs, char *replby,
    int *all, int *case_insensitive, BLOBCALLBACK result)
{
    char *str = intern_blobaspchar(b, 0);
    char *str2;

    UChar *tmp_str, *tmp_subs, *tmp_replby, *tmp_res;
    int len_str, len_subs, len_replby;

    tmp_str = utf82uchar(str, -1, &len_str);
    tmp_subs = utf82uchar(subs, -1, &len_subs);
    tmp_replby = utf82uchar(replby, -1, &len_replby);

    tmp_res = intern_u_replacestring(tmp_str, len_str, tmp_subs, len_subs,
	tmp_replby, len_replby, *all, *case_insensitive);
    str2 = uchar2utf8(tmp_res, 0);
    result = strblob(str2, result);

    free(tmp_str);
    free(tmp_subs);
    free(tmp_replby);
    free(tmp_res);
    free(str);
    free(str2);
    return result;
}

// gh 2009-09-14
// return the position of string sub in blob. 
// utf8-version of blobsubstr form blob_functions

int *u_blobsubstr(BLOBCALLBACK blob, char *sub)
{
    if (blob->blob_handle == NULL || !sub) {
	return NULL;
    } else {
	char *str = intern_blobaspchar(blob, 0);
	UChar *tmp_str, *tmp_sub, *pos;
	int *res = MALLOC(sizeof(int));

	tmp_str = utf82uchar(str, -1, NULL);
	tmp_sub = utf82uchar(sub, -1, NULL);

	pos = u_strstr(tmp_str, tmp_sub);
	*res = (!pos ? -1 : pos - tmp_str);

	free(tmp_str);
	free(tmp_sub);
	return res;
    }
}

// maybe later or if asked for
//F_BLOBCMP
//F_BLOBSTRPOS
//F_BLOBTEXTPOS
//F_BLOB2EXCEL

// *** date/time functions ***

// gh 2010-03-20
// ct 2010-11-06 expand length of tmp_str from 32 to 255
static char *intern_u_format_date(ISC_QUAD *ib_date, char *format, char *locale)
{
    UErrorCode status = U_ZERO_ERROR;
    UChar tmp_str[255];
    UChar *tmp_format = utf82uchar(format, -1, NULL);
    UDate tmp_date;
    struct tm t;
    UDateFormat *dfmt;

    isc_decode_date(ib_date, &t);
    tmp_date = mktime(&t) * 1000.0;
    dfmt = udat_open(UDAT_DEFAULT, UDAT_DEFAULT, locale, NULL, -1, NULL, 0, &status);
    udat_applyPattern(dfmt, FALSE, tmp_format, -1);
    udat_format(dfmt, tmp_date, tmp_str, 255, NULL, &status);
    udat_close(dfmt);
    free(tmp_format);
    return uchar2utf8(tmp_str, 1);
}

// ct 2010-11-06
char *u_date2strlang(ISC_QUAD *ib_date, char *format, char *locale)
{
    if (!ib_date  || !format || !locale) {
	return NULL;
    } else {
	return intern_u_format_date(ib_date, format, locale);
    }
}

// gh 2010-03-20
char *u_monthlong(ISC_QUAD *ib_date)
{
    if (!ib_date) {
	return NULL;
    } else {
	return intern_u_format_date(ib_date, "MMMM", NULL);
    }
}

// gh 2010-03-20
char *u_monthshort(ISC_QUAD *ib_date)
{
    if (!ib_date) {
	return NULL;
    } else {
	return intern_u_format_date(ib_date, "MMM", NULL);
    }
}

// gh 2010-03-20
char *u_dowlong(ISC_QUAD *ib_date)
{
    if (!ib_date) {
	return NULL;
    } else {
	return intern_u_format_date(ib_date, "EEEE", NULL);
    }
}

// gh 2010-03-20
char *u_dowshort(ISC_QUAD *ib_date)
{
    if (!ib_date) {
	return NULL;
    } else {
	return intern_u_format_date(ib_date, "EE", NULL);
    }
}

// gh 2010-11-05
char *u_monthlonglang(ISC_QUAD *ib_date, char *locale)
{
    if (!ib_date || !locale) {
	return NULL;
    } else {
	return intern_u_format_date(ib_date, "MMMM", locale);
    }
}

// gh 2010-11-05
char *u_monthshortlang(ISC_QUAD *ib_date, char *locale)
{
    if (!ib_date || !locale) {
	return NULL;
    } else {
	return intern_u_format_date(ib_date, "MMM", locale);
    }
}

// gh 2010-11-05
char *u_dowlonglang(ISC_QUAD *ib_date, char *locale)
{
    if (!ib_date || !locale) {
	return NULL;
    } else {
	return intern_u_format_date(ib_date, "EEEE", locale);
    }
}

// gh 2010-11-05
char *u_dowshortlang(ISC_QUAD *ib_date, char *locale)
{
    if (!ib_date || !locale) {
	return NULL;
    } else {
	return intern_u_format_date(ib_date, "EE", locale);
    }
}
