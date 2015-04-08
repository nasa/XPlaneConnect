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

void runTest(short (*f)())
{
    short result;
    
    printf("Test %i: ",testPassed+testFailed+1);
    result = (*f)(); // Run Test
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

short openTest() // openUDP Test
{
    printf("openUDP - ");
    struct xpcSocket sendPort = openUDP( 49062, "127.0.0.1", 49009 );
    return 0;
}

short closeTest() // closeUDP test
{
    printf("closeUDP - ");
    struct xpcSocket sendPort = openUDP( 49063, "127.0.0.1", 49009 );
    closeUDP(sendPort);
    sendPort = openUDP(49063, "127.0.0.1", 49009);
    closeUDP(sendPort);
    return 0;
}

short sendReadTest() // send/read Test
{
    printf("send/readUDP - ");
    
    // Initialization
    int i; // Iterator
    char test[] = {0, 1, 2, 3, 5};
    char buf[5] = {0};
    struct xpcSocket sendPort, recvPort;
    
    // Setup
    sendPort = openUDP( 49064, "127.0.0.1", 49063 );
    recvPort = openUDP( 49063, "127.0.0.1", 49009 );
    
    // Execution
    sendUDP( sendPort, test, sizeof(test) );
    readUDP( recvPort, buf, NULL ); // Test
    
    // Close
    closeUDP(sendPort);
    closeUDP(recvPort);
    
    // Tests
    for (i=0; i<4; i++)
    {
        if (test[i] != buf[i]) // Not received correctly
        {
            return -1;
        }
    }
    
    return 0;
}

short sendTEXTTest()
{
	printf("sendTEXT - ");

	// Setup
	struct xpcSocket sendPort = openUDP(49064, "127.0.0.1", 49009);
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

short requestDREFTest() // Request DREF Test (Required for next tests)
{
    printf("requestDREF - ");
    
    // Initialization
    int i; // Iterator
    char DREFArray[100][100];
    float *recDATA[100];
    short DREFSizes[100];
    struct xpcSocket sendPort, recvPort;
    short result = 0;
    
    // Setup
    for (i = 0; i < 100; i++) {
        recDATA[i]   = (float *) malloc(40*sizeof(float));
        memset(DREFArray[i],0,100);
    }
    sendPort = openUDP( 49064, "127.0.0.1", 49009 );
    recvPort = openUDP( 49008, "127.0.0.1", 49009 );
    strcpy(DREFArray[0],"sim/cockpit/switches/gear_handle_status");
    strcpy(DREFArray[1],"sim/cockpit2/switches/panel_brightness_ratio");
    for (i=0;i<2;i++) {
        DREFSizes[i] = (int) strlen(DREFArray[i]);
    }
    
    // Execution
    result = requestDREF(sendPort, recvPort, DREFArray, DREFSizes, 2, recDATA, DREFSizes);
    
    // Close
    closeUDP(sendPort);
    closeUDP(recvPort);
    
    // Tests
    if ( result < 0)// Request 2 values
    {
        return -1;
    }
    if (DREFSizes[0] != 1 || DREFSizes[1] != 4)
    {
        return -2;
    }
    return 0;
}

short sendDREFTest() // sendDREF test
{
    printf("sendDREF - ");
    
    // Initialization
    int i; // Iterator
    char DREFArray[100][100];
    float *recDATA[100];
    short DREFSizes[100];
    float value = 0.0;
    struct xpcSocket sendPort, recvPort;
    short result = 0;
    
    // Setup
    for (i = 0; i < 100; i++) {
        recDATA[i]   = (float *) malloc(40*sizeof(float));
        memset(DREFArray[i],0,100);
    }
    sendPort = openUDP( 49066, "127.0.0.1", 49009 );
    recvPort = openUDP( 49008, "127.0.0.1", 49009 );
    strcpy(DREFArray[0],"sim/cockpit/switches/gear_handle_status");
    for (i=0;i<1;i++) {
        DREFSizes[i] = (int) strlen(DREFArray[i]);
    }
    
    // Execution
    sendDREF(sendPort, DREFArray[0], DREFSizes[0], &value, 1);
    result = requestDREF(sendPort, recvPort, DREFArray, DREFSizes, 1, recDATA, DREFSizes);
    
    // Close
    closeUDP(sendPort);
    closeUDP(recvPort);
    
    // Tests
    if (result < 0)// Request 1 value
    {
        return -1;
    }
    if (DREFSizes[0] != 1)
    {
        return -2;
    }
    if (*recDATA[0] != value)
    {
        return -3;
    }
    return 0;
}

short sendDATATest() // sendDATA test
{
    printf("sendData - ");
    
    // Initialize
    int i,j; // Iterator
    char DREFArray[100][100];
    float data[4][9] = {0};
    float *recDATA[100];
    short DREFSizes[100];
    struct xpcSocket sendPort, recvPort;
    short result = 0;
    
    // Setup
    for (i = 0; i < 100; i++) {
        recDATA[i]   = (float *) malloc(40*sizeof(float));
        memset(DREFArray[i],0,100);
    }
    sendPort = openUDP( 49066, "127.0.0.1", 49009 );
    recvPort = openUDP( 49008, "127.0.0.1", 49009 );
    strcpy(DREFArray[0],"sim/aircraft/parts/acf_gear_deploy");
    for (i=0;i<1;i++) {
        DREFSizes[i] = (int) strlen(DREFArray[i]);
    }
    for (i=0;i<4;i++) { // Set array to -999
        for (j=0;j<9;j++) data[i][j] = -999;
    }
    data[0][0] = 14; // Gear
    data[0][1] = 1;
    data[0][2] = 0;
    
    // Execution
    sendDATA(sendPort, data, 1); // Gear
    result = requestDREF(sendPort, recvPort, DREFArray, DREFSizes, 1, recDATA, DREFSizes); // Test
    
    // Close
    closeUDP(sendPort);
    closeUDP(recvPort);
    
    // Tests
    if ( result < 0 )// Request 1 value
    {
        return -1;
    }
    if (DREFSizes[0] != 10)
    {
        return -2;
    }
    if (*recDATA[0] != data[0][1])
    {
        return -3;
    }
    return 0;
}

short sendCTRLTest() // sendCTRL test
{
	printf("sendCTRL - ");

	// Initialize
	int i; // Iterator
	char DREFArray[100][100];
	float CTRL[6] = { 0.0 };
	float *recDATA[100];
	short DREFSizes[100];
	struct xpcSocket sendPort, recvPort;
	short result;

	// Setup
	for (i = 0; i < 100; i++) {
		recDATA[i] = (float *)malloc(40 * sizeof(float));
		memset(DREFArray[i], 0, 100);
	}
	sendPort = openUDP(49066, "127.0.0.1", 49009);
	recvPort = openUDP(49008, "127.0.0.1", 49009);
	strcpy(DREFArray[0], "sim/cockpit2/controls/yoke_pitch_ratio");
	strcpy(DREFArray[1], "sim/cockpit2/controls/yoke_roll_ratio");
	strcpy(DREFArray[2], "sim/cockpit2/controls/yoke_heading_ratio");
	strcpy(DREFArray[3], "sim/flightmodel/engine/ENGN_thro");
	strcpy(DREFArray[4], "sim/cockpit/switches/gear_handle_status");
	strcpy(DREFArray[5], "sim/flightmodel/controls/flaprqst");
	for (i = 0; i < 100; i++) {
		DREFSizes[i] = (int)strlen(DREFArray[i]);
	}
	CTRL[3] = 0.8; // Throttle
	CTRL[4] = 1; // Gear
	CTRL[5] = 0.5; // Flaps

	// Execute
	sendCTRL(sendPort, 6, CTRL);
	result = requestDREF(sendPort, recvPort, DREFArray, DREFSizes, 6, recDATA, DREFSizes); // Test

	// Close
	closeUDP(sendPort);
	closeUDP(recvPort);

	// Tests
	if (result < 0)// Request 1 value
	{
		return -6;
	}
	for (i = 0; i<6; i++)
	{
		if (fabs(recDATA[i][0] - CTRL[i])>1e-4)
		{
            return -i - 1;
		}
	}

	return 0;
}

short sendpCTRLTest()
{
	printf("sendNonPlayerCTRL - ");

	// Initialize
	int i; // Iterator
	char DREFArray[100][100];
	float CTRL[6] = { 0.0 };
	float *recDATA[100];
	short DREFSizes[100];
	struct xpcSocket sendPort, recvPort;
	short result;

	// Setup
	for (i = 0; i < 100; i++)
	{
		recDATA[i] = (float *)malloc(40 * sizeof(float));
		memset(DREFArray[i], 0, 100);
	}
	sendPort = openUDP(49066, "127.0.0.1", 49009);
	recvPort = openUDP(49008, "127.0.0.1", 49009);
	strcpy(DREFArray[0], "sim/multiplayer/position/plane1_yolk_pitch");
	strcpy(DREFArray[1], "sim/multiplayer/position/plane1_yolk_roll");
	strcpy(DREFArray[2], "sim/multiplayer/position/plane1_yolk_yaw");
	strcpy(DREFArray[3], "sim/multiplayer/position/plane1_throttle");
	strcpy(DREFArray[4], "sim/multiplayer/position/plane1_gear_deploy");
	strcpy(DREFArray[5], "sim/multiplayer/position/plane1_flap_ratio");
	for (i = 0; i < 100; i++)
	{
		DREFSizes[i] = (int)strlen(DREFArray[i]);
	}
	CTRL[3] = 0.8; // Throttle
	CTRL[4] = 1; // Gear
	CTRL[5] = 0.5; // Flaps

	// Execute
	sendpCTRL(sendPort, 6, CTRL, 1);
	result = requestDREF(sendPort, recvPort, DREFArray, DREFSizes, 9, recDATA, DREFSizes); // Test

	// Close
	closeUDP(sendPort);
	closeUDP(recvPort);

	// Tests
	if (result < 0)// Request 1 value
	{
		return -6;
	}
	for (i = 0; i<6; i++)
	{
		if (fabs(recDATA[i][0] - CTRL[i])>1e-4)
		{
			return -i - 1;
		}
	}

	return 0;
}

short sendPOSITest() // sendPOSI test
{
    printf("sendPOSI - ");
    
    // Initialization
    int i; // Iterator
    char DREFArray[100][100];
    float POSI[8] = {0.0};
    float *recDATA[100];
    short DREFSizes[100];
    struct xpcSocket sendPort, recvPort;
    short result;
    
    // Setup
    for (i = 0; i < 100; i++) {
        recDATA[i]   = (float *) malloc(40*sizeof(float));
        memset(DREFArray[i],0,100);
    }
    sendPort = openUDP( 49063, "127.0.0.1", 49009 );
    recvPort = openUDP( 49008, "127.0.0.1", 49009 );
    strcpy(DREFArray[0],"sim/flightmodel/position/latitude");
    strcpy(DREFArray[1],"sim/flightmodel/position/longitude");
    strcpy(DREFArray[2],"sim/flightmodel/position/y_agl");
    strcpy(DREFArray[3],"sim/flightmodel/position/phi");
    strcpy(DREFArray[4],"sim/flightmodel/position/theta");
    strcpy(DREFArray[5],"sim/flightmodel/position/psi");
    strcpy(DREFArray[6],"sim/cockpit/switches/gear_handle_status");
    for (i=0;i<7;i++) {
        DREFSizes[i] = (int) strlen(DREFArray[i]);
    }
    POSI[0] = 37.524; // Lat
    POSI[1] = -122.06899; // Lon
    POSI[2] = 2500; // Alt
    POSI[3] = 0; // Pitch
    POSI[4] = 0; // Roll
    POSI[5] = 0; // Heading
    POSI[6] = 1; // Gear
    
    // Execution
    sendPOSI( sendPort, 0, 7, POSI );
    result = requestDREF(sendPort, recvPort, DREFArray, DREFSizes, 7, recDATA, DREFSizes); // Test
    
    // Close
    closeUDP(sendPort);
    closeUDP(recvPort);
    
    // Tests
    if ( result < 0 )// Request 1 value
    {
        return -7;
    }
    for (i=0;i<7-1;i++)
    {
        if (i==2)
        {
            continue;
        }
        if (fabs(recDATA[i][0]-POSI[i])>1e-4)
        {
            return -i;
        }
    }

    
    return 0;
}

short pauseTest() // pauseSim test
{
    printf("pauseSim - ");
    
    // Initialize
    int i; // Iterator
    char DREFArray[100][100];
    float *recDATA[100];
    short DREFSizes[100],RECSizes[100];
    struct xpcSocket sendPort, recvPort;
    short result;
    
    // Setup
    for (i = 0; i < 100; i++) {
        recDATA[i]   = (float *) malloc(40*sizeof(float));
        memset(DREFArray[i],0,100);
    }
    
    // Setup
    sendPort = openUDP( 49064, "127.0.0.1", 49009 );
    recvPort = openUDP( 49008, "127.0.0.1", 49009 );
    strcpy(DREFArray[0],"sim/operation/override/override_planepath");
    for (i=0;i<1;i++) {
        DREFSizes[i] = (int) strlen(DREFArray[i]);
    }
    
    // Execute
    pauseSim(sendPort, 1);
    result = requestDREF(sendPort, recvPort, DREFArray, DREFSizes, 1, recDATA, RECSizes); // Test
    
    // Close
    closeUDP(sendPort);
    closeUDP(recvPort);
    
    // Test
    if (result < 0)     {
        return -1;
    }
    if (recDATA[0][0] != 1)
    {
        return -2;
    }
    
    // Reopen
    sendPort = openUDP( 49064, "127.0.0.1", 49009 );
    recvPort = openUDP( 49008, "127.0.0.1", 49009 );
    
    // Execute 2
    pauseSim(sendPort, 0);
    result = requestDREF(sendPort, recvPort, DREFArray, DREFSizes, 1, recDATA, RECSizes); // Test
    
    // Close 2
    closeUDP(sendPort);
    closeUDP(recvPort);
    
    // Test 2
    if (result < 0)
    {
        return -3;
    }
    if (recDATA[0][0] != 0)
    {
        return -4;
    }
    
    return 0;
}

short connTest() // setConn test
{
    printf("setConn - ");
    
    // Initialize
    int i; // Iterator
    char DREFArray[100][100];
    float *recDATA[100];
    short DREFSizes[100];
    struct xpcSocket sendPort, recvPort;
    short result = 0;
#if (__APPLE__ || __linux)
    usleep(0);
#endif
    
    // Setup
    sendPort = openUDP( 49067, "127.0.0.1", 49009 );
    recvPort = openUDP( 49055, "127.0.0.1", 49009 );
    strcpy(DREFArray[0],"sim/cockpit/switches/gear_handle_status");
    for (i=0;i<1;i++) {
        DREFSizes[i] = (int) strlen(DREFArray[i]);
    }
    
    // Execution
    setCONN(sendPort, 49055);
    result = requestDREF(sendPort, recvPort, DREFArray, DREFSizes, 1, recDATA, DREFSizes); // Test
    
    // Close
    closeUDP(sendPort);
    closeUDP(recvPort);
    
    // Test
    if ( result < 0 )// No data received
    {
        return -1;
    }
    
    
    // Set up for next test
    sendPort = openUDP( 49067, "127.0.0.1", 49009 );
    setCONN(sendPort, 49008);
    closeUDP(sendPort);
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
    
    runTest(openTest);
    runTest(closeTest);
	runTest(sendReadTest);
	runTest(sendTEXTTest);
    runTest(requestDREFTest);
    runTest(sendDREFTest);
	runTest(sendDATATest);
	runTest(sendCTRLTest);
	runTest(sendpCTRLTest);
    runTest(sendPOSITest);
	runTest(pauseTest);
	runTest(connTest);
    
    printf( "----------------\nTest Summary\n\tFailed: %i\n\tPassed: %i\n", testFailed, testPassed );
    
    return 0;
}

