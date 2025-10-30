#include <stdio.h>
#include <sys/stat.h>

#include "list.h"
#include "list_log.h"
#include "debug.h"

int TestRandom1 (list_t *list);
int TestRealloc (list_t *list);
int TestDelete (list_t *list);
int Test4 (list_t *list);
int TestBeforeFunctions (list_t *list);
int TestBadEdge (list_t *list);
int TestLoop (list_t *list);

int TestRandom1 (list_t *list)
{
    size_t saveIdx = 0;

    LIST_DO_AND_CHECK (ListInsert (list, 0, 100, &saveIdx));

    LIST_DO_AND_CHECK (ListInsert (list, 1, 101, &saveIdx));

    LIST_DO_AND_CHECK (ListInsert (list, 2, 202, &saveIdx));

    LIST_DO_AND_CHECK (ListInsert (list, 1, 150, &saveIdx));

    LIST_DO_AND_CHECK (ListDelete (list, 2));

    return LIST_ERROR_OK;
}
int TestRealloc (list_t *list)
{
    size_t idx = 0;

    size_t doubleCapacity = list->capacity * 2 + 3;

    for (size_t i = 0; i < doubleCapacity; i++)
    {
        LIST_DO_AND_CHECK (ListInsert (list, i, (listDataType)(100 * i + i), &idx));
    }

    return LIST_ERROR_OK;
}
int TestDelete (list_t *list)
{
    size_t idx = 0;

    for (size_t i = 0; i < list->capacity; i++)
    {
        LIST_DO_AND_CHECK (ListInsert (list, i, (listDataType)(100 * i + i), &idx));
    }

    for (size_t i = 1; i < list->capacity; i++)
    {
        LIST_DO_AND_CHECK (ListDelete (list, i));
    }

    return LIST_ERROR_OK;
}
int Test4 (list_t *list)
{
    size_t idx = 0;

    for (size_t i = 0; i < list->capacity; i++)
    {
        LIST_DO_AND_CHECK (ListInsert (list, i, (listDataType)(100 * i + i), &idx));
    }

    LIST_DO_AND_CHECK (ListDelete (list, 4));

    LIST_DO_AND_CHECK (ListDelete (list, 2));

    return LIST_ERROR_OK;
}

int TestBeforeFunctions (list_t *list)
{
    size_t idx = 0;

    for (size_t i = 0; i < list->capacity; i++)
    {
        LIST_DO_AND_CHECK (ListInsertBefore (list, i, (listDataType)(100 * i + i), &idx));
    }

    LIST_DO_AND_CHECK (ListDeleteBefore (list, 3));
    LIST_DO_AND_CHECK (ListDeleteBefore (list, 7));
    LIST_DO_AND_CHECK (ListInsertBefore (list, 3, 333, &idx));
    LIST_DO_AND_CHECK (ListDeleteBefore (list, 0));
    LIST_DO_AND_CHECK (ListDeleteBefore (list, 5));
    LIST_DO_AND_CHECK (ListInsertBefore (list, 5, 555, &idx));

    return LIST_ERROR_OK;
}

int TestBadEdge (list_t *list)
{
    size_t idx = 0;

    for (size_t i = 0; i < list->capacity; i++)
    {
        LIST_DO_AND_CHECK ( ListInsert (list, i, (listDataType)(100 * i + i), &idx));
    }

    LIST_DO_AND_CHECK (ListDelete (list, 5));

    list->elements[3].prev = 666;
    list->elements[6].next = 228;
    list->elements[5].next = 272;
    LIST_DUMP (*list, "AFTER BAD EDIT");

    LIST_DO_AND_CHECK (ListDelete (list, 8));

    
    return LIST_ERROR_OK;
}

int TestLoop (list_t *list)
{
    size_t idx = 0;

    for (size_t i = 0; i < list->capacity; i++)
    {
        LIST_DO_AND_CHECK ( ListInsert (list, i, (listDataType)(100 * i + i), &idx));
    }

    list->elements[list->capacity-1].next = 2;

    LIST_DUMP (*list, "AFTER CREATING LOOP");
    
    LIST_DO_AND_CHECK (ListDelete (list, 3));
    
    return LIST_ERROR_OK;
}

// TODO:
// [*] default node is scary
// [*] в DUMP проходимся только по массиву, а не по next/prev
// [*] bidirectional nodes
// [*] непарные стрелки жирные
int main()
{
    list_t list;
    LIST_CTOR (list, 10);

    int status = TestLoop (&list);
    if (status != LIST_ERROR_OK)
    {
        ListDtor (&list);

        return status;
    }

    ListDtor (&list);
}
