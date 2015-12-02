#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    List *ls = list_create();
    printf("Charlie -> %s\n", (list_find(ls, "Charlie") == NULL ? "Not Found" : "FAIL"));
    char user[1024] = "Alice";

    char *key;
    key = (char *) malloc(sizeof(user) * sizeof(char));
    strcpy(key, user);
    list_add(ls, key, "123");

    strcpy(user, "Bob");
    key = (char *) malloc(sizeof(user) * sizeof(char));
    strcpy(key, user);
    list_add(ls, key, "345");

    strcpy(user, "Charles");
    key = (char *) malloc(sizeof(user) * sizeof(char));
    strcpy(key, user);
    list_add(ls, key, "678");

    printf("Alice -> '%s'\n", (char*) list_find(ls, "Alice"));
    printf("Bob -> '%s'\n", (char*) list_find(ls, "Bob"));
    printf("Charles -> '%s'\n", (char*) list_find(ls, "Charles"));

    printf("Charlie -> %s\n", (list_find(ls, "Charlie") == NULL ? "Not Found" : "FAIL"));

    printf("Size = %d\n", ls->size);
    list_add(ls, "Alice", "456");
    printf("Size = %d\n", ls->size);

    list_print(ls);
    list_free(ls);

	return EXIT_SUCCESS;
}



