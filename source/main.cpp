#include <stdio.h>
#include <sys/stat.h>

#include "list.h"
#include "debug.h"

int main()
{

    list_t list;
    LIST_CTOR (list, 10);

    LIST_DUMP (list, "test");

    ListDtor (&list);
}
