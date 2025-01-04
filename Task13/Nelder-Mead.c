//
// Created by jakubszwedowicz on 1/4/25.
//

#include "Nelder-Mead.h"
#include "macros.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initializeInitialSimplex(double **simplex, const int simplexSize, const int size,
                              const Function1ArgFillInitialVec fillInitialVec, const double distance) {
    // Create initial vector
    double x0[size]; // Interestingly, since C99 this type of declaration is allowed
    fillInitialVec(x0, size);
    fillInitialSimplex(simplex, x0, size, distance);
}

void fillInitialSimplex(double **simplex, const double *x0, const int size, const double distance) {
    memcpy(simplex[0], x0, size * sizeof(double));
    for (int i = 1; i < size + 1; i++) {
        memcpy(simplex[i], simplex[i - 1], size * sizeof(double));
        simplex[i][i - 1] += distance;
    }
}

int nelderMeadSequential(const Function1Arg func, const Function1ArgFillInitialVec fillInitialVec, const int size,
                         const double distance, const double alpha, const double beta, const double epsilon,
                         double *bestPoint, int *iterations
) {
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
        fprintf(stderr, "Failed to allocate memory for simplex\n");
        deallocateSimplex(&simplex, simplexSize);
        deallocateSimplex(&reflectedSimplex, simplexSize);
        deallocateSimplex(&expandedSimplex, simplexSize);
        deallocateSimplex(&contractedSimplex, simplexSize);
        return -1;
    }

    initializeInitialSimplex(simplex, simplexSize, size, fillInitialVec, distance);

    int minValuePointIndex = -1; {
        double minValue = DBL_MAX;
        // Step 2: Calculate values of function in each point of simplex & find the best one
        for (int i = 0; i < simplexSize; i++) {
            double value = func(simplex[i], size);
            if (value < minValue) {
                minValue = value;
                minValuePointIndex = i;
            }
        }
    }

    int iters = 1;
    for (int running = 1; running; iters++) {
        // Step 3: Reflection
        const int reflectedSimplexMinValuePointIndex = reflectSimplex(simplex, simplexSize, size, reflectedSimplex,
                                                                      minValuePointIndex,
                                                                      func);


        if (reflectedSimplexMinValuePointIndex != minValuePointIndex) {
            // Step 4: Expansion
            const int expandedSimplexMinValuePointIndex = expandSimplex(reflectedSimplex, simplexSize, size, alpha,
                                                                        expandedSimplex,
                                                                        reflectedSimplexMinValuePointIndex,
                                                                        func);

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
            const int contractedSimplexMinValuePointIndex = contractSimplex(simplex, simplexSize, size, beta,
                                                                            contractedSimplex,
                                                                            minValuePointIndex,
                                                                            func);
            swapSimplex(&simplex, &contractedSimplex);
            minValuePointIndex = contractedSimplexMinValuePointIndex;

            // Based on the algorithm description ONLY HERE we check the STOP condition.
            double dist = maxDistanceInSimplex(simplex, simplexSize, size);
            if (dist < epsilon) {
                running = 0;
            }
        }
    }

    // Return result:
    memcpy(bestPoint, simplex[minValuePointIndex], size * sizeof(double));
    *iterations = iters;

    deallocateSimplex(&simplex, simplexSize);
    deallocateSimplex(&reflectedSimplex, simplexSize);
    deallocateSimplex(&expandedSimplex, simplexSize);
    deallocateSimplex(&contractedSimplex, simplexSize);
    return 0;
}

int allocateSimplex(double ***simplex, const int simplexSize, const int size) {
    (*simplex) = (double **) malloc(simplexSize * sizeof(double *));
    if ((*simplex) == NULL) {
        return -1;
    }
    for (int i = 0; i < simplexSize; i++) {
        (*simplex)[i] = (double *) malloc(size * sizeof(double));
        if ((*simplex)[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free((*simplex)[j]);
            }
            free((*simplex));
            (*simplex) = NULL;
            return -1;
        }
    }
    return 0;
}

int deallocateSimplex(double ***simplex, const int simplexSize) {
    for (int i = 0; i < simplexSize; i++) {
        free((*simplex)[i]);
    }
    free((*simplex));
    (*simplex) = NULL; // I like my pointers NULL
    return 0;
}

double maxDistanceInSimplex(double **simplex, const int simplexSize, const int size) {
    double max_dist = 0.0;
    for (int i = 0; i < simplexSize; i++) {
        for (int j = i + 1; j < simplexSize; j++) {
            double dist = calculateEuclideanDistance(simplex[i], simplex[j], size);
            if (dist > max_dist)
                max_dist = dist;
        }
    }
    return max_dist;
}

double calculateEuclideanDistance(const double *vec1, const double *vec2, const int size) {
    double dist = 0.0;
    for (int i = 0; i < size; i++) {
        double diff = vec1[i] - vec2[i];
        dist += diff * diff;
    }

    return sqrt(dist);
}

int reflectSimplex(const double **const simplex, const int simplexSize, const int size, double **reflectedSimplex,
                   const int reflectionPointIndex, const Function1Arg func) {
    const double *const reflectionPoint = simplex[reflectionPointIndex];
    // It's possible here that memory overlaps thus we use memove and not memcpy
    memmove(reflectedSimplex[reflectionPointIndex], reflectionPoint, size * sizeof(double));

    int minValuePointIndex = reflectionPointIndex;
    double minValue = func(reflectionPoint, size);
    for (int i = 0; i < simplexSize; i++) {
        if (LIKELY(i != reflectionPointIndex)) {
            for (int j = 0; j < size; j++) {
                reflectedSimplex[i][j] = 2.0 * reflectionPoint[j] - simplex[i][j];
            }
            const double value = func(reflectedSimplex[i], size);
            if (value < minValue) {
                minValue = value;
                minValuePointIndex = i;
            }
        }
    }
    return minValuePointIndex;
}

int expandSimplex(const double **const simplex, const int simplexSize, const int size, const double alpha,
                  double **expandedSimplex, const int expansionPointIndex, const Function1Arg func
) {
    const double *const expansionPoint = simplex[expansionPointIndex];
    // It's possible here that memory overlaps thus we use memove and not memcpy
    memmove(expandedSimplex[expansionPointIndex], expansionPoint, size * sizeof(double));

    double minValue = func(expansionPoint, size);
    int minValuePointIndex = expansionPointIndex;
    for (int i = 0; i < simplexSize; i++) {
        if (LIKELY(i != expansionPointIndex)) {
            for (int j = 0; j < size; j++) {
                expandedSimplex[i][j] =
                        alpha * simplex[i][j] + (1.0 - alpha) * expansionPoint[j];
            }

            const double value = func(expandedSimplex[i], size);
            if (value < minValue) {
                minValue = value;
                minValuePointIndex = i;
            }
        }
    }

    return minValuePointIndex;
}

int contractSimplex(const double **const simplex, const int simplexSize, const int size, const double beta,
                    double **contractedSimplex, const int contractionPointIndex, const Function1Arg func
) {
    const double *const contractionPoint = simplex[contractionPointIndex];
    // It's possible here that memory overlaps thus we use memove and not memcpy
    memmove(contractedSimplex[contractionPointIndex], contractionPoint, size * sizeof(double));

    double minValue = func(contractionPoint, size);
    int minValuePointIndex = contractionPointIndex;
    for (int i = 0; i < simplexSize; i++) {
        if (LIKELY(i != contractionPointIndex)) {
            for (int j = 0; j < size; j++) {
                contractedSimplex[i][j] =
                        beta * simplex[i][j] + (1.0 - beta) * contractionPoint[j];
            }
            const double value = func(contractedSimplex[i], size);
            if (value < minValue) {
                minValue = value;
                minValuePointIndex = i;
            }
        }
    }

    return minValuePointIndex;
}

void swapSimplex(double ***simplex1, double ***simplex2) {
    double **tmp = (*simplex1);
    (*simplex1) = (*simplex2);
    (*simplex2) = tmp;
}
