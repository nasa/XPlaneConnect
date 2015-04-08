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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//#include "XPLMPlanes.h"
#include "XPLMProcessing.h"
#include "XPLMGraphics.h"
#include "xpcDrawing.h"
#include "xpcPluginTools.h"

#ifdef _WIN32 /* WIN32 SYSTEM */
#include <windows.h>
#elif (__APPLE__)
#include <mach/mach_time.h>
#endif

#define MAXCONN 50 // Maximum number of dedicated connections
#define RECVPORT 49009 // Port that the plugin receives commands on
#define SENDPORT 49097 // Port that the plugin sends on
#define OPS_PER_CYCLE 20 // Max Number of operations per cycle

static XPLMDataRef      XPLMSwitch; // for turning on/off simulation
struct xpcSocket        recSocket;
struct xpcSocket        sendSocket;

short					number_of_connections = 0;
short					current_connection = -1;

double					start,lap;
static double			timeConvert = 0.0;
int						benchmarkingSwitch = 0; // 1 = time for operations, 2 = time for op + cycle;
int						debugSwitch = 2;
int						cyclesToClear = -1; // Clear message bus every n cycles. -1 == dont clear
int						counter = 0;

struct connectionHistory
{
	short					connectionID;
	
	char					IP[16];
	unsigned short			recPort;
	unsigned short			fromPort;
	short					requestLength;
	XPLMDataRef				XPLMRequestedDRefs[100];
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
int handleWYPT();
int handleGETD(char *buf);
int handleDREF(char *buf);
int handleVIEW();
int handleDATA(char *buf, int buflen);
int handleTEXT(char *buf, int len);
short handleInput(struct XPCMessage * theMessage);

char setPOSI(short aircraft, float pos[3]);
char setORIENT(short aircraft, float orient[3]);
char setDREF(XPLMDataRef theDREF, float floatarray[], short arrayStart, short arraySize);
char setGEAR(short aircraft, float gear, char posi);
char setFLAP(float flap);
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
		
	updateLog("[EXEC] xpcPlugin Start", 22);
	
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
	
	// Build the DataRef Array
	buildXPLMDataRefs();
    
    //On/Off Switch for simulation
	XPLMSwitch = XPLMFindDataRef("sim/operation/override/override_planepath");
	
	XPLMRegisterFlightLoopCallback(
								   MyFlightLoopCallback,	/* Callback */
								   -1,						/* Interval (s)*/
								   NULL);					/* refcon not used. */
	
	return 1;
}

PLUGIN_API void	XPluginStop(void)
{
	char logmsg[100] = "[EXEC] xpcPlugin Shutdown";
	XPLMUnregisterFlightLoopCallback(MyFlightLoopCallback, NULL);
	updateLog(logmsg,strlen(logmsg));
}

PLUGIN_API void XPluginDisable(void)
{
	char logmsg[100] = "[EXEC] xpcPlugin Disabled, sockets closed";
	closeUDP(recSocket);
	closeUDP(sendSocket);
	updateLog(logmsg,strlen(logmsg));

	XPCClearMessage();
}

PLUGIN_API int XPluginEnable(void)
{
	char logmsg[100] = "[EXEC] xpcPlugin Enabled, sockets opened";
	char IP[16] = "127.0.0.1";
	recSocket = openUDP(RECVPORT, IP, 49009);
    sendSocket = openUDP(SENDPORT, IP, 49099);
	updateLog(logmsg,strlen(logmsg));
	memset(logmsg,0,strlen(logmsg));
	
	if (benchmarkingSwitch>0)
	{
		sprintf(logmsg,"[EXEC] Benchmarking Enabled (Verbosity: %i)",benchmarkingSwitch);
		updateLog(logmsg,strlen(logmsg));
		memset(logmsg,0,strlen(logmsg));
	}
	
	if (debugSwitch>0)
	{
		sprintf(logmsg,"[EXEC] Debug Enabled (Verbosity: %i)",debugSwitch);
		updateLog(logmsg,strlen(logmsg));
	}
	
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
	char logmsg[100] = {0};
#if (__APPLE__)
	double diff_t;
#endif
	XPCMessage theMessage;
	
	counter++;
	if (benchmarkingSwitch > 1)
	{
		sprintf(logmsg,"Cycle time %.6f",inElapsedSinceLastCall);
		updateLog(logmsg,strlen(logmsg));
		memset(logmsg,0,strlen(logmsg));
	}
	
	for (i=0;i<OPS_PER_CYCLE;i++)
	{
		if (benchmarkingSwitch > 0)
		{
#if (__APPLE__)
			start = (double)mach_absolute_time( ) * timeConvert;
#endif
		}
		readMessage(&recSocket, &theMessage);
		result = handleInput(&theMessage);

		if (benchmarkingSwitch > 0)
		{
#if (__APPLE__)
			lap = (double)mach_absolute_time( ) * timeConvert;
			diff_t = lap-start;
			sprintf(logmsg,"Runtime %.6f",diff_t);
			updateLog(logmsg,strlen(logmsg));
			memset(logmsg,0,strlen(logmsg));
#endif
		}
		
		if (result<=0) break; // No Signal
	}
	
	if (cyclesToClear != -1)
	{
		if (counter%cyclesToClear==0)
		{
			strcpy(logmsg,"[EXEC] Cleared UDP Buffer");
			updateLog(logmsg,strlen(logmsg));
			char IP[16] = "127.0.0.1";
			closeUDP(recSocket);
			recSocket = openUDP(RECVPORT, IP, 49009);
			
		}
	}
	return -1;
}

short handleInput(struct XPCMessage * theMessage)
{
	int i;
	char logmsg[100] = {0};
	char IP[16] = {0};
	unsigned short port = 0;
	
	current_connection = -1;
	
    if (theMessage->msglen > 0)  // If message received
	{
		if (debugSwitch>0)
		{
#ifdef _WIN32
#else
			printBufferToLog(*theMessage);
#endif
		}
		
		// Check for existing connection
		port = getIP(theMessage->recvaddr,IP);
		for (i=0; i<number_of_connections; i++)
		{
			if (strcmp(connectionList[i].IP,IP) == 0 && port == connectionList[i].fromPort && port > 0)
			{
				current_connection=i;
				break;
			}
		}
		
		if (current_connection == -1) //SETUP NEW CONNECTION
		{
			if (number_of_connections>=MAXCONN) // Handle hitting MAXCONN (COME UP WITH A BETTER SOLUTION)
			{
				updateLog("[EXEC] Hit maximum number of connections-Removing old ones",58);
				number_of_connections = 0;
			}
			
			memcpy(connectionList[number_of_connections].IP,IP,16);
			connectionList[number_of_connections].recPort = 49008;
			connectionList[number_of_connections].fromPort = port;
			current_connection = number_of_connections;
			number_of_connections ++;
			
			// Log Connection
			sprintf(logmsg,"[EXEC] New Connection [%i].  IP=%s, port=%i",number_of_connections,IP, port);
			updateLog(logmsg,strlen(logmsg));
			memset(logmsg,0,strlen(logmsg));
		}
		
        if (strncmp(theMessage->head,"CONN",4)==0) // Header = CONN (Connection)
		{
			handleCONN(theMessage->msg);
		}
		else if (strncmp(theMessage->head,"SIMU",4)==0) // Header = SIMU
		{
			handleSIMU(theMessage->msg);
		}
		else if (strncmp(theMessage->head,"POSI",4)==0) // Header = POSI (Position)
		{
			handlePOSI(theMessage->msg);
		}
		else if (strncmp(theMessage->head,"CTRL",4)==0) // Header = CTRL (Control)
		{
			handleCTRL(theMessage->msg);
		}
		else if (strncmp(theMessage->head,"WYPT",4)==0) // Header = WYPT (Waypoint Draw)
		{
			handleWYPT();
		}
		else if (strncmp(theMessage->head,"GETD",4)==0) // Header = GETD (Data Request)
		{
			handleGETD(theMessage->msg);
		}
		else if (strncmp(theMessage->head,"DREF",4)==0) // Header = DREF (By Data Ref) (this is slower than DATA)
		{
			handleDREF(theMessage->msg);
		}
		else if (strncmp(theMessage->head,"VIEW",4)==0) // Header = VIEW (Change View)
		{
			handleVIEW();
		}
		else if (strncmp(theMessage->head,"DATA",4)==0) // Header = DATA (UDP Data)
		{
			handleDATA(theMessage->msg, theMessage->msglen);
		}
		else if (strncmp(theMessage->head, "TEXT", 4) == 0) // Header = TEXT (Screen message)
		{
			handleTEXT(theMessage->msg, theMessage->msglen);
		}
		else if ((strncmp(theMessage->head,"DSEL",4)==0) || (strncmp(theMessage->head,"USEL",4)==0)) // Header = DSEL/USEL (Select UDP Send)
		{
			sendBUF(theMessage->msg,theMessage->msglen); // Send to UDP
		}
		else if ((strncmp(theMessage->head,"DCOC",4)==0) || (strncmp(theMessage->head,"UCOC",4)==0))
		{
			sendBUF(theMessage->msg,theMessage->msglen); // Send to UDP
		}
		else if ((strncmp(theMessage->head,"MOUS",4)==0) || (strncmp(theMessage->head,"CHAR",4)==0))
		{
			sendBUF(theMessage->msg,theMessage->msglen); // Send to UDP
		}
		else if (strncmp(theMessage->head,"MENU",4)==0)
		{
			sendBUF(theMessage->msg,theMessage->msglen); // Send to UDP
		}
		else if (strncmp(theMessage->head,"SOUN",4)==0)
		{
			sendBUF(theMessage->msg,theMessage->msglen); // Send to UDP
		}
		else if ((strncmp(theMessage->head,"FAIL",4)==0) || (strncmp(theMessage->head,"RECO",4)==0))
		{
			sendBUF(theMessage->msg,theMessage->msglen); // Send to UDP
		}
		else if (strncmp(theMessage->head,"PAPT",4)==0)
		{
			sendBUF(theMessage->msg,theMessage->msglen); // Send to UDP
		}
		else if ((strncmp(theMessage->head,"VEHN",4)==0) || (strncmp(theMessage->head,"VEH1",4)==0) || (strncmp(theMessage->head,"VEHA",4)==0))
		{
			sendBUF(theMessage->msg,theMessage->msglen); // Send to UDP
		}
		else if ((strncmp(theMessage->head,"OBJN",4)==0) || (strncmp(theMessage->head,"OBJL",4)==0))
		{
			sendBUF(theMessage->msg,theMessage->msglen); // Send to UDP
		}
		else if ((strncmp(theMessage->head,"GSET",4)==0) || (strncmp(theMessage->head,"ISET",4)==0))
		{
			sendBUF(theMessage->msg,theMessage->msglen); // Send to UDP
		}
		else if (strncmp(theMessage->head, "BOAT", 4) == 0)
		{
			sendBUF(theMessage->msg, theMessage->msglen); // Send to UDP
		}
		else
		{ //unrecognized header
			sprintf(logmsg,"[EXEC] ERROR: Command %s not recognized",theMessage->head);
			updateLog(logmsg, strlen(logmsg));
		}
		current_connection = -1;
    } // end if (buflen > 0)
	
	return theMessage->msglen;
}

void sendBUF( char buf[], int buflen)
{
	char IP[16] = "127.0.0.1";
	memcpy(sendSocket.xpIP,IP,16);
	sendSocket.xpPort = 49000;
	sendUDP(sendSocket,buf,buflen);
}

int handleCONN(char buf[])
{
	char logmsg[100] = {0};
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
		return updateLog("[CONN] ERROR: Port Number must be a number (NaN received)",51);
	}
	
	
	// UPDATE LOG
	sprintf(logmsg,"[CONN] Update Connection %i- Sending to port: %i",current_connection+1,connectionList[current_connection].recPort);
	updateLog(logmsg,strlen(logmsg));
	
	// SEND CONFIRMATION
	// TODO: Ivestigate why sending confirmation causes crashes on Windows 8
	//memcpy(sendSocket.xpIP,connectionList[current_connection].IP, sizeof(connectionList[current_connection].IP));
	//sendSocket.xpPort = connectionList[current_connection].recPort;
	//sendUDP(sendSocket, the_message, 5);
	
	return 0;
}

int handleSIMU(char buf[])
{
	int SIMUArray[1] = {buf[5]};
	char logmsg[100] = {0};
	
	if (SIMUArray[0] != SIMUArray[0]) // Is NaN
	{
		return updateLog("[SIMU] ERROR: Value must be a number (NaN received)",51);
	}
	
	XPLMSetDatavi(XPLMSwitch, SIMUArray, 0, 1);
	
	if (buf[5] == 0)
	{
		sprintf(logmsg,"[SIMU] Simulation Resumed (Conn %i)", current_connection+1);
		updateLog(logmsg,strlen(logmsg));
	}
	else
	{
		sprintf(logmsg,"[SIMU] Simulation Paused (Conn %i)", current_connection+1);
		updateLog(logmsg,strlen(logmsg));
	}
	
	return 0;
}

int handleTEXT(char *buf, int len)
{
	char msg[256] = { 0 };
	if (len < 14)
	{
		updateLog("[TEXT] ERROR: Length less than 14 bytes", 39);
		return -1;
	}
	size_t msgLen = (unsigned char)buf[13];
	if (msgLen == 0)
	{
		XPCClearMessage();
		updateLog("[TEXT] Text cleared", 19);
	}
	else
	{
		int x = *((int*)(buf + 5));
		int y = *((int*)(buf + 9));
		strncpy(msg, buf + 14, msgLen);
		XPCSetMessage(x, y, msg);
		updateLog("[TEXT] Text set", 15);
	}
	return 0;
}

char setDREF(XPLMDataRef theDREF, float floatarray[], short arrayStart, short arraySize)
{
	XPLMDataTypeID dataType;
	short i=arrayStart;
	
	if ((floatarray[i]<-997.5 && floatarray[i]>-999.5))
	{
		return 0; // Do not change
	}
	
	if (theDREF == XPLMDataRefs[0][0])
	{
		return -1;
	}
	
	if (theDREF) // VALID POINTER
	{
		if (floatarray[i] != floatarray[i]) // Is NaN
		{
			goto NANMessage;
		}
		
		dataType = XPLMGetDataRefTypes(theDREF);
		switch (dataType)
		{
			{case 1: //Integer
				XPLMSetDatai(theDREF,(int) floatarray[i]);
				break;}
				
			{case 4: //Double
				XPLMSetDatad(theDREF,(double) floatarray[i]);
				break;}
				
			{case 8: //Float Array
				fmini(XPLMGetDatavf(theDREF,NULL,0,8),arraySize); //find size of array
				if ( floatarray[0] != floatarray[0] ) // NaN Check
				{
					goto NANMessage;
				}
				
				XPLMSetDatavf(theDREF,floatarray,arrayStart,arraySize);
				break;}
				
			{case 16: //Integer Array
				int intArray[20];
				short length;
				length = fmini(XPLMGetDatavi(theDREF,NULL,0,8),arraySize); //find size of array
				for (i=arrayStart; i < arrayStart+length; i++)
				{
					intArray[i] = (int) floatarray[i];
				}
				XPLMSetDatavi(theDREF,intArray,0,length);
				break;}
				
			{default: //Float
				XPLMSetDataf(theDREF,floatarray[i]);
				break;}
		}
	}
	else
		return updateLog("[DREF] ERROR: invalid DREF",26);
	
	return 0;
	
NANMessage:
	return updateLog("[DREF] ERROR: Value must be a number (NaN received)",51);
}

char setGEAR(short aircraft, float gear, char posi)
{
	int i;
	float gearArray[8];
	
	if ((gear < - 8.5f && gear > 9.5f) || (gear < -997.9f && gear > -999.1f))
	{
		return -1; // Don't change command
	}
	
	if ( ( gear != gear ) || ( gear < -1.f ) || ( gear > 1.f ) ) // NaN & Positive test
	{
		return updateLog("[GEAR] ERROR: Value must be 0 or 1",51);
	}
	
	for (i=0;i<8;i++)
	{
		gearArray[i] = gear;
	}
	
	if (!aircraft)
	{ // Own Aircraft
		setDREF(XPLMDataRefs[14][7], gearArray, 0, 8);
	}
	else
	{ // Multiplayer
		setDREF(multiplayer[aircraft][6], gearArray, 0, 8);
	}
	
	if (posi)
	{
		XPLMSetDatavf(XPLMDataRefs[14][0], gearArray, 0, 1);
	}
	
	return 0;
}

char setPOSI(short aircraft, float pos[3])
{
	double local[3] = {0};
	int i;
	float tPos = pos[0] + pos[1] + pos[2];
	
	if (tPos != tPos) // Is NaN
	{
		return updateLog("[POSI] ERROR: Position must be a number (NaN received)",51);
	}
	
	XPLMWorldToLocal(pos[0],pos[1],pos[2],&local[0],&local[1],&local[2]);
	if (aircraft <= 0)
	{ // Main aircraft
		for (i=0; i<3; i++)
		{
			XPLMSetDatad(XPLMDataRefs[21][i],local[i]);
			XPLMSetDatad(XPLMDataRefs[20][i], pos[i]);
		}
	}
	else
	{ // Multiplayer
		for (i=0; i<3; i++)
		{
			XPLMSetDatad(multiplayer[aircraft][i], local[i]);
		}
	}
	
	return 0;
}

char setORIENT(short aircraft, float orient[3])
{
	int i;
	float q[4] = {0};
	float pi = (float) 0.00872664625997; // 1/2 rad
	float tOrient = orient[0] + orient[1] + orient[2];
	
	if ( tOrient != tOrient ) // Is NaN
	{
		return updateLog("[ORIENT] ERROR: Orientation must be a number (NaN received)",53);
	}
	
	if ( aircraft <= 0 ) // Main aircraft
	{
		XPLMSetDataf(XPLMDataRefs[17][0],orient[0]);
		XPLMSetDataf(XPLMDataRefs[17][1],orient[1]);
		XPLMSetDataf(XPLMDataRefs[17][2],orient[2]);
		
		//Convert to Quartonians (from: http://www.xsquawkbox.net/xpsdk/mediawiki/MovingThePlane), http://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770024290.pdf pA2)
		orient[2] = pi * orient[2]; // 1/2 raidians
		orient[0] = pi * orient[0]; // 1/2 raidians
		orient[1] = pi * orient[1]; // 1/2 raidians
		q[0] =  cos(orient[2]) * cos(orient[0]) * cos(orient[1]) + sin(orient[2]) * sin(orient[0]) * sin(orient[1]);
		q[1] =  cos(orient[2]) * cos(orient[0]) * sin(orient[1]) - sin(orient[2]) * sin(orient[0]) * cos(orient[1]);
		q[2] =  cos(orient[2]) * sin(orient[0]) * cos(orient[1]) + sin(orient[2]) * cos(orient[0]) * sin(orient[1]);
		q[3] =  sin(orient[2]) * cos(orient[0]) * cos(orient[1]) - cos(orient[2]) * sin(orient[0]) * sin(orient[1]);
		
		XPLMSetDatavf(XPLMDataRefs[17][4],q,0,4);
	}
	else
	{
		for (i=0; i<3; i++)
		{
			XPLMSetDataf(multiplayer[aircraft][i+3],orient[i]);
		}
	}
	
	return 0;
}

char setFLAP(float flap)
{
	float floatArray[1] = {flap};
	setDREF(XPLMDataRefs[13][3],floatArray,0,1);
	setDREF(XPLMDataRefs[13][4],floatArray,0,1);
	
	return 0;
}

int handlePOSI(char buf[])
{
	char logmsg[100];
	float position[8] = {0.0};
	float pos[3],orient[3];
	short aircraft = 0;
	float gear = -1.0;
	int autopilot[20] = {0};
	
	// UPDATE LOG
	sprintf(logmsg,"[POSI] Message Received (Conn %i)", current_connection+1);
	updateLog(logmsg, strlen(logmsg));
	
	aircraft = fmini(parsePOSI(buf,position,6, &gear),19);
	
	//ADD AIRCRAFT HANDLING- (-1 = single aircraft for player)
	if (aircraft > 0) // Multiplayer aircraft
	{
		XPLMGetDatavi(AIswitch, autopilot, 0, 20);
		autopilot[aircraft] = 1;
		XPLMSetDatavi(AIswitch, autopilot, 0, 20);
	}
	
	// Position
	memcpy(pos,position,3*sizeof(float));
	setPOSI(aircraft, pos);
	
	// Orientation
	memcpy(orient,&position[3],3*sizeof(float));
	setORIENT(aircraft, orient);
	
	//Landing Gear
	if (gear != -1)
	{
		setGEAR(aircraft, gear, 1);
	}
	
	return 0;
}

int handleCTRL(char buf[])
{
	char logmsg[100];
	xpcCtrl ctrl;
	float thr[8] = { 0 };
	short i;
	
	// UPDATE LOG
	sprintf(logmsg,"[CTRL] Message Received (Conn %i)", current_connection+1);
	updateLog(logmsg, strlen(logmsg));
	
	ctrl = parseCTRL(buf);
	if (ctrl.aircraft < 0) //parseCTRL failed
	{
		return 1;
	}
	if (ctrl.aircraft > 19) //Can only handle 19 non-player aircraft right now
	{
		return 2;
	}
	if (ctrl.aircraft == 0) //player aircraft
	{
		// SET CONTROLS
		XPLMSetDataf(XPLMDataRefs[11][0], ctrl.pitch);
		XPLMSetDataf(XPLMDataRefs[11][1], ctrl.roll);
		XPLMSetDataf(XPLMDataRefs[11][2], ctrl.yaw);

		// SET Throttle
		for (i = 0; i<8; i++)
		{
			thr[i] = ctrl.throttle;
		}
		XPLMSetDatavf(XPLMDataRefs[25][0], thr, 0, 8);
		XPLMSetDatavf(XPLMDataRefs[26][0], thr, 0, 8);
		setDREF(XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro_override"), thr, 0, 1);

		// SET Gear/Flaps
		if (ctrl.gear != -1)
		{
			setGEAR(0, ctrl.gear, 0); // Gear
		}
		if (ctrl.flaps < -999.5 || ctrl.flaps > -997.5) // Flaps
		{
			XPLMSetDataf(XPLMDataRefs[13][3], ctrl.flaps);
		}
	}
	else //non-player aircraft
	{
		// SET CONTROLS
		XPLMSetDataf(multiplayer[ctrl.aircraft][14], ctrl.pitch);
		XPLMSetDataf(multiplayer[ctrl.aircraft][15], ctrl.roll);
		XPLMSetDataf(multiplayer[ctrl.aircraft][16], ctrl.yaw);

		// SET Throttle
		for (i = 0; i<8; i++)
		{
			thr[i] = ctrl.throttle;
		}
		XPLMSetDatavf(multiplayer[ctrl.aircraft][13], thr, 0, 8);

		// SET Gear/Flaps
		if (ctrl.gear != -1)
		{
			float gear[10];
			for (int i = 0; i < 10; ++i)
			{
				gear[i] = ctrl.gear;
			}
			XPLMSetDatavf(multiplayer[ctrl.aircraft][6], gear, 0, 10);
		}
		if (ctrl.flaps < -999.5 || ctrl.flaps > -997.5) // Flaps
		{
			XPLMSetDataf(multiplayer[ctrl.aircraft][7], ctrl.flaps);
			XPLMSetDataf(multiplayer[ctrl.aircraft][8], ctrl.flaps);
		}
	}	
	return 0;
}

int handleWYPT()
{
	char logmsg[100];
	
	// UPDATE LOG
	sprintf(logmsg,"[WYPT] Message Received (Conn %i)- WAYPOINT DRAWING FEATURE UNDER CONSTRUCTION", current_connection+1);
	updateLog(logmsg, strlen(logmsg));
	
	return 0;
}

int handleGETD(char buf[])
{
	int length,i,k;
	XPLMDataTypeID dataType;
	char logmsg[400] = {0};
	int listLength = buf[5];
	char the_message[5000];
	char header[5] = {0};
	int count = 6;
	float values[100] = {-998};
	int intArray[100] = {-998};
	int DREFSizes[100] = {0};
	char *DREFArray[100];
	
	strncpy(header,"RESP",4);
	memcpy(&the_message,&header,4);
	
	if (listLength == 0) // USE LAST REQUEST
	{
		sprintf(logmsg,"[GETD] DATA Requested- repeat last request from connection %i (%i data refs)",current_connection+1,connectionList[current_connection].requestLength);
		
		if (connectionList[current_connection].requestLength < 0) // No past requests
		{
			updateLog(logmsg, strlen(logmsg));
			memset(logmsg,0,strlen(logmsg));
			sprintf(logmsg,"[GETD] ERROR- No previous requests from connection %i.",current_connection+1);
			return updateLog(logmsg, strlen(logmsg));
		}
	}
	else if (listLength > 0) // NEW REQUEST
	{
		connectionList[current_connection].requestLength = (short) listLength;
		
		for (int i = 0; i < connectionList[current_connection].requestLength; i++)
		{
			DREFArray[i] = (char *) malloc(100);
			memset(DREFArray[i],0,100);
		}
		
		parseGETD(buf,DREFArray,DREFSizes);
		sprintf(logmsg,"[GETD] DATA Requested- New Request for connection %i [%i]:",current_connection+1,listLength);
	}
	else
	{
		return -1;
	}
	
	// Update Log
	updateLog(logmsg,strlen(logmsg));
	memset(logmsg,0,strlen(logmsg));
	
	for (i=0;i<connectionList[current_connection].requestLength;i++)
	{
		if (listLength > 0)
		{
			connectionList[current_connection].XPLMRequestedDRefs[i] = XPLMFindDataRef(DREFArray[i]);
		}
		
		if (connectionList[current_connection].XPLMRequestedDRefs[i]) //Valid Pointer
		{
			dataType = XPLMGetDataRefTypes(connectionList[current_connection].XPLMRequestedDRefs[i]);
			switch (dataType)
			{
				{case 1: //Integer
					length = 1;
					values[0] = (float) XPLMGetDatai(connectionList[current_connection].XPLMRequestedDRefs[i]);
					break;}
					
				{case 4: //Double
					length = 1;
					values[0] = (float) XPLMGetDatad(connectionList[current_connection].XPLMRequestedDRefs[i]);
					break;}
					
				{case 8: //Float Array
					length = XPLMGetDatavf(connectionList[current_connection].XPLMRequestedDRefs[i],NULL,0,8); //find size of array
					XPLMGetDatavf(connectionList[current_connection].XPLMRequestedDRefs[i],values,0,fminl(length,100));
					break;}
					
				{case 16: //Integer Array
					length = XPLMGetDatavi(connectionList[current_connection].XPLMRequestedDRefs[i],NULL,0,8); //find size of array
					XPLMGetDatavi(connectionList[current_connection].XPLMRequestedDRefs[i],intArray,0,fminl(length,100));
					for (k=0; k < length; k++) {
						values[k]=(float) intArray[k];
					}
					break;}
					
				{default: //Float
					length = 1;
					values[0] = XPLMGetDataf(connectionList[current_connection].XPLMRequestedDRefs[i]);
					break;}
			}
			the_message[count] = length;
			memcpy(&the_message[count+1],&values,length*sizeof(float));
			count += 1 + length*sizeof(float);
		}
		else
		{
			sprintf(logmsg,"%s-ERROR: invalid DREF",DREFArray[i]);
			updateLog(logmsg, strlen(logmsg));
		}
	}
	the_message[5] = (char) connectionList[current_connection].requestLength;
	
	memcpy(sendSocket.xpIP,connectionList[current_connection].IP, sizeof(connectionList[current_connection].IP));
	sendSocket.xpPort = connectionList[current_connection].recPort;
	
	if (count > 6)
	{
		sendUDP(sendSocket, the_message, count);
	}
		
	return 0;
}

int handleDREF(char buf[])
{
	char logmsg[400]={0};
	
	char DREF[100] = {0};
	unsigned short lenDREF = 0;
	unsigned short lenVALUE = 0;
	float values[40] = {0.0};
	XPLMDataRef theDREF;
	
	parseDREF(buf, DREF, &lenDREF,values,&lenVALUE);
	
	// Input Varification
	if (lenDREF <= 0)
	{
		return 1;
	}
	
	// Handle DREF
	sprintf(logmsg,"[DREF] Request to set DREF value received (Conn %i): %s",current_connection+1,DREF);
	updateLog(logmsg,strlen(logmsg));
	
	theDREF = XPLMFindDataRef(DREF);
	setDREF(theDREF, values, 0, lenVALUE);
	
	return 0;
}

int handleVIEW()
{
	char logmsg[100];
	
	// UPDATE LOG
	sprintf(logmsg,"[VIEW] Message Received (Conn %i)- VIEW FEATURE UNDER CONSTRUCTION",current_connection+1);
	updateLog(logmsg, strlen(logmsg));
	
	return 0;
}

int handleDATA(char buf[], int buflen)
{
	int i,j,lines;
	char logmsg[100] = {0};
	float floatArray[10] = {0};
	float recValues[20][9] = {0};
	const float deg2rad = (float) 0.0174532925;
	float savedAlpha=-998;
	float savedHPath=-998;
	
	lines = parseDATA(buf, buflen, recValues); //Parse Data
	if (lines > 0)
	{
		// UPDATE LOG
		sprintf(logmsg,"[DATA] Message Received (Conn %i)",current_connection+1);
		updateLog(logmsg, strlen(logmsg));
		memset(logmsg,0,strlen(logmsg));
	}
	else
	{
		// UPDATE LOG
		sprintf(logmsg,"[DATA] WARNING: Empty data packet received (Conn %i)",current_connection+1);
		return updateLog(logmsg, strlen(logmsg));
	}
	
	for (i = 0; i<lines; i++) //Execute changes one line at a time
	{
		short dataRef = (short) recValues[i][0];
		
		if ( dataRef<0 || dataRef >134 )
		{   // DREF Check 1: This ensures that the received dataRef is in the range of 0-134
			sprintf(logmsg,"[DATA] ERROR: DataRef # must be between 0-134 (Rec: %hi)",dataRef);
			updateLog(logmsg,strlen(logmsg));
			memset(logmsg,0,strlen(logmsg));
			continue;
		}
		
		switch (dataRef)
		{
			{case 3: // Velocity
				// Velocity has to be broken into individual components in X-Plane World Coordinate System where x=East, y=Up.
				//Note: Add Sideslip
				
				float theta, alpha, hpath,v;
				int ind[3] = {1,3,4};
				theta = XPLMGetDataf(XPLMDataRefs[17][0]); //Theta
				
				if (savedAlpha != -998)
					alpha = savedAlpha;
				else
					alpha = XPLMGetDataf(XPLMDataRefs[18][0]); //Alpha
				
				if (savedHPath != -998)
					hpath = savedHPath;
				else
					hpath = XPLMGetDataf(XPLMDataRefs[18][2]); //Velocity Heading
				
				if ( ( hpath != hpath ) && ( alpha != alpha ) && ( theta != theta ) ) // NaN Check
				{
					updateLog("[DATA] ERROR: Value must be a number (NaN received)",51);
					break;
				}
				
				for (j=0;j<sizeof(ind)/sizeof(ind[0]);j++)
				{
					if (recValues[i][ind[j]] != -998)
					{
						v = recValues[i][ind[j]];
						
						XPLMSetDataf(XPLMDataRefs[21][3],v*cos((theta-alpha)*deg2rad)*sin(hpath*deg2rad));
						XPLMSetDataf(XPLMDataRefs[21][4],v*sin((theta-alpha)*deg2rad));
						XPLMSetDataf(XPLMDataRefs[21][5],-v*cos((theta-alpha)*deg2rad)*cos(hpath*deg2rad));
					}
				}
				break;}
			{case 17: // Orientation
				float orient[3] = {0};
				
				for (j=1; j<4; j++)
					if (recValues[i][j] == -998)
						recValues[i][j] = XPLMGetDataf(XPLMDataRefs[17][j-1]);
				
				memcpy(orient,&recValues[i][1],3*sizeof(float));
				
				setORIENT(0, orient);
				
				break;}
				
			{case 18: // Alpha, hpath etc.
				if ( ( recValues[i][1] != recValues[i][1] ) && ( recValues[i][3] != recValues[i][3] ) ) // NaN Check
	 			{
					updateLog("[DATA] ERROR: Value must be a number (NaN received)",51);
					break;
				}
				
				if (recValues[i][1] != -998)
					savedAlpha = recValues[i][1];
				
				if (recValues[i][3] != -998)
					savedHPath = recValues[i][3];
				
				break;}
				
			{case 20: // Position
				float local[3] = {0};
				
				for (j=1; j<4; j++)
					if (recValues[i][j+1] == -998)
						recValues[i][j+1] = XPLMGetDataf(XPLMDataRefs[20][j]);
				
				memcpy(local,&recValues[i][1],3*sizeof(float));
				setPOSI(0, local);
				
				break;}
				
			{case 25: // Throttle
				if ( recValues[i][1] != recValues[i][1] )
				{
					updateLog("[DATA] ERROR: Value must be a number (NaN received)",51);
					break;
				}
				
				for (j=0; j<8; j++)
					floatArray[j] = recValues[i][1];
				
				XPLMSetDatavf(XPLMDataRefs[25][0],floatArray,0,8);
				break;}
				
			{default: // Non-Special dataRefs (everything else)
				memcpy(floatArray,&recValues[i][1],8*sizeof(float));
				for (j=0; j<8; j++)
				{
					if (debugSwitch > 0)
					{
						char theString[100] = {0};
						sprintf(theString,"Setting Dataref %i.%i to %f",dataRef,j,floatArray[j]);
						updateLog(theString, strlen(theString));
					}
					
					if (dataRef==14 && j==0)
					{
						setGEAR(0, recValues[i][j+1], 1); // Landing Gear
						continue;
					}
					
					// Set DATAREF
					if (setDREF(XPLMDataRefs[dataRef][j],floatArray,j,8) == -1)
						sendBUF(buf,buflen);
				} //End for j=1:8
				break;}
		} // End switch(dataRef)
	}// End for(all lines)
		
	return 0;
} // End handleDATA
