#include <stdio.h>
#include <sys/stat.h>

#include "list.h"
#include "debug.h"

int Test1 (list_t *list);
int Test2 (list_t *list);
int Test3 (list_t *list);
int Test4 (list_t *list);

#define DO_AND_CHECK(action)                \
        do                                  \
        {                                   \
            int status = action;            \
            if (status != LIST_ERROR_OK)    \
                return status;              \
        } while (0)                       
        
int Test1 (list_t *list)
{
    size_t saveIdx = 0;

    DO_AND_CHECK (ListInsert (list, 0, 100, &saveIdx));
    LIST_DUMP (*list, "Insert after [0]");

    DO_AND_CHECK (ListInsert (list, 1, 101, &saveIdx));
    LIST_DUMP (*list, "Insert after [1]");

    DO_AND_CHECK (ListInsert (list, 2, 202, &saveIdx));
    LIST_DUMP (*list, "Insert after [2]");

    DO_AND_CHECK (ListInsert (list, 1, 150, &saveIdx));
    LIST_DUMP (*list, "Insert after [1]");

    DO_AND_CHECK (ListDelete (list, 2));
    LIST_DUMP (*list, "After delete of [2]");

    return LIST_ERROR_OK;
}
int Test2 (list_t *list)
{
    size_t idx = 0;

    for (size_t i = 0; i < list->len - 1; i++)
    {
        DO_AND_CHECK (ListInsert (list, 0, (listDataType)(100 * i + i), &idx));
        LIST_DUMP (*list, "After another insert");
    }

    return LIST_ERROR_OK;
}
int Test3 (list_t *list)
{
    for (size_t i = 1; i < list->len; i++)
    {
        DO_AND_CHECK (ListDelete (list, i));
        LIST_DUMP (*list, "After another delete");
    }

    return LIST_ERROR_OK;
}
int Test4 (list_t *list)
{
    size_t idx = 0;

    for (size_t i = 0; i < list->len - 1; i++)
    {
        DO_AND_CHECK (ListInsert (list, i, (listDataType)(100 * i + i), &idx));
        LIST_DUMP (*list, "After another insert");
    }


    DO_AND_CHECK (ListDelete (list, 4));
    LIST_DUMP (*list, "After delete of [2]");

    DO_AND_CHECK (ListDelete (list, 2));
    LIST_DUMP (*list, "After delete of [2]");

    return LIST_ERROR_OK;
}
int main()
{
    list_t list;
    LIST_CTOR (list, 6);

    LIST_DUMP (list, "After ctor");

    Test2 (&list);
    // Test3 (&list);

    // Test1 (&list);
    // Test4 (&list);

    ListDtor (&list);
}
