//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef WYPTTESTS_H
#define WYPTTESTS_H

#include "Test.h"
#include "xplaneConnect.h"

int testWYPT()
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

#endif
