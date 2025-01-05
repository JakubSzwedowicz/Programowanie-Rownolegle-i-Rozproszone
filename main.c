//
// Created by jakubszwedowicz on 1/3/25.
//

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "nelderMead.h"
#include "utils.h"

#define ALPHA 1.5
#define BETA  0.5
#define S     1.0


int main(int argc, char **argv) {
    int size = 0;
    int function = 0;
    double epsilon = 1e-3;

    if (parseArguments(argc, argv, &function, &size, &epsilon) != 0) {
        return 1;
    }

    double* bestPoint = malloc(size * sizeof(double));
    const Function1Arg func = getFunction(function);
    const Function1ArgFillInitialVec fillInitialVec = getFunctionFillInitialVec(function);
    int iters = 0;

    clock_t start = clock();
    nelderMeadSequential(func, fillInitialVec, size, S, ALPHA, BETA, epsilon, bestPoint,
                         &iters);
    clock_t end = clock();

    double time_spent = (double) (end - start) / CLOCKS_PER_SEC;
    printf("Sequential result:\n");
    printf("\tmin f(x)   = %f\n", quadraticFunction1(bestPoint, size));
    printf("\tmin x      = %s\n", print(bestPoint, size));
    printf("\tsize       = %d\n", size);
    printf("\tepsilon    = %.g\n", epsilon);
    printf("\titerations = %d\n", iters);
    printf("\ttime       = %.3f s\n", time_spent);

    free(bestPoint);
    return 0;
}
