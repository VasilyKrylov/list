#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>

#include "debug.h"
#include "list_log.h"
#include "file_utils.h"
#include "float_math.h"

#include "list.h"

int ResizeElements  (list_t *list);

int CheckIdxes              (list_t *list);
int CheckFreeListCount      (list_t *list);
int CheckElementsListCount  (list_t *list);
int CheckPrevEdges          (list_t *list);
int CheckNextEdges          (list_t *list);
int CheckFreeElementsPrev   (list_t *list);

int ListCtor (list_t *list, size_t capacity
              ON_DEBUG (, varInfo_t varInfo))
{
    assert (list);

    if (capacity > kListMaxLen)
        return LIST_ERROR_BIG_LEN;

    // + 1 for null element
    list->elements = (listElement_t *) calloc (capacity + 1, sizeof(listElement_t));
    if (list->elements == NULL)
        return LIST_ERROR_COMMON |
               COMMON_ERROR_ALLOCATING_MEMORY;

    list->free = 1;
    list->capacity = capacity;
    list->size = 0;

    list->elements[0] = {.data = 0, .next = 0, .prev = 0};
    for (size_t i = 1; i < capacity + 1; i++)
    {
        list->elements[i].data = kListPoison;
        list->elements[i].prev = kListPrevFree;
        list->elements[i].next = (int)i + 1;
    }
    list->elements[capacity] = {.data = kListPoison, .next = 0, .prev = kListPrevFree};

#ifdef PRINT_DEBUG
    list->varInfo = varInfo;

    int status = LogInit (&list->log);
    if (status != LIST_ERROR_OK)
        return status;

#endif // PRING_DEBUG

    LIST_DUMP (list, "After ctor");

    return LIST_VERIFY (list);
}

int ResizeElements (list_t *list)
{
    assert (list);

    if (list->size == list->capacity)
    {
        list->capacity *= 2;
        listElement_t *newElemenets = (listElement_t *) realloc (list->elements, 
                                                                 (list->capacity + 1) * sizeof(listElement_t));
        if (newElemenets == NULL)
        {
            perror ("Error trying to realloc elements of the list");

            return LIST_ERROR_COMMON |
                   COMMON_ERROR_ALLOCATING_MEMORY;
        }

        list->elements = newElemenets;
        list->free = list->size + 1;
        
        for (size_t i = list->size + 1; i < list->capacity + 1; i++)
        {
            list->elements[i].data = kListPoison;
            list->elements[i].prev = kListPrevFree;
            list->elements[i].next = (ssize_t) i + 1;
        }
        list->elements[list->capacity].next = kListStart;
        
        DEBUG_LOG ("list->elements reallocated to %lu capacity", list->capacity + 1);

        LIST_DUMP (list, "After realloc");
    }

    return LIST_ERROR_OK;
}

int ListInsert (list_t *list, size_t idx, listDataType val, size_t *insertedIdx)
{
    assert (list);
    assert (insertedIdx);

    char comment[kMaxCommentLen] = {};
    snprintf (comment, kMaxCommentLen, "before Insert() [%lu], val = %g", idx, val); // TODO: function
    LIST_DUMP (list, comment); // TODO: not another function, just vprintf

    LIST_DO_AND_CHECK (LIST_VERIFY (list));

    int status = ResizeElements (list);
    if (status != LIST_ERROR_OK)
        return status;

    list->size++;

    if (list->elements[idx].prev == kListPrevFree)
    {
        ERROR_LOG ("%s", "Can't insert after empty element :(");

        return LIST_ERROR_INSERT_AFTER_EMPTY;
    }

    if (idx > list->capacity)
    {
        ERROR_LOG ("Index [%lu] is grater than capacity = %lu", idx, list->capacity);

        return LIST_ERROR_WRONG_INDEX;        
    }


    *insertedIdx = list->free;
    list->free = (size_t) list->elements[list->free].next;

    list->elements[*insertedIdx].data = val;
    list->elements[*insertedIdx].next = list->elements[idx].next;
    list->elements[*insertedIdx].prev = (ssize_t)idx;

    ssize_t nextIdx = list->elements[idx].next;
    list->elements[nextIdx].prev = (ssize_t)*insertedIdx;
    list->elements[idx].next     = (ssize_t)*insertedIdx;

    LIST_DUMP (list, "after Insert()");
    
    return LIST_VERIFY (list);
}

int ListInsertBefore (list_t *list, size_t idx, listDataType val, size_t *insertedIdx)
{
    assert (list);
    assert (insertedIdx);

    if (idx > list->capacity)
    {
        ERROR_LOG ("Index [%lu] is grater than capacity = %lu", idx, list->capacity);

        return LIST_ERROR_WRONG_INDEX;        
    }
    if (list->elements[idx].prev == kListPrevFree)
    {
        ERROR_LOG ("%s", "Can't insert before empty element :(");

        return LIST_ERROR_INSERT_AFTER_EMPTY;
    }

    return ListInsert (list, (size_t)list->elements[idx].prev, val, insertedIdx);
}

int ListDelete (list_t *list, size_t idx)
{
    assert (list);

    char comment[kMaxCommentLen] = {};
    snprintf (comment, kMaxCommentLen, "before Delete() [%lu]", idx);
    LIST_DUMP (list, comment);

    LIST_DO_AND_CHECK (LIST_VERIFY (list));
    DEBUG_LOG ("%s", "first verify passed");

    ssize_t prevIdx = list->elements[idx].prev;
    ssize_t nextIdx = list->elements[idx].next;

    if (list->size == 0)
    {
        ERROR_LOG ("%s", "List is empty, but you are trying to delete something...");

        return LIST_ERROR_DELETE_IN_EMPTY_LIST;
    }
    if (list->elements[idx].prev == kListPrevFree)
    {
        ERROR_LOG ("%s", "Trying to delete free element... Are you okay?");

        return LIST_ERROR_DELETE_EMPTY_ELEMENT;
    }
    if (idx > list->capacity)
    {
        ERROR_LOG ("Index [%lu] is grater than capacity = %lu", idx, list->capacity);

        return LIST_ERROR_WRONG_INDEX;        
    }
    list->size--;

    list->elements[nextIdx].prev = prevIdx;
    list->elements[prevIdx].next = nextIdx;

    list->elements[idx].data = kListPoison;
    list->elements[idx].next = (int) list->free;
    list->elements[idx].prev = kListPrevFree;

    list->free = idx;

    LIST_DUMP (list, "after Delte()");

    return LIST_VERIFY (list);
}

int ListDeleteBefore (list_t *list, size_t idx)
{
    assert (list);

    if (idx > list->capacity)
    {
        ERROR_LOG ("Index [%lu] is grater than capacity = %lu", idx, list->capacity);

        return LIST_ERROR_WRONG_INDEX; 
    }

    return ListDelete (list, (size_t)list->elements[idx].prev);
}

#ifdef PRINT_DEBUG

#endif // PRINT_DEBUG

size_t ListGetHead (list_t *list)
{
    assert (list);

    return (size_t) list->elements[kListStart].next;
}

size_t ListGetTail (list_t *list)
{
    assert (list);

    return (size_t) list->elements[kListStart].prev;
}

bool IsValidIdx (list_t *list, size_t idx)
{
    assert (list);

    return idx < list->capacity + 1;
}

// node1->node2
// node2 is next for node1
bool IsBidirectional (list_t *list, size_t node1, size_t node2)
{
    assert (list);

    bool bidirectional = list->elements[node1].next == (ssize_t) node2 && 
                         list->elements[node2].prev == (ssize_t) node1;
    
    if (list->size > 1)
        bidirectional &= (list->elements[node2].next != (ssize_t) node1 &&
                          list->elements[node1].prev != (ssize_t) node2);
    
    return bidirectional;
}

void ListDtor (list_t *list)
{
    assert (list);

    free (list->elements);
    list->elements = NULL;

#ifdef PRINT_DEBUG
    fprintf (list->log.logFile, "%s", "</pre>\n");

    fclose (list->log.logFile);
#endif // PRINT_DEBUG
}

int ListVerify (list_t *list)
{
    assert (list);
    
    int error = LIST_ERROR_OK;

    error = CheckIdxes (list);

    if (error != LIST_ERROR_OK)
    {
        DEBUG_VAR ("%d", error);

        return error;
    }

    // FIXME: do not need this i think (?)
    error |= CheckFreeListCount     (list);
    error |= CheckElementsListCount (list);

    if (error != LIST_ERROR_OK)
    {
        DEBUG_VAR ("%d", error);
        
        return error;
    }

    error |= CheckNextEdges (list);
    error |= CheckPrevEdges (list);

    error |= CheckFreeElementsPrev (list);

    // FIXME: Check poison data

        // if (IsEqual(list->elements[i].data, kListPoison))
        // {
        //     ERROR_LOG ("Poison data in not free element [%lu]", i);

        //     error |= LIST_ERROR_POISON_IN_DATA;
        // }
    DEBUG_VAR ("%d", error);

    return error;
}

int CheckIdxes (list_t *list)
{
    DEBUG_LOG ("%s", "Check valid indexes");

    for (size_t i = 0; i < list->capacity + 1; i++)
    {
        if (list->elements[i].next > (ssize_t) list->capacity)
        {
            ERROR_LOG ("[%lu]->next = %zd; - index grater than capacity=%lu",
                        i, list->elements[i].next, list->capacity);

            return LIST_ERROR_IDX_OUT_OF_BOUNDS;
        }
        if (list->elements[i].prev > (ssize_t) list->capacity)
        {
            ERROR_LOG ("[%lu]->prev = %zd; - index grater than capacity=%lu",
                        i, list->elements[i].prev, list->capacity);

            return LIST_ERROR_IDX_OUT_OF_BOUNDS;
        }
    }
    
    return LIST_ERROR_OK;
}

int CheckFreeListCount (list_t *list)
{
    DEBUG_LOG ("%s", "Check loop in free list:");

    size_t cnt = 0;

    for (size_t idx = list->free; idx != kListStart; idx = (size_t)list->elements[idx].next)
    {
        cnt++;
    
        if (cnt > list->capacity)
        {
            ERROR_LOG ("%s", "List of free elements is looped");

            return LIST_ERROR_FREE_LOOPED;
        }
    }
    if (cnt < list->capacity)
    {
        // FIXME:
    }

    return LIST_ERROR_OK;
}

int CheckElementsListCount (list_t *list)
{
    DEBUG_LOG ("%s", "Check loop in data list:");

    size_t cnt = 0;

    for (size_t idx = ListGetHead (list); idx != kListStart; idx = (size_t)list->elements[idx].next)
    {
        cnt++;

        if (cnt > list->capacity)
        {
            ERROR_LOG ("%s", "List of data elements is looped");

            return LIST_ERROR_DATA_LOOPED;
        }
    }
    if (cnt < list->capacity)
    {
        // FIXME:
    }

    return LIST_ERROR_OK;
}
int CheckNextEdges (list_t *list)
{
    for (size_t i = 0; i < list->capacity; i++)
    {
        ssize_t nextIdx = list->elements[i].next;

        if (list->elements[i].prev != kListPrevFree &&
            !IsBidirectional (list, i, (size_t)nextIdx))
        {
            ERROR_LOG ("Next Edge between [%lu] and [%zd] elements is broken",
                       i, nextIdx);
            
            return LIST_ERROR_BROKEN_NEXT_EDGE;
        }
    }

    return LIST_ERROR_OK;
}
int CheckPrevEdges (list_t *list)
{
    int error = LIST_ERROR_OK;

    for (size_t i = 0; i < list->capacity; i++)
    {
        ssize_t prevIdx = list->elements[i].prev;

        if (prevIdx != kListPrevFree &&
            !IsBidirectional (list, (size_t)prevIdx, i))
        {
            ERROR_LOG ("Prev edge between [%zd] and [%lu] elements is broken",
                       prevIdx, i);

            error |= LIST_ERROR_BROKEN_PREV_EDGE;
        }
    }
    
    return error;
}

// int CheckNextEdges (list_t *list)
// {
//     DEBUG_LOG ("%s", "Check next edges:");

//     for (ssize_t idx = (ssize_t) ListGetHead (list); 
//          list->elements[idx].next != kListStart; 
//          idx = list->elements[idx].next)
//     {
//         DEBUG_VAR ("%zd", idx);


//         ssize_t prevIdx = list->elements[idx].prev;

//         if (list->elements[prevIdx].next != idx)
//         {
//             ERROR_LOG ("Next edge between [%zd] and [%zd] elements is broken",
//                        idx, prevIdx);
//             ERROR_LOG ("list->elements[%zd].prev = %zd;", idx, list->elements[idx].prev);

//             return LIST_ERROR_BROKEN_NEXT_EDGE;
//         }
//     }
    
//     return LIST_ERROR_OK;
// }

// int CheckPrevEdges (list_t *list)
// {
//     DEBUG_LOG ("%s", "Check prev edges:");

//     int error = LIST_ERROR_OK;

//     for (ssize_t idx = (ssize_t) ListGetTail (list); 
//          list->elements[idx].prev != kListStart; 
//          idx = list->elements[idx].prev)
//     {
//         DEBUG_VAR ("%zd", idx);
//         ssize_t nextIdx = list->elements[idx].next;

//         if (nextIdx == 0)
//             break;

//         if (list->elements[nextIdx].prev != idx)
//         {
//             ERROR_LOG ("Prev edge between [%zd] and [%zd] elements is broken",
//                        idx, nextIdx);
//             ERROR_LOG ("list->elements[%zd].next = %zd;", idx, list->elements[idx].next);
//             ERROR_LOG ("list->elements[%zd].prev = %zd;", nextIdx, list->elements[idx].prev);

//             error |= LIST_ERROR_BROKEN_PREV_EDGE;
//         }
                
//         if (IsEqual(list->elements[idx].data, kListPoison))
//         {
//             ERROR_LOG ("Poison data in not free element [%zd]", idx);

//             error |= LIST_ERROR_POISON_IN_DATA;
//         }
//     }

//     return error;
// }

int CheckFreeElementsPrev (list_t *list)
{
    DEBUG_LOG ("%s", "Check free elements:");

    size_t cnt = 0;

    for (size_t idx = list->free; idx != kListStart; idx = (size_t)list->elements[idx].next)
    {
        cnt++;

        if (list->elements[idx].prev != kListPrevFree)
        {
            ERROR_LOG ("%s", "[%lu].prev = %zd, but should be %zd");

            return LIST_ERROR_BROKEN_FREE_ELEMENT;
        }
    }

    return LIST_ERROR_OK;
}