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
    list->head = 1;
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

int ListDump (list_t *list, const char *comment,
              const char *FILE_, int LINE_, const char *FUNC_)
{
    fprintf (list->log.logFile,
             "<h3>DUMP called at %s:%d:%s(): %s</h3>\n",
             FILE_, LINE_, FUNC_, comment);
    fprintf (list->log.logFile,
             "%s {%s:%d}\n",
             list->varInfo.name, list->varInfo.file, list->varInfo.line);

    ListDumpImg (list);

    return LIST_ERROR_OK;
}

int ListDumpImg (list_t *list)
{
    fprintf (list->log.graphFile, "%s", "digraph G {\n"
                                        "rankdir=LR;\n");

    for (size_t i = 0; i < list->len; i++)
    {
        fprintf (list->log.graphFile,
                 "\telement%lu [shape=Mrecord; label = \"{data = [%g] | next = [%d] | prev = [%d]}\"];\n",
                 i, list->elements[i].data, list->elements[i].next, list->elements[i].prev);
    }
    fprintf (list->log.graphFile, "%s", "\tedge [style=invis];\n"
                                        "\t");
    for (size_t i = 0; i < list->len - 1; i++)
    {
        fprintf (list->log.graphFile, "element%lu->", i);
    }
    fprintf (list->log.graphFile, "element%lu;\n", list->len - 1);
    fprintf (list->log.graphFile, "%s", "\tedge [style=\"\"];\n");

    fprintf (list->log.graphFile, "%s", "}");

    char imgFileName[kImgNameLen] = {}; // FIXME: looks like shit tbh
    GetTimeStamp (imgFileName);
    strcat(imgFileName, ".svg");

    char command[256] = {}; // FXIME: fixed size len

    strcat (command, "dot ");
    strcat (command, list->log.logFolderPath);
    strcat (command, kGraphFileName);
    strcat (command, " -T svg -o ");
    strcat (command, list->log.imgFolderPath);
    strcat (command, imgFileName);

    DEBUG_VAR ("%s", command);
    system (command);
    DEBUG_VAR ("%s", command);

    fprintf (list->log.logFile,
             "<img src=\"img/%s\" hieght=\"500\">\n",
             imgFileName);

    return LIST_ERROR_OK;
}

void ListDtor (list_t *list)
{
    free (list->elements);
    list->elements = NULL;

#ifdef PRINT_DEBUG
    fprintf (list->log.logFile, "%s", "</pre>\n");

    fclose (list->log.logFile);
    fclose (list->log.graphFile);
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
