#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "recomputils.h"

typedef enum {
    LOGGER_LEVEL_ALL,
    LOGGER_LEVEL_VERBOSE,
    LOGGER_LEVEL_INFO,
    LOGGER_LEVEL_WARNING,
    LOGGER_LEVEL_ERROR,
    LOGGER_LEVEL_NONE,
} LoggerLevel;

extern LoggerLevel gLoggerLevel;

#ifndef LOGGER_PROJECT_NAME
#define LOGGER_PROJECT_NAME ""
#endif

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

#define Logger_printCurrentFuncAndLine(prefix) recomp_printf("%s %s:%s:%d:%s: ", prefix, LOGGER_PROJECT_NAME, __FILE_NAME__, __LINE__, __func__)

#define Logger_printLine(...)       \
    {                               \
        recomp_printf(__VA_ARGS__); \
        recomp_printf("\n");        \
    }                               \
    (void)0

#define Logger_printVerbose(...)                         \
    {                                                    \
        if (gLoggerLevel <= LOGGER_LEVEL_VERBOSE) {      \
            Logger_printCurrentFuncAndLine("[VERBOSE]"); \
            Logger_printLine(__VA_ARGS__);               \
        }                                                \
    }                                                    \
    (void)0

#define Logger_printInfo(...)                         \
    {                                                 \
        if (gLoggerLevel <= LOGGER_LEVEL_INFO) {      \
            Logger_printCurrentFuncAndLine("[INFO]"); \
            Logger_printLine(__VA_ARGS__);            \
        }                                             \
    }                                                 \
    (void)0

#define Logger_printWarning(...)                         \
    {                                                    \
        if (gLoggerLevel <= LOGGER_LEVEL_WARNING) {      \
            Logger_printCurrentFuncAndLine("[WARNING]"); \
            Logger_printLine(__VA_ARGS__);               \
        }                                                \
    }                                                    \
    (void)0

#define Logger_printError(...)                         \
    {                                                  \
        if (gLoggerLevel <= LOGGER_LEVEL_ERROR) {      \
            Logger_printCurrentFuncAndLine("[ERROR]"); \
            Logger_printLine(__VA_ARGS__);             \
        }                                              \
    }                                                  \
    (void)0

void Logger_setLoggerLevel(LoggerLevel level);

#endif
