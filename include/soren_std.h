#ifndef SOREN_STD_H
#define SOREN_STD_H

#ifdef SOREN_BUILD
    #if defined(_WIN32)
        #define SOREN_EXPORT __declspec(dllexport)
    #elif defined(__ELF__)
        #define SOREN_EXPORT __attribute__((visibility ("default")))
    #else
        #define SOREN_EXPORT
    #endif
#else
    #if defined(_WIN32)
        #define SOREN_EXPORT __declspec(dllimport)
    #else
        #define SOREN_EXPORT
    #endif
#endif

#include "external/e4c.h"

E4C_DECLARE_EXCEPTION(NotImplementedException);
E4C_DECLARE_EXCEPTION(SdlException);
E4C_DECLARE_EXCEPTION(JsonException);

#define SOREN_SDL_ASSERT(condition) if (!(condition)) throw(SdlException, SDL_GetError())

#ifndef SOREN_MANUAL_MEMORY

#include <gc.h>

static inline void* soren_malloc(size_t size) {
    void* result = GC_malloc(size);
    if (!result) {
        throw(NotEnoughMemoryException, "Failed to allocate memory");
    }

    return result;
}

static inline void* soren_calloc(size_t n, size_t size) {
    void* result = GC_malloc(size * n);
    if (!result) {
        throw(NotEnoughMemoryException, "Failed to allocate memory");
    }

    return result;
}

static inline void* soren_realloc(void* ptr, size_t size) {
    void* result = GC_realloc(ptr, size);
    if (!result) {
        throw(NotEnoughMemoryException, "Failed to reallocate memory");
    }

    return result;
}

static inline void soren_free(void* ptr) {
    GC_free(ptr);
}

SOREN_EXPORT soren_init(bool use_logger);

#define soren_assert(condition) E4C_ASSERT(condition)

#define gds_malloc soren_malloc
#define gds_realloc soren_realloc
#define gds_calloc soren_calloc
#define gds_free soren_free
#define gds_assert_arg E4C_ASSERT
#define gds_assert_bounds E4C_ASSERT

#define sso_string_malloc soren_malloc
#define sso_string_realloc soren_realloc
#define sso_string_calloc soren_calloc
#define sso_string_free soren_free

#else

#include <stdlib.h>

#define soren_malloc malloc
#define soren_calloc calloc
#define soren_realloc realloc
#define soren_free free

#endif

#include "external/mist_log.h"

SOREN_EXPORT extern Logger* soren_logger;

#ifndef SOREN_NO_THREADS

#if defined(_MSC_VER)

#define soren_thread_local __declspec(thread)

#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_THREADS__)

#include <threads.h>

#define soren_thread_local thread_local

#else

#define soren_thread_local __thread

#endif

#else

#define soren_thread_local

#endif

#endif