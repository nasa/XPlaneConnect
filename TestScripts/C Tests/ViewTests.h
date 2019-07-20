//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef VIEWTESTS_H
#define VIEWTESTS_H

#include "Test.h"
#include "xplaneConnect.h"

typedef enum
{
	XPC_VDREF_FORWARDS = 1000,
	XPC_VDREF_DOWN4 = 1001,
	XPC_VDREF_DOWN8 = 1002,
	XPC_VDREF_LEFT45 = 1004,
	XPC_VDREF_RIGHT45 = 1005,
	XPC_VDREF_LEFT90 = 1006,
	XPC_VDREF_RIGHT90 = 1007,
	XPC_VDREF_LEFT135 = 1008,
	XPC_VDREF_RIGHT135 = 1009,
	XPC_VDREF_BACKWARD = 1010,
	XPC_VDREF_LEFTUP = 1011,
	XPC_VDREF_RIGHTUP = 1012,
	XPC_VDREF_AIRPORTBEACONTOWER = 1014,
	XPC_VDREF_ONRUNWAY = 1015,
	XPC_VDREF_CHASE = 1017,
	XPC_VDREF_FOLLOW = 1018,
	XPC_VDREF_FOLLOWWITHPANEL = 1019,
	XPC_VDREF_SPOT = 1020,
	XPC_VDREF_SPOTMOVING = 1021,
	XPC_VDREF_FULLSCREENWITHHUD = 1023,
	XPC_VDREF_FULLSCREENNOHUD = 1024,
	XPC_VDREF_STRAIGHTDOWN = 1025,
	XPC_VDREF_3DCOCKPIT = 1026
} VIEW_DREF;

int doViewTest(VIEW_TYPE viewCommand, VIEW_DREF viewResult)
{
	// Setup
	char* dref = "sim/graphics/view/view_type";
	float value;
	int size = 1;

	// Execute command
	XPCSocket sock = openUDP(IP);
	int result = sendVIEW(sock, viewCommand);
	if (result >= 0)
	{
		result = getDREF(sock, dref, &value, &size);
	}
	closeUDP(sock);
	if (result < 0)
	{
		return -1;
	}

	if ((int)value != viewResult)
	{
		return -2;
	}
	return 0;
}

int testView()
{
	int result = doViewTest(XPC_VIEW_FORWARDS, XPC_VDREF_FORWARDS);
	if (result < 0)
	{
		return -1;
	}

	return doViewTest(XPC_VIEW_CHASE, XPC_VDREF_CHASE);
}
#endif
