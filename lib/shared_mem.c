#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <unistd.h>

#include "prints.h"
#include "shared_mem.h"

// hide implementation
typedef struct shared_mem_s {
    char* name;
    void* map_ptr;
    void* ptr;
    size_t size;
    int fd;
} sh_mem_t;

// variables for ALL shared resources
// TODO another approach - list for shared name (malloc & free control)
// void free_sh_mem_context(); before exiting
static bool is_proc_owner = false;
// needed for `atomicity` of multiple shm creation
static bool owner_process_set = false;

sh_mem_t* shared_mem_new()
{
    return (sh_mem_t*) malloc(sizeof(sh_mem_t));
}

void shared_mem_free(sh_mem_t* obj)
{
    free(obj->name);
    free(obj);
}

void shared_mem_ctor(sh_mem_t* obj, const char* name, size_t size)
{
    obj->size = size;
    obj->name = (char*) malloc(strlen(name) + 1);
    strcpy(obj->name, name);
    obj->fd = shm_open(obj->name, O_RDWR, 0600);

    if (obj->fd >= 0) {
        if (!owner_process_set) {
            is_proc_owner = false;
            owner_process_set = true;
        }
        log_info("The shared memory %s is opened.\n", obj->name);

    } else if (errno == ENOENT) {
        log_info("WARN: The shared memory %s does not exist.\n", obj->name);
        obj->fd = shm_open(obj->name, O_CREAT | O_RDWR, 0600);

        if (obj->fd >= 0) {
            if (!owner_process_set) {
                is_proc_owner = true;
                owner_process_set = true;
            }

            log_info("The shared memory %s is created and opened.\n", obj->name);

            if (ftruncate(obj->fd, obj->size) < 0) {
                fprintf(stderr, "ERROR(%s): Truncation failed: %s\n", obj->name, strerror(errno));
                exit(1);
            }

        } else {
            fprintf(stderr, "ERROR(%s): Failed to create shared memory: %s\n", obj->name,
                strerror(errno));
            exit(1);
        }

    } else {
        fprintf(
            stderr, "ERROR(%s): Failed to create shared memory: %s\n", obj->name, strerror(errno));
        exit(1);
    }

    obj->map_ptr = mmap(0, obj->size, PROT_READ | PROT_WRITE, MAP_SHARED, obj->fd, 0);

    if (obj->map_ptr == MAP_FAILED) {
        fprintf(stderr, "ERROR(%s): Mapping failed: %s\n", name, strerror(errno));
        exit(1);
    }

    obj->ptr = (char*) obj->map_ptr;
}

void shared_mem_dtor(sh_mem_t* obj)
{
    if (munmap(obj->map_ptr, obj->size) < 0) {
        fprintf(stderr, "ERROR(%s): Unmapping failed: %s\n", obj->name, strerror(errno));
        exit(1);
    }

    log_info("The shared memory %s is unmapped.\n", obj->name);
    if (close(obj->fd) < 0) {
        fprintf(stderr, "ERROR(%s): Closing the shared memory fd failed: %s\n", obj->name,
            strerror(errno));
        exit(1);
    }

    log_info("The shared memory %s is closed.\n", obj->name);
    if (is_proc_owner) {
        if (shm_unlink(obj->name) < 0) {
            fprintf(stderr, "ERROR(%s): Unlinking the shared memory failed: %s\n", obj->name,
                strerror(errno));
            exit(1);
        }

        log_info("The shared memory %s is deleted.\n", obj->name);
    }
}

char* shared_mem_getptr(sh_mem_t* obj)
{
    return obj->ptr;
}

bool shared_mem_is_owner(sh_mem_t* obj)
{
    return is_proc_owner;
}

// args for future LIST implementation
void shared_mem_set_owner(sh_mem_t* obj, bool is_owner)
{
    is_proc_owner = is_owner;
}
