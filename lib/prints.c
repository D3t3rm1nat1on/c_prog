#define _GNU_SOURCE // for gettid
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "prints.h"

void print_args(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i) {
        fprintf(stderr, "argv[%d] = [%s]\n", i, argv[i]);
    }
}

void print_id()
{
    fprintf(stderr, "My PPID = %d\n", getppid());
    fprintf(stderr, "My PID  = %d\n", getpid());
    fprintf(stderr, "My TID  = %d\n", gettid());
    fprintf(stderr, "My GID  = %d\n", getgid());
}

void prints_id(const char* str)
{
    fprintf(stderr, "%s PPID = %d\n", str, getppid());
    fprintf(stderr, "%s PID  = %d\n", str, getpid());
    fprintf(stderr, "%s TID  = %d\n", str, gettid());
    fprintf(stderr, "%s GID  = %d\n", str, getgid());
}
