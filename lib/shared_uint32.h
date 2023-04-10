#ifndef SHARED_UINT32_H
#define SHARED_UINT32_H

#include <stdint.h>

typedef struct shared_uint32_s sh_uint32_t;

sh_uint32_t* shared_uint32_new();
void shared_uint32_free(sh_uint32_t* obj);

void shared_uint32_ctor(sh_uint32_t* obj, const char* name);
void shared_uint32_dtor(sh_uint32_t* obj);

void shared_uint32_set_value(sh_uint32_t* obj, uint32_t value);
void shared_uint32_set_value_if_owner(sh_uint32_t* obj, uint32_t value);
uint32_t shared_uint32_get_value(sh_uint32_t* obj);

#endif
