#include "prints.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

void child_func(uint32_t* cnt_ptr, pthread_mutex_t* mtx_ptr, uint32_t count)
{
    prints_id("----> child");

    printf("----> increasing shared counter by %d\n", count);

    for (int i = 0; i < count; i++) {
        pthread_mutex_lock(mtx_ptr);
        *cnt_ptr += 1;
        pthread_mutex_unlock(mtx_ptr);
    }

    printf("-----> child process finished\n");
}
