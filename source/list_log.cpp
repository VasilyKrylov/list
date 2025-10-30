#include <stdio.h>
#include <time.h>

#include "list.h"
#include "file_utils.h"

#include "list_log.h"

#ifdef PRINT_DEBUG
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

    snprintf (log->graphFilePath, kFileNameLen, "%s%s",
              log->logFolderPath, kGraphFileName);

    snprintf (log->imgFolderPath, kFileNameLen, "%s%s",
              log->logFolderPath, kImgFolderName);


    if (SafeMkdir (kParentDumpFolderName) != LIST_ERROR_OK)
        return LIST_ERROR_COMMON | 
               COMMON_ERROR_CREATING_FILE;

    if (SafeMkdir (log->logFolderPath) != LIST_ERROR_OK)
        return LIST_ERROR_COMMON | 
            COMMON_ERROR_CREATING_FILE;

    if (SafeMkdir (log->imgFolderPath) != LIST_ERROR_OK)
        return LIST_ERROR_COMMON | 
            COMMON_ERROR_CREATING_FILE;

    log->logFile = fopen (log->logFilePath, "w");
    if (log->logFile == NULL)
    {
        ERROR_LOG ("Error opening file \"%s\"", log->logFilePath);
        
        return LIST_ERROR_COMMON |
               COMMON_ERROR_OPENING_FILE;
    }

    log->graphFile = fopen (log->graphFilePath, "w");
    if (log->graphFile == NULL)
    {
        ERROR_LOG ("Error opening file \"%s\"", log->graphFilePath);
        
        return LIST_ERROR_COMMON |
               COMMON_ERROR_OPENING_FILE;
    }

    fprintf (log->logFile, "%s", "<pre>\n");

    return LIST_ERROR_OK;
}
#endif // PRINT_DEBUG