#ifndef SHARED_MUTEX_H
#define SHARED_MUTEX_H

#include <pthread.h>

typedef struct shared_mutex_s sh_mutex_t;

sh_mutex_t* shared_mutex_new();
void shared_mutex_free(sh_mutex_t* obj);

void shared_mutex_ctor(sh_mutex_t* obj, const char* name);
void shared_mutex_dtor(sh_mutex_t* obj);

pthread_mutex_t* shared_mutex_getptr(sh_mutex_t* obj);

void shared_mutex_lock(sh_mutex_t* obj);
void shared_mutex_unlock(sh_mutex_t* obj);
#if !defined(__APPLE__)
void shared_mutex_make_consistent(sh_mutex_t* obj);
#endif

#endif
