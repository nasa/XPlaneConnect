//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define feq(x, y) (fabs(x - y) < 1e-4)

#define IP "127.0.0.1"

extern int testFailed;
extern int testPassed;

void runTest(int(*test)(), char* name);

int compareFloat(float expected, float actual);
int compareArray(float expected[], float actual[], int size);
int compareArrays(float* expected[], int esizes[], float* actual[], int asizes[], int count);

#endif