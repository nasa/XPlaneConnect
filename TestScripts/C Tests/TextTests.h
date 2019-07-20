//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef TEXTTESTS_H
#define TEXTTESTS_H

#include "Test.h"
#include "xplaneConnect.h"

int testTEXT()
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

#endif
