//
// Created by jakubszwedowicz on 1/3/25.
//

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>

#include "nelderMead.h"
#include "utils.h"
#include "logger.h"

#define ALPHA 1.5
#define BETA  0.5
#define S     1.0


int main(int argc, char **argv) {
    int size = 0;
    int function = 0;
    double epsilon = 1e-3;
    char logfile[1024] = "results.log";
    char command[1024] = "";
    int numberOfThreads = 1;

    if (parseArguments(argc, argv, &function, &size, &epsilon, &numberOfThreads, logfile, sizeof(logfile), command, sizeof(command)) != 0) {
        return 1;
    }
    if (openLogFile(logfile) != 0) {
        fprintf(stderr, "WARNING: Could not open results.log. Continuing without a log.\n");
    }

    double* bestPoint = malloc(size * sizeof(double));
    const Function1Arg func = getFunction(function);
    const Function1ArgFillInitialVec fillInitialVec = getFunctionFillInitialVec(function);
    int iters = 0;

    double elapsed = 0;
    {
        struct timeval start, end;

        gettimeofday(&start, NULL);
        nelderMeadOpenMP(func, fillInitialVec, size, S, ALPHA, BETA, epsilon, bestPoint, &iters, numberOfThreads);
        gettimeofday(&end, NULL);

        elapsed = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);
    }
    logMessage("Parallel result (OpenMP):\n");
    logMessage("\tCommand          = %s\n", command);
    logMessage("\tFunction         = %s\n", getFunctionName(function));
    logMessage("\tmin f(x)         = %f\n", quadraticFunction1(bestPoint, size));
    logMessage("\tmin x            = %s\n", print(bestPoint, size));
    logMessage("\tsize             = %d\n", size);
    logMessage("\tepsilon          = %.g\n", epsilon);
    logMessage("\topenMPThreads    = %d\n", numberOfThreads);
    logMessage("\tOpenMPIProcesses = %d\n", 1);
    logMessage("\titerations       = %d\n", iters);
    logMessage("\ttime             = %.6f s\n", elapsed);


    free(bestPoint);
    return 0;
}
