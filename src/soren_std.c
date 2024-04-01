#include <soren_std.h>
#include "soren_init.h"
#include <external/parson.h>
#include <external/mist_log.h>

E4C_DEFINE_EXCEPTION(NotImplementedException, "Function not implemented", RuntimeException);
E4C_DEFINE_EXCEPTION(SdlException, "SDL encountered an error", RuntimeException);
E4C_DEFINE_EXCEPTION(JsonException, "Error processing JSON", RuntimeException);

SOREN_EXPORT Logger* soren_logger = NULL;

static void* log_exceptions(const e4c_exception* exception) {
    log_error(soren_logger, "%s thrown in %s on line %d:\n%s", exception->name, exception->file, exception->line, exception->message);
    return NULL;
}

SOREN_EXPORT soren_init(bool use_logger) {
    if (use_logger) {
        mist_log_set_allocation_functions(soren_malloc, soren_realloc, soren_calloc, soren_free);
        soren_logger = log_logger_create();
        LogTarget* console = log_target_console_create("${time:format=%x %r} | ${level} | ${message}", LOG_TRACE, LOG_FATAL);
        log_add_target(soren_logger, console);

        e4c_context_set_handlers(NULL, NULL, log_exceptions, NULL);
    }

    json_set_allocation_functions(soren_malloc, soren_free);

    soren_resource_map_init();
    soren_enum_parser_init();
    rng_init();
}