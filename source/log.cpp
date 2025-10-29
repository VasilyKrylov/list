#include <stdio.h>
#include <time.h>

#include "log.h"

int DumpInit ()
{
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

// TODO no macro
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

// FIXME w+
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
}