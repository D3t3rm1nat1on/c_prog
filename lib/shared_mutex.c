#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "prints.h"
#include "shared_mem.h"
#include "shared_mutex.h"

typedef struct shared_mutex_s {
    sh_mem_t* shm;
    pthread_mutex_t* ptr;
} sh_mutex_t;

sh_mutex_t* shared_mutex_new()
{
    sh_mutex_t* obj = (sh_mutex_t*) malloc(sizeof(sh_mutex_t));
    obj->shm = shared_mem_new();
    return obj;
}

void shared_mutex_free(sh_mutex_t* obj)
{
    shared_mem_free(obj->shm);
    free(obj);
}

void shared_mutex_ctor(sh_mutex_t* obj, const char* name)
{
    shared_mem_ctor(obj->shm, name, sizeof(pthread_mutex_t));
    obj->ptr = (pthread_mutex_t*) shared_mem_getptr(obj->shm);
    if (shared_mem_is_owner(obj->shm)) {
        pthread_mutexattr_t mutex_attr;
        int e = -1;
        if ((e = pthread_mutexattr_init(&mutex_attr))) {
            fprintf(stderr, "ERROR(%s): Initializing mutex attrs failed: %s\n", name, strerror(e));
            exit(1);
        }
#if !defined(__APPLE__)
        if ((e = pthread_mutexattr_setrobust(&mutex_attr, PTHREAD_MUTEX_ROBUST))) {
            fprintf(
                stdout, "ERROR(%s): Setting the mutex as robust failed: %s\n", name, strerror(e));
            exit(1);
        }
#endif
        if ((e = pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED))) {
            fprintf(stderr, "ERROR(%s): Failed to set as process-shared: %s\n", name, strerror(e));
            exit(1);
        }
        if ((e = pthread_mutex_init(obj->ptr, &mutex_attr))) {
            fprintf(stderr, "ERROR(%s): Initializing the mutex failed: %s\n", name, strerror(e));
            exit(1);
        }
        if ((e = pthread_mutexattr_destroy(&mutex_attr))) {
            fprintf(stderr, "ERROR(%s): Destruction of mutex attrs failed: %s\n", name, strerror(e));
            exit(1);
        }
    }
}

void shared_mutex_dtor(sh_mutex_t* obj)
{
    if (shared_mem_is_owner(obj->shm)) {
        int e = -1;
        if ((e = pthread_mutex_destroy(obj->ptr))) {
            fprintf(stderr, "WARN: Destruction of the mutex failed: %s\n", strerror(e));
        }
    }
    shared_mem_dtor(obj->shm);
}

pthread_mutex_t* shared_mutex_getptr(sh_mutex_t* obj)
{
    return obj->ptr;
}

#if !defined(__APPLE__)
void shared_mutex_make_consistent(sh_mutex_t* obj)
{
    int e = -1;
    if ((e = pthread_mutex_consistent(obj->ptr))) {
        fprintf(stderr, "ERROR: Making the mutex consistent failed: %s\n", strerror(e));
        exit(1);
    }
}
#endif

void shared_mutex_lock(sh_mutex_t* obj)
{
    int e = -1;
    if ((e = pthread_mutex_lock(obj->ptr))) {
#if !defined(__APPLE__)
        if (e == EOWNERDEAD) {
            fprintf(stderr, "WARN: The owner of the mutex is dead ...\n");
            shared_mutex_make_consistent(obj);
            fprintf(stderr, "INFO: I'm the new owner!\n");
            shared_mem_set_owner(obj->shm, true);
            return;
        }
#endif
        fprintf(stderr, "ERROR: Locking the mutex failed: %s\n", strerror(e));
        exit(1);
    }
}

void shared_mutex_unlock(sh_mutex_t* obj)
{
    int e = -1;
    if ((e = pthread_mutex_unlock(obj->ptr))) {
        fprintf(stderr, "ERROR: Unlocking the mutex failed: %s\n", strerror(e));
        exit(1);
    }
}
