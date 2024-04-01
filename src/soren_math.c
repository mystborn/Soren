#include <soren_math.h>

#include <soren_std.h>

static SDL_TLSID random_tls_id;
static soren_thread_local Random* random;

void rng_init(void) {
    random_tls_id = SDL_CreateTLS();
}

SOREN_EXPORT Random* random_instance(void) {
    if (!random) {
        time_t t = time(NULL);
        random = random_create(((uint32_t)(t >> 32)) ^ (uint32_t)t);
        SDL_SetTLS(random_tls_id, random, random_free);
    }

    return random;
}