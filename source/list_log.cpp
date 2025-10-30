#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "list.h"
#include "file_utils.h"

#include "list_log.h"

#ifdef PRINT_DEBUG
static size_t imageCounter = 0;

const char * const kGray        = "666666"; // TODO:
const char * const kBlue        = "6666ff";
const char * const kDarkBlue    = "3333ff";
const char * const kGreen       = "33cc33";
const char * const kDarkGreen   = "006600";
const char * const kYellow      = "ffcc00";

int ListDumpImg     (list_t *list);
int DumpMakeConfig  (list_t *list);
int DumpMakeImg     (list_t *list);

int LogInit (listLog_t *log)
{
    time_t t = time (NULL);
    struct tm tm = *localtime (&t);

    snprintf (log->logFolderPath, kFileNameLen, "%s%d-%02d-%02d_%02d:%02d:%02d/",
              kParentDumpFolderName,
              tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
              tm.tm_hour,        tm.tm_min,     tm.tm_sec);

    snprintf (log->logFilePath,   kFileNameLen, "%s%s",
              log->logFolderPath, kLogFileName);

    snprintf (log->imgFolderPath, kFileNameLen, "%s%s",
              log->logFolderPath, kImgFolderName);

    snprintf (log->dotFolderPath, kFileNameLen, "%s%s",
              log->logFolderPath, kDotFolderName);


    if (SafeMkdir (kParentDumpFolderName) != LIST_ERROR_OK)
        return LIST_ERROR_COMMON | 
               COMMON_ERROR_CREATING_FILE;

    if (SafeMkdir (log->logFolderPath) != LIST_ERROR_OK)
        return LIST_ERROR_COMMON | 
            COMMON_ERROR_CREATING_FILE;

    if (SafeMkdir (log->imgFolderPath) != LIST_ERROR_OK)
        return LIST_ERROR_COMMON | 
            COMMON_ERROR_CREATING_FILE;

    if (SafeMkdir (log->dotFolderPath) != LIST_ERROR_OK)
        return LIST_ERROR_COMMON | 
            COMMON_ERROR_CREATING_FILE;

    log->logFile = fopen (log->logFilePath, "w");
    if (log->logFile == NULL)
    {
        ERROR_LOG ("Error opening file \"%s\"", log->logFilePath);
        
        return LIST_ERROR_COMMON |
               COMMON_ERROR_OPENING_FILE;
    }

    fprintf (log->logFile, "%s", "<pre>\n");

    return LIST_ERROR_OK;
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
             "<h3>DUMP called at %s:%d:%s(): <font style=\"color: green;\">%s</font></h3>\n",
             FILE_, LINE_, FUNC_, comment);
    fprintf (list->log.logFile,
             "%s[%p] initialized in {%s:%d}\n",
             list->varInfo.name, list, list->varInfo.file, list->varInfo.line);

    fprintf (list->log.logFile, "list->capacity = %lu\n", list->capacity);
    fprintf (list->log.logFile, "list->size = %lu\n", list->size);
    fprintf (list->log.logFile, "list->free = %lu\n", list->free);

    fprintf (list->log.logFile, "%s", "list->data: ");
    for (size_t i = 0; i < list->capacity + 1; i++)
        fprintf (list->log.logFile, "%4g, ", list->elements[i].data);

    fprintf (list->log.logFile, "%s", "\nlist->next: ");
    for (size_t i = 0; i < list->capacity + 1; i++)
        fprintf (list->log.logFile, "%4zd, ", list->elements[i].next);

    fprintf (list->log.logFile, "%s", "\nlist->prev: ");
    for (size_t i = 0; i < list->capacity + 1; i++)
        fprintf (list->log.logFile, "%4zd, ", list->elements[i].prev);

    fprintf (list->log.logFile, "%s", "\n");

    LIST_DO_AND_CHECK (ListDumpImg (list));

    fprintf (list->log.logFile, "%s", "<hr>\n\n");
    
    return LIST_ERROR_OK;
}

int ListDumpImg (list_t *list)
{
    imageCounter++;

    LIST_DO_AND_CHECK (DumpMakeConfig(list));
    LIST_DO_AND_CHECK (DumpMakeImg(list));

    return LIST_ERROR_OK;
}

int DumpMakeConfig (list_t *list)
{
    char graphFilePath[kFileNameLen + 22] = {};
    snprintf (graphFilePath, kFileNameLen + 22, "%s%lu.dot", list->log.dotFolderPath, imageCounter);

    FILE *graphFile  = fopen (graphFilePath, "w");
    if (graphFile == NULL)
    {
        ERROR_LOG ("Error opening file \"%s\"", graphFilePath);

        return LIST_ERROR_COMMON |
               COMMON_ERROR_OPENING_FILE;
    }

    fprintf (graphFile          ,   "digraph G {\n"
                                    "\trankdir=LR;\n"
                                    "\tsplines=ortho;\n");
                                    // "\tHEAD [shape=Mrecord; style=\"filled\"; fillcolor=\"#%s\";]",
                                    // kYellow);

    // Style for each node
    for (size_t i = 0; i < list->capacity + 1; i++)
    {
        const char *color = kBlue;
        if (i == (size_t) kListStart)
            color = kGray;
        else if (i == ListGetHead (list))
            color = kDarkBlue;
        else if (i == list->free)
            color = kDarkGreen;
        else if (list->elements[i].prev == kListPrevFree)
            color = kGreen; 
        
        fprintf (graphFile          ,
                 "\telement%lu [shape=Mrecord; style=\"filled\"; fillcolor=\"#%s\"; "
                 "label = \"idx = [%lu] | data = [%g] | next = [%zd] | prev = [%zd]\"];\n",
                 i, color,
                 i, list->elements[i].data, list->elements[i].next, list->elements[i].prev);
    }

    // to make them in one line
    fprintf (graphFile, "%s", "\tedge [color=invis];\n"
                                        "\t");
    for (size_t i = 0; i < list->capacity + 1 ; i++)
    {
        fprintf (graphFile, "element%lu->", i);
    }
    fprintf (graphFile          , "element%lu;\n"
                                  "\tedge [style=\"\"];\n"
                                  "\t", 
                                  list->capacity - 1);
    
    // connect free elements
    for (size_t i = list->free; ; i = (size_t) list->elements[i].next)
    {
        if (list->elements[i].next == kListStart || i == kListStart)
        {
            fprintf (graphFile, "element%lu[color=green; constraint=false];\n", i);
            break;
        }
        else
            fprintf (graphFile, "element%lu->", i);
    }

    fprintf (graphFile, "\telement%lu->", kListStart);
    for (size_t i = ListGetHead (list); ; i = (size_t) list->elements[i].next)
    {
        if (i == kListStart)
        {
            fprintf (graphFile, "element%lu[color=blue; constraint=false];\n", i);
            
            break;
        }
        else
            fprintf (graphFile, "element%lu->", i);
    }

    fprintf (graphFile, "\telement%lu->", kListStart);
    for (size_t i = ListGetTail (list); ; i = (size_t) list->elements[i].prev)
    {
        DEBUG_LOG ("\t list->elements[i].prev = %zd;", list->elements[i].prev);

        if (i == kListStart)
        {
            fprintf (graphFile, "element%lu[color=red; constraint=false];\n", i);
            break;
        }
        else
            fprintf (graphFile, "element%lu->", i);
    }

    // fprintf (graphFile          , "\tHEAD->element%lu[color=yellow; constraint=false];\n", ListGetHead (list));

    fprintf (graphFile, "%s", "}");
    fclose (graphFile);
    
    return LIST_ERROR_OK;
}

int DumpMakeImg (list_t *list)
{
    char imgFileName[kFileNameLen] = {};
    snprintf (imgFileName, kFileNameLen, "%lu.svg", imageCounter);

    const size_t kMaxCommandLen = 256;
    char command[kMaxCommandLen] = {};

    snprintf (command, kMaxCommandLen, "dot %s%lu.dot -T svg -o %s%s", 
              list->log.dotFolderPath, imageCounter,
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
#endif // PRINT_DEBUG