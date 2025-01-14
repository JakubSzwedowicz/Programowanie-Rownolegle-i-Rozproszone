//
// Created by jakubszwedowicz on 1/6/25.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"


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

void initializeInitialSimplex(double **simplex, const int simplexSize, const int size,
                              const Function1ArgFillInitialVec fillInitialVec, const double distance) {
    // Create initial vector
    double x0[size]; // Interestingly, since C99 this type of declaration is allowed
    fillInitialVec(x0, size);
    fillInitialSimplex(simplex, x0, simplexSize, size, distance);
}

void fillInitialSimplex(double **simplex, const double *x0, const int simplexSize, const int size,
                        const double distance) {
    memcpy(simplex[0], x0, size * sizeof(double));
    for (int i = 1; i < simplexSize; i++) {
        memcpy(simplex[i], simplex[0], size * sizeof(double));
        simplex[i][i - 1] += distance;
    }
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

void swapSimplex(double ***simplex1, double ***simplex2) {
    double **tmp = (*simplex1);
    (*simplex1) = (*simplex2);
    (*simplex2) = tmp;
}