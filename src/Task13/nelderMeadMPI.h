//
// Created by jakubszwedowicz on 1/6/25.
//

#ifndef NELDERMEADMPI_H
#define NELDERMEADMPI_H

#include "common.h"

int nelderMeadOpenMPAndMPI(const Function1Arg func, const Function1ArgFillInitialVec fillInitialVec, const int size,
                           const double distance, const double alpha, const double beta, const double epsilon,
                           double *bestPoint, int *iterations, const int openMPThreads);

int findMinValuePointIndexOpenMPAndMPI(const Function1Arg func, double **simplex, const int simplexSize, const int size,
                                       const int rank, const int numberOfProcesses);

int reflectSimplexOpenMPAndMPI(const double **simplex, const int simplexSize, const int size, double **reflectedSimplex,
                               const int reflectionPointIndex, const Function1Arg func, const int rank,
                               const int numberOfProcesses);

int expandSimplexOpenMPAndMPI(const double **simplex, const int simplexSize, const int size, const double alpha,
                              double **expandedSimplex, const int expansionPointIndex, const Function1Arg func,
                              const int rank, const int numberOfProcesses);

int contractSimplexOpenMPAndMPI(const double **simplex, const int simplexSize, const int size, const double beta,
                                double **contractedSimplex, const int contractionPointIndex, const Function1Arg func,
                                const int rank, const int numberOfProcesses);

int getPointsPerProcess(const int simplexSize, const int rank, const int numberOfProcesses);

int getStartingIdxOfProcess(const int simplexSize, const int rank, const int numberOfProcesses);

void synchronizeSimplex(double **simplex, const int simplexSize, const int size);

#endif //NELDERMEADMPI_H
