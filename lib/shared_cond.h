#ifndef SHARED_COND_H
#define SHARED_COND_H

#include <pthread.h>

#include "shared_mutex.h"

typedef struct shared_cond_s sh_cond_t;

sh_cond_t* shared_cond_new();
void shared_cond_free(sh_cond_t* obj);

void shared_cond_ctor(sh_cond_t* obj, const char* name);
void shared_cond_dtor(sh_cond_t* obj);

void shared_cond_wait(sh_cond_t* obj, sh_mutex_t* mutex);
void shared_cond_timedwait(sh_cond_t* obj, sh_mutex_t* mutex, long int time_nanosec);
void shared_cond_broadcast(sh_cond_t* obj);

#endif
