// SO IN1 20B LAB07
// Jakub Kleban
// kj49865@zut.edu.pl

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
double pi_thread = 1.0;

struct arg_struct {
    int size;
    int first;
};

/* function to be executed by the new thread */
void* CalculatePi(void* arguments)
{
    struct arg_struct *args = arguments;

    pthread_t tid = pthread_self();
    printf("Thread #%lu size=%d first=%d\n", tid, args->size, args->first);

    double prod = 1;
    for (long long i = args->first; i < (args->size + args->first); i++) {
        prod = prod * (2 * i) * (2 * i) / (((2 * i) - 1) * ((2*i) + 1));
    }

    pthread_mutex_lock(&mutex);
    pi_thread = pi_thread * prod;
    pthread_mutex_unlock(&mutex);

    printf("Thread #%lu prod=%.20f\n", tid, prod);
    return NULL;
}

int main(int argc, char** argv)
{
    //program akceptuje dokladnie dwoch argumentow wywolania
    if (argc != 3) {
        fprintf(stderr, "Program wymaga dwoch argumentow wywolania\n");
        return 1;
    }

    //otrzymany argument 1 ma byc liczba naturalna
    for (int i = 0; argv[1][i]; i++) {
        if (! isdigit(argv[1][i])) {
            fprintf(stderr, "Otrzymany argument 1 nie jest liczba naturalna\n");
            return 2;
        }
    }

    //otrzymany argument 2 ma byc liczba naturalna
    for (int i = 0; argv[2][i]; i++) {
        if (! isdigit(argv[2][i])) {
            fprintf(stderr, "Otrzymany argument 2 nie jest liczba naturalna\n");
            return 2;
        }
    }

    //liczba otrzymana w argumencie ma pochodzi� z przedzia�u <1..1000000000>
    long long n = (long long) strtol(argv[1], (char **)NULL, 10);
    if (n < 1 || n > 1000000000) {
        fprintf(stderr, "1 liczba otrzymana w argumencie ma pochodzic z przedzialu <1..1000000000>\n");
        return 3;
    }

    //liczba otrzymana w argumencie ma pochodzi� z przedzia�u <1..100>
    int w = (int) strtol(argv[2], (char **)NULL, 10);
    if (w < 1 || w > 100) {
        fprintf(stderr, "2 liczba otrzymana w argumencie ma pochodzic z przedzialu <1..100>\n");
        return 3;
    }

    //Czas
    clock_t begin, end;

    //Oblicz z watkami
    begin = clock();
    pthread_t thread_id[w];

    int size = n / w;
    int rest = n % w;

    struct arg_struct args[w];

    for (int i = 0; i < w; i++) {
        args[i].size = size;

        if (i == 0) { //first
            args[i].first = 1;
        } else {
            args[i].first = args[i-1].first + size;
        }

        if (i == w-1) { //last
            args[i].size = size + rest;
        }

        int rc = pthread_create(&thread_id[i], NULL, CalculatePi, (void *)&args[i]);
        if(rc)
        {
            printf("Failed to create process with id = %d \n", rc);
            exit(1);
        }
    }

    for (int i = 0; i < w; i++) {
        pthread_join(thread_id[i], NULL);
    }
    end = clock();

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("w/Threads: PI=%.20f time=%.5fs\n", pi_thread * 2, time_spent);

    //Oblicz bez watkow
    begin = clock();

    double pi = 1;
    for (long long i = 1; i<=n; i++) {
        pi = pi * (2 * i) * (2 * i) / (((2 * i) - 1) * ((2*i) + 1));
    }
    pi = pi * 2;

    end = clock();

    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("wo/Threads: PI=%.20f time=%.5fs\n", pi, time_spent);
    return 0;
}
