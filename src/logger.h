//
// Created by jakubszwedowicz on 1/5/25.
//

#ifndef LOGGER_H
#define LOGGER_H

int openLogFile(const char *filename);

void closeLogFile(void);

void logMessage(const char *fmt, ...);

#endif /* LOGGER_H */

