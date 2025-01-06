//
// Created by jakubszwedowicz on 1/5/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "nelderMead.h"

// Would prefer using gtest but C++ is prohibited in this project
static void test_allocate_deallocate_simplex(void)
{
    printf("Running test_allocate_deallocate_simplex...\n");

    const int size = 5;
    const int simplexSize = size + 1;
    double **simplex = NULL;

    int allocRes = allocateSimplex(&simplex, simplexSize, size);
    assert(allocRes == 0);
    assert(simplex != NULL);

    for (int i = 0; i < simplexSize; i++) {
        assert(simplex[i] != NULL);
    }

    int deallocRes = deallocateSimplex(&simplex, simplexSize);
    assert(deallocRes == 0);

    assert(simplex == NULL);

    printf("OK: test_allocate_deallocate_simplex passed.\n\n");
}

static void test_fill_initial_simplex(void)
{
    printf("Running test_fill_initial_simplex...\n");

    const int size = 3;
    const int simplexSize = size + 1;
    const double distance = 1.0;
    double **simplex = NULL;
    int rc = allocateSimplex(&simplex, simplexSize, size);
    assert(rc == 0);

    double x0[3] = { 2.0, 5.0, -1.0 };

    fillInitialSimplex(simplex, x0, simplexSize, size, distance);

    for (int j = 0; j < size; j++) {
        assert(fabs(simplex[0][j] - x0[j]) < 1e-12);
    }

    for (int i = 1; i < simplexSize; i++) {
        for (int j = 0; j < size; j++) {
            double expected = x0[j];
            if (j == (i - 1)) {
                expected += distance;
            }
            assert(fabs(simplex[i][j] - expected) < 1e-12);
        }
    }

    deallocateSimplex(&simplex, simplexSize);
    printf("OK: test_fill_initial_simplex passed.\n\n");
}

/* ---------------------------------------------------------------------------
 * 3) A small function to test Nelder–Mead in dimension=2
 *    f(x,y) = (x - 1)^2 + (y + 2)^2.
 *    The global min is (1, -2) with value = 0.
 * ------------------------------------------------------------------------- */
static double test_function_2d(const double *vec, int n)
{
    /* We'll assume n=2 here. */
    if (n < 2) return 1e30; /* or some large penalty */
    double x = vec[0];
    double y = vec[1];
    return (x - 1.0) * (x - 1.0) + (y + 2.0) * (y + 2.0);
}

static int fill_function_2d(double *vec, int n)
{
    if (n < 2) return -1;
    /* Start from some far point to see if NM can converge. */
    vec[0] = 5.0;
    vec[1] = 5.0;
    return 0;
}

static void test_nelder_mead_2d(void)
{
    printf("Running test_nelder_mead_2d...\n");

    const int size = 2;
    double bestPoint[2];
    int iterations = 0;

    double distance = 1.0;
    double alpha    = 1.5;
    double beta     = 0.5;
    double epsilon  = 1e-6;

    int ret = nelderMeadOpenMP(
        test_function_2d,
        fill_function_2d,
        size,
        distance, alpha, beta, epsilon,
        bestPoint, &iterations, 1
    );
    assert(ret == 0);

    assert(iterations > 1);

    /* Check that bestPoint is near (1, -2). */
    double x = bestPoint[0];
    double y = bestPoint[1];
    double val = test_function_2d(bestPoint, size);

    /* use some small tolerance, e.g. 1e-3 or 1e-4 */
    assert(fabs(x - 1.0) < 1e-3);
    assert(fabs(y + 2.0) < 1e-3);
    assert(val < 1e-5);

    printf("OK: test_nelder_mead_2d passed.\n\n");
}

int main(void)
{
    printf("===== Starting Nelder-Mead C Tests =====\n\n");

    test_allocate_deallocate_simplex();
    test_fill_initial_simplex();
    test_nelder_mead_2d();

    printf("All tests passed successfully.\n");
    return 0;
}
