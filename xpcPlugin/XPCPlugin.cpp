//
//  XPCPlugin Beta
//
//  DESCRIPTION
//      XPCPlugin Facilitates Communication to and from the XPlane
//
//  REQUIREMENTS
//      1. X-Plane Version 9.0 or newer (untested on previous versions)
//      2. XPCPlugin.xpl-must be placed in [X-Plane Directory]/Resources/plugins
//      3. OS X 10.8 or newer (untested on previous versions/Windows)
//
//  INSTRUCTIONS
//      1. Move xpcPlugin.xpl into [X-Plane Directory]/Resources/plugins
//
//  COMMAND TYPES
//      DATA: Works like UDP Commands
//      SIMU: Pauses/Unpauses Simulation
//		CONN: Set Returned Port Number
//      DREF: Sets value to specific DREF (see http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html)
//      GETD: Requests specific DREF
//		POSI: Set Position
//		CTRL: Set Control Variables
//		WYPT: Add/Edit a Waypoint (PLANNED)
//		VIEW: Set Simulation View (PLANNED)
//		TEXT: Add Text to Screen (PLANNED)
//
//  NOTICES:
//      Copyright ã 2013-2014 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.  All Rights Reserved.
//
//  DISCLAIMERS
//      No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
//
//      Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
//
//  X-Plane API
//      Copyright (c) 2008, Sandy Barbour and Ben Supnik  All rights reserved.
//      Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//      * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//      * Neither the names of the authors nor that of X-Plane or Laminar Research may be used to endorse or promote products derived from this software without specific prior written permission from the authors or Laminar Research, respectively.
//
//      X-Plane API SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//  CONTACT
//      For questions email Christopher Teubert (christopher.a.teubert@nasa.gov)
//
//	CONTRIBUTORS
//		CT: Christopher Teubert (christopher.a.teubert@nasa.gov)
//
//  TO DO
//		1. Handle hitting maxcon
//		2. POSI:  Alpha/Beta
//		3. Error Handling
//		4. CTRL: add SpeedBrakes
//		5. SIMU: -1 = switch
//		6. SENDBUF: Add message
//		7. Ability to add text to screen
//		8. Bound checking
//
// BEGIN CODE

#define _WINSOCKAPI_  

// XPC Includes
#include "Log.h"
#include "DataManager.h"
#include "DataMaps.h"
#include "Drawing.h"
#include "Message.h"
#include "MessageHandlers.h"
#include "UDPSocket.h"

// XPLM Includes
//#include "XPLMPlanes.h"
#include "XPLMProcessing.h"
#include "XPLMGraphics.h"

// System Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef _WIN32 /* WIN32 SYSTEM */
#include <windows.h>
#elif (__APPLE__)
#include <mach/mach_time.h>
#endif

#define MAXCONN 50 // Maximum number of dedicated connections
#define RECVPORT 49009 // Port that the plugin receives commands on
#define SENDPORT 49097 // Port that the plugin sends on
#define OPS_PER_CYCLE 20 // Max Number of operations per cycle

XPC::UDPSocket* recvSocket = nullptr;
XPC::UDPSocket* sendSocket = nullptr;

double					start,lap;
static double			timeConvert = 0.0;
int						benchmarkingSwitch = 0; // 1 = time for operations, 2 = time for op + cycle;
int						cyclesToClear = -1; // Clear message bus every n cycles. -1 == dont clear
int						counter = 0;

PLUGIN_API int XPluginStart(char *	outName, char *	outSig, char *	outDesc);
static float MyFlightLoopCallback(float, float, int inCounter, void * inRefcon);
PLUGIN_API void	XPluginStop(void);
PLUGIN_API void XPluginDisable(void);
PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID	inFromWho, int inMessage, void * inParam);

PLUGIN_API int XPluginStart(	char *		outName,
							char *		outSig,
							char *		outDesc)
{
	static FILE				*logFile;	// Log File
	strcpy(outName, "xplaneConnect");
	strcpy(outSig, "NASA.xplaneConnect");
	strcpy(outDesc, "X Plane Communications Toolbox");
	
	logFile = fopen("xpcLog.txt","w");
	if (logFile != NULL) // If opened correctly
	{
		fprintf(logFile,"\n");
		fclose(logFile);
	}
	
	XPC::Log::WriteLine("[EXEC] xpcPlugin Start");
	
#if (__APPLE__)
	if ( abs(timeConvert) <= 1e-9 ) // is about 0
	{
		mach_timebase_info_data_t timeBase;
		(void)mach_timebase_info( &timeBase );
		timeConvert = (double)timeBase.numer /
		(double)timeBase.denom /
		1000000000.0;
	}
#endif
	XPC::DataManager::Initialize();
	
	XPLMRegisterFlightLoopCallback(
								   MyFlightLoopCallback,	/* Callback */
								   -1,						/* Interval (s)*/
								   NULL);					/* refcon not used. */
	
	return 1;
}

PLUGIN_API void	XPluginStop(void)
{
	XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, NULL);
	XPC::Log::WriteLine("[EXEC] xpcPlugin Shutdown");
}

PLUGIN_API void XPluginDisable(void)
{
	// Close sockets
	delete recvSocket;
	delete sendSocket;
	recvSocket = nullptr;
	sendSocket = nullptr;

	// Stop rendering messages to screen.
	XPC::Drawing::ClearMessage();

	// Stop rendering waypoints to screen.
	XPC::Drawing::ClearWaypoints();

	XPC::Log::WriteLine("[EXEC] xpcPlugin Disabled, sockets closed");
}

PLUGIN_API int XPluginEnable(void)
{
	// Open sockets
	recvSocket = new XPC::UDPSocket(RECVPORT);
	sendSocket = new XPC::UDPSocket(SENDPORT);
    XPC::MessageHandlers::SetSocket(sendSocket);

	XPC::Log::WriteLine("[EXEC] xpcPlugin Enabled, sockets opened");
	if (benchmarkingSwitch > 0)
	{
		XPC::Log::FormatLine("[EXEC] Benchmarking Enabled (Verbosity: %i)", benchmarkingSwitch);
	}
#if LOG_VERBOSITY > 0
	XPC::Log::FormatLine("[EXEC] Debug Logging Enabled (Verbosity: %i)", LOG_VERBOSITY);
#endif
	
	return 1;
}

PLUGIN_API void XPluginReceiveMessage(	XPLMPluginID	inFromWho,
									  int				inMessage,
									  void *			inParam)
{
}

float	MyFlightLoopCallback(		float    inElapsedSinceLastCall,
							 float    inElapsedTimeSinceLastFlightLoop,
							 int      inCounter,
							 void *   inRefcon)
{
	int i;
#if (__APPLE__)
	double diff_t;
#endif
	
	counter++;
	if (benchmarkingSwitch > 1)
	{
		XPC::Log::FormatLine("Cycle time %.6f", inElapsedSinceLastCall);
	}
	
	for (i=0;i<OPS_PER_CYCLE;i++)
	{
		if (benchmarkingSwitch > 0)
		{
#if (__APPLE__)
			start = (double)mach_absolute_time( ) * timeConvert;
#endif
		}
		XPC::Message msg = XPC::Message::ReadFrom(*recvSocket);
		if (msg.GetHead() == "")
		{
			break;
		}
		XPC::MessageHandlers::HandleMessage(msg);

		if (benchmarkingSwitch > 0)
		{
#if (__APPLE__)
			lap = (double)mach_absolute_time( ) * timeConvert;
			diff_t = lap-start;
			XPC::Log::FormatLine("Runtime %.6f",diff_t);
#endif
		}
	}
	
	if (cyclesToClear != -1)
	{
		if (counter%cyclesToClear==0)
		{
			XPC::Log::WriteLine("[EXEC] Cleared UDP Buffer");
			delete recvSocket;
			recvSocket = new XPC::UDPSocket(RECVPORT);
		}
	}
	return -1;
}