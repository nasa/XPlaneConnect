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
#include "UDPSocket.h"
#include "xpcPluginTools.h"

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

short					number_of_connections = 0;
short					current_connection = -1;

double					start,lap;
static double			timeConvert = 0.0;
int						benchmarkingSwitch = 0; // 1 = time for operations, 2 = time for op + cycle;
int						cyclesToClear = -1; // Clear message bus every n cycles. -1 == dont clear
int						counter = 0;

struct connectionHistory
{
	short					connectionID;
	
	char					IP[16];
	unsigned short			recPort;
	unsigned short			fromPort;
	short					requestLength;
	std::string				XPLMRequestedDRefs[100];
};

connectionHistory connectionList[MAXCONN];

PLUGIN_API int XPluginStart(char *	outName, char *	outSig, char *	outDesc);
static float MyFlightLoopCallback(float, float, int inCounter, void * inRefcon);
PLUGIN_API void	XPluginStop(void);
PLUGIN_API void XPluginDisable(void);
PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID	inFromWho, int inMessage, void * inParam);

int handleSIMU(char buf[]);
int handleCONN(char buf[]);
int handlePOSI(char buf[]);
int handleCTRL(char buf[]);
int handleWYPT(char buf[], int len);
int handleGETD(char *buf);
int handleDREF(char *buf);
int handleVIEW();
int handleDATA(char *buf, int buflen);
int handleTEXT(char *buf, int len);
short handleInput(XPC::Message& msg);

void sendBUF(char buf[], int buflen);

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
	short result;
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
		result = handleInput(msg);

		if (benchmarkingSwitch > 0)
		{
#if (__APPLE__)
			lap = (double)mach_absolute_time( ) * timeConvert;
			diff_t = lap-start;
			XPC::Log::FormatLine("Runtime %.6f",diff_t);
#endif
		}
		
		if (result<=0) break; // No Signal
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

short handleInput(XPC::Message& msg)
{
	int i;
	char IP[16] = { 0 };
	unsigned short port = 0;

	current_connection = -1;

	if (msg.GetSize() == 0)
	{
		// No message received
		return 0;
	}

	msg.PrintToLog();

	// Check for existing connection
	port = getIP(msg.GetSource(), IP);
	for (i = 0; i < number_of_connections; i++)
	{
		if (strcmp(connectionList[i].IP, IP) == 0 && port == connectionList[i].fromPort && port > 0)
		{
			current_connection = i;
			break;
		}
	}

	if (current_connection == -1) //SETUP NEW CONNECTION
	{
		if (number_of_connections >= MAXCONN) // Handle hitting MAXCONN (COME UP WITH A BETTER SOLUTION)
		{
			XPC::Log::WriteLine("[EXEC] Hit maximum number of connections-Removing old ones");
			number_of_connections = 0;
		}

		memcpy(connectionList[number_of_connections].IP, IP, 16);
		connectionList[number_of_connections].recPort = 49008;
		connectionList[number_of_connections].fromPort = port;
		current_connection = number_of_connections;
		number_of_connections++;

		// Log Connection
		XPC::Log::FormatLine("[EXEC] New Connection [%i].  IP=%s, port=%i", number_of_connections, IP, port);
	}

	std::string head = msg.GetHead();
	if (head == "CONN") // Header = CONN (Connection)
	{
		handleCONN((char*)msg.GetBuffer());
	}
	else if (head == "SIMU") // Header = SIMU
	{
		handleSIMU((char*)msg.GetBuffer());
	}
	else if (head == "POSI") // Header = POSI (Position)
	{
		handlePOSI((char*)msg.GetBuffer());
	}
	else if (head == "CTRL") // Header = CTRL (Control)
	{
		handleCTRL((char*)msg.GetBuffer());
	}
	else if (head == "WYPT") // Header = WYPT (Waypoint Draw)
	{
		handleWYPT((char*)msg.GetBuffer(), msg.GetSize());
	}
	else if (head == "GETD") // Header = GETD (Data Request)
	{
		handleGETD((char*)msg.GetBuffer());
	}
	else if (head == "DREF") // Header = DREF (By Data Ref) (this is slower than DATA)
	{
		handleDREF((char*)msg.GetBuffer());
	}
	else if (head == "VIEW") // Header = VIEW (Change View)
	{
		handleVIEW();
	}
	else if (head == "DATA") // Header = DATA (UDP Data)
	{
		handleDATA((char*)msg.GetBuffer(), msg.GetSize());
	}
	else if (head == "TEXT") // Header = TEXT (Screen message)
	{
		handleTEXT((char*)msg.GetBuffer(), msg.GetSize());
	}
	else if ((head == "DSEL") || (head == "USEL")) // Header = DSEL/USEL (Select UDP Send)
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else if ((head == "DCOC") || (head == "UCOC"))
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else if ((head == "MOUS") || (head == "CHAR"))
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else if (head == "MENU")
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else if (head == "SOUN")
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else if ((head == "FAIL") || (head == "RECO"))
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else if (head == "PAPT")
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else if ((head == "VEHN") || head == "VEH1" || (head == "VEHA"))
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else if ((head == "OBJN") || (head == "OBJL"))
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else if ((head == "GSET") || (head == "ISET"))
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else if (head == "BOAT")
	{
		sendBUF((char*)msg.GetBuffer(), msg.GetSize()); // Send to UDP
	}
	else
	{ //unrecognized header
		XPC::Log::FormatLine("[EXEC] ERROR: Command %s not recognized", head);
	}
	current_connection = -1;

	return 0;
}

void sendBUF( char buf[], int buflen)
{
	sendSocket->SendTo((std::uint8_t*)buf, buflen, "127.0.0.1", 49000);
}

int handleCONN(char buf[])
{
	char the_message[7];
	char header[5];
	
	strncpy(header,"CONF",4);
	memcpy(&the_message,&header,4);
	the_message[4] = (char) current_connection;
	
	// COPY PORT
	memcpy(&(connectionList[current_connection].recPort),&buf[5],sizeof(connectionList[current_connection].recPort));
	
	if (connectionList[current_connection].recPort <= 1) // Is not valid port
	{
		connectionList[current_connection].recPort = 49008;
		XPC::Log::WriteLine("[CONN] ERROR: Port Number must be a number (NaN received)");
		return 1;
	}
	
	
	// UPDATE LOG
	XPC::Log::FormatLine("[CONN] Update Connection %i- Sending to port: %i",
		current_connection + 1,
		connectionList[current_connection].recPort);
	
	// SEND CONFIRMATION
	// TODO: Ivestigate why sending confirmation causes crashes on Windows 8
	//memcpy(sendSocketet.xpIP,connectionList[current_connection].IP, sizeof(connectionList[current_connection].IP));
	//sendSocketet.xpPort = connectionList[current_connection].recPort;
	//sendUDP(sendSocketet, the_message, 5);
	
	return 0;
}

int handleSIMU(char buf[])
{
	int SIMUArray[1] = {buf[5]};
	
	if (SIMUArray[0] != SIMUArray[0]) // Is NaN
	{
		XPC::Log::WriteLine("[SIMU] ERROR: Value must be a number (NaN received)");
		return 1;
	}
	
	int value = buf[5];
	XPC::DataManager::Set(XPC::DREF::Pause, &value, 1);
	
	if (buf[5] == 0)
	{
		XPC::Log::FormatLine("[SIMU] Simulation Resumed (Conn %i)", current_connection + 1);
	}
	else
	{
		XPC::Log::FormatLine("[SIMU] Simulation Paused (Conn %i)", current_connection + 1);
	}
	
	return 0;
}

int handleTEXT(char *buf, int len)
{
	char msg[256] = { 0 };
	if (len < 14)
	{
		XPC::Log::WriteLine("[TEXT] ERROR: Length less than 14 bytes");
		return -1;
	}
	size_t msgLen = (unsigned char)buf[13];
	if (msgLen == 0)
	{
		XPC::Drawing::ClearMessage();
		XPC::Log::WriteLine("[TEXT] Text cleared");
	}
	else
	{
		int x = *((int*)(buf + 5));
		int y = *((int*)(buf + 9));
		strncpy(msg, buf + 14, msgLen);
		XPC::Drawing::SetMessage(x, y, msg);
		XPC::Log::WriteLine("[TEXT] Text set");
	}
	return 0;
}

int handlePOSI(char buf[])
{
	float position[8] = {0.0};
	float pos[3],orient[3];
	short aircraft = 0;
	float gear = -1.0;
	int autopilot[20] = {0};
	
	// UPDATE LOG
	XPC::Log::FormatLine("[POSI] Message Received (Conn %i)", current_connection + 1);
	
	aircraft = fmini(parsePOSI(buf,position,6, &gear),19);
	
	if (aircraft > 0)
	{
		// Enable AI for the aircraft we are setting
		float ai[20];
		std::size_t result = XPC::DataManager::GetFloatArray(XPC::DREF::PauseAI, ai, 20);
		if (result == 20) // Only set values if they were retrieved successfully.
		{
			ai[aircraft] = 1;
			XPC::DataManager::Set(XPC::DREF::PauseAI, ai, 0, 20);
		}
	}
	
	// Position
	memcpy(pos,position,3*sizeof(float));
	XPC::DataManager::SetPosition(pos, aircraft);
	
	// Orientation
	memcpy(orient,&position[3],3*sizeof(float));
	XPC::DataManager::SetOrientation(orient, aircraft);
	
	//Landing Gear
	if (gear != -1)
	{
		XPC::DataManager::SetGear(gear, false, aircraft);
	}
	
	return 0;
}

int handleCTRL(char buf[])
{
	xpcCtrl ctrl;
	float thr[8] = { 0 };
	short i;
	
	// UPDATE LOG
	XPC::Log::FormatLine("[CTRL] Message Received (Conn %i)", current_connection + 1);
	
	ctrl = parseCTRL(buf);
	if (ctrl.aircraft < 0) //parseCTRL failed
	{
		return 1;
	}
	if (ctrl.aircraft > 19) //Can only handle 19 non-player aircraft right now
	{
		return 2;
	}
	// SET CONTROLS
	XPC::DataManager::Set(XPC::DREF::YokePitch, ctrl.pitch, ctrl.aircraft);
	XPC::DataManager::Set(XPC::DREF::YokeRoll, ctrl.roll, ctrl.aircraft);
	XPC::DataManager::Set(XPC::DREF::YokeHeading, ctrl.yaw, ctrl.aircraft);

	// SET Throttle
	for (i = 0; i<8; i++)
	{
		thr[i] = ctrl.throttle;
	}
	XPC::DataManager::Set(XPC::DREF::ThrottleSet, thr, 8, ctrl.aircraft);
	XPC::DataManager::Set(XPC::DREF::ThrottleActual, thr, 8, ctrl.aircraft);
	if (ctrl.aircraft == 0)
	{
		XPC::DataManager::Set("sim/flightmodel/engine/ENGN_thro_override", thr, 1);
	}

	// SET Gear/Flaps
	if (ctrl.gear != -1)
	{
		XPC::DataManager::SetGear(ctrl.gear, false, ctrl.aircraft);
	}
	if (ctrl.flaps < -999.5 || ctrl.flaps > -997.5) // Flaps
	{
		XPC::DataManager::Set(XPC::DREF::FlapSetting, ctrl.flaps, ctrl.aircraft);
	}
	return 0;
}

int handleWYPT(char buf[], int len)
{
	// UPDATE LOG
	XPC::Log::FormatLine("[WYPT] Message Received (Conn %i)", current_connection + 1);

	xpcWypt wypt = parseWYPT(buf);
	if (wypt.op < 0)
	{
		XPC::Log::FormatLine("[WYPT] Failed to parse command. ERR:%i", wypt.op);
		return -1;
	}
	else
	{
		XPC::Log::FormatLine("[WYPT] Performing operation %i", wypt.op);
	}

	switch (wypt.op)
	{
	case xpc_WYPT_ADD:
		XPC::Drawing::AddWaypoints(wypt.points, wypt.numPoints);
		break;
	case xpc_WYPT_DEL:
		XPC::Drawing::RemoveWaypoints(wypt.points, wypt.numPoints);
		break;
	case xpc_WYPT_CLR:
		XPC::Drawing::ClearWaypoints();
		break;
	default: //If parseWYPT is doing its job, we shouldn't ever hit this.
		return -2;
	}
	return 0;
}

int handleGETD(char buf[])
{
	int DREFSizes[100] = { 0 };
	char *DREFArray[100];
	std::uint8_t drefCount = buf[5];
	if (drefCount == 0) // USE LAST REQUEST
	{
		XPC::Log::FormatLine("[GETD] DATA Requested- repeat last request from connection %i (%i data refs)",
			current_connection + 1,
			connectionList[current_connection].requestLength);

		if (connectionList[current_connection].requestLength < 0) // No past requests
		{
			XPC::Log::FormatLine("[GETD] ERROR- No previous requests from connection %i.", current_connection + 1);
			return 1;
		}
	}
	else if (drefCount > 0) // NEW REQUEST
	{
		connectionList[current_connection].requestLength = (short)drefCount;

		for (int i = 0; i < connectionList[current_connection].requestLength; i++)
		{
			DREFArray[i] = (char *)malloc(100);
			memset(DREFArray[i], 0, 100);
		}

		parseGETD(buf, DREFArray, DREFSizes);
		XPC::Log::FormatLine("[GETD] DATA Requested- New Request for connection %i [%i]:",
			current_connection + 1,
			drefCount);
	}
	else
	{
		return -1;
	}

	std::uint8_t response[4096] = "RESP";
	response[5] = drefCount;
	std::size_t cur = 6;
	for (int i = 0; i < drefCount; ++i)
	{
		float values[255];
		std::size_t count = XPC::DataManager::Get(DREFArray[i], values, 255);
		response[cur] = count;
		memcpy(response + 1 + cur, values, count * sizeof(float));
		cur += 1 + count * sizeof(float);
	}

	char* host = connectionList[current_connection].IP;
	std::uint16_t port = connectionList[current_connection].recPort;
	sendSocket->SendTo(response, cur, host, port);
		
	return 0;
}

int handleDREF(char buf[])
{	
	char DREF[100] = {0};
	unsigned short lenDREF = 0;
	unsigned short lenVALUE = 0;
	float values[40] = {0.0};
	
	parseDREF(buf, DREF, &lenDREF,values,&lenVALUE);
	
	// Input Varification
	if (lenDREF <= 0)
	{
		return 1;
	}
	
	// Handle DREF
	XPC::Log::FormatLine("[DREF] Request to set DREF value received (Conn %i): %s", current_connection + 1, DREF);
	
	XPC::DataManager::Set(DREF, values, lenVALUE);
	
	return 0;
}

int handleVIEW()
{
	XPC::Log::FormatLine("[VIEW] Message Received (Conn %i)- VIEW FEATURE UNDER CONSTRUCTION",
		current_connection + 1);
	return 0;
}

int handleDATA(char buf[], int buflen)
{
	int i,j,lines;
	float floatArray[10] = {0};
	float recValues[20][9] = {0};
	const float deg2rad = (float) 0.0174532925;
	float savedAlpha=-998;
	float savedHPath=-998;
	
	lines = parseDATA(buf, buflen, recValues); //Parse Data
	if (lines > 0)
	{
		// UPDATE LOG
		XPC::Log::FormatLine("[DATA] Message Received (Conn %i)", current_connection + 1);
	}
	else
	{
		// UPDATE LOG
		XPC::Log::FormatLine("[DATA] WARNING: Empty data packet received (Conn %i)", current_connection + 1);
	}
	
	for (i = 0; i<lines; i++) //Execute changes one line at a time
	{
		short dataRef = (short) recValues[i][0];
		
		if ( dataRef<0 || dataRef >134 )
		{   // DREF Check 1: This ensures that the received dataRef is in the range of 0-134
			XPC::Log::FormatLine("[DATA] ERROR: DataRef # must be between 0 - 134 (Rec: %hi)",dataRef);
			continue;
		}
		
		switch (dataRef)
		{
			{case 3: // Velocity
				// Velocity has to be broken into individual components in X-Plane World Coordinate System where x=East, y=Up.
				//Note: Add Sideslip
				
				float theta, alpha, hpath,v;
				int ind[3] = {1,3,4};
				theta = XPC::DataManager::GetFloat(XPC::DREF::Pitch);
				
				if (savedAlpha != -998)
					alpha = savedAlpha;
				else
					alpha = XPC::DataManager::GetFloat(XPC::DREF::AngleOfAttack);
				
				if (savedHPath != -998)
					hpath = savedHPath;
				else
					hpath = XPC::DataManager::GetFloat(XPC::DREF::HPath); //Velocity Heading
				
				if ( ( hpath != hpath ) && ( alpha != alpha ) && ( theta != theta ) ) // NaN Check
				{
					XPC::Log::WriteLine("[DATA] ERROR: Value must be a number (NaN received)");
					break;
				}
				
				for (j=0;j<sizeof(ind)/sizeof(ind[0]);j++)
				{
					if (recValues[i][ind[j]] != -998)
					{
						v = recValues[i][ind[j]];

						XPC::DataManager::Set(XPC::DREF::LocalVX, v*cos((theta - alpha)*deg2rad)*sin(hpath*deg2rad));
						XPC::DataManager::Set(XPC::DREF::LocalVY, v*sin((theta - alpha)*deg2rad));
						XPC::DataManager::Set(XPC::DREF::LocalVZ, -v*cos((theta - alpha)*deg2rad)*cos(hpath*deg2rad));
					}
				}
				break;}
			{case 17: // Orientation
				float orient[3]
				{
					recValues[i][1] == -998 ? XPC::DataManager::GetFloat(XPC::DREF::Pitch) : recValues[i][1],
					recValues[i][2] == -998 ? XPC::DataManager::GetFloat(XPC::DREF::Roll) : recValues[i][2],
					recValues[i][3] == -998 ? XPC::DataManager::GetFloat(XPC::DREF::HeadingTrue) : recValues[i][3]
				};
				XPC::DataManager::SetOrientation(orient);
				break;}
				
			{case 18: // Alpha, hpath etc.
				if ( ( recValues[i][1] != recValues[i][1] ) && ( recValues[i][3] != recValues[i][3] ) ) // NaN Check
	 			{
					XPC::Log::WriteLine("[DATA] ERROR: Value must be a number (NaN received)");
					break;
				}
				
				if (recValues[i][1] != -998)
					savedAlpha = recValues[i][1];
				
				if (recValues[i][3] != -998)
					savedHPath = recValues[i][3];
				
				break;}
				
			{case 20: // Position
				float pos[3]
				{
					recValues[i][2] == -998 ? XPC::DataManager::GetFloat(XPC::DREF::Latitude) : recValues[i][2],
					recValues[i][3] == -998 ? XPC::DataManager::GetFloat(XPC::DREF::Longitude) : recValues[i][3],
					recValues[i][4] == -998 ? XPC::DataManager::GetFloat(XPC::DREF::AGL) : recValues[i][4]
				};
				XPC::DataManager::SetPosition(pos);
				break;}
				
			{case 25: // Throttle
				if ( recValues[i][1] != recValues[i][1] )
				{
					XPC::Log::WriteLine("[DATA] ERROR: Value must be a number (NaN received)");
					break;
				}
				
				for (j=0; j<8; j++)
					floatArray[j] = recValues[i][1];
				
				XPC::DataManager::Set(XPC::DREF::ThrottleSet, floatArray, 8);
				break;}
				
			{default: // Non-Special dataRefs (everything else)
				memcpy(floatArray,&recValues[i][1],8*sizeof(float));
				for (j=0; j<8; j++)
				{
					if (LOG_VERBOSITY > 0)
					{
						XPC::Log::FormatLine("Setting Dataref %i.%i to %f", dataRef, j, floatArray[j]);
					}
					
					if (dataRef==14 && j==0)
					{
						XPC::DataManager::SetGear(recValues[i][j + 1], false);
						continue;
					}

					XPC::DREF dref = XPC::XPData[dataRef][j];
					if (dref == XPC::DREF::None)
					{
						sendBUF(buf, buflen);
					}
					else
					{
						XPC::DataManager::Set(dref, floatArray, 8);
					}
				} //End for j=1:8
				break;}
		} // End switch(dataRef)
	}// End for(all lines)
		
	return 0;
} // End handleDATA
