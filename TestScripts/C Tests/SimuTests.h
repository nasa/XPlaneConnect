//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef SIMUTESTS_H
#define SIMIUTESTS_H

#include "Test.h"
#include "xplaneConnect.h"

int doSIMUTest(int value, float expected)
{
	int size = 20;
	float actual[20];
	char* dref = "sim/operation/override/override_planepath";

	XPCSocket sock = openUDP(IP);
	int result = pauseSim(sock, value);
	if (result >= 0)
	{
		result = getDREF(sock, dref, actual, &size);
	}
	closeUDP(sock);
	if (result < 0)
	{
		return -1;
	}

	for (int i = 0; i < 20; ++i)
	{
		if (!feq(actual[i], expected) && !isnan(expected))
		{
			return -100 - i;
		}
	}
    return 0;
}

int testSIMU_Basic()
{
	int result = doSIMUTest(0, 0);
	if (result < 0)
	{
		return -1;
	}

	result = doSIMUTest(1, 1);
	if (result < 0)
	{
		return -2;
	}

	result = doSIMUTest(0, 0);
	if (result < 0)
	{
		return -3;
	}
	return 0;
}

int testSIMU_Toggle()
{
	int result = doSIMUTest(0, 0);
	if (result < 0)
	{
		return -1;
	}

	result = doSIMUTest(2, 1);
	if (result < 0)
	{
		return -1;
	}

	result = doSIMUTest(2, 0);
	if (result < 0)
	{
		return -3;
	}
	return 0;
}

#endif
