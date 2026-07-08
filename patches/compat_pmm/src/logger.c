#include "global.h"
#include "logger.h"
#include "modding.h"
#include "recompconfig.h"

LoggerLevel gLoggerLevel = LOGGER_LEVEL_WARNING;

void Logger_setLoggerLevel(LoggerLevel level) {
    if (level < 0 || level > LOGGER_LEVEL_NONE) {
        Logger_printWarning("Called Logger_setLoggerLevel with invalid level %d", level);
        return;
    }

    gLoggerLevel = level;
}

RECOMP_CALLBACK(".", _internal_preInitHashObjects) void setupLoggerFirstTime(void) {
    Logger_setLoggerLevel(recomp_get_config_u32("logging_level"));
}

void updateLoggerLevel_on_Play_Main(void) {
    LoggerLevel level = recomp_get_config_u32("logging_level");

    if (level != gLoggerLevel) {
        Logger_setLoggerLevel(level);
    }
}
