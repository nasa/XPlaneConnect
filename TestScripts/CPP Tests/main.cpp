//
//  main.cpp
//  XPC Tests
//
//  Created by Chris Teubert on 11/25/14.
//  Copyright (c) 2014 Chris Teubert. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <exception>
#include "xplaneconnect.h"

int testFailed = 0;
int testPassed = 0;

void runTest(void (*f)())
{
    try {
        std::cout << "Test " << testPassed+testFailed+1<<": ";
        (*f)(); // Run Test
        std::cout << "PASSED\n";
        testPassed++;
    }
    catch (int i)
    {
        std::cerr << "FAILED\n\tError: " << i << std::endl;
        testFailed++;
    }
    catch (char c)
    {
        std::cerr << "FAILED\n\tError: " << c << std::endl;
        testFailed++;
    }
    
}

void test1() // openUDP Test
{
    std::cout << "openUDP - ";
    struct xpcSocket sendPort = openUDP( 49062, "127.0.0.1", 49009 );
}

void test2() // closeUDP test
{
    std::cout << "closeUDP - ";
    struct xpcSocket sendPort = openUDP( 49063, "127.0.0.1", 49009 );
    closeUDP(sendPort);
    sendPort = openUDP( 49063, "127.0.0.1", 49009 );
}

void test3() // send/read Test
{
    std::cout << "send/readUDP - ";
    
    // Initialization
        int i; // Iterator
        char test[] = {0, 1, 2, 3};
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
                throw 1;
            }
        }
    
}

void test4() // Request DREF Test (Required for next tests)
{
    std::cout << "requestDREF - ";
    
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
        strcpy(DREFArray[1],"cockpit2/switches/panel_brightness_ratio");
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
            throw 1;
        }
        if (DREFSizes[0] != 1 || DREFSizes[1] != 4)
        {
            throw 2;
        }
}

void test5() // sendDREF test
{
    std::cout << "sendDREF - ";
    
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
            throw 1;
        }
        if (DREFSizes[0] != 1)
        {
            throw 2;
        }
        if (*recDATA[0] != value)
        {
            throw 3;
        }
}

void test6() // sendDATA test
{
    std::cout << "sendData - ";
    
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
            throw 1;
        }
        if (DREFSizes[0] != 10)
        {
            throw 2;
        }
        if (*recDATA[0] != data[0][1])
        {
            throw 3;
        }
}

void test7() // sendCTRL test
{
    std::cout << "sendCTRL - ";
    
    // Initialize
        int i; // Iterator
        char DREFArray[100][100];
        float CTRL[5] = {0.0};
        float *recDATA[100];
        short DREFSizes[100];
        struct xpcSocket sendPort, recvPort;
        short result;
    
    // Setup
        for (i = 0; i < 100; i++) {
            recDATA[i]   = (float *) malloc(40*sizeof(float));
            memset(DREFArray[i],0,100);
        }
        sendPort = openUDP( 49066, "127.0.0.1", 49009 );
        recvPort = openUDP( 49008, "127.0.0.1", 49009 );
    strcpy(DREFArray[0],"sim/cockpit2/controls/yoke_pitch_ratio");
    strcpy(DREFArray[1],"sim/cockpit2/controls/yoke_roll_ratio");
    strcpy(DREFArray[2],"sim/cockpit2/controls/yoke_heading_ratio");
    strcpy(DREFArray[3],"sim/flightmodel/engine/ENGN_thro");
    strcpy(DREFArray[4],"sim/cockpit/switches/gear_handle_status");
    strcpy(DREFArray[5],"sim/flightmodel/controls/flaprqsts");
    for (i=0;i<5;i++) {
        DREFSizes[i] = (int) strlen(DREFArray[i]);
    }
    CTRL[3] = 0.8; // Throttle
    
    // Execute
    sendCTRL(sendPort, 4, CTRL);
    result = requestDREF(sendPort, recvPort, DREFArray, DREFSizes, 1, recDATA, DREFSizes); // Test
    
    // Close
    closeUDP(sendPort);
    closeUDP(recvPort);
    
    // Tests
    if ( result < 0 )// Request 1 value
    {
        throw -6;
    }
    for (i=0;i<6-1;i++)
    {
        if (abs(recDATA[i][0]-CTRL[i])>1e-4)
        {
            throw -i;
        }
    }
}

void test8() // sendPOSI test
{
    std::cout << "sendPOSI - ";
    
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
        throw -1;
    }
    if (recDATA[0][0] != 1)
    {
        throw -2;
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
        throw -3;
    }
    if (recDATA[0][0] != 0)
    {
        throw -4;
    }
}

void test9() // pauseSim test
{
    std::cout << "pauseSim - ";
    
    // Initialize
        struct xpcSocket sendPort, recvPort;
    
    // Setup
        sendPort = openUDP( 49067, "127.0.0.1", 49009 );
        recvPort = openUDP( 49008, "127.0.0.1", 49009 );
    
    // Execute
        pauseSim(sendPort, 1);
    
    // Close
        closeUDP(sendPort);
        closeUDP(recvPort);
    
    // Test
}

void test10() // setConn test
{
    std::cout << "setConn - ";
    
    // Initialize
        int i; // Iterator
        char DREFArray[100][100];
        float *recDATA[100];
        short DREFSizes[100];
        struct xpcSocket sendPort, recvPort;
        short result = 0;
        usleep(0);
    
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
            throw 1;
        }

    
    // Set up for next test
        sendPort = openUDP( 49067, "127.0.0.1", 49009 );
        setCONN(sendPort, 49009);
        closeUDP(sendPort);
}

int main(int argc, const char * argv[])
{
    std::cout << "XPC Tests-cpp ";
    
#ifdef _WIN32
    std::cout << "(Windows)\n";
#elif (__APPLE__)
    std::cout << "(Mac) \n";
#elif (__linux)
    std::cout << "(Linux) \n";
#endif
    
    runTest(test1);
    runTest(test2);
    runTest(test3);
    runTest(test4);
    runTest(test5);
    runTest(test6);
    runTest(test7);
    runTest(test8);
    runTest(test9);
    runTest(test10);
    
    std::cout << "----------------\nTest Summary\n\tFailed: " << testFailed << "\n\tPassed: " << testPassed << std::endl;
    
    return 0;
}

