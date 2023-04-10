#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "prints.h"
#include "shared_cond.h"
#include "shared_mem.h"
#include "shared_mutex.h"

typedef struct shared_cond_s {
    sh_mem_t* shm;
    pthread_cond_t* ptr;
} sh_cond_t;

sh_cond_t* shared_cond_new()
{
    sh_cond_t* obj = (sh_cond_t*) malloc(sizeof(sh_cond_t));
    obj->shm = shared_mem_new();
    return obj;
}

void shared_cond_free(sh_cond_t* obj)
{
    shared_mem_free(obj->shm);
    free(obj);
}

void shared_cond_ctor(sh_cond_t* obj, const char* name)
{
    shared_mem_ctor(obj->shm, name, sizeof(pthread_cond_t));
    obj->ptr = (pthread_cond_t*) shared_mem_getptr(obj->shm);
    if (shared_mem_is_owner(obj->shm)) {
        pthread_condattr_t cond_attr;
        int e = -1;
        if ((e = pthread_condattr_init(&cond_attr))) {
            fprintf(stderr, "ERROR(%s): Initializing cv attrs failed: %s\n", name, strerror(e));
            exit(1);
        }
        if ((e = pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED))) {
            fprintf(
                stdout, "ERROR(%s): Setting as process shared failed: %s\n", name, strerror(e));
            exit(1);
        }
        if ((e = pthread_cond_init(obj->ptr, &cond_attr))) {
            fprintf(stderr, "ERROR(%s): Initializing the cv failed: %s\n", name, strerror(e));
            exit(1);
        }
        if ((e = pthread_condattr_destroy(&cond_attr))) {
            fprintf(
                stdout, "ERROR(%s): Destruction of cond attrs failed: %s\n", name, strerror(e));
            exit(1);
        }
    }
}

void shared_cond_dtor(sh_cond_t* obj)
{
    if (shared_mem_is_owner(obj->shm)) {
        int e = -1;
        if ((e = pthread_cond_destroy(obj->ptr))) {
            fprintf(stderr, "WARN: Destruction of the cv failed: %s\n", strerror(e));
        }
    }
    shared_mem_dtor(obj->shm);
}

void shared_cond_wait(sh_cond_t* obj, sh_mutex_t* mutex)
{
    int e = -1;
    if ((e = pthread_cond_wait(obj->ptr, shared_mutex_getptr(mutex)))) {
        fprintf(stderr, "ERROR: Waiting on the cv failed: %s\n", strerror(e));
        exit(1);
    }
}

void shared_cond_timedwait(sh_cond_t* obj, sh_mutex_t* mutex, long int time_nanosec)
{
    int e = -1;
    struct timespec ts;
    ts.tv_sec = ts.tv_nsec = 0;
    if ((e = clock_gettime(CLOCK_REALTIME, &ts))) {
        fprintf(stderr, "ERROR: Failed at reading current time: %s\n", strerror(errno));
        exit(1);
    }
    ts.tv_sec += (int) (time_nanosec / (1000L * 1000 * 1000));
    ts.tv_nsec += time_nanosec % (1000L * 1000 * 1000);
    if ((e = pthread_cond_timedwait(obj->ptr, shared_mutex_getptr(mutex), &ts))) {
#if !defined(__APPLE__)
        if (e == EOWNERDEAD) {
            fprintf(stderr, "WARN: The owner of the cv's mutex is dead ...\n");
            shared_mutex_make_consistent(mutex);
            fprintf(stderr, "INFO: I'm the new owner!\n");
            shared_mem_set_owner(obj->shm, true);
            return;
        } else if (e == ETIMEDOUT) {
#else
        if (e == ETIMEDOUT) {
#endif
            return;
        }
        fprintf(stderr, "ERROR: Waiting on the cv failed: %s\n", strerror(e));
        exit(1);
    }
}

void shared_cond_broadcast(sh_cond_t* obj)
{
    int e = -1;
    if ((e = pthread_cond_broadcast(obj->ptr))) {
        fprintf(stderr, "ERROR: Broadcasting on the cv failed: %s\n", strerror(e));
        exit(1);
    }
}
