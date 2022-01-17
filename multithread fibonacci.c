#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    //program akceptuje dok³adnie jeden argument wywo³ania
    if (argc != 2) {
        fprintf(stderr, "Program wymaga jednego argumentu wywolania\n");
        return 1;
    }

    //otrzymany argument ma byc liczba naturalna
    for (int i = 0; argv[1][i]; i++) {
        if (!isdigit(argv[1][i])) {
            fprintf(stderr, "Otrzymany argument nie jest liczba naturalna\n");
            return 2;
        }
    }

    //liczba otrzymana w argumencie ma pochodziæ z przedzia³u <1..13>
    int liczba = (int) strtol(argv[1], (char **)NULL, 10);
    if (liczba < 1 || liczba > 13) {
        fprintf(stderr, "Liczba otrzymana w argumencie ma pochodziæ z przedzia³u <1..13>\n");
        return 3;
    }

    //jeœli otrzymany argument jest równy 1 lub 2, program od razu koñczy pracê z kodem zakoñczenia 1
    if (liczba == 1 || liczba == 2) {
        return 1;
    }

    STARTUPINFO startupinfo;
    PROCESS_INFORMATION processinfo[2];

    memset(&startupinfo, 0, sizeof(startupinfo));
    memset(&processinfo, 0, sizeof(processinfo));
    startupinfo.cb = sizeof(startupinfo);

    char argline[50];
    sprintf(argline, "%s %d", argv[0], liczba - 1);
    if(!CreateProcessA(NULL, argline, NULL, NULL, 0, 0, NULL, NULL, &startupinfo, &processinfo[0])) {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return -1;
    }

    sprintf(argline, "%s %d", argv[0], liczba - 2);
    if(!CreateProcessA(NULL, argline, NULL, NULL, 0, 0, NULL, NULL, &startupinfo, &processinfo[1])) {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return -1;
    }

    //rodzic czeka na zakoñczenie 1 procesu
    WaitForSingleObject(processinfo[0].hProcess, INFINITE);
    LPDWORD output1;
    GetExitCodeProcess(processinfo[0].hProcess, &output1);
    printf("%d\t%d\t%d\t%d\n", GetCurrentProcessId(), processinfo[0].dwProcessId, liczba - 1, (int)output1);
    CloseHandle(processinfo[0].hProcess);
    CloseHandle(processinfo[0].hThread);

    //rodzic czeka na zakoñczenie 2 procesu
    WaitForSingleObject(processinfo[1].hProcess, INFINITE);
    LPDWORD output2;
    GetExitCodeProcess(processinfo[1].hProcess, &output2);
    printf("%d\t%d\t%d\t%d\n", GetCurrentProcessId(), processinfo[1].dwProcessId, liczba - 2, (int)output2);
    CloseHandle(processinfo[1].hProcess);
    CloseHandle(processinfo[1].hThread);

    int suma = (int)output1 + (int)output2;
    printf("%d\t\t\t%d\n\n", GetCurrentProcessId(), suma); //swój PID i sumê kodów powrotów obu dzieci

    return suma;
}
