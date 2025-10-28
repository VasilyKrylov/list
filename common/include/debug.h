// 26.10.2025 version of debug.h:
//     * ERROR() -> ERROR_LOG()
//     * Every macro requires ;

#ifndef K_DEBUG_H
#define K_DEBUG_H

#include <stdio.h>

#define RED_COLOR      "\33[31m"
#define GREEN_COLOR    "\33[32m"
#define YELLOW_COLOR   "\33[33m"
#define BLUE_COLOR     "\33[34m"

#define RED_BOLD_COLOR      "\33[1;31m"
#define GREEN_BOLD_COLOR    "\33[1;32m"
#define YELLOW_BOLD_COLOR   "\33[1;33m"
#define BLUE_BOLD_COLOR     "\33[1;34m"
#define COLOR_END           "\33[0m"

#define ERROR_LOG(format, ...)   fprintf (stderr, RED_BOLD_COLOR "[ERROR] %s:%d:%s(): " format "\n" COLOR_END, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define ERROR_PRINT(format, ...) fprintf (stderr, RED_BOLD_COLOR format "\n" COLOR_END, __VA_ARGS__)

#define DO_AND_CHECK(action)                \
        do                                  \
        {                                   \
            int status = action;            \
            if (status != LIST_ERROR_OK)    \
                return status;              \
        } while (0)                       
enum commonErrors
{
    COMMON_ERROR_OK                     = 0,
    COMMON_ERROR_ALLOCATING_MEMORY      = 1 << 0,
    COMMON_ERROR_REALLOCATING_MEMORY    = 1 << 1,
    COMMON_ERROR_OPENING_FILE           = 1 << 2,
    COMMON_ERROR_NULL_POINTER           = 1 << 3,
    COMMON_ERROR_SSCANF                 = 1 << 4,
    COMMON_ERROR_WRITE_TO_FILE          = 1 << 5,
    COMMON_ERROR_TO_EARLY_EOF           = 1 << 6,
    COMMON_ERROR_CREATING_FILE          = 1 << 7,
    COMMON_ERROR_RUNNING_SYSTEM_COMMAND = 1 << 8 // TODO: add text messages
};

// TODO: PrintCommonError()

#ifdef PRINT_DEBUG
    #define DEBUG_LOG(format, ...)                                      \
            do {                                                        \
                fprintf(stderr, BLUE_BOLD_COLOR                         \
                        "[DEBUG] %s:%d:%s(): " format "\n" COLOR_END,   \
                        __FILE__, __LINE__, __func__, __VA_ARGS__);     \
            } while(0)

    #define DEBUG_PRINT(format, ...)                                            \
            do {                                                                \
                fprintf(stderr, BLUE_BOLD_COLOR format COLOR_END, __VA_ARGS__); \
            } while(0)

    #define DEBUG_VAR(format, name)                              \
            do {                                                 \
                DEBUG_LOG ("%s = \"" format "\";", #name, name); \
            } while(0)
    #define ON_DEBUG(...) __VA_ARGS__

#else
    #define DEBUG_LOG(format, ...)
    #define DEBUG_PRINT(format, ...)
    #define DEBUG_VAR(format, name)
    #define ON_DEBUG(...)

#endif // PRINT_DEBUG

void PrintCommonError (int error);

#endif // K_DEBUG_H
