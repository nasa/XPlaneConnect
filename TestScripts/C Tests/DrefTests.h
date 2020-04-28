//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef DREFTESTS_H
#define DREFTESTS_H

#include "Test.h"
#include "xplaneConnect.h"

int doGETDTest(const char* drefs[], float* expected[], int count, int sizes[])
{
	// Setup memory
	int* asizes = (int*)malloc(sizeof(int) * count);
	float** actual = (float**)malloc(sizeof(float*) * count);
	for (int i = 0; i < count; ++i)
	{
		asizes[i] = sizes[i];
		actual[i] = (float*)malloc(sizeof(float) * sizes[i]);
	}

	// Execute command
	XPCSocket sock = openUDP(IP);
	int result = getDREFs(sock, drefs, actual, count, asizes);
	closeUDP(sock);
	if (result < 0)
	{
		return -1;
	}

	// Test sizes and values
	result = compareArrays(expected, sizes, actual, asizes, count);
	for (int i = 0; i < count; ++i)
	{
		free(actual[i]);
	}
	free(actual);
	free(asizes);
	return result;
}

int doDREFTest(const char* drefs[], float* values[], float* expected[], int count, int sizes[])
{
	// Setup memory
	int* asizes = (int*)malloc(sizeof(int) * count);
	float** actual = (float**)malloc(sizeof(float*) * count);
	for (int i = 0; i < count; ++i)
	{
		asizes[i] = sizes[i];
		actual[i] = (float*)malloc(sizeof(float) * sizes[i]);
	}

	// Execute command
	XPCSocket sock = openUDP(IP);
	int result = sendDREFs(sock, drefs, values, sizes, count);
	if (result >= 0)
	{
		result = getDREFs(sock, drefs, actual, count, asizes);
	}
	closeUDP(sock);
	if (result < 0)
	{
		return -1;
	}

	// Test sizes and values
	result = compareArrays(expected, sizes, actual, asizes, count);
	for (int i = 0; i < count; ++i)
	{
		free(actual[i]);
	}
	free(actual);
	free(asizes);
	return result;
}

int testGETD_Basic()
{
	const char* drefs[] =
	{
		"sim/cockpit/switches/gear_handle_status", //int
		"sim/cockpit/autopilot/altitude", //float
		"sim/aircraft/prop/acf_prop_type", //int[8]
		"sim/cockpit2/switches/panel_brightness_ratio", //float[4]
		"sim/aircraft/view/acf_tailnum", //byte[40]
		"sim/flightmodel/position/elevation" //double
	};
	int sizes[] = { 1, 1, 8, 4, 40, 1 };
	float* expected[6];
	for (int i = 0; i < 6; ++i)
	{
		expected[i] = (float*)malloc(sizeof(float) * sizes[i]);
		for (int j = 0; j < sizes[i]; ++j)
		{
			expected[i][j] = NAN;
		}
	}

	return doGETDTest(drefs, expected, 6, sizes);
}

int testGETD_TestFloat()
{
	const char* dref = "sim/test/test_float";
	int size = 1;
	float* expected[1];
	expected[0] = (float*)malloc(sizeof(float));
	expected[0][0] = 0.0F;

	int result = doGETDTest(&dref, expected, 1, &size);
	free(expected[0]);
	return result;
}

int testGETD_Types()
{
	const char* drefs[] =
	{
		"sim/cockpit/switches/gear_handle_status", //int
		"sim/cockpit/autopilot/altitude", //float
		"sim/aircraft/prop/acf_prop_type", //int[8]
		"sim/cockpit2/switches/panel_brightness_ratio", //float[4]
		"sim/aircraft/view/acf_tailnum", //byte[40]
		"sim/flightmodel/position/elevation" //double
	};
	int sizes[] = { 1, 1, 8, 4, 40, 1 };
	float* data[6];
	for (int i = 0; i < 6; ++i)
	{
		data[i] = (float*)malloc(sizeof(float) * sizes[i]);
	}

	// Execute command
	XPCSocket sock = openUDP(IP);
	int result = getDREFs(sock, drefs, data, 6, sizes);
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -1;
	}
	// Verify sizes
	if (sizes[0] != 1 || sizes[1] != 1 || sizes[2] != 8
		|| sizes[3] != 4 || sizes[4] != 40 || sizes[5] != 1)
	{
		return -2;
	}
	// Verify integer drefs are integers
	if ((float)((int)data[0][0]) != data[0][0])
	{
		return -3;
	}
	for (int i = 0; i < 8; ++i)
	{
		if ((float)((int)data[2][i]) != data[2][i])
		{
			return -3;
		}
	}
	for (int i = 0; i < 40; ++i)
	{
		if ((float)((char)data[4][i]) != data[4][i])
		{
			return -3;
		}
	}
	// Verify tail number has at least one valid character
	if (data[4][0] <= 0 || data[4][0] > 127)
	{
		return -4;
	}
	return 0;
}

int testDREF()
{
	const char* drefs[] =
	{
		"sim/cockpit/switches/gear_handle_status", //int
		"sim/cockpit/autopilot/altitude", //float
		"sim/aircraft/prop/acf_prop_type", //int[8]
		"sim/cockpit2/switches/panel_brightness_ratio", //float[4]
		"sim/aircraft/view/acf_tailnum", //byte[40]
		"sim/flightmodel/position/elevation" //double - Read only
	};
	float* values[6];
	float* expected[6];
	int sizes[6];

	// Setup
	sizes[0] = 1;
	values[0] = (float*)malloc(sizes[0] * sizeof(float));
	expected[0] = values[0];
	values[0][0] = 1;

	sizes[1] = 1;
	values[1] = (float*)malloc(sizes[1] * sizeof(float));
	expected[1] = values[1];
	values[1][0] = 4000.0F;

	sizes[2] = 8;
	values[2] = (float*)malloc(sizes[2] * sizeof(float));
	expected[2] = values[2];
	for (int i = 0; i < 8; ++i)
	{
		values[2][i] = 0;
	}

	sizes[3] = 4;
	values[3] = (float*)malloc(sizes[3] * sizeof(float));
	expected[3] = values[3];
	for (int i = 0; i < 4; ++i)
	{
		values[3][i] = 0.25F;
	}

	sizes[4] = 40;
	values[4] = (float*)malloc(sizes[4] * sizeof(float));
	expected[4] = (float*)malloc(sizes[4] * sizeof(float));
	memset(values[4], 0, sizes[4] * sizeof(float));
	values[4][0] = 78.0F; //N
	values[4][1] = 55.0F; //7
	values[4][2] = 52.0F; //4
	values[4][3] = 56.0F; //8
	values[4][4] = 53.0F; //5
	values[4][5] = 89.0F; //Y

	expected[4][0] = 78.0F; //N
	expected[4][1] = 55.0F; //7
	expected[4][2] = 52.0F; //4
	expected[4][3] = 56.0F; //8
	expected[4][4] = 53.0F; //5
	expected[4][5] = 89.0F; //Y
	for (int i = 6; i < sizes[4]; ++i)
	{
		expected[4][i] = NAN;
	}

	sizes[5] = 1;
	values[5] = (float*)malloc(sizes[5] * sizeof(float));
	expected[5] = (float*)malloc(sizes[5] * sizeof(float));
	values[5][0] = 5000.0F;
	expected[5][0] = NAN;

	return doDREFTest(drefs, values, expected, 6, sizes);
}

#endif
