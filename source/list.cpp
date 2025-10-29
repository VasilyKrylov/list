#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>

#include "debug.h"

#include "list.h"
#include "file_utils.h"
#include "float_math.h"

const char * const kGray        = "666666"; // TODO:
const char * const kBlue        = "6666ff";
const char * const kDarkBlue    = "000099";
const char * const kGreen       = "33cc33";
const char * const kDarkGreen   = "006600";

int ListDumpImg     (list_t *list);
int DumpMakeConfig  (list_t *list);
int DumpMakeImg     (list_t *list);


int ListCtor (list_t *list, size_t capacity
              ON_DEBUG (, varInfo_t varInfo))
{
    assert (list);

    if (capacity > kListMaxLen)
        return LIST_ERROR_BIG_LEN;

    list->elements = (listElement_t *) calloc (capacity, sizeof(listElement_t));
    if (list->elements == NULL)
        return LIST_ERROR_COMMON |
               COMMON_ERROR_ALLOCATING_MEMORY;

    list->free = 1;
    list->capacity = capacity;
    list->len = 0;

    list->elements[0] = {.data = 0, .next = 0, .prev = 0};
    for (size_t i = 1; i < capacity - 1; i++)
    {
        list->elements[i].data = kListPoison;
        list->elements[i].prev = kListPrevFree;
        list->elements[i].next = (int)i + 1;
    }
    list->elements[capacity - 1] = {.data = kListPoison, .next = 0, .prev = kListPrevFree};


#ifdef PRINT_DEBUG
    list->varInfo = varInfo;

    LogInit ()


#endif // PRING_DEBUG

    LIST_DUMP (*list, "After ctor");

    return LIST_VERIFY (list);
}

int ResizeElements (list_t *list)
{
    assert (list);

    if (list->len + 1 == list->capacity)
    {
        list->capacity *= 2;
        listElement_t *newElemenets = (listElement_t *) realloc (list->elements, 
                                                                 list->capacity * sizeof(listElement_t));
        list->elements[list->capacity - 1].next = kListStart;
        if (newElemenets == NULL)
        {
            perror ("Error trying to realloc elements of the list");

            return LIST_ERROR_COMMON |
                   COMMON_ERROR_ALLOCATING_MEMORY;
        }

        list->elements = newElemenets;
        
        list->free = list->len;
        for (size_t i = list->len; i < list->capacity; i++)
        {
#ifdef PRINT_DEBUG
            list->elements[i].data = kListPoison;
            list->elements[i].prev = kListPrevFree;
#endif // PRINT_DEBUG
            list->elements[i].next = list->len + 1;
        }
        
        DEBUG_LOG ("list->elements reallocated to %lu capacity", list->capacity);
    }

    return LIST_ERROR_OK;
}

int ListInsert (list_t *list, size_t idx, listDataType val, size_t *insertedIdx)
{
    assert (list);
    assert(insertedIdx);

    char comment[kMaxCommentLen] = {};
    snprintf (comment, kMaxCommentLen, "before Insert() [%lu], val = %g", idx, val); // TODO function
    LIST_DUMP (*list, comment);

    DO_AND_CHECK (LIST_VERIFY (list));

    if (list->len == list->capacity)
    {
    }
    list->len++;

    if (list->elements[idx].prev == kListPrevFree)
    {
        ERROR_LOG ("%s", "Can't insert after empty element :(");

        return LIST_ERROR_INSERT_AFTER_EMPTY;
    }
    //check idxes


    *insertedIdx = list->free;
    list->free = (size_t) list->elements[list->free].next;

    list->elements[*insertedIdx].data = val;
    list->elements[*insertedIdx].next = list->elements[idx].next;
    list->elements[*insertedIdx].prev = (ssize_t)idx;

    ssize_t nextIdx = list->elements[idx].next;
    list->elements[nextIdx].prev = (ssize_t)*insertedIdx;
    list->elements[idx].next     = (ssize_t)*insertedIdx;

    LIST_DUMP (*list, "after Insert()");
    
    return LIST_VERIFY (list);
}

int ListDelete (list_t *list, size_t idx)
{
    assert (list);

    char comment[kMaxCommentLen] = {};
    snprintf (comment, kMaxCommentLen, "before Delete() [%lu]", idx);
    LIST_DUMP (*list, comment);

    DO_AND_CHECK (LIST_VERIFY (list));

    ssize_t prevIdx = list->elements[idx].prev;
    ssize_t nextIdx = list->elements[idx].next;

    if (list->len == 0)
    {
        ERROR_LOG ("%s", "List is empty, but you are trying to delete something...");

        return LIST_ERROR_DELETE_IN_EMPTY_LIST;
    }
    list->len--;

    if (list->elements[idx].prev == kListPrevFree)
    {
        ERROR_LOG ("%s", "Trying to delete free element... Are you okay?");

        return LIST_ERROR_DELETE_EMPTY_ELEMENT;
    }

    list->elements[nextIdx].prev = prevIdx;
    list->elements[prevIdx].next = nextIdx;

    list->elements[idx].data = kListPoison;
    list->elements[idx].next = (int) list->free;
    list->elements[idx].prev = kListPrevFree;

    list->free = idx;

    LIST_DUMP (*list, "after Delte()");

    return LIST_VERIFY (list);
}

int ListDump (list_t *list, const char *comment,
              const char *FILE_, int LINE_, const char *FUNC_)
{
    assert(list);
    assert(comment);
    assert(FILE_);
    assert(FUNC_);

    DEBUG_PRINT ("%s", "\n");
    DEBUG_LOG ("comment = \"%s\";", comment);

    fprintf (list->log.logFile,
             "<h3>DUMP called at %s:%d:%s(): %s</h3>\n",
             FILE_, LINE_, FUNC_, comment);
    fprintf (list->log.logFile,
             "%s {%s:%d}\n",
             list->varInfo.name, list->varInfo.file, list->varInfo.line);

    fprintf (list->log.logFile, "list->capacity = %lu\n", list->capacity);
    fprintf (list->log.logFile, "list->len = %lu\n", list->len);
    fprintf (list->log.logFile, "list->free = %lu\n", list->free);

    fprintf (list->log.logFile, "%s", "list->data: ");
    for (size_t i = 0; i < list->capacity; i++)
        fprintf (list->log.logFile, "%4g, ", list->elements[i].data);

    fprintf (list->log.logFile, "%s", "\nlist->next: ");
    for (size_t i = 0; i < list->capacity; i++)
        fprintf (list->log.logFile, "%4zd, ", list->elements[i].next);

    fprintf (list->log.logFile, "%s", "\nlist->prev: ");
    for (size_t i = 0; i < list->capacity; i++)
        fprintf (list->log.logFile, "%4zd, ", list->elements[i].prev);

    fprintf (list->log.logFile, "%s", "\n");

    ListDumpImg (list);

    fprintf (list->log.logFile, "%s", "<hr>\n\n");
    
    return LIST_ERROR_OK;
}

#define CREATE_FILE_OR_RETURN(file, name)                       \
        do {                                                    \
            file  = fopen (name, "w+");                         \
            if (file == NULL)                                   \
            {                                                   \
                ERROR_LOG ("Error opening file \"%s\"", name);  \
                return LIST_ERROR_COMMON |                      \
                    COMMON_ERROR_OPENING_FILE;                  \
            }                                                   \
        } while(0)

int ListDumpImg (list_t *list)
{
    DO_AND_CHECK (DumpMakeConfig(list));
    DO_AND_CHECK (DumpMakeImg(list));

    return LIST_ERROR_OK;
}

int DumpMakeConfig (list_t *list)
{
    CREATE_FILE_OR_RETURN (list->log.graphFile, list->log.graphFilePath);
#undef CREATE_DIR_OR_RETURN

    fprintf (list->log.graphFile, "%s", "digraph G {\n"
                                        "\trankdir=LR;\n");

    // Style for each node
    for (size_t i = 0; i < list->capacity; i++)
    {
        const char *color = kBlue;
        if (i == (size_t) kListStart)
            color = kGray;
        else if ((ssize_t)i == list->elements[kListStart].next)
            color = kDarkBlue;
        else if (i == list->free)
            color = kDarkGreen;
        else if (list->elements[i].prev == kListPrevFree)
            color = kGreen; 
        
        fprintf (list->log.graphFile,
                 "\telement%lu [shape=Mrecord; style=\"filled\"; fillcolor=\"#%s\"; "
                 "label = \"data = [%g] | next = [%zd] | prev = [%zd]\"];\n",
                 i, color,
                 list->elements[i].data, list->elements[i].next, list->elements[i].prev);
    }

    // to make them in one line
    fprintf (list->log.graphFile, "%s", "\tedge [color=invis];\n"
                                        "\t");
    for (size_t i = 0; i < list->capacity - 1; i++)
    {
        fprintf (list->log.graphFile, "element%lu->", i);
    }
    fprintf (list->log.graphFile, "element%lu;\n"
                                  "\tedge [style=\"\"];\n"
                                  "\t", 
                                  list->capacity - 1);
    
    // connect free elements
    for (size_t i = list->free; ; i = (size_t) list->elements[i].next)
    {
        if (list->elements[i].next == kListStart || i == kListStart)
        {
            fprintf (list->log.graphFile, "element%lu[color=green constraint=false];\n", i);
            break;
        }
        else
            fprintf (list->log.graphFile, "element%lu->", i);
    }

    fprintf (list->log.graphFile, "%s", "\t");
    size_t i = kListStart; // TODO: do not use i in second for loop
    for (; ; i = (size_t) list->elements[i].next)
    {
        if (list->elements[i].next == kListStart)
        {
            fprintf (list->log.graphFile, "element%lu[color=red; constraint=false];\n", i);
            break;
        }
        else
            fprintf (list->log.graphFile, "element%lu->", i);
    }

    for (size_t j = i; ; j = (size_t) list->elements[j].prev)
    {
        if (j == kListStart)
        {
            fprintf (list->log.graphFile, "element%lu[color=blue; constraint=false];\n", j);
            break;
        }
        else
            fprintf (list->log.graphFile, "element%lu->", j);
    }

    fprintf (list->log.graphFile, "%s", "}");
    fclose (list->log.graphFile);
    
    return LIST_ERROR_OK;
}

int DumpMakeImg (list_t *list)
{
    static size_t imageCounter = 0;
    imageCounter++;

    char imgFileName[kImgNameLen] = {};
    snprintf (imgFileName, kImgNameLen, "%lu.svg", imageCounter);

    const size_t kMaxCommandLen = 256;
    char command[kMaxCommandLen] = {};

    snprintf (command, kMaxCommandLen, "dot %s%s -T svg -o %s%s", 
              list->log.logFolderPath, kGraphFileName,
              list->log.imgFolderPath, imgFileName);

    int status = system (command);
    DEBUG_VAR ("%d", status);
    if (status != 0)
    {
        ERROR_LOG ("ERROR executing command \"%s\"", command);
        
        return LIST_ERROR_COMMON |
               COMMON_ERROR_RUNNING_SYSTEM_COMMAND;
    }

    fprintf (list->log.logFile,
             "<img src=\"img/%s\" hieght=\"500\">\n",
             imgFileName);

    DEBUG_VAR ("%s", command);

    return LIST_ERROR_OK;
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

// FIXME: ABSOLUTE SHIT
int ListVerify (list_t *list)
{
    assert (list);
    
    int error = LIST_ERROR_OK;

    // TODO: listGetHead
    // TODO: listGetTail
    ssize_t nextAfterHead = list->elements[list->elements[kListStart].next].next;
    ssize_t idx = nextAfterHead;

    DEBUG_LOG ("%s", "Check next edges:");

    for (idx = nextAfterHead; list->elements[idx].next != kListStart; idx = list->elements[idx].next)
    {
        DEBUG_VAR ("%zd", idx);

        ssize_t prevIdx = list->elements[idx].prev;

        if (list->elements[prevIdx].next != idx)
        {
            ERROR_LOG ("Next edge between [%zd] and [%zd] elements is broken",
                       idx, prevIdx);
            ERROR_LOG ("list->elements[%zd].prev = %zd;", idx, list->elements[idx].prev);

            error |= LIST_ERROR_BROKEN_NEXT_EDGE;
        }
    }

    DEBUG_LOG ("%s", "Check prev edges:");

    for (; list->elements[idx].prev != kListStart; idx = list->elements[idx].prev)
    {
        DEBUG_VAR ("%zd", idx);
        ssize_t nextIdx = list->elements[idx].next;

        if (nextIdx == 0)
            break;

        if (list->elements[nextIdx].prev != idx)
        {
            ERROR_LOG ("Prev edge between [%zd] and [%zd] elements is broken",
                       idx, nextIdx);
            ERROR_LOG ("list->elements[%zd].next = %zd;", idx, list->elements[idx].next);
            ERROR_LOG ("list->elements[%zd].prev = %zd;", nextIdx, list->elements[idx].prev);

            error |= LIST_ERROR_BROKEN_PREV_EDGE;
        }


        if (IsEqual(list->elements[idx].data, kListPoison))
        {
            ERROR_LOG ("Poison data in data element [%zd]", idx);

            error |= LIST_ERROR_POISON_IN_DATA;
        }

    }

    // TODO: check free elements for poison

    return error;
}
