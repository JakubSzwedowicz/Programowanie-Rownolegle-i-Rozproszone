//
// Created by jakubszwedowicz on 1/4/25.
//

#ifndef UTILS_H
#define UTILS_H

typedef double (*Function1Arg)(const double *vec, const int size);

typedef int (*Function1ArgFillInitialVec)(double *vec, const int size);

int parseArguments(int argc, char **argv, int *func, int *size, double *epsilon, int *ompThreads, char *logfile,
                   const int logfileSize, char *command, const int commandSize);

Function1Arg getFunction(const int func);

Function1ArgFillInitialVec getFunctionFillInitialVec(const int func);

double quadraticFunction1(const double *vec, const int size);

int quadraticFunction1FillInitialVec(double *vec, const int size);

double trigonometricFunction16(const double *vec, const int size);

int trigonometricFunction16FillInitialVec(double *vec, const int size);

double quarticFunction17(const double *vec, const int size);

int quarticFunction17FillInitialVec(double *vec, const int size);

const char *print(const double *vec, const int size);

const char* printSimplex(const double** simplex, const int simplexSize, const int size);

const char *getFunctionName(const int func);

#endif //UTILS_H
