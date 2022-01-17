// The program calculates the value of π using Leibniz's formula
// A program to study multithreading in a Windows environment

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <ctype.h>

HANDLE mutex;
double pi_thread = 0.0;

struct arg_struct {
    int size;
    int first;
};

double powMinusOne(int n){
    return (n % 2) ? -1.0 : 1.0;
}

double leibniz_calculate(int n) {
    return ( powMinusOne(n) / (2*n + 1));
}

DWORD WINAPI Leibniz(LPVOID arguments)
{
    struct arg_struct *args = arguments;

    DWORD tid = GetCurrentThreadId();
    printf("Thread #%lu size=%d first=%d\n", tid, args->size, args->first);

    double prod = 0;
    for (long long i = args->first; i < (args->size + args->first); i++) {
        prod = prod + leibniz_calculate(i);
    }

    WaitForSingleObject(mutex, INFINITE);
    pi_thread = pi_thread + prod;
    ReleaseMutex(mutex);

    printf("Thread #%lu prod=%.20f\n", tid, prod);
    return 0;
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

    mutex = CreateMutex(NULL, FALSE, NULL);
    HANDLE  threads[w];
    DWORD threadId[w];

    int size = n / w;
    int rest = n % w;

    struct arg_struct args[w];

    for (int i = 0; i < w; i++) {
        args[i].size = size;

        if (i == 0) { //first
            args[i].first = 0;
        } else {
            args[i].first = args[i-1].first + size;
        }

        if (i == w-1) { //last
            args[i].size = size + rest;
        }

        threads[i] = CreateThread(NULL, 0, Leibniz, &args[i], 0, &threadId[i]);
    }

    for (int i = 0; i < w; i++) {
        WaitForSingleObject(threads[i], INFINITE);
        CloseHandle(threads[i]);
    }
    end = clock();

    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("w/Threads: PI=%.20f time=%.5fs\n", pi_thread * 4, time_spent);

    //Oblicz bez watkow
    begin = clock();

    double pi = 0.0;
    for (long long i = 0; i < n; i++) {
        pi = pi + leibniz_calculate(i);
    }
    pi = pi * 4;

    end = clock();

    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("wo/Threads: PI=%.20f time=%.5fs\n", pi, time_spent);
    return 0;
}
