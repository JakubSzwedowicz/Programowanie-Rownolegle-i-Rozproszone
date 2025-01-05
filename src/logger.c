//
// Created by jakubszwedowicz on 1/5/25.
//

#include "logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "logger.h"

// We keep a single global FILE* for simplicity.
static FILE *gLogFile = NULL;

int openLogFile(const char *filename)
{
    gLogFile = fopen(filename, "w");
    if (!gLogFile) {
        fprintf(stderr, "ERROR: Could not open log file: %s\n", filename);
        return -1;
    }
    return 0;
}

void closeLogFile(void)
{
    if (gLogFile) {
        fclose(gLogFile);
        gLogFile = NULL;
    }
}

void logMessage(const char *fmt, ...) {
    if (!gLogFile) {
        /* If no file is open, fallback to stderr. */
        va_list fallbackArgs;
        va_start(fallbackArgs, fmt);
        vfprintf(stderr, fmt, fallbackArgs);
        va_end(fallbackArgs);
        return;
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(gLogFile, fmt, args);
    va_end(args);
}