//
// Created by jakubszwedowicz on 1/3/25.
//

#include <mpi.h>
#include <nelderMeadMPI.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "nelderMeadMPI.h"
#include "utils.h"
#include "logger.h"

#define ALPHA 1.5
#define BETA  0.5
#define S     1.0


int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // sleep(5);

    int size = 0;
    int function = 0;
    double epsilon = 1e-3;
    char logfile[1024] = "results.log";
    char command[1024] = "";
    int numberOfThreads = 1;

    if (parseArguments(argc, argv, &function, &size, &epsilon, &numberOfThreads, logfile, sizeof(logfile), command,
                       sizeof(command)) != 0) {
        MPI_Finalize();
        return 1;
    }
    if (rank == 0 && openLogFile(logfile) != 0) {
        fprintf(stderr, "WARNING: Could not open results.log. Continuing without a log.\n");
    }

    double *bestPoint = malloc(size * sizeof(double));
    const Function1Arg func = getFunction(function);
    const Function1ArgFillInitialVec fillInitialVec = getFunctionFillInitialVec(function);
    int iters = 0;

    double elapsed = 0; {
        struct timeval start, end;

        gettimeofday(&start, NULL);
        nelderMeadOpenMPAndMPI(func, fillInitialVec, size, S, ALPHA, BETA, epsilon, bestPoint, &iters, numberOfThreads);
        gettimeofday(&end, NULL);

        elapsed = (end.tv_sec - start.tv_sec) + ((end.tv_usec - start.tv_usec) / 1000000.0);
    }
    if (rank == 0) {
        int numberOfProcesses;
        MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);
        logMessage("Distributed Parallel result (OpenMP + OpenMPI):\n");
        logMessage("\tCommand          = %s\n", command);
        logMessage("\tFunction         = %s\n", getFunctionName(function));
        logMessage("\tmin f(x)         = %f\n", quadraticFunction1(bestPoint, size));
        logMessage("\tmin x            = %s\n", print(bestPoint, size));
        logMessage("\tsize             = %d\n", size);
        logMessage("\tepsilon          = %.g\n",epsilon);
        logMessage("\topenMPThreads    = %d\n", numberOfThreads);
        logMessage("\tOpenMPIProcesses = %d\n", numberOfProcesses);
        logMessage("\titerations       = %d\n", iters);
        logMessage("\ttime             = %.6f s\n", elapsed);
    }


    free(bestPoint);
    MPI_Finalize();
    return 0;
}
