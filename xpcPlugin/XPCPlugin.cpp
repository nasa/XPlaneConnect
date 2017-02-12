// Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
//
// DISCLAIMERS
//     No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND,
//     EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT
//     THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF
//     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY
//     THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED,
//     WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN
//     ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS,
//     HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT
//     SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING
//     THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
//
//     Waiver and Indemnity: RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES
//     GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF
//     RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES
//     OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING
//     FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE
//     UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT,
//     TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE
//     IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
//
// X-Plane API
// Copyright(c) 2008, Sandy Barbour and Ben Supnik All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files(the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Neither the names of the authors nor that of X - Plane or Laminar Research
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission from the authors or
//     Laminar Research, respectively.


// X-Plane Connect Plugin
//
// DESCRIPTION
//     XPCPlugin Facilitates Communication to and from the XPlane
//
// INSTRUCTIONS
//     See Readme.md in the root of this repository or the wiki hosted on GitHub at
//     https://github.com/nasa/XPlaneConnect/wiki for requirements, installation instructions,
//     and detailed documentation.
//
// CONTACT
//     For questions email Christopher Teubert (christopher.a.teubert@nasa.gov)
//
// CONTRIBUTORS
//     CT: Christopher Teubert (christopher.a.teubert@nasa.gov)
//     JW: Jason Watkins (jason.w.watkins@nasa.gov)

// XPC Includes
#include "DataManager.h"
#include "Drawing.h"
#include "Log.h"
#include "MessageHandlers.h"
#include "UDPSocket.h"

// XPLM Includes
#include "XPLMProcessing.h"

// System Includes
#include <cstring>
#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

#define RECVPORT 49009 // Port that the plugin receives commands on
#define OPS_PER_CYCLE 20 // Max Number of operations per cycle

XPC::UDPSocket* sock = NULL;

double start;
double lap;
static double timeConvert = 0.0;
int benchmarkingSwitch = 0; // 1 = time for operations, 2 = time for op + cycle;
int cyclesToClear = -1; // Clear message bus every n cycles. -1 == dont clear
int counter = 0;

PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc);
PLUGIN_API void	XPluginStop(void);
PLUGIN_API void XPluginDisable(void);
PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, int inMessage, void* inParam);
static float XPCFlightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void* inRefcon);

PLUGIN_API int XPluginStart(char* outName, char* outSig, char* outDesc)
{
	strcpy(outName, "X-Plane Connect [Version 1.2.0]");
	strcpy(outSig, "NASA.XPlaneConnect");
	strcpy(outDesc, "X Plane Communications Toolbox\nCopyright (c) 2013-2015 United States Government as represented by the Administrator of the National Aeronautics and Space Administration. All Rights Reserved.");

#if (__APPLE__)
	if ( abs(timeConvert) <= 1e-9 ) // is about 0
	{
		mach_timebase_info_data_t timeBase;
		(void)mach_timebase_info(&timeBase);
		timeConvert = (double)timeBase.numer /
		(double)timeBase.denom /
		1000000000.0;
	}
#endif
	XPC::Log::Initialize("1.1.1");
	XPC::Log::WriteLine(LOG_INFO, "EXEC", "Plugin Start");
	XPC::DataManager::Initialize();

	return 1;
}

PLUGIN_API void	XPluginStop(void)
{
	XPC::Log::WriteLine(LOG_INFO, "EXEC", "Plugin Shutdown");
	XPC::Log::Close();
}

PLUGIN_API void XPluginDisable(void)
{
	XPLMUnregisterFlightLoopCallback(XPCFlightLoopCallback, NULL);

	// Close sockets
	delete sock;
	sock = NULL;

	// Stop rendering messages to screen.
	XPC::Drawing::ClearMessage();

	// Stop rendering waypoints to screen.
	XPC::Drawing::ClearWaypoints();

	XPC::Log::WriteLine(LOG_INFO, "EXEC", "Plugin Disabled, sockets closed");
}

PLUGIN_API int XPluginEnable(void)
{
	// Open sockets
	sock = new XPC::UDPSocket(RECVPORT);
	XPC::MessageHandlers::SetSocket(sock);

	XPC::Log::WriteLine(LOG_INFO, "EXEC", "Plugin Enabled, sockets opened");
	if (benchmarkingSwitch > 0)
	{
		XPC::Log::FormatLine(LOG_INFO, "EXEC", "Benchmarking Enabled (Verbosity: %i)", benchmarkingSwitch);
	}
	XPC::Log::FormatLine(LOG_INFO, "EXEC", "Debug Logging Enabled (Verbosity: %i)", LOG_LEVEL);
	
	float interval = -1; // Call every frame
	void* refcon = NULL; // Don't pass anything to the callback directly
	XPLMRegisterFlightLoopCallback(XPCFlightLoopCallback, interval, refcon);

	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFromWho, int inMessage, void* inParam)
{
	// XPC doesn't have anything useful to say to other plugins, so simply ignore
	// any messages received.
}

float XPCFlightLoopCallback(float inElapsedSinceLastCall,
	float inElapsedTimeSinceLastFlightLoop,
	int inCounter,
	void* inRefcon)
{
#if (__APPLE__)
	double diff_t;
#endif

	counter++;
	if (benchmarkingSwitch > 1)
	{
		XPC::Log::FormatLine(LOG_DEBUG, "EXEC", "Cycle time %.6f", inElapsedSinceLastCall);
	}

	for (int i = 0; i < OPS_PER_CYCLE; i++)
	{
		if (benchmarkingSwitch > 0)
		{
#if (__APPLE__)
			start = (double)mach_absolute_time( ) * timeConvert;
#endif
		}

		XPC::Message msg = XPC::Message::ReadFrom(*sock);
		if (msg.GetHead() == "")
		{
			break;
		}
		XPC::MessageHandlers::HandleMessage(msg);

		if (benchmarkingSwitch > 0)
		{
#if (__APPLE__)
			lap = (double)mach_absolute_time( ) * timeConvert;
			diff_t = lap - start;
			XPC::Log::FormatLine(LOG_INFO, "EXEC", "Runtime %.6f", diff_t);
#endif
		}
	}

	if (cyclesToClear != -1 && counter%cyclesToClear == 0)
	{
		XPC::Log::WriteLine(LOG_DEBUG, "EXEC", "Cleared UDP Buffer");
		delete sock;
		sock = new XPC::UDPSocket(RECVPORT);
	}
	return -1;
}
