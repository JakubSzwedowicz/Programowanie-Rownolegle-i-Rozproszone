//
// Created by jakubszwedowicz on 1/6/25.
//

#include "nelderMeadMPI.h"
#include "macros.h"
#include "common.h"

#include <float.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils.h"

const char *printt(const double *vec, const int size) {
    static char buffer[1024];
    char *ptr = buffer;
    long unsigned offset = 0;

    for (int i = 0; i < size; i++) {
        offset += snprintf(ptr + offset, sizeof(buffer) - offset, "%.2f ", vec[i]);
        if (offset >= sizeof(buffer)) {
            break;
        }
    }

    return buffer;
}

struct MPIReductionData {
    double value;
    int index;
};

int nelderMeadOpenMPAndMPI(const Function1Arg func, const Function1ArgFillInitialVec fillInitialVec, const int size,
                           const double distance, const double alpha, const double beta, const double epsilon,
                           double *bestPoint, int *iterations, const int openMPThreads) {
    int rank, numberOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

    omp_set_num_threads(openMPThreads);
    omp_set_dynamic(0);

    // Step 1: Creating initial simplex
    const int simplexSize = size + 1;
    double **simplex = NULL;
    double **reflectedSimplex = NULL;
    double **expandedSimplex = NULL;
    double **contractedSimplex = NULL;

    if (allocateSimplex(&simplex, simplexSize, size) == -1 ||
        allocateSimplex(&reflectedSimplex, simplexSize, size) == -1 ||
        allocateSimplex(&expandedSimplex, simplexSize, size) == -1 ||
        allocateSimplex(&contractedSimplex, simplexSize, size) == -1) {
        if (rank == 0) {
            fprintf(stderr, "Failed to allocate memory for simplex\n");
        }
        deallocateSimplex(&simplex, simplexSize);
        deallocateSimplex(&reflectedSimplex, simplexSize);
        deallocateSimplex(&expandedSimplex, simplexSize);
        deallocateSimplex(&contractedSimplex, simplexSize);
        MPI_Finalize();
        return -1;
    }

    if (rank == 0) {
        initializeInitialSimplex(simplex, simplexSize, size, fillInitialVec, distance);
    }
    for (int i = 0; i < simplexSize; i++) {
        MPI_Bcast(simplex[i], size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    // Step 2: Calculate values of function in each point of simplex & find the best one. Split points between processes
    int minValuePointIndex = findMinValuePointIndexOpenMPAndMPI(func, simplex, simplexSize, size, rank,
                                                                numberOfProcesses);

    int iters = 0;
    for (int running = 1; running; iters++) {
        // Step 3: Reflection
        const int reflectedSimplexMinValuePointIndex = reflectSimplexOpenMPAndMPI(
            (const double **const) simplex, simplexSize, size,
            reflectedSimplex,
            minValuePointIndex,
            func, rank, numberOfProcesses);

        if (reflectedSimplexMinValuePointIndex != minValuePointIndex) {
            // Step 4: Expansion
            const int expandedSimplexMinValuePointIndex = expandSimplexOpenMPAndMPI(
                (const double **) reflectedSimplex,
                simplexSize, size, alpha,
                expandedSimplex,
                reflectedSimplexMinValuePointIndex,
                func, rank, numberOfProcesses);


            if (expandedSimplexMinValuePointIndex != reflectedSimplexMinValuePointIndex) {
                swapSimplex(&simplex, &expandedSimplex);
                minValuePointIndex = expandedSimplexMinValuePointIndex;
            } else {
                swapSimplex(&simplex, &reflectedSimplex);
                minValuePointIndex = reflectedSimplexMinValuePointIndex;
            }
        } else {
            // Step 5: Contraction
            // Based on the formula in the algorithm description we contract the simplex, not reflectedSimplex
            const int contractedSimplexMinValuePointIndex = contractSimplexOpenMPAndMPI(
                (const double **) simplex, simplexSize, size, beta,
                contractedSimplex,
                minValuePointIndex,
                func, rank, numberOfProcesses);
            swapSimplex(&simplex, &contractedSimplex);
            minValuePointIndex = contractedSimplexMinValuePointIndex;

            // Based on the algorithm description ONLY HERE we check the STOP condition.
            double dist = maxDistanceInSimplex(simplex, simplexSize, size);
            if (dist < epsilon) {
                running = 0;
            }
        }
    }

    if (rank == 0) {
        // Return result:
        memcpy(bestPoint, simplex[minValuePointIndex], size * sizeof(double));
    }
    *iterations = iters;

    deallocateSimplex(&simplex, simplexSize);
    deallocateSimplex(&reflectedSimplex, simplexSize);
    deallocateSimplex(&expandedSimplex, simplexSize);
    deallocateSimplex(&contractedSimplex, simplexSize);
    return 0;
}

int findMinValuePointIndexOpenMPAndMPI(const Function1Arg func, double **simplex, const int simplexSize,
                                       const int size,
                                       const int rank,
                                       const int numberOfProcesses) {
    double minValue = DBL_MAX;
    int minValuePointIndex = -1;
    const int startIdx = getStartingIdxOfProcess(simplexSize, rank, numberOfProcesses);
    const int endIdx = startIdx + getPointsPerProcess(simplexSize, rank, numberOfProcesses);

#pragma omp parallel
    {
        double localMinValue = DBL_MAX;
        int localMinIndex = -1;

#pragma omp for
        for (int i = startIdx; i < endIdx; i++) {
            const double value = func(simplex[i], size);
            if (value < localMinValue) {
                localMinValue = value;
                localMinIndex = i;
            }
        }

#pragma omp critical
        {
            if (localMinValue < minValue) {
                minValue = localMinValue;
                minValuePointIndex = localMinIndex;
            }
        }
    }


    struct MPIReductionData localData, globalData;
    localData.value = minValue;
    localData.index = minValuePointIndex;

    MPI_Allreduce(&localData, &globalData, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

    return globalData.index;
}

int reflectSimplexOpenMPAndMPI(const double **simplex, const int simplexSize, const int size,
                               double **reflectedSimplex, const int reflectionPointIndex,
                               const Function1Arg func, const int rank, const int numberOfProcesses) {
    const double *const reflectionPoint = simplex[reflectionPointIndex];
    memmove(reflectedSimplex[reflectionPointIndex], reflectionPoint, size * sizeof(double));

    int minValuePointIndex = reflectionPointIndex;
    double minValue = func(reflectionPoint, size);

    const int startIdx = getStartingIdxOfProcess(simplexSize, rank, numberOfProcesses);
    const int endIdx = startIdx + getPointsPerProcess(simplexSize, rank, numberOfProcesses);
#pragma omp parallel
    {
        double localMinValue = DBL_MAX;
        int localMinIdx = -1;

#pragma omp for
        for (int i = startIdx; i < endIdx; i++) {
            if (LIKELY(i != reflectionPointIndex)) {
                for (int j = 0; j < size; j++) {
                    reflectedSimplex[i][j] = 2.0 * reflectionPoint[j] - simplex[i][j];
                }
                const double value = func(reflectedSimplex[i], size);
                if (value < localMinValue) {
                    localMinValue = value;
                    localMinIdx = i;
                }
            }
        }

#pragma omp critical
        {
            if (localMinValue < minValue) {
                minValue = localMinValue;
                minValuePointIndex = localMinIdx;
            }
        }
    }

    struct MPIReductionData localData, globalData;
    localData.value = minValue;
    localData.index = minValuePointIndex;

    MPI_Allreduce(&localData, &globalData, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);
    synchronizeSimplex(reflectedSimplex, simplexSize, size);

    return globalData.index;
}

int expandSimplexOpenMPAndMPI(const double **simplex, const int simplexSize, const int size, const double alpha,
                              double **expandedSimplex, const int expansionPointIndex,
                              const Function1Arg func, const int rank, const int numberOfProcesses) {
    const double *const expansionPoint = simplex[expansionPointIndex];
    // It's possible here that memory overlaps thus we use memove and not memcpy
    memmove(expandedSimplex[expansionPointIndex], expansionPoint, size * sizeof(double));

    double minValue = func(expansionPoint, size);
    int minValuePointIndex = expansionPointIndex;

    const int startIdx = getStartingIdxOfProcess(simplexSize, rank, numberOfProcesses);
    const int endIdx = startIdx + getPointsPerProcess(simplexSize, rank, numberOfProcesses);
#pragma omp parallel
    {
        double localMinValue = DBL_MAX;
        int localMinIdx = -1;

#pragma omp for
        for (int i = startIdx; i < endIdx; i++) {
            if (LIKELY(i != expansionPointIndex)) {
                for (int j = 0; j < size; j++) {
                    expandedSimplex[i][j] =
                            alpha * simplex[i][j] + (1.0 - alpha) * expansionPoint[j];
                }

                const double value = func(expandedSimplex[i], size);
                if (value < localMinValue) {
                    localMinValue = value;
                    localMinIdx = i;
                }
            }
        }
#pragma omp critical
        {
            if (localMinValue < minValue) {
                minValue = localMinValue;
                minValuePointIndex = localMinIdx;
            }
        }
    }

    struct MPIReductionData localData, globalData;
    localData.value = minValue;
    localData.index = minValuePointIndex;

    MPI_Allreduce(&localData, &globalData, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);
    synchronizeSimplex(expandedSimplex, simplexSize, size);

    return globalData.index;
}

int contractSimplexOpenMPAndMPI(const double **simplex, const int simplexSize, const int size, const double beta,
                                double **contractedSimplex, const int contractionPointIndex,
                                const Function1Arg func, const int rank, const int numberOfProcesses) {
    const double *const contractionPoint = simplex[contractionPointIndex];
    // It's possible here that memory overlaps thus we use memove and not memcpy
    memmove(contractedSimplex[contractionPointIndex], contractionPoint, size * sizeof(double));

    double minValue = func(contractionPoint, size);
    int minValuePointIndex = contractionPointIndex;

    const int startIdx = getStartingIdxOfProcess(simplexSize, rank, numberOfProcesses);
    const int endIdx = startIdx + getPointsPerProcess(simplexSize, rank, numberOfProcesses);
#pragma omp parallel
    {
        double localMinValue = minValue;
        int localMinIdx = contractionPointIndex;

#pragma omp for
        for (int i = startIdx; i < endIdx; i++) {
            if (LIKELY(i != contractionPointIndex)) {
                for (int j = 0; j < size; j++) {
                    contractedSimplex[i][j] =
                            beta * simplex[i][j] + (1.0 - beta) * contractionPoint[j];
                }
                const double value = func(contractedSimplex[i], size);
                if (value < localMinValue) {
                    localMinValue = value;
                    localMinIdx = i;
                }
            }
        }

#pragma omp critical
        {
            if (localMinValue < minValue) {
                minValue = localMinValue;
                minValuePointIndex = localMinIdx;
            }
        }
    }

    struct MPIReductionData localData, globalData;
    localData.value = minValue;
    localData.index = minValuePointIndex;

    MPI_Allreduce(&localData, &globalData, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);
    synchronizeSimplex(contractedSimplex, simplexSize, size);

    return globalData.index;
}


int getPointsPerProcess(const int simplexSize, const int rank, const int numberOfProcesses) {
    int pointsPerProc = simplexSize / numberOfProcesses;
    int remainder = simplexSize % numberOfProcesses;
    return rank < remainder ? pointsPerProc + 1 : pointsPerProc;
}

int getStartingIdxOfProcess(const int simplexSize, const int rank, const int numberOfProcesses) {
    int pointsPerProc = simplexSize / numberOfProcesses;
    int remainder = simplexSize % numberOfProcesses;
    return rank * pointsPerProc + (rank < remainder ? rank : remainder);
}

void synchronizeSimplex(double **simplex, const int simplexSize, const int size) {
    int rank;
    int numberOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

    for (int root = 0; root < numberOfProcesses; root++) {
        const int startIdx = getStartingIdxOfProcess(simplexSize, root, numberOfProcesses);
        const int endIdx = startIdx + getPointsPerProcess(simplexSize, root, numberOfProcesses);
        // printf("synchronizeSimplex Rank %d: currently processing root %d, startIdx: %d, endIdx: %d\n", rank, root, startIdx, endIdx);
        for (int i = startIdx; i < endIdx; i++) {
            MPI_Bcast(simplex[i], size, MPI_DOUBLE, root, MPI_COMM_WORLD);
        }
    }
}
