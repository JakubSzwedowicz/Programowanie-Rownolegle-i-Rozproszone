//
// Created by jakubszwedowicz on 1/3/25.
//

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "Nelder-Mead.h"
#include "utils.h"

#define EPSILON 1e-6
#define ALPHA 1.5
#define BETA  0.5
#define S     1.0


int main(char argc, char **argv) {
    int size = 0;
    int function = 0;

    if (parseArguments(argc, argv, &function, &size) != 0) {
        return 1;
    }

    double* bestPoint = malloc(size * sizeof(double));
    const Function1Arg func = getFunction(function);
    const Function1ArgFillInitialVec fillInitialVec = getFunctionFillInitialVec(function);
    int iters = 0;

    clock_t start = clock();
    nelderMeadSequential(func, fillInitialVec, size, S, ALPHA, BETA, EPSILON, bestPoint,
                         &iters);
    clock_t end = clock();

    double time_spent = (double) (end - start) / CLOCKS_PER_SEC;
    printf("Sequential result:\n");
    printf("\tmin f(x)   = %f\n", quadraticFunction1(bestPoint, size));
    printf("\tmin x      = %s\n", print(bestPoint, size));
    printf("\titerations = %d\n", iters);
    printf("\ttime       = %.3f s\n", time_spent);

    free(bestPoint);
    return 0;
}
