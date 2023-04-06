#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#define _GNU_SOURCE
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include "prints.h"
#include "shared_mutex.h"

int main(int argc, char** argv)
{
    prints_id("Parent");

    sh_mtx_t sh_mtx = { 
        .sh_res = {
            .name = SH_MTX_NAME,
            .size = SH_MTX_SIZE,
        },
        .mtx_ptr = NULL
    };

    sh_cnt_t sh_cnt = {
        .sh_res = {
            .name = SH_CNT_NAME,
            .size = SH_CNT_SIZE,
        },
        .cnt_ptr = NULL
    };

    if (init_shared_mutex(&sh_mtx)) {
        fprintf(stderr, "ERROR: Failed to init_shared_mutex\n");
        close_shared_mutex(&sh_mtx);
        exit(EXIT_FAILURE);
    }

    if (init_shared_counter(&sh_cnt)) {
        fprintf(stderr, "ERROR: Failed to init_shared_counter\n");
        close_shared_counter(&sh_cnt);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        log_error("can't fork, error occurred\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        child_func(sh_cnt.cnt_ptr, sh_mtx.mtx_ptr, CHILD_COUNT);
        exit(EXIT_SUCCESS);
    }

    printf("increasing shared counter by %d\n", PARENT_COUNT);

    for (int i = 0; i < PARENT_COUNT; i++) {
        pthread_mutex_lock(sh_mtx.mtx_ptr);
        *sh_cnt.cnt_ptr += 1;
        pthread_mutex_unlock(sh_mtx.mtx_ptr);
    }

    printf("waiting for child proc to end\n");
    do {
        int status;
        if (waitpid(pid, &status, 0) > 0) {
            int wifexited = WIFEXITED(status);
            int wexitstatus = WEXITSTATUS(status);

            if (wifexited && !wexitstatus) {
                printf("program execution successful\n");
                break;
            }

            if (wifexited && wexitstatus) {
                printf("program terminated normally, but returned a non-zero status [%d]\n",
                    wexitstatus);
                break;
            }

            printf("program didn't terminate normally\n");
        } else {
            printf("waitpid() failed\n");
        }
    } while (0);

    uint32_t result = *sh_cnt.cnt_ptr;

    close_shared_mutex(&sh_mtx);
    close_shared_counter(&sh_cnt);

    printf("result = %d\n", result);
    if (result != EXPECTED_COUNT) {
        printf("wrong value, expected %d\n", EXPECTED_COUNT);
    }

    return 0;
}
