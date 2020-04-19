//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef CTRLTESTS_H
#define CTRLTESTS_H

#include "Test.h"
#include "xplaneConnect.h"

int doCTRLTest(XPCSocket *sock, const char* drefs[7], float values[], int size, int ac, float expected[7])
{
	float* data[7];
	int sizes[7];
	for (int i = 0; i < 7; ++i)
	{
		data[i] = (float*)malloc(sizeof(float) * 16);
		sizes[i] = 16;
	}

	// Execute command
	int result = sendCTRL(*sock, values, size, ac);
	int d = 0.0f;
	if (result >= 0)
	{
		result = getDREFs(*sock, drefs, data, 7, sizes);
	}

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

int doGETCTest(float values[7], int ac, float expected[7])
{
	// Execute Test
	float actual[7];
	XPCSocket sock = openUDP(IP);
	int result = sendCTRL(sock, values, 7, ac);
	if (result >= 0)
	{
		result = getCTRL(sock, actual, ac);
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

int basicCTRLTest(const char** drefs, int ac)
{
	// Set control surfaces to known state.
	float CTRL[6] = { 0.0F, 0.0F, 0.0F, 0.8F, 1.0F, 0.5F };
	float expected[7] = { 0.0F, 0.0F, 0.0F, NAN, NAN, NAN, NAN };
	XPCSocket sock = openUDP(IP);
	pauseSim(sock, 1); // Pause so the controls wont change between 2 and 3
	int result = doCTRLTest(&sock, drefs, CTRL, 3, ac, expected);
	if (result < 0)
	{
		return -10000 + result;
	}
	crossPlatformUSleep(SLEEP_AMOUNT);

	// Test control surfaces and set other values to known state.
	expected[0] = CTRL[0] = 0.2F;
	expected[1] = CTRL[1] = 0.1F;
	expected[2] = CTRL[2] = 0.1F;
	expected[3] = 0.8F;
	expected[4] = 1.0F;
	expected[5] = 0.5F;
	result = doCTRLTest(&sock, drefs, CTRL, 6, ac, expected);
	if (result < 0)
	{
		return -20000 + result;
	}
	crossPlatformUSleep(SLEEP_AMOUNT);

	// Test other values and verify control surfaces unchanged.
	expected[0] = CTRL[0] = 0.15F;
	expected[1] = CTRL[1] = 0.15F;
	expected[2] = CTRL[2] = 0.15F;
	expected[3] = CTRL[3] = 0.9F;
	CTRL[4] = -998;
	CTRL[5] = -998;
	result = doCTRLTest(&sock, drefs, CTRL, 6, ac, expected);

	pauseSim(sock, 0);
	closeUDP(sock);
	if (result < 0)
	{
        if (result == -1004) {
            printf("GEAR FAILURE... ARE YOU USING AN AIRCRAFT WITH FIXED GEARS?   ");
        }
		return -30000 + result;
	}
	return 0;
}

int testCTRL_Player()
{
	const char* drefs[] =
	{
		"sim/cockpit2/controls/yoke_pitch_ratio",
		"sim/cockpit2/controls/yoke_roll_ratio",
		"sim/cockpit2/controls/yoke_heading_ratio",
		"sim/flightmodel/engine/ENGN_thro",
		"sim/cockpit/switches/gear_handle_status",
		"sim/flightmodel/controls/flaprqst",
		"sim/flightmodel/controls/sbrkrqst"
	};
	return basicCTRLTest(drefs, 0);
}

int testCTRL_NonPlayer()
{
	const char* drefs[] =
	{
		"sim/multiplayer/position/plane1_yolk_pitch",
		"sim/multiplayer/position/plane1_yolk_roll",
		"sim/multiplayer/position/plane1_yolk_yaw",
		"sim/multiplayer/position/plane1_throttle",
		"sim/multiplayer/position/plane1_gear_deploy",
		"sim/multiplayer/position/plane1_flap_ratio",
		"sim/multiplayer/position/plane1_speedbrake_ratio"
	};
	return basicCTRLTest(drefs, 1);
}

int testCTRL_Speedbrakes()
{
	const char* drefs[] =
	{
		"sim/cockpit2/controls/yoke_pitch_ratio",
		"sim/cockpit2/controls/yoke_roll_ratio",
		"sim/cockpit2/controls/yoke_heading_ratio",
		"sim/flightmodel/engine/ENGN_thro",
		"sim/cockpit/switches/gear_handle_status",
		"sim/flightmodel/controls/flaprqst",
		"sim/flightmodel/controls/sbrkrqst"
	};

	// Arm
	float CTRL[7] = { -998, -998, -998, -998, -998, -998, -0.5F };
	float expected[7] = { NAN, NAN, NAN, NAN, NAN, NAN, -0.5F };
	XPCSocket sock = openUDP(IP);
	pauseSim(sock, 1); // Pause so the controls wont change between 2 and 3
	int result = doCTRLTest(&sock, drefs, CTRL, 7, 0, expected);
	if (result < 0)
	{
		return -10000 + result;
	}

	// Set to full
	expected[6] = CTRL[6] = 1.5F;
	result = doCTRLTest(&sock, drefs, CTRL, 7, 0, expected);
	if (result < 0)
	{
		return -20000 + result;
	}

	// Retract
	expected[6] = CTRL[6] = 0.0F;
	result = doCTRLTest(&sock, drefs, CTRL, 7, 0, expected);
	pauseSim(sock, 0);
	closeUDP(sock);
	if (result < 0)
	{
		return -30000 + result;
	}
	return 0;
}

int testGETC()
{
	float CTRL[7] = { 0.0F, 0.0F, 0.0F, 0.8F, 1.0F, 0.5F, -1.5F };
	return doGETCTest(CTRL, 0, CTRL);
}

int testGETC_NonPlayer()
{
	float CTRL[7] = { 0.0F, 0.0F, 0.0F, 0.8F, 1.0F, 0.5F, -1.5F };
	return doGETCTest(CTRL, 2, CTRL);
}
#endif
