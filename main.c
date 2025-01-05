//
// Created by jakubszwedowicz on 1/3/25.
//

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

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

    if (parseArguments(argc, argv, &function, &size, &epsilon, logfile, sizeof(logfile)) != 0) {
        return 1;
    }
    if (openLogFile(logfile) != 0) {
        fprintf(stderr, "WARNING: Could not open results.log. Continuing without a log.\n");
    }


    double* bestPoint = malloc(size * sizeof(double));
    const Function1Arg func = getFunction(function);
    const Function1ArgFillInitialVec fillInitialVec = getFunctionFillInitialVec(function);
    int iters = 0;

    clock_t start = clock();
    nelderMeadSequential(func, fillInitialVec, size, S, ALPHA, BETA, epsilon, bestPoint,
                         &iters);
    clock_t end = clock();

    double time_spent = (double) (end - start) / CLOCKS_PER_SEC;
    logMessage("Sequential result:\n");
    logMessage("\tmin f(x)   = %f\n", quadraticFunction1(bestPoint, size));
    logMessage("\tmin x      = %s\n", print(bestPoint, size));
    logMessage("\tsize       = %d\n", size);
    logMessage("\tepsilon    = %.g\n", epsilon);
    logMessage("\titerations = %d\n", iters);
    logMessage("\ttime       = %.3f s\n", time_spent);

    free(bestPoint);
    return 0;
}
