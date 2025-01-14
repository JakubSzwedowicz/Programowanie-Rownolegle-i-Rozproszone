//
// Created by jakubszwedowicz on 1/4/25.
//

#ifndef NELDER_MEAD_H
#define NELDER_MEAD_H

#include "common.h"

// Returns 0 if the algorithm converged, 1 otherwise
int nelderMeadOpenMP(const Function1Arg func, const Function1ArgFillInitialVec fillInitialVec, const int size,
                         const double distance, const double alpha, const double beta, const double epsilon,
                         double *bestPoint, int *iterations, const int openMPThreads
);


int findMinValuePointIndex(const Function1Arg func, double **simplex, const int simplexSize, const int size);
int findMinValuePointIndexMPI(const Function1Arg func, double **simplex, const int simplexSize, const int size, const int numberOfProcesses, const int rank);

// Returns index of the point with the smallest value
int reflectSimplex(const double **const simplex, const int simplexSize, const int size, double **reflectedSimplex,
                   const int reflectionPointIndex, Function1Arg func);

// Returns index of the point with the smallest value
int expandSimplex(const double **const simplex, const int simplexSize, const int size, const double alpha,
                  double **expandedSimplex, const int expansionPointIndex, Function1Arg func);

// Returns index of the point with the smallest value
// Strictly speaking finding the smallest value might not be necessary inside this function
// because we might meet the STOP condition before another loop
// but for the sake of readability this function returns index of minValuePoint anyway.
int contractSimplex(const double **const simplex, const int simplexSize, const int size, const double beta,
                    double **contractedSimplex, const int contractionPointIndex, const Function1Arg func);

#endif //NELDER_MEAD_H
