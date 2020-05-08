/*
 * simplestopwatch
 * Copyright © 2020 Victor Ocampo
 * All rights reserved
 */

#include "sdl-life.h"

// This function just rounds up a floating point number to a interger
int roundUp(float n) {
        if((n - (int)n) == 0)
                return (int)n;
        return (int)n + 1;
}

void usage(void) {
        fprintf(stderr, "usage: sdl-life [-fps fpsCap] [-cellcolor color] [-fillcell] [-grid] [-f/-file file] [-r/-rules/-rulestring rulestring]\n");
        exit(1);
}

void die(const char *f, const size_t l, const char *fmt, ...) {
        va_list vargs;

        va_start(vargs, fmt);

        SDL_LogMessageV(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_CRITICAL, fmt, vargs);

        va_end(vargs);

        closeSdl();

        exit(EXIT_FAILURE);
}
