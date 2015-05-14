//
//  main.cpp
//  XPC Tests
//
//  Created by Chris Teubert on 11/25/14.
//  Copyright (c) 2014 Chris Teubert. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "xplaneConnect.h"

#define IP "127.0.0.1"

int testFailed = 0;
int testPassed = 0;

void runTest(int (*test)(), char* name)
{
    int result = test(); // Run Test
	printf("Test %i: %s - ", testPassed + testFailed + 1, name);
    if (result == 0)
    {
        printf("PASSED\n");
        testPassed++;
    }
    else
    {
        printf("FAILED\n\tError: %i\n", result);
        testFailed++;
    }
}

int openTest() // openUDP Test
{
    XPCSocket sock = openUDP("localhost");
	int result = strncmp(sock.xpIP, "127.0.0.1", 16);
	closeUDP(sock);
    return result;
}

int closeTest() // closeUDP test
{
    XPCSocket sendPort = aopenUDP(IP, 49009, 49063);
    closeUDP(sendPort);
    sendPort = aopenUDP(IP, 49009, 49063);
    closeUDP(sendPort);
    return 0;
}

int sendTEXTTest()
{
	// Setup
	XPCSocket sendPort = openUDP(IP);
	int x = 100;
	int y = 700;
	char* msg = "This is an X-Plane Connect test message.\nThis should be a new line.\r\nThat will be parsed as two line breaks.";

	// Test
	sendTEXT(sendPort, msg, x, y);
	// NOTE: Manually verify that msg appears on the screen in X-Plane!

	sendTEXT(sendPort, "Another test message", x, y);
	// NOTE: Manually verify that msg appears on the screen and that no part of the previous
	//       message is visible.

	sendTEXT(sendPort, NULL, -1, -1);

	// Cleanup
	closeUDP(sendPort);
	return 0;
}

int getDREFTest() // Request DREF Test (Required for next tests)
{
    // Initialization
	// Get one DREF of each type (int, float, int[], float[], double, byte[])
	#define GETD_COUNT 6
	char* drefs[GETD_COUNT] =
	{
		"sim/cockpit/switches/gear_handle_status", //int
		"sim/cockpit/autopilot/altitude", //float
		"sim/aircraft/prop/acf_prop_type", //int[8]
		"sim/cockpit2/switches/panel_brightness_ratio", //float[4]
		"sim/aircraft/view/acf_tailnum", //byte[40]
		"sim/flightmodel/position/elevation" //double
	};
	float* data[GETD_COUNT];
	int sizes[GETD_COUNT];
	XPCSocket sock = openUDP(IP);
    
    // Setup
	for (int i = 0; i < GETD_COUNT; ++i)
	{
		data[i] = (float*)malloc(256 * sizeof(float));
		sizes[i] = 256;
	}
    
    // Execution
	int result = getDREFs(sock, drefs, data, GETD_COUNT, sizes);
    
    // Close
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

int sendDREFTest() // sendDREF test
{
    // Initialization
	// Set one DREF of each type (int, float, int[], float[], double, byte[])
	// Also set one read-only to make sure it fails
	#define DREF_COUNT 6
	char* drefs[DREF_COUNT] =
	{
		"sim/cockpit/switches/gear_handle_status", //int
		"sim/cockpit/autopilot/altitude", //float
		"sim/aircraft/prop/acf_prop_type", //int[8]
		"sim/cockpit2/switches/panel_brightness_ratio", //float[4]
		"sim/aircraft/view/acf_tailnum", //byte[40]
		"sim/flightmodel/position/elevation" //double - Read only
	};
	float* data[DREF_COUNT];
	int sizes[DREF_COUNT];
	float* values[DREF_COUNT];
	XPCSocket sock = openUDP(IP);
	
	// Setup
	sizes[0] = 1;
	values[0] = (float*)malloc(sizes[0] * sizeof(float));
	values[0][0] = 1;

	sizes[1] = 1;
	values[1] = (float*)malloc(sizes[1] * sizeof(float));
	values[1][0] = 4000.0F;

	sizes[2] = 8;
	values[2] = (float*)malloc(sizes[2] * sizeof(float));
	for (int i = 0; i < 8; ++i)
	{
		values[2][i] = 0;
	}

	sizes[3] = 4;
	values[3] = (float*)malloc(sizes[3] * sizeof(float));
	for (int i = 0; i < 4; ++i)
	{
		values[3][i] = 0.25F;
	}

	sizes[4] = 40;
	values[4] = (float*)malloc(sizes[4] * sizeof(float));
	memset(values[4], 0, sizes[4] * sizeof(float));
	values[4][0] = 78.0F; //N
	values[4][1] = 55.0F; //7
	values[4][2] = 52.0F; //4
	values[4][3] = 56.0F; //8
	values[4][4] = 53.0F; //5
	values[4][5] = 89.0F; //Y

	sizes[5] = 1;
	values[5] = (float*)malloc(sizes[5] * sizeof(float));
	values[5][0] = 5000.0F;

    // Execution
	for (int i = 0; i < DREF_COUNT; ++i)
	{
		sendDREF(sock, drefs[i], values[i], sizes[i]);
		data[i] = (float*)malloc(256 * sizeof(float));
		sizes[i] = 256;
	}
	int result = getDREFs(sock, drefs, data, DREF_COUNT, sizes);
    
    // Close
	closeUDP(sock);
    
    // Tests
    if (result < 0)
    {
        return -1;
    }
	// Verify gear handle was set
	if (sizes[0] != 1 || data[0][0] != 1)
	{
		return -2;
	}
	// Verify autopilot altitude was set
	if (sizes[1] != 1 || data[1][0] != 4000.0F)
	{
		return -3;
	}
	// Verify prop type was set
	if (sizes[2] != 8)
	{
		return -4;
	}
	for (int i = 0; i < 8; ++i)
	{
		if (data[2][i] != values[2][i])
		{
			return -4;
		}
	}
	// Verify panel brightness was set
	if (sizes[3] != 4)
	{
		return -5;
	}
	for (int i = 0; i < 4; ++i)
	{
		if (data[3][i] != values[3][i])
		{
			return -5;
		}
	}
	// Verify tail number was set
	for (int i = 0; i < 6; ++i)
	{
		if (data[4][i] != values[4][i])
		{
			return -6;
		}
	}
	// Verify aircraft elevation was NOT set
	if (sizes[5] != 1 || data[5][0] == 5000.0F)
	{
		return -7;
	}
    return 0;
}

int sendDATATest() // sendDATA test
{
    // Initialize
    int i,j; // Iterator
	char* drefs[100] = 
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
    if ( result < 0 )// Request 1 value
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

int psendCTRLTest() // sendCTRL test
{
	// Initialize
	char* drefs[100] =
	{
		"sim/cockpit2/controls/yoke_pitch_ratio",
		"sim/cockpit2/controls/yoke_roll_ratio",
		"sim/cockpit2/controls/yoke_heading_ratio",
		"sim/flightmodel/engine/ENGN_thro",
		"sim/cockpit/switches/gear_handle_status",
		"sim/flightmodel/controls/flaprqst"
	};
	float* data[100];
	int sizes[100];
	float CTRL[6] = { 0.0F, 0.0F, 0.0F, 0.8F, 1.0F, 0.5F };
	XPCSocket sock = openUDP(IP);

	// Setup
	for (int i = 0; i < 100; i++)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
	}

	// Execute 1
	// 0 pitch, roll, yaw
	sendCTRL(sock, CTRL, 3, 0);
	int result = getDREFs(sock, drefs, data, 6, sizes);

	// Close socket
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -1;
	}
	for (int i = 0; i < 3; i++)
	{
		if (fabs(data[i][0] - CTRL[i]) > 1e-4)
		{
			return -i - 11;
		}
	}

	sock = openUDP(IP);
	// Execute 2
	// Set non-zero pitch, roll, & yaw. Also set throttle, gear, and flaps
	CTRL[0] = 0.2F;
	CTRL[1] = 0.1F;
	CTRL[2] = 0.1F;
	sendCTRL(sock, CTRL, 6, 0);
	result = getDREFs(sock, drefs, data, 6, sizes);

	// Close socket
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -2;
	}
	for (int i = 0; i < 6; i++)
	{
		if (fabs(data[i][0] - CTRL[i]) > 1e-4)
		{
			return -i - 21;
		}
	}

	sock = openUDP(IP);
	// Execute 2
	// Set non-zero pitch, roll, & yaw. Also set throttle, gear, and flaps
	CTRL[0] = -998.0F;
	CTRL[1] = -998.0F;
	CTRL[2] = -998.0F;
	sendCTRL(sock, CTRL, 6, 0);
	result = getDREFs(sock, drefs, data, 6, sizes);

	// Close socket
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -3;
	}
	for (int i = 0; i < 6; i++)
	{
		if (fabs(data[i][0] - CTRL[i]) > 1e-2)
		{
			return -i - 31;
		}
	}

	return 0;
}

int sendCTRLTest()
{
	// Initialize
	char* drefs[100] =
	{
		"sim/multiplayer/position/plane1_yolk_pitch",
		"sim/multiplayer/position/plane1_yolk_roll",
		"sim/multiplayer/position/plane1_yolk_yaw",
		"sim/multiplayer/position/plane1_throttle",
		"sim/multiplayer/position/plane1_gear_deploy",
		"sim/multiplayer/position/plane1_flap_ratio",
	};
	float* data[100];
	int sizes[100];
	float CTRL[6] = { 0.0F, 0.0F, 0.0F, 0.8F, 1.0F, 0.5F };
	XPCSocket sock = openUDP(IP);

	// Setup
	for (int i = 0; i < 100; i++)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
	}

	// Execute 1
	// 0 pitch, roll, yaw
	sendCTRL(sock, CTRL, 3, 1);
	int result = getDREFs(sock, drefs, data, 6, sizes);

	// Close socket
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -1;
	}
	for (int i = 0; i < 3; i++)
	{
		if (fabs(data[i][0] - CTRL[i]) > 1e-4)
		{
			return -i - 11;
		}
	}

	sock = openUDP(IP);
	// Execute 2
	// Set non-zero pitch, roll, & yaw. Also set throttle, gear, and flaps
	CTRL[0] = 0.2F;
	CTRL[1] = 0.1F;
	CTRL[2] = 0.1F;
	sendCTRL(sock, CTRL, 6, 1);
	result = getDREFs(sock, drefs, data, 6, sizes);

	// Close socket
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -2;
	}
	for (int i = 0; i < 6; i++)
	{
		if (fabs(data[i][0] - CTRL[i]) > 1e-4)
		{
			return -i - 21;
		}
	}

	sock = openUDP(IP);
	// Execute 2
	// Set non-zero pitch, roll, & yaw. Also set throttle, gear, and flaps
	CTRL[0] = -998.0F;
	CTRL[1] = -998.0F;
	CTRL[2] = -998.0F;
	sendCTRL(sock, CTRL, 6, 1);
	result = getDREFs(sock, drefs, data, 6, sizes);

	// Close socket
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -3;
	}
	for (int i = 0; i < 6; i++)
	{
		if (fabs(data[i][0] - CTRL[i]) > 1e-2)
		{
			return -i - 31;
		}
	}

	return 0;
}

int psendPOSITest() // sendPOSI test
{
	// Initialization
	int i; // Iterator
	char* drefs[100] =
	{
		"sim/flightmodel/position/latitude",
		"sim/flightmodel/position/longitude",
		"sim/flightmodel/position/elevation",
		"sim/flightmodel/position/theta",
		"sim/flightmodel/position/phi",
		"sim/flightmodel/position/psi",
		"sim/cockpit/switches/gear_handle_status"
	};
	float* data[100];
	int sizes[100];
	float POSI[7] = { 37.524F, -122.06899F, 2500, 0, 0, 0, 1 };
	XPCSocket sock = openUDP(IP);

	// Setup
	for (i = 0; i < 100; i++)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
	}

	// Execution 1
	pauseSim(sock, 1);
	sendPOSI(sock, POSI, 7, 0);
	int result = getDREFs(sock, drefs, data, 7, sizes);
	pauseSim(sock, 0);

	// Close
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -1;
	}
	for (i = 0; i < 7; ++i)
	{
		if (fabs(data[i][0] - POSI[i]) > 1e-4)
		{
			return -i - 11;
		}
	}

	// Setup 2
	sock = openUDP(IP);
	POSI[0] = -998.0F;
	POSI[1] = -998.0F;
	POSI[2] = -998.0F;
	POSI[3] = 5.0F;
	POSI[4] = -5.0F;
	POSI[5] = 10.0F;
	POSI[6] = 0;

	// Execution 2
	pauseSim(sock, 1);
	float *loc[3];
    for(int i = 0; i < 3; ++i)
    {
        loc[i] = (float*)malloc(sizeof(float));
    }
	getDREFs(sock, drefs, &loc, 3, sizes);
	sendPOSI(sock, POSI, 7, 0);
	result = getDREFs(sock, drefs, data, 7, sizes);
	pauseSim(sock, 0);

	// Close
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -2;
	}
	// Compare position to make sure they weren't set
	for (int i = 0; i < 3; ++i)
	{
		// Note: Because the sim was paused when both of these were read, we really do expect *exactly*
		//       the same value even though we are comparing floats.
		if (data[i][0] != loc[i][0])
		{
			return -i - 21;
		}
	}
	// Compare everything else.
	for (i = 3; i < 7; ++i)
	{
		if (fabs(data[i][0] - POSI[i]) > 1e-4)
		{
			return -i - 21;
		}
	}

	// Setup 3
	sock = openUDP(IP);
	POSI[0] = 37.524F;
	POSI[1] = -122.06899F;
	POSI[2] = 20000;
	POSI[3] = 15.0F;
	POSI[4] = -25.0F;
	POSI[5] = -10.0F;
	POSI[6] = 1;

	// Execution 2
	pauseSim(sock, 1);
	sendPOSI(sock, POSI, 3, 0);
	result = getDREFs(sock, drefs, data, 7, sizes);
	pauseSim(sock, 0);

	// Close
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -3;
	}
	// Compare position to make sure it was set.
	for (int i = 0; i < 3; ++i)
	{
		if (fabs(data[i][0] - POSI[i]) > 1e-4)
		{
			return -i - 31;
		}
	}
	// Compare everything else to make sure it *wasn't*.
	for (i = 3; i < 7; ++i)
	{
		if (fabs(data[i][0] - POSI[i]) < 1)
		{
			return -i - 31;
		}
	}

	return 0;
}

int sendPOSITest() // sendPOSI test
{
	// Initialization
	int i; // Iterator
	char* drefs[100] =
	{
		// TODO: Can't get global position for multiplayer a/c?
		"sim/multiplayer/position/plane1_lat",
		"sim/multiplayer/position/plane1_lon",
		"sim/multiplayer/position/plane1_el",
		"sim/multiplayer/position/plane1_the",
		"sim/multiplayer/position/plane1_phi",
		"sim/multiplayer/position/plane1_psi",
		"sim/multiplayer/position/plane1_gear_deploy"
	};
	float* data[100];
	int sizes[100];
	float POSI[7] = { 37.524F, -122.06899F, 2500, 0, 0, 0, 1 };
	XPCSocket sock = openUDP(IP);

	// Setup
	for (i = 0; i < 100; i++)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
	}

	// Execution 1
	pauseSim(sock, 1);
	sendPOSI(sock, POSI, 7, 1);
	int result = getDREFs(sock, drefs, data, 7, sizes);
	pauseSim(sock, 0);

	// Close
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -1;
	}
	for (i = 0; i < 7; ++i)
	{
		if (fabs(data[i][0] - POSI[i]) > 1e-4)
		{
			return -i - 11;
		}
	}

	// Setup 2
	sock = openUDP(IP);
	POSI[0] = -998.0F;
	POSI[1] = -998.0F;
	POSI[2] = -998.0F;
	POSI[3] = 5.0F;
	POSI[4] = -5.0F;
	POSI[5] = 10.0F;
	POSI[6] = 0;

	// Execution 2
	pauseSim(sock, 1);
	float* loc[3];
    for(int i = 0; i < 3; ++i)
    {
        loc[i] = (float*)malloc(sizeof(float));
    }
	getDREFs(sock, drefs, loc, 3, sizes);
	sendPOSI(sock, POSI, 7, 1);
	result = getDREFs(sock, drefs, data, 7, sizes);
	pauseSim(sock, 0);

	// Close
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -2;
	}
	// Compare position to make sure they weren't set
	for (int i = 0; i < 3; ++i)
	{
		// Note: Because the sim was paused when both of these were read, we really do expect *exactly*
		//       the same value even though we are comparing floats.
		if (data[i][0] != loc[i][0])
		{
			return -i - 21;
		}
	}
	// Compare everything else.
	for (i = 3; i < 7; ++i)
	{
		if (fabs(data[i][0] - POSI[i]) > 1e-4)
		{
			return -i - 21;
		}
	}

	// Setup 3
	sock = openUDP(IP);
	POSI[0] = 37.524F;
	POSI[1] = -122.06899;
	POSI[2] = 20000;
	POSI[3] = 15.0F;
	POSI[4] = -25.0F;
	POSI[5] = -10.0F;
	POSI[6] = 1;

	// Execution 2
	pauseSim(sock, 1);
	sendPOSI(sock, POSI, 3, 1);
	result = getDREFs(sock, drefs, data, 7, sizes);
	pauseSim(sock, 0);

	// Close
	closeUDP(sock);

	// Tests
	if (result < 0)
	{
		return -3;
	}
	// Compare position to make sure it was set.
	for (int i = 0; i < 3; ++i)
	{
		if (fabs(data[i][0] - POSI[i]) > 1e-4)
		{
			return -i - 31;
		}
	}
	// Compare everything else to make sure it *wasn't*.
	for (i = 3; i < 7; ++i)
	{
		if (fabs(data[i][0] - POSI[i]) < 1)
		{
			return -i - 31;
		}
	}

	return 0;
}

int sendWYPTTest()
{
	// Setup
	XPCSocket sock = openUDP(IP);
	float points[] =
	{
		37.5245F, -122.06899F, 2500,
		37.455397F, -122.050037F, 2500,
		37.469567F, -122.051411F, 2500,
		37.479376F, -122.060509F, 2300,
		37.482237F, -122.076130F, 2100,
		37.474881F, -122.087288F, 1900,
		37.467660F, -122.079391F, 1700,
		37.466298F, -122.090549F, 1500,
		37.362562F, -122.039223F, 1000,
		37.361448F, -122.034416F, 1000,
		37.361994F, -122.026348F, 1000,
		37.365541F, -122.022572F, 1000,
		37.373727F, -122.024803F, 1000,
		37.403869F, -122.041283F, 50,
		37.418544F, -122.049222F, 6
	};

	// Test
	sendWYPT(sock, XPC_WYPT_CLR, NULL, 0);
	sendWYPT(sock, XPC_WYPT_ADD, points, 15);
	// NOTE: Visually ensure waypoints are added in the sim

	// Cleanup
	closeUDP(sock);
	return 0;
}

int pauseTest() // pauseSim test
{
    // Initialize
	// Note: Always run this test to the end so that the sim ends up unpaused in the
	//       case where commands are working but reading results isn't.
	int result = 0; 
	char* drefs[100] =
	{
		"sim/operation/override/override_planepath"
	};
    float* data[100];
	int sizes[100];
	XPCSocket sock = openUDP(IP);
    
    // Setup
    for (int i = 0; i < 100; i++)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
    }
    
    // Execute
    pauseSim(sock, 1);
	result = getDREF(sock, drefs[0], data[0], sizes);

    // Test
    if (result < 0)
	{
        result = -1;
    }
	if (data[0][0] != 1)
    {
        result = -2;
    }

	if (result == 0)
	{
		// Execute 2
		pauseSim(sock, 0);
		result = getDREF(sock, drefs[0], data[0], sizes);

		// Test 2
		if (result < 0)
		{
			result = -3;
		}
		if (data[0][0] != 0)
		{
			result = -4;
		}
	}

	// Close
	closeUDP(sock);
    
    return result;
}

int connTest() // setConn test
{
    // Initialize
	char* drefs[100] =
	{
		"sim/cockpit/switches/gear_handle_status"
	};
    float* data[100];
	int sizes[100];
	XPCSocket sock = openUDP(IP);
#if (__APPLE__ || __linux)
    usleep(0);
#endif
    
    // Setup
	for (int i = 0; i < 100; ++i)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
	}
    
    // Execution
    setCONN(&sock, 49055);
	int result = getDREF(sock, drefs[0], data[0], sizes);
    
    // Close
    closeUDP(sock);
    
    // Test
    if ( result < 0 )// No data received
    {
        return -1;
    }
    return 0;
}

int main(int argc, const char * argv[])
{
    printf("XPC Tests-c ");
    
#ifdef _WIN32
    printf("(Windows)\n");
#elif (__APPLE__)
    printf("(Mac) \n");
#elif (__linux)
    printf("(Linux) \n");
#endif
    
	runTest(openTest, "open");
	runTest(closeTest, "close");
	runTest(pauseTest, "SIMU");
	runTest(getDREFTest, "GETD");
	runTest(sendDREFTest, "DREF");
	runTest(sendDATATest, "DATA");
	runTest(sendCTRLTest, "CTRL");
	runTest(psendCTRLTest, "CTRL (player)");
	runTest(sendPOSITest, "POSI");
	runTest(psendPOSITest, "POSI (player)");
	runTest(sendWYPTTest, "WYPT");
	runTest(sendTEXTTest, "TEXT");
	runTest(connTest, "CONN");
    
    printf( "----------------\nTest Summary\n\tFailed: %i\n\tPassed: %i\n", testFailed, testPassed );
    
    return 0;
}

