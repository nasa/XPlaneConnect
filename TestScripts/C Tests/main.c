//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#include "Test.h"
#include "UDPTests.h"
#include "DrefTests.h"
#include "SimuTests.h"
#include "CtrlTests.h"
#include "PosiTests.h"
#include "DataTests.h"
#include "TextTests.h"
#include "ViewTests.h"
#include "WyptTests.h"

int main(int argc, const char * argv[])
{
    printf("XPC Tests-c ");
    
#ifdef _WIN32
    printf("(Windows)\n");
#elif (__APPLE__)
    printf("(Mac) \n");
#elif (__linux)
    printf("(Linux) \n");
#else
	printf("(Unable to determine operating system) \n")
#endif
    
	// Basic Networking
	runTest(testOpen, "open");
	runTest(testClose, "close");
	// Datarefs
	runTest(testGETD_Basic, "GETD");
	runTest(testGETD_Types, "GETD (types)");
	runTest(testGETD_TestFloat, "GETD (test float)");
	runTest(testDREF, "DREF");
	// Pause
	runTest(testSIMU_Basic, "SIMU");
	runTest(testSIMU_Toggle, "SIMU (toggle)");
	// CTRL
	runTest(testCTRL_Player, "CTRL (player)");
	runTest(testCTRL_NonPlayer, "CTRL (non-player)");
	runTest(testCTRL_Speedbrakes, "CTRL (speedbrakes)");
	// POSI
	runTest(testPOSI_Player, "POSI (player)");
	runTest(testPOSI_NonPlayer, "POSI (non-player)");
	// Data
	runTest(testDATA, "DATA");
	// Text
	runTest(testTEXT, "TEXT");
	// Waypoints
	runTest(testWYPT, "WYPT");
	// View
	runTest(testView, "VIEW");
	// setConn
	runTest(testCONN, "CONN");
    
    printf( "----------------\nTest Summary\n\tFailed: %i\n\tPassed: %i\n", testFailed, testPassed );
	printf("Press any key to exit.");
	getchar();
    
    return 0;
}

