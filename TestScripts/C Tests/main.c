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
#include "xplaneconnect.h"

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
    XPCSocket sock = openUDP("127.0.0.1", 49009, 49062);
	closeUDP(sock);
    return 0;
}

int closeTest() // closeUDP test
{
    XPCSocket sendPort = openUDP("127.0.0.1", 49009, 49063);
    closeUDP(sendPort);
    sendPort = openUDP("127.0.0.1", 49009, 49063);
    closeUDP(sendPort);
    return 0;
}

int sendReadTest() // send/read Test
{
    // Initialization
    char test[] = {0, 1, 2, 3, 5};
    char buf[5] = {0};
	XPCSocket outSock = openUDP("127.0.0.1", 49063, 49064);
	XPCSocket inSock = openUDP("127.0.0.1", 49009, 49063);
    
    // Execution
	sendUDP(outSock, test, sizeof(test));
	readUDP(inSock, buf, sizeof(buf), NULL);
    
    // Close
	closeUDP(outSock);
	closeUDP(inSock);
    
    // Tests
	for (int i = 0; i < sizeof(buf); i++)
	{
		if (test[i] != buf[i]) // Not received correctly
		{
			return -1;
		}
	}
    
    return 0;
}

int sendTEXTTest()
{
	// Setup
	XPCSocket sendPort = openUDP("127.0.0.1", 49009, 49064);
	int x = 100;
	int y = 700;
	char* msg = "XPlaneConnect test message. Now with 100% fewer new lines!";

	// Test
	sendTEXT(sendPort, msg, x, y);
	// NOTE: Manually verify that msg appears on the screen in X-Plane!

	// Cleanup
	closeUDP(sendPort);
	return 0;
}

int requestDREFTest() // Request DREF Test (Required for next tests)
{
    // Initialization
	char* drefs[100] =
	{
		"sim/cockpit/switches/gear_handle_status",
		"sim/cockpit2/switches/panel_brightness_ratio"
	};
	float* data[100];
	int sizes[100];
	XPCSocket sock = openUDP("127.0.0.1", 49009, 49064);
    
    // Setup
	for (int i = 0; i < 100; ++i)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
	}
    
    // Execution
    int result = getDREFs(sock, drefs, data, 2, sizes);
    
    // Close
	closeUDP(sock);
    
    // Tests
    if ( result < 0)// Request 2 values
    {
        return -1;
    }
	if (sizes[0] != 1 || sizes[1] != 4)
    {
        return -2;
    }
    return 0;
}

int sendDREFTest() // sendDREF test
{
    // Initialization
	char* drefs[100] =
	{
		"sim/cockpit/switches/gear_handle_status"
	};
	float* data[100];
	int sizes[100];
	XPCSocket sock = openUDP("127.0.0.1", 49009, 49066);
	float value = 1.0F;
	
	// Setup
	for (int i = 0; i < 100; ++i)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
	}

    // Execution
	sendDREF(sock, drefs[0], &value, 1);
	int result = getDREFs(sock, drefs, data, 1, sizes);
    
    // Close
	closeUDP(sock);
    
    // Tests
    if (result < 0)// Request 1 value
    {
        return -1;
    }
	if (sizes[0] != 1)
    {
        return -2;
    }
	if (data[0][0] != value)
    {
        return -3;
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
	XPCSocket sock = openUDP("127.0.0.1", 49009, 49066);

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
	XPCSocket sock = openUDP("127.0.0.1", 49009, 49066);

	// Setup
	for (int i = 0; i < 100; i++)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
	}

	// Execute
	psendCTRL(sock, CTRL, 6);
	int result = getDREFs(sock, drefs, data, 6, sizes);

	// Close socket
	closeUDP(sock);

	// Tests
	if (result < 0)// Request 1 value
	{
		return -6;
	}
	for (int i = 0; i < 6; i++)
	{
		if (fabs(data[i][0] - CTRL[i]) > 1e-4)
		{
			return -i - 1;
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
	XPCSocket sock = openUDP("127.0.0.1", 49009, 49066);

	// Setup
	for (int i = 0; i < 100; i++)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
	}

	// Execute
	sendCTRL(sock, CTRL, 6, 1);
	int result = getDREFs(sock, drefs, data, 6, sizes);

	// Close socket
	closeUDP(sock);

	// Tests
	if (result < 0)// Request 1 value
	{
		return -6;
	}
	for (int i = 0; i < 6; i++)
	{
		if (fabs(data[i][0] - CTRL[i]) > 1e-4)
		{
			return -i - 1;
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
		"sim/flightmodel/position/latitude",
		"sim/flightmodel/position/longitude",
		"sim/flightmodel/position/y_agl",
		"sim/flightmodel/position/phi",
		"sim/flightmodel/position/theta",
		"sim/flightmodel/position/psi",
		"sim/cockpit/switches/gear_handle_status"
	};
	float* data[100];
	int sizes[100];
	float POSI[8] = { 37.524F, -122.06899F, 2500, 0, 0, 0, 1 };
	XPCSocket sock = openUDP("127.0.0.1", 49009, 49063);
    
    // Setup
    for (i = 0; i < 100; i++)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
    }
    
    // Execution
	sendPOSI(sock, POSI, 7, 0);
    int result = getDREFs(sock, drefs, data, 7, sizes);
    
    // Close
	closeUDP(sock);
    
    // Tests
    if ( result < 0 )// Request 1 value
    {
        return -7;
    }
    for (i=0;i<7-1;i++)
    {
		if (i == 2)
		{
			continue;
		}
		if (fabs(data[i][0] - POSI[i]) > 1e-4)
		{
			return -i;
		}
    }

    
    return 0;
}

int sendWYPTTest()
{
	// Setup
	XPCSocket sock = openUDP("127.0.0.1", 49009, 49064);
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
	sendWYPT(sock, XPC_WYPT_ADD, points, 15);
	// NOTE: Visually ensure waypoints are added in the sim

	// Cleanup
	closeUDP(sock);
	return 0;
}

int pauseTest() // pauseSim test
{
    // Initialize
    int i; // Iterator
	char* drefs[100] =
	{
		"sim/operation/override/override_planepath"
	};
    float* data[100];
	int sizes[100];
	XPCSocket sock = openUDP("127.0.0.1", 49009, 49064);
    
    // Setup
    for (i = 0; i < 100; i++)
	{
		data[i] = (float*)malloc(40 * sizeof(float));
		sizes[i] = 40;
    }
    
    // Execute
    pauseSim(sock, 1);
	int result = getDREF(sock, drefs[0], data[0], sizes);
    
    // Close
    closeUDP(sock);
    
    // Test
    if (result < 0)
	{
        return -1;
    }
	if (data[0][0] != 1)
    {
        return -2;
    }
    
    // Reopen
	sock = openUDP("127.0.0.1", 49009, 49064);
    
    // Execute 2
	pauseSim(sock, 0);
	result = getDREF(sock, drefs[0], data[0], sizes);
    
    // Close 2
    closeUDP(sock);
    
    // Test 2
    if (result < 0)
    {
        return -3;
    }
    if (data[0][0] != 0)
    {
        return -4;
    }
    
    return 0;
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
	XPCSocket sock = openUDP("127.0.0.1", 49009, 49067);
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
	runTest(sendReadTest, "send/read");
	runTest(sendTEXTTest, "TEXT");
	runTest(requestDREFTest, "GETD");
	runTest(sendDREFTest, "DREF");
	runTest(sendDATATest, "DATA");
	runTest(sendCTRLTest, "CTRL");
	runTest(psendCTRLTest, "CTRL (player)");
	runTest(sendPOSITest, "POSI");
	runTest(sendWYPTTest, "WYPT");
	runTest(pauseTest, "SIMU");
	runTest(connTest, "CONN");
    
    printf( "----------------\nTest Summary\n\tFailed: %i\n\tPassed: %i\n", testFailed, testPassed );
    
    return 0;
}

