/*

gcc -Wl,-soname,libbla.so.1 -fPIC -shared -o libbla.so.1.0 pcrs.c gto_functions.c -lpcre
sudo cp libbla.so.1.0 /usr/lib/
sudo ln -s /usr/lib/libbla.so.1.0 /usr/lib/libbla.so.1
sudo ln -s /usr/lib/libbla.so.1.0 /usr/lib/libbla.so

gcc -o test_thread_use.c.exe test_thread_use.c -lFreeAdhocUdf -lpthread

*/

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "test_thread_use.h"


char *test1();
char *normalizeRegnumber(char *regnum);

char * normal_results[1000000];
char * thread_results[1000000];
pthread_t tid[2];

/*char *testGnumbers[] = {
    "6789     qw -  1  ",
    "  67-89     qw -  1  ",
    "12-31AA5",
    "ПА 4452",
};
*/


void* doSomeThing(void *arg)
{
    pthread_t id = pthread_self();
    for (int i = 0; i < sizeof(testGnumbers) / sizeof(testGnumbers[0]); i++)
    {
        printf("%ld: _%s_ -> _%s_\n", id, testGnumbers[i], normalizeRegnumber(testGnumbers[i]));
    }

    return NULL;
}

int main(void) {
    time_t timer;
    time(&timer);
    srand(timer);

    for (int i = 0; i < 1000000; ++i)
    {
        normal_results[i] = normalizeRegnumber(testGnumbers[i]);
        printf("_%s_ -> _%s_\n", testGnumbers[i], normal_results[i]);
    }

    // int i = 0;
    // int err;

    // while (i < 2)
    // {
    //     err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
    //     if (err != 0)
    //         printf("\ncan't create thread :[%s]", strerror(err));
    //     else
    //         printf("\n Thread created successfully\n");
    //     i++;
    // }

    // sleep(5);
    return 0;
}

