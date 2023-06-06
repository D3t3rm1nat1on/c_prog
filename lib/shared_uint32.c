#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>

#include "prints.h"
#include "shared_mem.h"
#include "shared_uint32.h"

typedef struct shared_uint32_s {
    sh_mem_t* shm;
    uint32_t* ptr;
} sh_uint32_t;

sh_uint32_t* shared_uint32_new(const char* name)
{
    sh_uint32_t* obj = (sh_uint32_t*) malloc(sizeof(sh_uint32_t));
    obj->shm = shared_mem_new();
    return obj;
}

void shared_uint32_free(sh_uint32_t* obj)
{
    shared_mem_free(obj->shm);
    free(obj);
}

void shared_uint32_ctor(sh_uint32_t* obj, const char* name)
{
    shared_mem_ctor(obj->shm, name, sizeof(uint32_t));
    obj->ptr = (uint32_t*) shared_mem_getptr(obj->shm);
}

void shared_uint32_dtor(sh_uint32_t* obj)
{
    shared_mem_dtor(obj->shm);
}

void shared_uint32_set_value(sh_uint32_t* obj, uint32_t value)
{
    *(obj->ptr) = value;
}

void shared_uint32_set_value_if_owner(sh_uint32_t* obj, uint32_t value)
{
    if (shared_mem_is_owner(obj->shm)) {
        *(obj->ptr) = value;
    }
}

uint32_t shared_uint32_get_value(sh_uint32_t* obj)
{
    msync((obj->ptr), 4, MS_SYNC);
    log_info("returning %d\n", *(obj->ptr));
    return *(obj->ptr);
}
