#include <stdio.h>
#include <sys/stat.h>

#include "list.h"
#include "debug.h"

int main()
{

    list_t list;
    LIST_CTOR (list, 10);

    LIST_DUMP (list, "Before insert");

    size_t saveIdx = 0;
    ListInsert (&list, 0, 100, &saveIdx);

    LIST_DUMP (list, "After insert");

    ListDtor (&list);
}
