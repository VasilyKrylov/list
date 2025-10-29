#include <stdio.h>
#include <sys/stat.h>

#include "list.h"
#include "debug.h"

int Test1 (list_t *list);
int Test2 (list_t *list);
int Test3 (list_t *list);
int Test4 (list_t *list);
        
int Test1 (list_t *list)
{
    size_t saveIdx = 0;

    DO_AND_CHECK (ListInsert (list, 0, 100, &saveIdx));

    DO_AND_CHECK (ListInsert (list, 1, 101, &saveIdx));

    DO_AND_CHECK (ListInsert (list, 2, 202, &saveIdx));

    DO_AND_CHECK (ListInsert (list, 1, 150, &saveIdx));

    DO_AND_CHECK (ListDelete (list, 2));

    return LIST_ERROR_OK;
}
int Test2 (list_t *list)
{
    size_t idx = 0;

    for (size_t i = 0; i < list->capacity - 1; i++)
    {
        DO_AND_CHECK (ListInsert (list, i, (listDataType)(100 * i + i), &idx));
    }

    return LIST_ERROR_OK;
}
int Test3 (list_t *list)
{
    for (size_t i = 1; i < list->capacity; i++)
    {
        DO_AND_CHECK (ListDelete (list, i));
    }

    return LIST_ERROR_OK;
}
int Test4 (list_t *list)
{
    size_t idx = 0;

    for (size_t i = 0; i < list->capacity - 1; i++)
    {
        DO_AND_CHECK (ListInsert (list, i, (listDataType)(100 * i + i), &idx));
    }

    DO_AND_CHECK (ListDelete (list, 4));

    DO_AND_CHECK (ListDelete (list, 2));

    return LIST_ERROR_OK;
}

// TODO: realloc() up (down is optional)
// TODO: auto dump before and after 
// TODO: Next() and Prev()
// TODO: InsertBefore
int main()
{
    list_t list;
    LIST_CTOR (list, 6);

    int status = Test1 (&list);
    if (status != LIST_ERROR_OK)
    {
        ListDtor (&list);
        return status;
    }

    // Test3 (&list);

    // Test1 (&list);
    // Test4 (&list);

    ListDtor (&list);
}
