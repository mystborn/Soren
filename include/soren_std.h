#ifndef SOREN_STD_H
#define SOREN_STD_H

#include "e4c/e4c.h"

E4C_DECLARE_EXCEPTION(NotImplementedException);

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

#define gds_malloc soren_malloc
#define gds_realloc soren_realloc
#define gds_calloc soren_calloc
#define gds_free soren_free
#define gds_assert_arg E4C_ASSERT
#define gds_assert_bounds E4C_ASSERT

#else

#include <stdlib.h>

#define soren_malloc malloc
#define soren_calloc calloc
#define soren_realloc realloc
#define soren_free free

#endif

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