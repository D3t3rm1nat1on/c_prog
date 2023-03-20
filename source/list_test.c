#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    linked_list_t list;
    linked_list_init(&list);

    char* names[] = { "Andrey", "James", "Selina", "Martin" };

    for (int i = 0; i < sizeof(names) / sizeof(char*); i++) {
        linked_list_node_t* node = malloc(sizeof(linked_list_node_t));
        node->item = names[i];
        printf("appending name %s\n", (char*) node->item);
        linked_list_push(&list, node);
    }

    printf(" == now delete == \n");
    while (list.count) {
        linked_list_node_t* node = linked_list_pop(&list);
        printf("deleted %s\n", (char*) node->item);
        free(node);
    }

}
