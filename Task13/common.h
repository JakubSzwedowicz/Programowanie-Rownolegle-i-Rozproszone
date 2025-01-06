//
// Created by jakubszwedowicz on 1/6/25.
//

#ifndef COMMON_H
#define COMMON_H

typedef double (*Function1Arg)(const double *vec, const int size);

typedef int (*Function1ArgFillInitialVec)(double *vec, const int size);

double maxDistanceInSimplex(double **simplex, const int simplexSize, const int size);

double calculateEuclideanDistance(const double *vec1, const double *vec2, const int size);

void initializeInitialSimplex(double **simplex, const int simplexSize, const int size,
                              const Function1ArgFillInitialVec fillInitialVec, const double distance);

void fillInitialSimplex(double **simplex, const double *x0, const int simplexSize, const int size,
                        const double distance);

int allocateSimplex(double ***simplex, const int simplexSize, const int size);

int deallocateSimplex(double ***simplex, const int simplexSize);

void swapSimplex(double ***simplex1, double ***simplex2);

#endif //COMMON_H
