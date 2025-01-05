//
// Created by jakubszwedowicz on 1/4/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "utils.h"

int parseArguments(int argc, char **argv, int *func, int *size, double *epsilon) {
    int opt;
    while ((opt = getopt(argc, argv, "f:s:e:")) != -1) {
        switch (opt) {
            case 'f':
                *func = strtol(optarg, NULL, 10);
                if (*func != 1 && *func != 16 && *func != 17) {
                    fprintf(stderr, "Function number must be 1, 16 or 17.\n");
                    return 1;
                }
                break;
            case 's':
                *size = strtol(optarg, NULL, 10);
                if (*size <= 0) {
                    fprintf(stderr, "Size must be greater than 0.\n");
                    return 1;
                }
                break;
            case 'e':
                *epsilon = pow(10, -strtol(optarg, NULL, 10));
                if (*epsilon < 0) {
                    fprintf(stderr, "Epsilon cannot be negative.\n");
                    return 1;
                }
                break;
            case '?':
            default:
                fprintf(stderr, "Usage: %s -f <function_number> -s <size> -e <epsilon_exponent_value>\n", argv[0]);
                return 1;
        }
    }

    if (*func == 0 || *size <= 0 || *epsilon < 0) {
        fprintf(stderr, "Usage: %s -f <function_number> -s <size> -e <epsilon_exponent_value>\n"
                "Examples:\n"
                "\t./app -f 1 -s 50 -e 3\n"
                "\t./app -f 16 -s 100 -e 6\n"
                "\t./app -f 17 -s 150 -e 9\n"
                "Options:\n"
                "\t-f     Use function number 1, 16 or 17.\n"
                "\t-s     Pass vector size such as 50, 100, 150. Greater are not recommended.\n"
                "\t-e     Pass epsilon exponent such as 3, 6, 9 to be used in 1e-x\n", argv[0]);
        return 1;
    }

    return 0;
}

Function1Arg getFunction(const int func) {
    switch (func) {
        case 1:
            fprintf(stdout, "Using function 1: f(x) = sum3->N(100(x_i^2 + x_{i-1}^2) + x_{i-2}^2)\n");
            return quadraticFunction1;
        case 16:
            fprintf(
                stdout, "Using function 16: f(x) = sum1->N(n - sum1->N(cos(x_j)) + (i)(1 - cos(x_i)) - sin(x_i))^2)\n");
            return trigonometricFunction16;
        case 17:
            fprintf(stdout, "Using function 17: f(x) = (sum1->N(i * x_i^2))^2\n");
            return quarticFunction17;
        default:
            return NULL;
    }
}

Function1ArgFillInitialVec getFunctionFillInitialVec(const int func) {
    switch (func) {
        case 1:
            return quadraticFunction1FillInitialVec;
        case 16:
            return trigonometricFunction16FillInitialVec;
        case 17:
            return quarticFunction17FillInitialVec;
        default:
            return NULL;
    }
}

double quadraticFunction1(const double *vec, const int size) {
    double sum = 0;
    for (int i = 2; i < size; i++) {
        sum += 100 * (vec[i] * vec[i] + vec[i - 1] * vec[i - 1]) + vec[i - 2] * vec[i - 2];
    }
    return sum;
}

int quadraticFunction1FillInitialVec(double *vec, const int size) {
    for (int i = 0; i < size; i++) {
        vec[i] = 3;
    }
    return 0;
}


double trigonometricFunction16(const double *vec, const int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        double sum2 = 0;
        for (int j = 0; j < size; j++) {
            sum2 += cos(vec[j]);
        }
        sum += pow(size - sum2 + (i + 1) * (1 - cos(vec[i])) - sin(vec[i]), 2);
    }
    return sum;
}

int trigonometricFunction16FillInitialVec(double *vec, const int size) {
    for (int i = 0; i < size; i++) {
        vec[i] = 1;
    }
    return 0;
}

double quarticFunction17(const double *vec, const int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += (i + 1) * pow(vec[i], 2);
    }
    return pow(sum, 2);
}

int quarticFunction17FillInitialVec(double *vec, const int size) {
    for (int i = 0; i < size; i++) {
        vec[i] = 1;
    }
    return 0;
}

const char *print(const double *vec, const int size) {
    static char buffer[1024];
    char *ptr = buffer;
    long unsigned offset = 0;

    for (int i = 0; i < size; i++) {
        offset += snprintf(ptr + offset, sizeof(buffer) - offset, "%.2f ", vec[i]);
        if (offset >= sizeof(buffer)) {
            break;
        }
    }

    return buffer;
}
