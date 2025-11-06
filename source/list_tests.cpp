#include <stdio.h>

#include "list_tests.h"

#include "list.h"

// ---------- TESTS FOR VISUAL DEBUGGING ---------------

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
    size_t fixedCapacity = list->capacity;

    for (size_t i = 0; i < fixedCapacity; i++)
    {
        LIST_DO_AND_CHECK ( ListInsert (list, i, (listDataType)(100 * i + i), &idx));
    }

    LIST_DO_AND_CHECK (ListDelete (list, 5));
    LIST_DO_AND_CHECK (ListDelete (list, 8));

    list->elements[3].prev = 666;
    list->elements[6].next = 228;
    list->elements[5].next = 272;
    LIST_DUMP (list, "AFTER BAD EDIT");

    if ((ListVerify (list) & LIST_ERROR_IDX_OUT_OF_BOUNDS) == 0)
        return LIST_ERROR_IDX_OUT_OF_BOUNDS;
    
    return LIST_ERROR_OK;
}
int TestLoop (list_t *list)
{
    size_t idx = 0;
    size_t fixedCapacity = list->capacity;

    for (size_t i = 0; i < fixedCapacity; i++)
    {
        LIST_DO_AND_CHECK ( ListInsert (list, i, (listDataType)(100 * i + i), &idx));
    }

    list->elements[list->capacity-1].next = 2;

    LIST_DUMP (list, "AFTER CREATING LOOP");
    
    LIST_DO_AND_CHECK (ListDelete (list, 3));
    
    return LIST_ERROR_OK;
}


// ------- AUTOMATIC TESTS -------------------

int TestBadIdxes (list_t *list)
{
    for (size_t i = 1; i < list->capacity + 1; i++)
    {
        LIST_DUMP (list, "Before bad next idx");

        ssize_t savedIdx = list->elements[i].next;
        list->elements[i].next = (ssize_t) (list->capacity * 10 + i);
        
        if ((ListVerify (list) & LIST_ERROR_IDX_OUT_OF_BOUNDS) == 0)
            return LIST_ERROR_IDX_OUT_OF_BOUNDS;

        LIST_DUMP (list, "After bad next idx");

        list->elements[i].next = savedIdx;
    }

    for (size_t i = 1; i < list->capacity + 1; i++)
    {
        LIST_DUMP (list, "Before bad prev idx");
     
        ssize_t savedIdx = list->elements[i].prev;
        list->elements[i].prev = ssize_t (list->capacity * 10 + i);

        if ((ListVerify (list) & LIST_ERROR_IDX_OUT_OF_BOUNDS) == 0)
            return LIST_ERROR_IDX_OUT_OF_BOUNDS;

        LIST_DUMP (list, "After bad prev idx");

        list->elements[i].prev = savedIdx;
    }
    
    return LIST_ERROR_OK;
}

int TestBadLoop (list_t *list)
{
    
    LIST_DUMP (list, "Before loop created");
    
    ssize_t saveNext = list->elements[3].next;
    
    list->elements[3].next = list->elements[3].prev;
    LIST_DUMP (list, "After loop created");
    
    int rightError = LIST_ERROR_BROKEN_NEXT_EDGE & LIST_ERROR_BROKEN_PREV_EDGE;
    if ((ListVerify (list) & rightError) == 0)
        return rightError;
    
    list->elements[3].next = saveNext;
        

    LIST_DUMP (list, "Before loop created");

    ssize_t savePrev = list->elements[5].prev;
    
    list->elements[5].prev = list->elements[5].next;
    LIST_DUMP (list, "After loop created");

    if ((ListVerify (list) & rightError) == 0)
        return rightError;
    
    list->elements[5].prev = savePrev;

    return LIST_ERROR_OK;
}   


#define TEST_FUNC(FunctionName)                                     \
        {                                                           \
            int status = FunctionName (&list);                      \
            if (status != LIST_ERROR_OK)                            \
            {                                                       \
                ERROR_LOG ("%s", "Error in " #FunctionName "()");   \
                ListDtor (&list);                                   \
                                                                    \
                return status;                                      \
            }                                                       \
        }
int TestVerificator ()
{
    list_t list;
    LIST_CTOR (list, 10);

    size_t idx;

    for (size_t i = 0; i < list.capacity / 2; i++)
    {
        LIST_DO_AND_CLEAR (ListInsert (&list, i, (listDataType)(100 * i + i), &idx),
                           ListDtor (&list));
    }

    TEST_FUNC (TestBadLoop);
    TEST_FUNC (TestBadIdxes);

    ListDtor (&list);

    return 0;
}
#undef TEST_FUNC