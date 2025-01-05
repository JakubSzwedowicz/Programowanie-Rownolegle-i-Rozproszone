//
// Created by jakubszwedowicz on 1/4/25.
//

#ifndef NELDER_MEAD_H
#define NELDER_MEAD_H

typedef double (*Function1Arg)(const double *vec, const int size);

typedef int (*Function1ArgFillInitialVec)(double *vec, const int size);

// Returns 0 if the algorithm converged, 1 otherwise
int nelderMeadSequential(const Function1Arg func, const Function1ArgFillInitialVec fillInitialVec, const int size,
                         const double distance, const double alpha, const double beta, const double epsilon,
                         double *bestPoint, int *iterations
);

double maxDistanceInSimplex(double **simplex, const int simplexSize, const int size);

double calculateEuclideanDistance(const double *vec1, const double *vec2, const int size);

void initializeInitialSimplex(double **simplex, const int simplexSize, const int size,
                              const Function1ArgFillInitialVec fillInitialVec, const double distance);

void fillInitialSimplex(double **simplex, const double *x0, const int simplexSize, const int size, const double distance);

int allocateSimplex(double ***simplex, const int simplexSize, const int size);

int deallocateSimplex(double ***simplex, const int simplexSize);

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

void swapSimplex(double ***simplex1, double ***simplex2);
#endif //NELDER_MEAD_H
