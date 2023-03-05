#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    linked_list_t list;
    linked_list_init(&list);

    char* names[] = { "Andrey", "James", "Selina", "Martin" };

    for (int i = 0; i < sizeof(names) / sizeof(char*); i++) {
        printf("iteration...\n");
        linked_list_node_t* node = malloc(sizeof(linked_list_node_t));
        node->item = names[i];
        printf("appending name %s\n", (char*) node->item);
        printf("list.first = %p\n", list.first);
        printf("node addr  = %p\n", node);
        printf("list.last  = %p\n", list.last);
        printf("list.count = %d\n\n", list.count);
        linked_list_push(&list, node);
    }

    printf("now delete\n");

    while (list.count) {
        free(linked_list_pop(&list));
    }

    printf("list.first = %p\n", list.first);
    printf("list.last = %p\n", list.last);
}
