/*****************************************************************************
 * FreeAdhocUDF - copyright (c) 2004 - 2010 adhoc dataservice GmbH
 * Christoph Theuring ct / Peter Mandrella pm / Georg Horn gh
 * <http://freeadhocudf.org> eMail <help@freeadhocudf.org>
 * based on FreeUDFLibC - copyright (c) 1999 Gregory Deatz
 *
 * File Name:   checksum_functions.c
 * Description: this module contains UDFs to generate checksums
 * for exported functions list see checksum_functions.h
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
#include <string.h>
#include "global.h"
#include "gto_functions.h"
#include "pcrs.h"

int patterns_err = 0;
pcrs_job * patterns[7];

#define PATTERNS_COUNT (sizeof(patterns) / sizeof(patterns[0]))


// транслитерация госномера (лат. A -> рус. А и т.д.)
// static unsigned char regNumberTranslitCp1251 [256] =
// {
// 	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
// 	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
// 	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
// 	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
// 	64, 0xc0, 0xc2, 0xd1, 68, 0xc5, 70, 71, 0xcd, 0xb2, 74, 0xca, 76, 0xcc, 78, 0xce,
// 	0xd0, 81, 82, 83, 0xd2, 85, 86, 87, 0xd5, 0xd3, 90, 91, 92, 93, 94, 95,
// 	96, 0xe0, 0xe2, 0xf1, 100, 0xe5, 102, 103, 0xed, 0xb3, 106, 0xea, 108, 0xec, 110, 0xee,
// 	0xf0, 113, 114, 115, 0xf2, 117, 118, 119, 0xf5, 0xf3, 122, 123, 124, 125, 126, 127,
// 	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
// 	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
// 	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
// 	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
// 	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
// 	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
// 	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
// 	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
// };


static unsigned char regNumberTranslitUppercaseCp1251 [256] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64, 0xc0, 0xc2, 0xd1, 68, 0xc5, 70, 71, 0xcd, 0xb2, 74, 0xca, 76, 0xcc, 78, 0xce,
	0xd0, 81, 82, 83, 0xd2, 85, 86, 87, 0xd5, 0xd3, 90, 91, 92, 93, 94, 95,
	96, 0xc0, 0xc2, 0xd1, 68, 0xc5, 70, 71, 0xcd, 0xb2, 74, 0xca, 76, 0xcc, 78, 0xce,
	0xd0, 81, 82, 83, 0xd2, 85, 86, 87, 0xd5, 0xd3, 90, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 178, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223
};


int init_patterns() {
	// если уже была неудачная попытка компиляции регулярных выражений - просто возвращаем ошибку:
	if (patterns_err != 0) {
		return patterns_err;
	}
	if (patterns[PATTERNS_COUNT - 1]) {
		return 0;
	}

	int err, i;

	for (i = 0; i < PATTERNS_COUNT; ++i)
	{
		patterns[i] = NULL;
	}

	i = 0;
	// trim
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/(^\\s+)|(\\s+$)//g", &err))) {
		return patterns_err = err;
	}

	// серии пробельных символов:
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/\\s+/ /g", &err))) {
		return patterns_err = err;
	}

	// удалить пробелы вокруг дефиса:
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/\\s*-\\s*/-/g", &err))) {
		return patterns_err = err;
	}

	// дефис в номерах вида 45-12:
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/(\\d\\d)-(\\d\\d)/$1$2/g", &err))) {
		return patterns_err = err;
	}

	// добавим дефис в номер вида '1231AA5': 1231AA5 -> 1231AA-5
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/([A-ZА-ЯІ])\\s*(\\d[^\\d]*)$/$1$2/g", &err))) {
		return patterns_err = err;
	}

	// удалим пробел между цифрами и буквами: "4452 TT-1" -> "4452TT-1"
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/(@|\\d{4,})\\s+([A-ZА-ЯІ])/$1$2/g", &err))) {
		return patterns_err = err;
	}

	// удалим пробел между цифрами и буквами: "ПА 4452" -> "ПА4452"
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/([A-ZА-ЯІ])\\s+(@|\\d{4,})/$1$2/g", &err))) {
		return patterns_err = err;
	}

	for (int i = 0; i < PATTERNS_COUNT - 1; ++i)	{
		patterns[i]->next = patterns[i + 1];
	}

	return 0;
}

void __attribute((constructor)) init_function(void) {
	patterns_err = 0;
	init_patterns();
	// FILE * f = fopen("log.log", "a");
	// if (f) {
	// 	fprintf(f, "constructor func %d\n", patterns_err);
	// 	for (int i = 0; i < PATTERNS_COUNT; ++i)
	// 	{
	// 		fprintf(f, "%d %ld\n", i, patterns[i]);
	// 	}
	// 	fclose(f);
	// }
}

void free_patterns() {
	for (int i = 0; i < PATTERNS_COUNT ; ++i)
	{
		if (patterns[i]) {
			pcrs_free_job(patterns[i]);
			patterns[i] = NULL;
		}
	}
}

void __attribute((destructor)) fini_function(void) {
	free_patterns();
	// FILE * f = fopen("log.log", "a");
	// if (f) {
	// 	fprintf(f, "destructor func\n");
	// 	fclose(f);
	// }
}


char *test1() {
	char * result = MALLOC(10);
	sprintf(result, "Hu!");
	return result;
}

int gto_modulo( int * a, int * b)
{
	if (*b == 0)
		return -1; // return something suitably stupid.
	else
		return *a % *b;
}

char * ComposeGNumber(char *letters, int * digits) {
//char buf[12];
	if (!letters) {
		return NULL;
	}
	int len = strlen(letters);
	char *dog = strchr(letters, '@');
	char *result;

	if (dog) {
		result = MALLOC(len + 10); //могут вставиться 2 дефиса и цифры номера, на всякий случай выделим немного больше
		char *r2 = result;
		strncpy(r2, letters, dog - letters);
		r2 += (dog - letters);
		if (dog > letters && *(dog - 1) >= '0' && *(dog - 1) <= '9') {
			*r2++ = '-';
		}
		r2 += sprintf(r2, "%04d", * digits);
		if ((dog < letters + len - 1) && *(dog + 1) >= '0' && *(dog + 1) <= '9') {
			*r2++ = '-';
		}
		strcpy(r2, dog + 1);

	} else
	{
		result = MALLOC(len + 1); //могут вставиться 2 дефиса и цифры номера, на всякий случай выделим немного больше
		strcpy(result, letters);
	}
	return result;
}


char *normalizeRegnumber(char *regnum) {
	// if (init_patterns() != 0) {
	// 	return NULL;
	// }
	if (patterns_err != 0) {
		return NULL;
	}
	char *out = NULL;
	char in[50];
	int err;
	size_t length;
	length = strlen(regnum);

	//char * in = (char *)malloc(length + 1);
	unsigned char *ptrD = in, *ptrS = regnum;
	for (int i = 0; i <= length; i++) 	{
		*ptrD++ =  regNumberTranslitUppercaseCp1251[*ptrS++];
	}

	if (0 > (err = pcrs_execute_list(patterns[0], in, length, &out, &length))) {
		return NULL;
	} else {
		char * result = MALLOC(length + 1);
		strcpy(result, out);
		free(out);
		return result;
	}
}
