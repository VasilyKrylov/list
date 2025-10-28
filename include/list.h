#ifndef K_LIST_H
#define K_LIST_H

#include <stdio.h>

#include "debug.h"

typedef double listDataType;

static const int kListStart = 0;
static const listDataType kListPoison = -666;
static const size_t kListMaxLen = (1UL << 32);

static const char kParentDumpFolderName[] = "dump/";
static const char kImgFolderName[]        = "img/";
static const char kLogFileName[]          = "log.html";
static const char kGraphFileName[]        = "dot.txt";


static const size_t kTimeDateLen           = sizeof ("2024-01-01_19:19:19");

static const size_t kDumpFolderNameLen     = sizeof ("2024-01-01_19:19:19/");
static const size_t kImgFolderNameLen      = kDumpFolderNameLen + sizeof (kImgFolderName);
static const size_t kImgNameLen            = sizeof ("2024-01-01_19:19:19.svg"); // FIMXE: check extension

static const size_t kLogFolderPathLen      = sizeof (kParentDumpFolderName) + kDumpFolderNameLen;
static const size_t kLogFilePathLen        = kLogFolderPathLen + sizeof (kLogFileName);
static const size_t kGraphFilePathLen      = kLogFolderPathLen + sizeof (kGraphFileName);
static const size_t kImgFolderPathLen      = kLogFolderPathLen + sizeof (kImgFolderName) + kImgNameLen;

#ifdef PRINT_DEBUG
struct varInfo_t
{
    const char *name = NULL;
    const char *file = NULL;
    int line         = 0;
    const char *func = NULL;
};
struct listLog_t
{
    char logFolderPath[kLogFolderPathLen] = {};
    char imgFolderPath[kImgFolderPathLen] = {};
    char logFilePath[kLogFilePathLen]  = {};
    char graphFilePath[kGraphFilePathLen] = {};

    FILE *logFile  = NULL;
    FILE *graphFile = NULL;
};

#define LIST_CTOR(listName, size) ListCtor (&listName, size,                        \
                                            varInfo_t{.name = #listName,            \
                                                      .file = __FILE__,             \
                                                      .line = __LINE__,             \
                                                      .func = __func__});

#else
    #define LIST_CTOR(listName, size) ListCtor (&listName, size);
#endif // PRING_DEBUG

struct listElement_t
{
    listDataType data = kListPoison;

    int next = 0;
    int prev = 0;
};

struct list_t
{
    listElement_t *elements = NULL;

    size_t free = 1;
    size_t head = 0;
    size_t len = 0;

#ifdef PRINT_DEBUG
    varInfo_t varInfo = {};

    listLog_t log = {};
#endif
};

enum listError_t
{
    LIST_ERROR_OK              = 0,
    LIST_ERROR_NULL_STRUCT     = 1 << 0,
    LIST_ERROR_NULL_DATA       = 1 << 1,
    LIST_ERROR_BIG_LEN         = 1 << 2,
    LIST_ERROR_INSERT_AFTER    = 1 << 3,

    LIST_ERROR_COMMON          = 1 << 31
};

#define LIST_DUMP(listName, comment) ListDump (&listName, comment, __FILE__, __LINE__, __func__)

int ListCtor    (list_t *list, size_t len
                 ON_DEBUG (,varInfo_t varInfo));
int ListInsert  (list_t *list, size_t idx, listDataType val, size_t *insertedIdx);
int ListDelete  (list_t *list, size_t idx);
int ListDump    (list_t *list, const char *comment,
                 const char *_FILE, int _LINE, const char * _FUNC);
void ListDtor   (list_t *list);

#endif //K_LIST_H
