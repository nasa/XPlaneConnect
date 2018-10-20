//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef UDPTESTS_H
#define UDPTESTS_H

#include "Test.h"
#include "xplaneConnect.h"

int testOpen()
{
	XPCSocket sock = openUDP("localhost");
	int result = strncmp(sock.xpIP, "127.0.0.1", 16);
	closeUDP(sock);
	return result;
}

int testClose()
{
	XPCSocket sendPort = aopenUDP(IP, 49009, 49063);
	closeUDP(sendPort);
	sendPort = aopenUDP(IP, 49009, 49063);
	closeUDP(sendPort);
	return 0;
}

int testCONN()
{
	// Initialize
	char* drefs[] =
	{
		"sim/cockpit/switches/gear_handle_status"
	};
	float data[1];
	int size = 1;
	XPCSocket sock = openUDP(IP);
#if (__APPLE__ || __linux)
	usleep(0);
#endif

	// Execution
	setCONN(&sock, 49055);
	int result = getDREF(sock, drefs[0], data, &size);

	// Close
	closeUDP(sock);

	// Test
	if (result < 0)// No data received
	{
		return -1;
	}
	return 0;
}

#endif
