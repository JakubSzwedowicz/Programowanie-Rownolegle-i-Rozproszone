//
// Created by jakubszwedowicz on 1/4/25.
//

#ifndef UTILS_H
#define UTILS_H

typedef double (*Function1Arg)(const double *vec, const int size);

typedef int (*Function1ArgFillInitialVec)(double *vec, const int size);

int parseArguments(int argc, char **argv, int *func, int *size);

Function1Arg getFunction(const int func);

Function1ArgFillInitialVec getFunctionFillInitialVec(const int func);

double quadraticFunction1(const double *vec, const int size);

int quadraticFunction1FillInitialVec(double *vec, const int size);

double trigonometricFunction16(const double *vec, const int size);

int trigonometricFunction16FillInitialVec(double *vec, const int size);

double quarticFunction17(const double *vec, const int size);

int quarticFunction17FillInitialVec(double *vec, const int size);

#endif //UTILS_H
