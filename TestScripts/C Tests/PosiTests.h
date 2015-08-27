//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef POSITESTS_H
#define POSITESTS_H

#include "Test.h"
#include "xplaneConnect.h"

int doPOSITest(char* drefs[7], float values[], int size, int ac, float expected[7])
{
	float* data[7];
	int sizes[7];
	for (int i = 0; i < 7; ++i)
	{
		data[i] = (float*)malloc(sizeof(float) * 10);
		sizes[i] = 10;
	}

	// Execute command
	XPCSocket sock = openUDP(IP);
	pauseSim(sock, 1);
	int result = sendPOSI(sock, values, size, ac);
	if (result >= 0)
	{
		result = getDREFs(sock, drefs, data, 7, sizes);
	}
	pauseSim(sock, 0);
	closeUDP(sock);
	if (result < 0)
	{
		return -1;
	}

	// Test values
	float actual[7];
	for (int i = 0; i < 7; ++i)
	{
		actual[i] = data[i][0];
	}
	return compareArray(expected, actual, 7);
}

int doGETPTest(float values[7], int ac, float expected[7])
{
	// Execute Test
	float actual[7];
	XPCSocket sock = openUDP(IP);
	int result = sendPOSI(sock, values, 7, ac);
	if (result >= 0)
	{
		result = getPOSI(sock, actual, ac);
	}
	closeUDP(sock);
	if (result < 0)
	{
		return -1;
	}

	// Test values
	for (int i = 0; i < 7; ++i)
	{
		if (fabs(expected[i] - actual[i]) > 1e-4)
		{
			return -10 - i;
		}
	}
	return 0;
}

int basicPOSITest(char** drefs, int ac)
{
	// Set psoition and initial orientation
	float POSI[7] = { 37.524F, -122.06899F, 2500, 0, 0, 0, 1 };
	float expected[7] = { 37.524F, -122.06899F, 2500, 0, 0, 0, 1 };
	int result = doPOSITest(drefs, POSI, 7, ac, expected);
	if (result < 0)
	{
		return -10000 + result;
	}

	// Set orientation
	POSI[0] = -998.0F;
	POSI[1] = -998.0F;
	POSI[2] = -998.0F;
	POSI[3] = 5.0F;
	POSI[4] = -5.0F;
	POSI[5] = 10.0F;
	POSI[6] = 0;

	float *loc[3];
	for (int i = 0; i < 3; ++i)
	{
		loc[i] = (float*)malloc(sizeof(float));
	}
	int sizes[3] = { 1, 1, 1 };
	XPCSocket sock = openUDP(IP);
	pauseSim(sock, 1);
	getDREFs(sock, drefs, loc, 3, sizes);
	closeUDP(sock);

	expected[0] = loc[0][0];
	expected[1] = loc[1][0];
	expected[2] = loc[2][0];
	expected[3] = 5.0F;
	expected[4] = -5.0F;
	expected[5] = 10.0F;
	expected[6] = 0.0F;
	result = doPOSITest(drefs, POSI, 7, ac, expected);
	if (result < 0)
	{
		return -20000 + result;
	}
    return 0;
}

int testPOSI_Player()
{
	char* drefs[] =
	{
		"sim/flightmodel/position/latitude",
		"sim/flightmodel/position/longitude",
		"sim/flightmodel/position/elevation",
		"sim/flightmodel/position/theta",
		"sim/flightmodel/position/phi",
		"sim/flightmodel/position/psi",
		"sim/cockpit/switches/gear_handle_status"
	};
	return basicPOSITest(drefs, 0);
}

int testPOSI_NonPlayer()
{
	char* drefs[] =
	{
		"sim/multiplayer/position/plane1_lat",
		"sim/multiplayer/position/plane1_lon",
		"sim/multiplayer/position/plane1_el",
		"sim/multiplayer/position/plane1_the",
		"sim/multiplayer/position/plane1_phi",
		"sim/multiplayer/position/plane1_psi",
		"sim/multiplayer/position/plane1_gear_deploy"
	};
	return basicPOSITest(drefs, 1);
}

int testGetPOSI_Player()
{
	float POSI[7] = { 37.524F, -122.06899F, 2500, 0, 0, 0, 1 };
	return doGETPTest(POSI, 0, POSI);
}

int testGetPOSI_NonPlayer()
{
	float POSI[7] = { 37.624F, -122.06899F, 1500, 0, 0, 0, 1 };
	return doGETPTest(POSI, 3, POSI);
}

#endif