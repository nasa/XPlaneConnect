//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef DATATESTS_H
#define DATATESTS_H

#include "Test.h"
#include "xplaneConnect.h"

int testDATA()
{
	// Initialize
	int i, j; // Iterator
	const char* drefs[100] =
	{
		"sim/aircraft/parts/acf_gear_deploy"
	};
	float* data[100]; // array for result of getDREFs
	int sizes[100];
	float DATA[4][9]; // Array for sendDATA
	XPCSocket sock = openUDP(IP);

	// Setup
	for (int i = 0; i < 100; ++i)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
	}
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 9; j++)
		{
			data[i][j] = -998;
		}
	}
	DATA[0][0] = 14; // Gear
	DATA[0][1] = 1;
	DATA[0][2] = 0;

	// Execution
	sendDATA(sock, DATA, 1);
	int result = getDREFs(sock, drefs, data, 1, sizes);

	// Close
	closeUDP(sock);

	// Tests
	if (result < 0)// Request 1 value
	{
		return -1;
	}
	if (sizes[0] != 10)
	{
		return -2;
	}
	if (*data[0] != data[0][1])
	{
		return -3;
	}
	return 0;
}

#endif
