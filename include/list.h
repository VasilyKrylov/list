#ifndef K_LIST_H
#define K_LIST_H

#include <stdio.h>

#include "list_log.h"
#include "debug.h"

typedef double listDataType;

const size_t kListStart          = 0;
const size_t kMaxCommentLen         = 64;

const ssize_t kListPrevFree      = -1;
const listDataType kListPoison   = -666;
const size_t kListMaxLen         = (1UL << 32);

#define LIST_DO_AND_CHECK(action)           \
        do                                  \
        {                                   \
            int status = action;            \
            DEBUG_VAR("%d", status);        \
                                            \
            if (status != LIST_ERROR_OK)    \
                return status;              \
        } while (0)                       

#define LIST_DO_AND_CLEAR(action, clearAction)      \
        do                                          \
        {                                           \
            int status = action;                    \
            DEBUG_VAR("%d", status);                \
                                                    \
            if (status != LIST_ERROR_OK)            \
            {                                       \
                clearAction;                        \
                                                    \
                return status;                      \
            }                                       \
        } while (0)                       

#ifdef PRINT_DEBUG

struct varInfo_t
{
    const char *name = NULL;
    const char *file = NULL;
    int line         = 0;
    const char *func = NULL;
};
#define LIST_CTOR(listName, size) ListCtor (&listName, size,                        \
                                            varInfo_t{.name = #listName,            \
                                                      .file = __FILE__,             \
                                                      .line = __LINE__,             \
                                                      .func = __func__})
#define LIST_VERIFY(list) ListVerify(list)
#define LIST_DUMP(listName, comment) ListDump (listName, comment, __FILE__, __LINE__, __func__)

#else

#define LIST_CTOR(listName, size) ListCtor (&listName, size) // NOTE: removed ;
#define LIST_VERIFY(list) LIST_ERROR_OK;
#define LIST_DUMP(list, comment) 

#endif // PRING_DEBUG

struct listElement_t
{
    listDataType data = kListPoison;

    ssize_t next = 0; // to be the same type as prev
    ssize_t prev = 0; // -1 for free elements
};
struct list_t
{
    listElement_t *elements = NULL;

    size_t free = 1;
    size_t capacity = 0;
    size_t size = 0;

#ifdef PRINT_DEBUG
    varInfo_t varInfo = {};

    listLog_t log = {};
#endif
};

enum listError_t
{
    LIST_ERROR_OK                       = 0,
    LIST_ERROR_NULL_STRUCT              = 1 << 0,
    LIST_ERROR_NULL_DATA                = 1 << 1,
    LIST_ERROR_BIG_LEN                  = 1 << 2,
    LIST_ERROR_INSERT_AFTER_EMPTY       = 1 << 3,
    LIST_ERROR_BROKEN_NEXT_EDGE         = 1 << 4,
    LIST_ERROR_BROKEN_PREV_EDGE         = 1 << 5,
    LIST_ERROR_BROKEN_FREE_ELEMENT      = 1 << 6,
    LIST_ERROR_POISON_IN_DATA           = 1 << 7,
    LIST_ERROR_DELETE_IN_EMPTY_LIST     = 1 << 8,
    LIST_ERROR_DELETE_EMPTY_ELEMENT     = 1 << 9,
    LIST_ERROR_WRONG_INDEX              = 1 << 10,
    LIST_ERROR_DATA_LOOPED              = 1 << 11,
    LIST_ERROR_FREE_LOOPED              = 1 << 12,
    LIST_ERROR_IDX_OUT_OF_BOUNDS        = 1 << 13,

    LIST_ERROR_COMMON                   = 1 << 31
};

int ListCtor            (list_t *list, size_t capacity
                         ON_DEBUG (,varInfo_t varInfo));
int ListInsert          (list_t *list, size_t idx, listDataType val, size_t *insertedIdx);
int ListInsertBefore    (list_t *list, size_t idx, listDataType val, size_t *insertedIdx);
int ListDelete          (list_t *list, size_t idx);
int ListDeleteBefore    (list_t *list, size_t idx);
size_t ListGetHead      (list_t *list);
size_t ListGetTail      (list_t *list);
bool IsValidIdx         (list_t *list, size_t idx);
bool IsBidirectional    (list_t *list, size_t node1, size_t node2);
int ListVerify          (list_t *list);
void ListDtor           (list_t *list);

#endif //K_LIST_H