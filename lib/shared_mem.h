#ifndef SHARED_MEM_H
#define SHARED_MEM_H

#include <stdbool.h>
#include <stdint.h>

typedef struct shared_mem_s sh_mem_t;

sh_mem_t* shared_mem_new();
void shared_mem_free(sh_mem_t* obj);

void shared_mem_ctor(sh_mem_t* obj, const char* name, size_t size);
void shared_mem_dtor(sh_mem_t* obj);

char* shared_mem_getptr(sh_mem_t* obj);

bool shared_mem_is_owner(sh_mem_t* obj);
void shared_mem_set_owner(sh_mem_t* obj, bool is_owner);

#endif
