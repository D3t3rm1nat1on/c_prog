#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "shared_mem.h"
#include "shared_uint32.h"
#include "shared_mutex.h"
#include "shared_cond.h"

bool is_sigint = false;
sh_cond_t* cond = NULL;
sh_mutex_t* mutex = NULL;
sh_uint32_t* counter = NULL;

void sigint_handler(int signo)
{
    fprintf(stderr, "\nHandling INT signal: %d ...\n", signo);
    is_sigint = true;
}

int process_args(int argc, char** argv)
{
    if (argc < 2) {
        fprintf(stderr, "ERROR: You have to provide the process number.\n");
        exit(1);
    }
    int my_number = atol(argv[1]);
    if (my_number < 1) {
        fprintf(stderr, "ERROR: number must be greater than 0.\n");
        exit(1);
    }

    return my_number;
}

int main(int argc, char** argv)
{
    signal(SIGINT, sigint_handler);

    int my_number = process_args(argc, argv);

    printf("My number is %d!\n", my_number);

    counter = shared_uint32_new();
    shared_uint32_ctor(counter, "cv_counter0");
    shared_uint32_set_value_if_owner(counter, 1);

    mutex = shared_mutex_new();
    shared_mutex_ctor(mutex, "cv_mutex0");

    cond = shared_cond_new();
    shared_cond_ctor(cond, "cv_cond0");

    // dangerous place, but ignore in test scenario
    // (resources created & deleted by 1st, after only inited in 2nd)
    // mutex PRIO_PROT problem after pthread_mutex_destroy()
    // (undefined behavior)

    shared_mutex_lock(mutex);

    uint32_t cur;
    while ((cur = shared_uint32_get_value(counter)) < my_number) {
        printf("curent turn: %d\n", cur);
        if (is_sigint) {
            break;
        }

        printf("Waiting for the signal, just for 5 seconds ...\n");
        shared_cond_timedwait(cond, mutex, 5L * 1000 * 1000 * 1000);

        if (is_sigint) {
            break;
        }
        printf("Checking condition ...\n");
    }

    if (is_sigint) {
        printf("Exiting ...\n");
        shared_mutex_unlock(mutex);
        goto destroy;
    }

    shared_uint32_set_value(counter, my_number + 1);
    printf("My turn! %d ...\n", my_number);
    shared_mutex_unlock(mutex);
    sleep(1);
    shared_cond_broadcast(cond);

destroy:
    shared_cond_dtor(cond);
    shared_cond_free(cond);
    shared_mutex_dtor(mutex);
    shared_mutex_free(mutex);
    shared_uint32_dtor(counter);
    shared_uint32_free(counter);
    return 0;
}
