#include "list.h"
#include "stddef.h"

void linked_list_push(linked_list_t* l, linked_list_node_t* node)
{
    if (l->count == 0) {
        l->first = l->last = node;
        l->count++;
        return;
    }

    node->prev = l->last;
    node->next = NULL;
    l->last->next = node;
    l->last = node;
    l->count++;
}

linked_list_node_t* linked_list_pop(linked_list_t* l)
{
    if (l->count == 0)
        return NULL;

    if (l->count == 1) {
        linked_list_node_t* node = l->last;
        l->first = l->last = NULL;
        l->count--;
        return node;
    }

    linked_list_node_t* node = l->last;
    l->last = node->prev;
    l->last->next = NULL;
    node->prev = NULL;

    l->count--;
    return node;
}

void linked_list_init(linked_list_t* l)
{
    l->first = l->last = NULL;
    l->count = 0;
}
