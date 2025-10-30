#ifndef K_LIST_LOG_H
#define K_LIST_LOG_H

#include "list.h"

const char kParentDumpFolderName[] = "dump/";
const char kImgFolderName[]        = "img/";
const char kLogFileName[]          = "log.html";
const char kGraphFileName[]        = "dot.txt";

const size_t kLogFolderPathLen = 32;
const size_t kFileNameLen      = 64;
// Now unused:
// const size_t kTimeDateLen           = sizeof ("2024-01-01_19:19:19");

// const size_t kDumpFolderNameLen     = sizeof ("2024-01-01_19:19:19/");
// const size_t kImgFolderNameLen      = kDumpFolderNameLen + sizeof (kImgFolderName);
// const size_t kImgNameLen            = sizeof ("2024-01-01_19:19:19.svg"); // FIMXE: check extension

// const size_t kLogFolderPathLen      = sizeof (kParentDumpFolderName) + kDumpFolderNameLen;
// const size_t kLogFilePathLen        = kLogFolderPathLen + sizeof (kLogFileName);
// const size_t kGraphFilePathLen      = kLogFolderPathLen + sizeof (kGraphFileName);
// const size_t kImgFolderPathLen      = kLogFolderPathLen + sizeof (kImgFolderName) + kImgNameLen;

struct listLog_t
{
    char logFolderPath [kLogFolderPathLen] = {}; // dump/[date-time]
    char imgFolderPath [kFileNameLen]      = {}; // dump/[date-time]/img
    char logFilePath   [kFileNameLen]      = {}; // dump/[date-time]/log.html
    char graphFilePath [kFileNameLen]      = {}; // dump/[date-time]/dot.txt

    FILE *logFile  = NULL;
    FILE *graphFile = NULL; // TODO: make more than one dot.txt file
    // dot1.txt dot2.txt ...
};

int LogInit (listLog_t *log);

#endif // K_LIST_LOG_H