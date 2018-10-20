//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#ifdef LINUX
#include <unistd.h>
#endif
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define SLEEP_AMOUNT 100000

#define feq(x, y) (fabs(x - y) < 1e-4)

#define IP "127.0.0.1"

void crossPlatformUSleep(int uSleep);

extern int testFailed;
extern int testPassed;

void runTest(int(*test)(), char* name);

int compareFloat(float expected, float actual);
int compareArray(float expected[], float actual[], int size);
int compareArrays(float* expected[], int esizes[], float* actual[], int asizes[], int count);
int compareDoubleArray(double expected[], double actual[], int size);
int compareDoubleArrays(double* expected[], int esizes[], double* actual[], int asizes[], int count);

#endif
