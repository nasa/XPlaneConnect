//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#include "Test.h"

int testFailed = 0;
int testPassed = 0;

void runTest(int(*test)(), char* name)
{
	printf("Running test %s... ", name);
	int result = test(); // Run Test
	if (result == 0)
	{
		printf("PASSED\n");
		testPassed++;
	}
	else
	{
		printf("Test %s - FAILED\n\tError: %i\n", name, result);
		testFailed++;
	}
}

void crossPlatformUSleep(int uSleep) {
#ifdef _WIN32
    Sleep(uSleep/1000);
#else
    usleep(uSleep);
#endif
}

int compareFloat(float expected, float actual)
{
	return feq(expected, actual) || isnan(expected) ? 0 : -1;
}

int compareArray(float expected[], float actual[], int size)
{
	return compareArrays(&expected, &size, &actual, &size, 1);
}

int compareArrays(float* expected[], int esizes[], float* actual[], int asizes[], int count)
{
	for (int i = 0; i < count; ++i)
	{
		if (esizes[i] != asizes[i])
		{
			return -100 - i;
		}
		for (int j = 0; j < esizes[i]; ++j)
		{
			if (!feq(actual[i][j], expected[i][j]) && !isnan(expected[i][j]))
			{
				return -1000 - i * 100 - j;
			}
		}
	}
	return 0;
}

int compareDoubleArray(double expected[], double actual[], int size)
{
	return compareDoubleArrays(&expected, &size, &actual, &size, 1);
}

int compareDoubleArrays(double* expected[], int esizes[], double* actual[], int asizes[], int count)
{
	for (int i = 0; i < count; ++i)
	{
		if (esizes[i] != asizes[i])
		{
			return -100 - i;
		}
		for (int j = 0; j < esizes[i]; ++j)
		{
			if (!feq(actual[i][j], expected[i][j]) && !isnan(expected[i][j]))
			{
				return -1000 - i * 100 - j;
			}
		}
	}
	return 0;
}
