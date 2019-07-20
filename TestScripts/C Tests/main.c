//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
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

int main(int argc, const char * argv[]) {
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
    crossPlatformUSleep(SLEEP_AMOUNT);
	runTest(testClose, "close");
    crossPlatformUSleep(SLEEP_AMOUNT);

	// Datarefs
	runTest(testGETD_Basic, "GETD");
    crossPlatformUSleep(SLEEP_AMOUNT);
	runTest(testGETD_Types, "GETD (types)");
    crossPlatformUSleep(SLEEP_AMOUNT);
	runTest(testGETD_TestFloat, "GETD (test float)");
    crossPlatformUSleep(SLEEP_AMOUNT);
	runTest(testDREF, "DREF");
	// Pause
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testSIMU_Basic, "SIMU");
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testSIMU_Toggle, "SIMU (toggle)");
	// CTRL
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testCTRL_Player, "CTRL (player)");
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testCTRL_NonPlayer, "CTRL (non-player)");
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testCTRL_Speedbrakes, "CTRL (speedbrakes)");
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testGETC, "GETC (player)");
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testGETC_NonPlayer, "GETC (Non-player)");
	// POSI
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testPOSI_Player, "POSI (player)");
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testPOSI_NonPlayer, "POSI (non-player)");
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testGetPOSI_Player, "GETP (player)");
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testGetPOSI_NonPlayer, "GETP (non-player)");
	// Data
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testDATA, "DATA");
	// Text
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testTEXT, "TEXT");
	// Waypoints
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testWYPT, "WYPT");
	// View
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testView, "VIEW");
	// setConn
    crossPlatformUSleep(SLEEP_AMOUNT);
    runTest(testCONN, "CONN");

    printf( "----------------\nTest Summary\n\tFailed: %i\n\tPassed: %i\n", testFailed, testPassed );
	printf("Press any key to exit.");
	getchar();

    return 0;
}
