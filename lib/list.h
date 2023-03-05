#include "stdint.h"

typedef struct linked_list_node linked_list_node_t;
typedef struct linked_list linked_list_t;

struct linked_list_node {
    void* item;
    linked_list_node_t* prev;
    linked_list_node_t* next;
};

struct linked_list {
    uint32_t count;
    linked_list_node_t* first;
    linked_list_node_t* last;
};

void linked_list_push(linked_list_t* l, linked_list_node_t* item);
linked_list_node_t* linked_list_pop(linked_list_t* l);
void linked_list_init(linked_list_t* l);
