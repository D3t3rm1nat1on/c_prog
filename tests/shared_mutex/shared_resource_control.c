#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "prints.h"
#include "shared_mutex.h"

int init_shared_res(sh_res_t* sh_res)
{
    if (sh_res->name == NULL) {
        log_error("name is NULL\n");
        return 1;
    }

    sh_res->fd = shm_open(sh_res->name, O_CREAT | O_RDWR, 0600);
    if (sh_res->fd < 0) {
        log_error("Failed to create shared memory: %s\n", strerror(errno));
        return 2;
    }
    log_info("shm_open \"%s\", fd: %d\n", sh_res->name, sh_res->fd);

    if (ftruncate(sh_res->fd, sh_res->size) < 0) {
        log_error("Truncation failed: %s\n", strerror(errno));
        return 3;
    }
    log_info("The memory region is truncated to %ld bytes\n", sh_res->size);
    return 0;
}

int init_shared_mutex(sh_mtx_t* sh_mtx)
{
    if (init_shared_res((sh_res_t*) sh_mtx)) {
        log_error("Init shared res failed\n");
        return 1;
    }

    int fd = sh_mtx->sh_res.fd;
    size_t size = sh_mtx->sh_res.size;

    void* map = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        log_error("Mapping failed: %s\n", strerror(errno));
        return 2;
    }
    log_info("mapped mutex: %p\n", map);

    sh_mtx->mtx_ptr = map;

    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr)) {
        perror("pthread_mutexattr_init");
        return 3;
        //exit(EXIT_FAILURE);
    }
    if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) {
        perror("pthread_mutexattr_setpshared");
        return 4;
        //exit(EXIT_FAILURE);
    }
    if (pthread_mutex_init(sh_mtx->mtx_ptr, &attr)) {
        perror("pthread_mutex_init");
        return 5;
        //exit(EXIT_FAILURE);
    }

    return 0;
}

int init_shared_counter(sh_cnt_t* sh_cnt)
{
    if (init_shared_res((sh_res_t*) sh_cnt)) {
        log_error("Init shared res failed\n");
        return 1;
    }

    int fd = sh_cnt->sh_res.fd;
    size_t size = sh_cnt->sh_res.size;

    void* map = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
        log_error("Mapping failed: %s\n", strerror(errno));
        return 2;
    }
    log_info("mapped mutex: %p\n", map);

    sh_cnt->cnt_ptr = map;

    *(sh_cnt->cnt_ptr) = 0;

    return 0;
}

int close_shared_res(sh_res_t* sh_res)
{
    if (close(sh_res->fd) < 0) {
        log_error("Closing shared memory failed: %s\n", strerror(errno));
        return 1;
    }
    log_info("closed fd: %d\n", sh_res->fd);

    if (shm_unlink(sh_res->name) < 0) {
        log_error("Unlinking shared memory failed: %s\n", strerror(errno));
        return 1;
    }
    log_info("unlinked: %s\n", sh_res->name);

    return 0;
}

int close_shared_mutex(sh_mtx_t* sh_mtx)
{
    size_t size = sh_mtx->sh_res.size;

    if (munmap(sh_mtx->mtx_ptr, size) < 0) {
        log_error("Unmapping failed: %s\n", strerror(errno));
        return 1;
    }
    log_info("unmapped mutex: %p\n", sh_mtx->mtx_ptr);

    if (close_shared_res((sh_res_t*) sh_mtx)) {
        log_error("Close shared res failed: %s\n", strerror(errno));
        return 2;
    }

    return 0;
}

int close_shared_counter(sh_cnt_t* sh_cnt)
{
    size_t size = sh_cnt->sh_res.size;

    if (munmap(sh_cnt->cnt_ptr, size) < 0) {
        log_error("Unmapping failed: %s\n", strerror(errno));
        return 1;
    }
    log_info("unmapped counter: %p\n", sh_cnt->cnt_ptr);

    if (close_shared_res((sh_res_t*) sh_cnt)) {
        log_error("Close shared res failed: %s\n", strerror(errno));
        return 2;
    }

    return 0;
}
