//
// Created by jakubszwedowicz on 1/14/25.
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "nelderMeadMPI.h"
#include "common.h"

void initializeSimplex(double **simplex, const int simplexSize, const int size) {
    for (int i = 0; i < simplexSize; i++) {
        for (int j = 0; j < size; j++) {
            simplex[i][j] = 0;
        }
    }
}

int compareSimplex(double **s1, double **s2, const int rows, const int cols) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fabs(s1[i][j] - s2[i][j]) > 1e-6) {
                printf("compareSimplex Rank %d: s1[%d][%d] = %lf, s2[%d][%d] = %lf\n", rank, i, j, s1[i][j], i, j,
                       s2[i][j]);
                return 0;
            }
        }
    }
    return 1;
}

const char *printSimplex(double **simplex, const int simplexSize, const int size) {
    static char buffer[2024];
    char *ptr = buffer;
    long unsigned offset = 0;

    offset += snprintf(ptr + offset, sizeof(buffer) - offset, "[");
    for (int i = 0; i < simplexSize; i++) {
        if (i != 0) offset += snprintf(ptr + offset, sizeof(buffer) - offset, "\n");
        offset += snprintf(ptr + offset, sizeof(buffer) - offset, "[");
        for (int j = 0; j < size; j++) {
            offset += snprintf(ptr + offset, sizeof(buffer) - offset, "%.2f ", simplex[i][j]);
            if (offset >= sizeof(buffer)) {
                return "error";
            }
        }
        offset += snprintf(ptr + offset, sizeof(buffer) - offset, "]");
    }
    offset += snprintf(ptr + offset, sizeof(buffer) - offset, "]");
    return buffer;
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, numProcs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    if (rank == 0) printf("===== Starting Nelder-Mead MPI C Tests =====\n\n");

    const int size = 4; // Rozmiar wektora (liczba kolumn)
    const int simplexSize = size + 1; // Liczba punktów sympleksu

    double **simplex = NULL;
    int res = allocateSimplex(&simplex, simplexSize, size);
    if (!simplex) {
        fprintf(stderr, "Proces %d: Błąd alokacji macierzy simplex.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    initializeSimplex(simplex, simplexSize, size);

    int startIdx = getStartingIdxOfProcess(simplexSize, rank, numProcs);
    int endIdx = startIdx + getPointsPerProcess(simplexSize, rank, numProcs);
    for (int i = startIdx; i < endIdx; i++) {
        for (int j = 0; j < size; j++) {
            simplex[i][j] = rank;
        }
    }

    synchronizeSimplex(simplex, simplexSize, size);

    double **expectedSimplex = NULL;
    res = allocateSimplex(&expectedSimplex, simplexSize, size);
    for (int root = 0; root < numProcs; root++) {
        int startIdx = getStartingIdxOfProcess(simplexSize, root, numProcs);
        int endIdx = startIdx + getPointsPerProcess(simplexSize, root, numProcs);
        for (int i = startIdx; i < endIdx; i++) {
            for (int j = 0; j < size; j++) {
                expectedSimplex[i][j] = root;
            }
        }
    }


    int same = compareSimplex(simplex, expectedSimplex, simplexSize, size);
    // printf("Rank %d: simplex = \n%s\n", rank, printSimplex(simplex, simplexSize, size));
    // printf("Rank %d: expectedSimplex = \n%s\n", rank, printSimplex(expectedSimplex, simplexSize, size));

    for (int i = 0; i < simplexSize; i++) {
        free(simplex[i]);
        free(expectedSimplex[i]);
    }
    free(simplex);
    free(expectedSimplex);

    if (rank == 0) {
        if (same == 1) printf("All tests passed successfully.\n");
        else printf("Rank %d: Some tests failed.\n", rank);
    }
    MPI_Finalize();
    return same ? 0 : 1;
}
