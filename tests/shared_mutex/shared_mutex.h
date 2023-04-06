#ifndef SHARED_MUTEX_H
#define SHARED_MUTEX_H

#include <pthread.h>
#include <stdint.h>

#define SH_MTX_SIZE sizeof(pthread_mutex_t)
#define SH_MTX_NAME "mutex"

#define SH_CNT_SIZE sizeof(uint32_t)
#define SH_CNT_NAME "counter"

#define CHILD_COUNT 33333333
#define PARENT_COUNT 44444444
#define EXPECTED_COUNT (CHILD_COUNT + PARENT_COUNT)

typedef struct {
    int fd;
    char* name;
    size_t size;
} sh_res_t;

typedef struct {
    sh_res_t sh_res;
    pthread_mutex_t* mtx_ptr;
} sh_mtx_t;

typedef struct {
    sh_res_t sh_res;
    uint32_t* cnt_ptr;
} sh_cnt_t;

int init_shared_res(sh_res_t* sh_res);
int init_shared_mutex(sh_mtx_t* sh_mtx);
int init_shared_counter(sh_cnt_t* sh_cnt);
void child_func(uint32_t* cnt_ptr, pthread_mutex_t* mtx_ptr, uint32_t count);
int close_shared_res(sh_res_t* sh_res);
int close_shared_mutex(sh_mtx_t* sh_mtx);
int close_shared_counter(sh_cnt_t* sh_cnt);

#endif
