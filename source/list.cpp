#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <time.h>

#include "debug.h"

#include "list.h"
#include "file_utils.h"

void GetTimeStamp (char *str);
int ListDumpImg (list_t *list);

int ListCtor (list_t *list, size_t len
              ON_DEBUG (, varInfo_t varInfo))
{
    assert (list);

    if (len > kListMaxLen)
        return LIST_ERROR_BIG_LEN;

    list->elements = (listElement_t *)calloc (len, sizeof(listElement_t));
    if (list->elements == NULL)
        return LIST_ERROR_COMMON |
               COMMON_ERROR_ALLOCATING_MEMORY;

    list->free = 1;
    list->head = 0;
    list->len  = len;

    list->elements[0] = {.data = 0, .next = 0, .prev = 0};
    for (size_t i = 1; i < len - 1; i++)
    {
        list->elements[i].data = kListPoison;
        list->elements[i].prev = -1;
        list->elements[i].next = (int)i + 1;
    }
    list->elements[len - 1] = {.data = kListPoison, .next = 0, .prev = 0};


#ifdef PRINT_DEBUG
    list->varInfo = varInfo;
    // for (size_t i = 1; i < len; i++)
    //     list->elements[i].data = kListPoison;

    time_t t = time (NULL);
    struct tm tm = *localtime (&t);

    snprintf (list->log.logFolderPath, kLogFolderPathLen, "%s%d-%02d-%02d_%02d:%02d:%02d/",
              kParentDumpFolderName,
              tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
              tm.tm_hour,        tm.tm_min,     tm.tm_sec); // NOTE: check return code?

    snprintf (list->log.logFilePath,   kLogFilePathLen, "%s%s",
              list->log.logFolderPath, kLogFileName);

    snprintf (list->log.graphFilePath, kGraphFilePathLen, "%s%s",
              list->log.logFolderPath, kGraphFileName);

    snprintf (list->log.imgFolderPath, kImgFolderPathLen, "%s%s",
              list->log.logFolderPath, kImgFolderName);

#define CREATE_DIR_OR_RETURN(name)              \
        do {                                    \
            int status = SafeMkdir (name);      \
            if (status != COMMON_ERROR_OK)      \
                return LIST_ERROR_COMMON |      \
                    status;                     \
        } while (0)

    CREATE_DIR_OR_RETURN (kParentDumpFolderName);
    CREATE_DIR_OR_RETURN (list->log.logFolderPath);
    CREATE_DIR_OR_RETURN (list->log.imgFolderPath);

#undef CREATE_DIR_OR_RETURN

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

    CREATE_FILE_OR_RETURN (list->log.logFile,   list->log.logFilePath);
    CREATE_FILE_OR_RETURN (list->log.graphFile, list->log.graphFilePath);

    fprintf (list->log.logFile, "%s", "<pre>\n");
#undef CREATE_FILE_OR_RETURN

#endif // PRING_DEBUG

    return LIST_ERROR_OK;
}

int ListInsert (list_t *list, size_t idx, listDataType val, size_t *insertedIdx)
{
    assert (list);

    if (list->free == kListStart)
    {
        ERROR_LOG ("%s", "List is full");

        return 1;
    }

    *insertedIdx = list->free;
    list->elements[*insertedIdx].data = val;
    
    list->free = (size_t) list->elements[list->free].next;


    if (idx == kListStart)
    {
        list->elements[*insertedIdx].next = (int) list->head;
        list->elements[*insertedIdx].prev = 0;

        list->head = *insertedIdx;
    }
    else if (list->elements[idx].next == kListStart)
    {
        list->elements[*insertedIdx].next = (int) kListStart;
        list->elements[*insertedIdx].prev = (int) idx;

        list->elements[idx].next = (int)*insertedIdx;
    }
    else
    {
        list->elements[*insertedIdx].next = list->elements[idx].next;
        list->elements[*insertedIdx].prev = (int) idx;

        int nextIdx = list->elements[idx].next;
        list->elements[nextIdx].prev = (int) *insertedIdx;

        list->elements[idx].next =  (int) *insertedIdx;
    }
    
    return LIST_ERROR_OK;
}

int ListDelete (list_t *list, size_t idx)
{
    assert (list);

    if (list->head == kListStart)
    {
        ERROR_LOG ("%s", "List is empty, but you are trying to delete something...");

        return 1;
    }
    if (list->elements[idx].prev == -1)
    {
        ERROR_LOG ("%s", "Trying to delete free element... Are you okay?");

        return 1;
    }

    int prevIdx = list->elements[idx].prev;
    int nextIdx = list->elements[idx].next;

    if (prevIdx == 0)
    {
        list->elements[nextIdx].prev = prevIdx;

        list->head = (size_t) list->elements[idx].next;
    }
    else if (nextIdx == 0)
    {
        list->elements[prevIdx].next = nextIdx;
    }
    else
    {
        list->elements[nextIdx].prev = prevIdx;
        list->elements[prevIdx].next = nextIdx;
    }

    list->elements[idx].data = kListPoison;
    list->elements[idx].next = (int) list->free;
    list->elements[idx].prev = -1;

    list->free = idx;

    return LIST_ERROR_OK;
}

int ListDump (list_t *list, const char *comment,
              const char *FILE_, int LINE_, const char *FUNC_)
{
    fprintf (list->log.logFile,
             "<h3>DUMP called at %s:%d:%s(): %s</h3>\n",
             FILE_, LINE_, FUNC_, comment);
    fprintf (list->log.logFile,
             "%s {%s:%d}\n",
             list->varInfo.name, list->varInfo.file, list->varInfo.line);

    fprintf (list->log.logFile, "list->len = %lu\n", list->len);

    fprintf (list->log.logFile, "%s", "list->data: ");
    for (size_t i = 0; i < list->len; i++)
        fprintf (list->log.logFile, "%4g, ", list->elements[i].data);

    fprintf (list->log.logFile, "%s", "\nlist->next: ");
    for (size_t i = 0; i < list->len; i++)
        fprintf (list->log.logFile, "%4d, ", list->elements[i].next);

    fprintf (list->log.logFile, "%s", "\nlist->prev: ");
    for (size_t i = 0; i < list->len; i++)
        fprintf (list->log.logFile, "%4d, ", list->elements[i].prev);

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
    static size_t imageCounter = 0;
    imageCounter++;

    CREATE_FILE_OR_RETURN (list->log.graphFile, list->log.graphFilePath);
#undef CREATE_DIR_OR_RETURN

    fprintf (list->log.graphFile, "%s", "digraph G {\n"
                                        "\trankdir=LR;\n");

    for (size_t i = 0; i < list->len; i++)
    {
        fprintf (list->log.graphFile,
                 "\telement%lu [shape=Mrecord; label = \"{data = [%g] | next = [%d] | prev = [%d]}\"];\n",
                 i, list->elements[i].data, list->elements[i].next, list->elements[i].prev);
    }
    fprintf (list->log.graphFile, "%s", "\tedge [color=invis];\n"
                                        "\t");
    for (size_t i = 0; i < list->len - 1; i++)
    {
        fprintf (list->log.graphFile, "element%lu->", i);
    }
    fprintf (list->log.graphFile, "element%lu;\n", list->len - 1);
    fprintf (list->log.graphFile, "%s", "\tedge [style=\"\"];\n");

    fprintf (list->log.graphFile, "%s", "\t");
    for (size_t i = list->free; ; i = (size_t) list->elements[i].next)
    {
        if (list->elements[i].next == kListStart)
        {
            fprintf (list->log.graphFile, "element%lu[color=green];\n", i);
            break;
        }
        else
            fprintf (list->log.graphFile, "element%lu->", i);
    }

    fprintf (list->log.graphFile, "%s", "\t");
    size_t i = list->head;
    for (i = list->head; ; i = (size_t) list->elements[i].next)
    {
        if (list->elements[i].next == kListStart)
        {
            fprintf (list->log.graphFile, "element%lu[color=red];\n", i);
            break;
        }
        else
            fprintf (list->log.graphFile, "element%lu->", i);
    }

    for (size_t j = i; ; j = (size_t) list->elements[j].prev)
    {
        if (list->elements[j].prev == kListStart)
        {
            fprintf (list->log.graphFile, "element%lu[color=blue];\n", j);
            break;
        }
        else
            fprintf (list->log.graphFile, "element%lu->", j);
    }

    fprintf (list->log.graphFile, "%s", "}");
    fclose (list->log.graphFile);

    char imgFileName[kImgNameLen] = {}; // FIXME: looks like shit tbh
    sprintf (imgFileName, "%lu", imageCounter);
    strcat (imgFileName, ".svg"); 

    char command[256] = {}; // FXIME: magic number

    strcat (command, "dot ");
    strcat (command, list->log.logFolderPath);
    strcat (command, kGraphFileName);
    strcat (command, " -T svg -o ");
    strcat (command, list->log.imgFolderPath);
    strcat (command, imgFileName); // FIXME: ABSOLUTLY HORRIBLE! COMPLEXITY IS 0(n^2)!!!!!!!!!!!!!!

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
    free (list->elements);
    list->elements = NULL;

#ifdef PRINT_DEBUG
    fprintf (list->log.logFile, "%s", "</pre>\n");

    fclose (list->log.logFile);
    // fclose (list->log.graphFile);
#endif // PRINT_DEBUG
}

void GetTimeStamp (char *str)
{
    time_t t = time (NULL);
    tm tm = *localtime (&t);

    // NOTE: not safe at all
    sprintf (str, "%d-%02d-%02d_%02d:%02d:%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
             tm.tm_hour,        tm.tm_min,     tm.tm_sec);
}
