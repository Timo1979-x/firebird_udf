// gcc -lpcre -o test.exe test.c pcrs.c /usr/lib/x86_64-linux-gnu/libpcre.a

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pcre.h>
#include "pcrs.h"


volatile int patterns_err = 0;
pcrs_job * patterns[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

#define PATTERNS_COUNT sizeof(patterns) / sizeof(patterns[0])

char *testGnumbers[] = {
	"6789     qw -  1  ",
	"  67-89     qw -  1  ",
	"12-31AA5",
	"ПА 4452",
};


static unsigned char regNumberTranslitCp1251 [256] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64, 0xc0, 0xc2, 0xd1, 68, 0xc5, 70, 71, 0xcd, 0xb2, 74, 0xca, 76, 0xcc, 78, 0xce,
	0xd0, 81, 82, 83, 0xd2, 85, 86, 87, 0xd5, 0xd3, 90, 91, 92, 93, 94, 95,
	96, 0xe0, 0xe2, 0xf1, 100, 0xe5, 102, 103, 0xed, 0xb3, 106, 0xea, 108, 0xec, 110, 0xee,
	0xf0, 113, 114, 115, 0xf2, 117, 118, 119, 0xf5, 0xf3, 122, 123, 124, 125, 126, 127,
	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,

};

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


void convert_test() {
	unsigned char * table = regNumberTranslitUppercaseCp1251;
	for (int i = 32; i < 256; ++i)
	{
		printf("%c %0x -> %c %0x\n", i, i, table[i], table[i]);
	}

	printf("\n\nDiffs:\n");
	for (int i = 32; i < 256; ++i)
	{
		if (i != table[i]) {
			printf("%c %0x -> %c %0x\n", i, i, table[i], table[i]);
		}
	}
}



int pcre_test() {

	/*char pattern[] = "[ес]"; // шаблон (регулярное выражение)
	char str[] = "тест естественно";  // разбираемая строка
	*/

	char pattern[] = "[ес]"; // шаблон (регулярное выражение)
	char str[] = " тест";  // разбираемая строка

	// создание таблицы перекодировки для локали ru
	/*const unsigned char *tables = NULL;
	setlocale (LC_CTYPE, (const char *) "ru.");
	tables = pcre_maketables();*/

	// компилирование регулярного выражения во внутреннее представление
	pcre *re;
	int options = 0;
	const char *error;
	int erroffset;
	re = pcre_compile ((char *) pattern, options, &error, &erroffset, NULL);

	if (!re) { // в случае ошибки компиляции
		fprintf(stderr, "Failed at offset %d: %s\n", erroffset, error);
	} else {
		int count = 0;
		int ovector[30];

		count = pcre_exec (re, NULL, (char *) str, strlen(str), 0, 0, ovector, 30);
		// выполнение сопоставления с образцом
		if (!count) { // если нет совпадений
			printf("No match\n");
		} else {
			//вывод пар {начало, конец} совпадения
			for (int c = 0; c < 2 * count; c += 2) {
				if (ovector[c] < 0) { // или <unset> для несопоставившихся подвыражений
					printf("<unset>\n");
				} else {
					printf("%d %d\n", ovector[c], ovector[c + 1]);
				}
			}
		}
	}

	// освобождаем данные, под которые выделялась память
	pcre_free((void *) re);
	//pcre_free((void *) tables);
	return 0;
}

void pcrs_test() {
	char * pattern = "s/es/bla/g";
	pcrs_job *job;
	char *result;
	int err = 10, linenum = 0;


	char * text = "_ teSt me Especially";
	size_t length = strlen(text);
	/*
	 * Compile the job
	 */
	if (NULL == (job = pcrs_compile_command(pattern, &err)))
	{
		fprintf(stderr, "Compile error:  %s (%d).\n",  pcrs_strerror(err), err);
		return;
	}
	else {
		printf("success complie. err: %d\n", err);
	}

	if (0 > (err = pcrs_execute(job, text, length, &result, &length)))
	{
		fprintf(stderr, "Exec error:  %s (%d)\n", pcrs_strerror(err), err);
	}
	else
	{
		printf("%s\n", result);
		//fwrite(result, 1, length, stdout);
		free(result);
	}

	pcrs_free_job(job);
}

int init_patterns() {
	// если уже была неудачная попытка компиляции регулярных выражений - просто возвращаем ошибку:
	if (patterns[PATTERNS_COUNT - 1] == NULL && patterns_err != 0) {
		return patterns_err;
	}
	if (patterns[PATTERNS_COUNT - 1]) {
		printf("patterns already prepared\n");
		return 0;
	}

	int err, i = 0;



	// trim
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/(^\\s+)|(\\s+$)//g", &err))) {
		fprintf(stderr, "1 Compile error:  %s (%d).\n",  pcrs_strerror(err), err);
		return patterns_err = err;
	}

	// серии пробельных символов:
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/\\s+/ /g", &err))) {
		fprintf(stderr, "1 Compile error:  %s (%d).\n",  pcrs_strerror(err), err);
		return patterns_err = err;
	}
	//patterns[0] = g_regnumPatternStripSpaces;

	// удалить пробелы вокруг дефиса:
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/\\s*-\\s*/-/g", &err))) {
		fprintf(stderr, "2 Compile error:  %s (%d).\n",  pcrs_strerror(err), err);
		return patterns_err = err;
	}


	// дефис в номерах вида 45-12:
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/(\\d\\d)-(\\d\\d)/$1$2/g", &err))) {
		fprintf(stderr, "3 Compile error:  %s (%d).\n",  pcrs_strerror(err), err);
		return patterns_err = err;
	}
	//patterns[2] = g_regnumPatternRemoveUnnecessaryDash;

	// добавим дефис в номер вида '1231AA5': 1231AA5 -> 1231AA-5
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/([A-ZА-ЯІ])\\s*(\\d[^\\d]*)$/$1$2/g", &err))) {
		fprintf(stderr, "4 Compile error:  %s (%d).\n",  pcrs_strerror(err), err);
		return patterns_err = err;
	}

	// удалим пробел между цифрами и буквами: "4452 TT-1" -> "4452TT-1"
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/(@|\\d{4,})\\s+([A-ZА-ЯІ])/$1$2/g", &err))) {
		fprintf(stderr, "5 Compile error:  %s (%d).\n",  pcrs_strerror(err), err);
		return patterns_err = err;
	}

	// удалим пробел между цифрами и буквами: "ПА 4452" -> "ПА4452"
	if (NULL == (patterns[i++] =
	                 pcrs_compile_command("s/([A-ZА-ЯІ])\\s+(@|\\d{4,})/$1$2/g", &err))) {
		fprintf(stderr, "6 Compile error:  %s (%d).\n",  pcrs_strerror(err), err);
		return patterns_err = err;
	}
	//patterns[5] = g_regnumPatternRemoveUnnecessarySpace2;

	for (int i = 0; i < PATTERNS_COUNT - 1; ++i)	{
		patterns[i]->next = patterns[i + 1];
	}
	return 0;
}


char *normalizeRegnumber(char *regnum) {
	if (init_patterns() != 0) {
		return NULL;
	}
	char *out = NULL;
	char in[50];
	int err;
	size_t length;
	length = strlen(regnum);

	//char * in = (char *)malloc(length + 1);
	char *ptrD = in, *ptrS = regnum;
	for (int i = 0; i <= length; i++) 	{
		*ptrD++ =  regNumberTranslitUppercaseCp1251[*ptrS++];
	}

	if (0 > (err = pcrs_execute_list(patterns[0], in, length, &out, &length))) {
		fprintf(stderr, "Exec error:  %s (%d)\n", pcrs_strerror(err), err);
	}
	return out;
}


void main(int argc, char **argv) {
	//convert_test();
	// pcrs_test();
	int r;
	if ((r = init_patterns()) != 0) {
		printf("error: %d\n", r);
		return;
	}
	printf("testing...\n");
	for (int i = 0; i < sizeof(testGnumbers) / sizeof(testGnumbers[0]); i++)
	{
		printf("_%s_ -> _%s_\n", testGnumbers[i], normalizeRegnumber(testGnumbers[i]));
	}
}
