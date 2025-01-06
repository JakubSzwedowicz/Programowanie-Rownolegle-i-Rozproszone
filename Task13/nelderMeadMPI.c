//
// Created by jakubszwedowicz on 1/6/25.
//

#include "nelderMeadMPI.h"


int nelderMeadMPI(const Function1Arg func, const Function1ArgFillInitialVec fillInitialVec, const int size,
                 const double distance, const double alpha, const double beta, const double epsilon,
                 double *bestPoint, int *iterations) {
    int rank, numberOfProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numberOfProcesses);

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
    int minValuePointIndex = findMinValuePointIndexMPI(func, simplex, simplexSize, size, rank, numberOfProcesses);

    int iters = 0;
    for (int running =1; running; iters++) {
        // Step 3: Reflection
        // Obliczenie centroidu (wszystkie punkty oprócz najgorszego)
        double centroid[size];
        memset(centroid, 0, sizeof(double) * size);

        for (int i = 0; i < simplexSize; i++) {
            if (i != minValuePointIndex) {
                for (int j = 0; j < size; j++) {
                    centroid[j] += simplex[i][j];
                }
            }
        }

        // Podział centroidu między procesy
        for (int j = 0; j < size; j++) {
            centroid[j] /= (simplexSize - 1);
        }

        // Refleksja: tworzenie nowego punktu
        double reflectionPoint[size];
        for (int j = 0; j < size; j++) {
            reflectionPoint[j] = 2.0 * centroid[j] - simplex[minValuePointIndex][j];
        }

        // Rozesłanie punktu refleksji do wszystkich procesów
        MPI_Bcast(reflectionPoint, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // Ocena punktu refleksji przez wszystkie procesy
        double reflectionValue = func(reflectionPoint, size);

        // Zbiór refleksji do znalezienia najlepszego reflektowanego punktu
        struct {
            double value;
            int index;
        } refl_local_data, refl_global_data;

        refl_local_data.value = reflectionValue;
        refl_local_data.index = -1; // Nie jest potrzebny indeks

        MPI_Allreduce(&refl_local_data, &refl_global_data, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

        // Decyzja: czy ekspansja, czy kontrakcja
        int do_expansion = 0;
        int do_contraction = 0;

        if (reflectionValue < func(simplex[0], size)) {
            do_expansion = 1;
        } else if (reflectionValue < func(simplex[minValuePointIndex], size)) {
            // Przyjmujemy refleksję
            memcpy(simplex[minValuePointIndex], reflectionPoint, sizeof(double) * size);
        } else {
            do_contraction = 1;
        }

        if (do_expansion) {
            // Ekspansja: tworzenie punktu ekspansji
            double expansionPoint[size];
            for (int j = 0; j < size; j++) {
                expansionPoint[j] = centroid[j] + alpha * (reflectionPoint[j] - centroid[j]);
            }

            // Rozesłanie punktu ekspansji do wszystkich procesów
            MPI_Bcast(expansionPoint, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

            // Ocena punktu ekspansji przez wszystkie procesy
            double expansionValue = func(expansionPoint, size);

            // Decyzja: czy przyjąć ekspansję czy refleksję
            if (expansionValue < reflectionValue) {
                // Przyjmujemy ekspansję
                memcpy(simplex[minValuePointIndex], expansionPoint, sizeof(double) * size);
            } else {
                // Przyjmujemy refleksję
                memcpy(simplex[minValuePointIndex], reflectionPoint, sizeof(double) * size);
            }
        }

        if (do_contraction) {
            // Kontrakcja: tworzenie punktu kontrakcji
            double contractionPoint[size];
            for (int j = 0; j < size; j++) {
                contractionPoint[j] = centroid[j] + beta * (simplex[minValuePointIndex][j] - centroid[j]);
            }

            // Rozesłanie punktu kontrakcji do wszystkich procesów
            MPI_Bcast(contractionPoint, size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

            // Ocena punktu kontrakcji przez wszystkie procesy
            double contractionValue = func(contractionPoint, size);

            // Decyzja: czy przyjąć kontrakcję czy wszystkie punkty są kurczone
            if (contractionValue < func(simplex[minValuePointIndex], size)) {
                // Przyjmujemy kontrakcję
                memcpy(simplex[minValuePointIndex], contractionPoint, sizeof(double) * size);
            } else {
                // Kurczenie: ściskanie wszystkich punktów względem najlepszego
                for (int i = 1; i < simplexSize; i++) {
                    for (int j = 0; j < size; j++) {
                        simplex[i][j] = simplex[0][j] + 0.5 * (simplex[i][j] - simplex[0][j]);
                    }
                }
            }
        }

        // Sprawdzenie warunków zakończenia: maksymalna odległość między punktami sympleksu
        double local_max_dist = 0.0;
        for (int i = 0; i < simplexSize; i++) {
            for (int j = i + 1; j < simplexSize; j++) {
                double dist = calculateEuclideanDistance(simplex[i], simplex[j], size);
                if (dist > local_max_dist)
                    local_max_dist = dist;
            }
        }

        double global_max_dist;
        MPI_Allreduce(&local_max_dist, &global_max_dist, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);

        if (global_max_dist < epsilon) {
            running = 0;
        }

        // Aktualizacja liczby iteracji
        *iterations = iters;

        // Kończenie pętli
        if (!running) {
            // Znalezienie najlepszego punktu
            double local_best_value = DBL_MAX;
            int local_best_idx = -1;
            for (int i = 0; i < simplexSize; i++) {
                double value = func(simplex[i], size);
                if (value < local_best_value) {
                    local_best_value = value;
                    local_best_idx = i;
                }
            }

            struct {
                double value;
                int index;
            } best_local, best_global;

            best_local.value = local_best_value;
            best_local.index = local_best_idx;

            MPI_Allreduce(&best_local, &best_global, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

            if (rank == 0) {
                memcpy(bestPoint, simplex[best_global.index], sizeof(double) * size);
            }

            break;
        }
    }

    // Czyszczenie pamięci
    deallocateSimplex(&simplex, simplexSize);
    deallocateSimplex(&reflectedSimplex, simplexSize);
    deallocateSimplex(&expandedSimplex, simplexSize);
    deallocateSimplex(&contractedSimplex, simplexSize);

    return 0;
}

int findMinValuePointIndexMPI(const Function1Arg func, double **simplex, const int simplexSize, const int size, const int numberOfProcesses, const int rank) {
    int minValuePointIndex = -1;
    int startIdx = 0;
    {
        for (int p = 0; p < rank; p++) {
            startIdx += getPointsPerProcess(simplexSize, numberOfProcesses, p);
        }

        double localMinValue = DBL_MAX;
        int localMinIdx = -1;

        const int endIdx = startIdx + getPointsPerProcess(simplexSize, numberOfProcesses, rank);
        for (int i = startIdx; i < endIdx && i < simplexSize; i++) {
            double value = func(simplex[i], size);
            if (value < localMinValue) {
                localMinValue = value;
                localMinIdx = i;
            }
        }

        localData.value = localMinValue;
        localData.index = localMinIdx;

        MPI_Allreduce(&localData, &globalData, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

        minValuePointIndex = globalData.index;
    }
    return minValuePointIndex;
}

int getPointsPerProcess(const int simplexSize, const int numProcs, const int rank) {
    static int pointsPerProc = simplexSize / numProcs;
    static int remainder = simplexSize % numProcs;
    return rank < remainder ? pointsPerProc + 1 : pointsPerProc;
}