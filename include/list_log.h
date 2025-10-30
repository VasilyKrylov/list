#ifndef K_LIST_LOG_H
#define K_LIST_LOG_H

#include "list.h"

#ifdef PRINT_DEBUG
int LogInit             (listLog_t *log);
int ListDump            (list_t *list, const char *comment,
                         const char *_FILE, int _LINE, const char * _FUNC);
#endif // PRINT_DEBUG

#endif // K_LIST_LOG_H