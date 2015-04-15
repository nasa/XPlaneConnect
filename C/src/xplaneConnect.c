//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
//
//DISCLAIMERS
//    No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, 
//    EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT
//    THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF
//    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY
//    THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED,
//    WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN
//    ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS,
//    HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT
//    SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING
//    THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
//
//    Waiver and Indemnity: RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES
//    GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF
//    RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES
//    OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING
//    FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE
//    UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT,
//    TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE
//    IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.

//  X-Plane Connect Client
//
//  DESCRIPTION
//      Communicates with the XPC plugin to facilitate controling and gathering data from X-Plane.
//
//  INSTRUCTIONS
//      See Readme.md in the root of this repository or the wiki hosted on GitHub at
//      https://github.com/nasa/XPlaneConnect/wiki for requirements, installation instructions,
//      and detailed documentation.
//
//  CONTACT
//      For questions email Christopher Teubert (christopher.a.teubert@nasa.gov)
//
//	CONTRIBUTORS
//		CT: Christopher Teubert (christopher.a.teubert@nasa.gov)
//		JW: Jason Watkins (jason.w.watkins@nasa.gov)
#include "xplaneConnect.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#ifdef _WIN32 /* WIN32 SYSTEM */
#include <WS2tcpip.h>

// From http://www.c-plusplus.de/forum/109539-full
void usleep(__int64 usec)
{
	HANDLE timer;
	LARGE_INTEGER ft;
		
	ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time
		
	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}
#endif

void printError(char *functionName, char *errorMessage);
short sendRequest(XPCSocket recfd, char DREFArray[][100], short DREFSizes[], short listLength);

void printError(char *functionName, char *errorMessage)
{
	printf("[%s] ERROR: %s\n", functionName, errorMessage);
}

/*****************************************************************************/
/****                       Low Level UDP functions                       ****/
/*****************************************************************************/
XPCSocket openUDP(unsigned short port, const char *xpIP, unsigned short xpPort)
{
	XPCSocket sock;
	
	// Setup Port
	struct sockaddr_in recvaddr;
	recvaddr.sin_family = AF_INET;
	recvaddr.sin_addr.s_addr = INADDR_ANY;
	recvaddr.sin_port = htons(port);
	
	// Set X-Plane Port and IP
	if (strcmp(xpIP, "localhost") == 0)
	{
		xpIP = "127.0.0.1";
	}
	strncpy(sock.xpIP, xpIP, 16);
	sock.xpPort = xpPort == 0 ? 49009 : xpPort;
	
#ifdef _WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printError("OpenUDP", "WSAStartup failed");
		exit(EXIT_FAILURE);
	}
#endif
	
	if ((sock.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		printError("OpenUDP", "Socket creation failed");
		exit(EXIT_FAILURE);
	}
	if (bind(sock.sock, (struct sockaddr*)&recvaddr, sizeof(recvaddr)) == -1)
	{
		printError("OpenUDP", "Socket bind failed");
		exit(EXIT_FAILURE);
	}

#ifdef _WIN32
	DWORD timeout = 1; // Minimum socket timeout in Windows is 1ms
#else
	struct timeval timeout;
	tv.tv_sec = 0;
	tv.tv_usec = 500;
#endif
	if (setsockopt(sock.sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
	{
		printError("OpenUDP", "Failed to set timeout");
	}	
	return sock;
}

void closeUDP(XPCSocket socketNumber)
{
#ifdef _WIN32
	int result = closesocket(socketNumber.sock);
#else
	int result = close(socketNumber.sock);
#endif
	if (result < 0)
	{
		printError("closeUDP", "Failed to close socket");
		exit(EXIT_FAILURE);
	}
}

int sendUDP(XPCSocket sock, char buffer[], int len)
{
	// Preconditions
	if (len <= 0)
	{
		printError("sendUDP", "Message length must be positive.");
		return -1;
	}
	
	// Set up destination address
	struct sockaddr_in dst;
	dst.sin_family = AF_INET;
	dst.sin_port = htons(sock.xpPort);
	inet_pton(AF_INET, sock.xpIP, &dst.sin_addr.s_addr);

	int result = sendto(sock.sock, buffer, len, 0, (const struct sockaddr*)&dst, sizeof(dst));
	if (result < 0)
	{
		printError("sendUDP", "Send operation failed.");
		return -2;
	}
	if (result < len)
	{
		printError("sendUDP", "Unexpected number of bytes sent.");
	}
	return result;
}

int readUDP(XPCSocket sock, char buffer[], int len, struct sockaddr* recvaddr)
{
	socklen_t recvaddrlen = sizeof(*recvaddr);
#ifdef _WIN32
	// Windows readUDP needs the select command- minimum timeout is 1ms.
	// Without this playback becomes choppy

	// Definitions
	FD_SET stReadFDS;
	FD_SET stExceptFDS;

	// Setup for Select
	FD_ZERO(&stReadFDS);
	FD_SET(sock.sock, &stReadFDS);
	FD_ZERO(&stExceptFDS);
	FD_SET(sock.sock, &stExceptFDS);

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 250;

	// Select Command
	int status = select(-1, &stReadFDS, (FD_SET*)0, &stExceptFDS, &tv);
	if (status < 0)
	{
		printError("readUDP", "Select command error");
		return -1;
	}
	if (status == 0)
	{
		// No data
		return 0;
	}
	status = recvfrom(sock.sock, buffer, len, 0, (struct sockaddr*)&recvaddr, &recvaddrlen);
#else
	// For apple or linux-just read - will timeout in 0.5 ms
	status = (int)recvfrom(sock.sock, dataRef, len, 0, recvaddr, &recvaddrlen);
#endif
	if (status < 0)
	{
		printError("readUDP", "Error reading socket");
	}
	return status;
}
/*****************************************************************************/
/****                    End Low Level UDP functions                      ****/
/*****************************************************************************/

short sendDATA(XPCSocket recfd, float dataRef[][9], unsigned short rows)
{
	int i;
	unsigned short length = rows*9*sizeof(float) + 5;
	char message[5000];
	unsigned short step = 9*sizeof(float);
	
	strncpy(message,"DATA",4);
	
	for (i=0;i<rows;i++)
	{
		message[5+i*step] = (char) dataRef[i][0];
		message[6+i*step] = (char) 0;
		message[7+i*step] = (char) 0;
		message[8+i*step] = (char) 0;
		memcpy(&message[9+i*step], &dataRef[i][1], 8*sizeof(float));
	}
	
	sendUDP(recfd, message, length);
	
	return 0;
}

short sendDREF(XPCSocket recfd, const char *dataRef, unsigned short length_of_DREF, float *values, unsigned short number_of_values) {
	char message[5000];
	int length = 7+length_of_DREF+number_of_values*sizeof(float);
	
	//HEADER
	strncpy(message,"DREF",4);
	
	//DREF
	message[5] = (char) length_of_DREF;
	memcpy(&message[6],dataRef,length_of_DREF);
	
	//VALUES
	message[length_of_DREF+6] = (char) number_of_values;
	memcpy(&message[length_of_DREF+7],&values[0],sizeof(float)*number_of_values);
	
	sendUDP(recfd, message, length);
	return 0;
}

short sendRequest(XPCSocket recfd, char DREFArray[][100], short DREFSizes[], short listLength)
{
	int i;
	char message[5000]={0};
	int length = 6;
	
	//HEADER
	strncpy(message,"GETD",4);
	
	message[5] = (char) listLength; //Number of Values
	
	//The rest
	for (i=0; i < listLength; i++)
	{
		message[length] = (char) DREFSizes[i];
		memcpy(&message[length+1],DREFArray[i],message[length]);
		length += message[length] + 1;
	}
	
	sendUDP(recfd, message, length);
	
	return 0;
}

short requestDREF(XPCSocket sendfd, XPCSocket recfd, char DREFArray[][100], short DREFSizes[], short listLength,  float *resultArray[], short arraySizes[])
{
	int i;
	
	sendRequest(sendfd, DREFArray, DREFSizes, listLength);
	
	for (i=0; i<80; i++)
	{
		int size = readDREF(recfd, resultArray, arraySizes);
		if (size != -1) // Received input
		{
			return size;
		}
	}
	
	return -1;
}

short pauseSim(XPCSocket recfd, short pause)
{
	char message[7];
	
	//Preconditions
	if (pause != 0 && pause != 1)
	{
		return errorReport("pauseSim", "Please set pause to 0 or 1");
	}
	
	strncpy(message,"SIMU",4);
	message[5] = (char) pause;
	message[6] = 0;
	
	sendUDP(recfd, message,6);
	
	return 0;
}

short setCONN(XPCSocket recfd,  unsigned short recPort)
{
	char message[10] = {0};
	int length;
	strncpy(message,"CONN",4);
	
	memcpy(&message[5],&recPort,sizeof(recPort));
	
	length = 5 + sizeof(recPort);
	
	sendUDP(recfd,message, length);
	
	return 0;
}

short sendPOSI(XPCSocket recfd, short ACNum, short numArgs, float valueArray[])
{
	char message[40] = {0};
	int i;
	short position = 6;
	
	// Input Verification
	if (numArgs < 1)
	{
		return errorReport("POSI", "Must have atleast one argument");
	}
	
	// Header
	strncpy(message,"POSI",4);
	
	// Aircraft
	message[5] = (char) ACNum;
	
	// States
	
	for (i=0; i<7; i++)
	{
		float val = -998.5;
		
		if ( i < numArgs )
		{
			val = valueArray[i];
		}
		
		memcpy(&message[position],&val,sizeof(float));
		position += sizeof(float);
	}
	
	sendUDP(recfd, message, position);
	return 0;
}

short sendCTRL(XPCSocket recfd, short numArgs, float valueArray[])
{
	return sendpCTRL(recfd, numArgs, valueArray, 0);
}

short sendpCTRL(XPCSocket recfd, short numArgs, float valueArray[], char acNum)
{
	char message[27] = { 0 };
	int i;
	short position = 5;
	
	// Input Verification
	if (numArgs < 1)
	{
		return errorReport("CTRL", "Must have atleast one argument");
	}
	
	// Header
	strncpy(message,"CTRL",4);
	
	// States
	for (i=0;i<6;i++)
	{
		float val = -998.5;
		
		if (i<numArgs)
		{
			val = valueArray[i];
		}
		
		if (i==4) // Integer-gear
		{
			message[position] = (short int) val;
			position += 1;
		}
		else // float
		{
			// Float Values
			memcpy(&message[position],&val,sizeof(float));
			position += sizeof(float);
		}		
	}
	message[position] = acNum;
	
	sendUDP(recfd, message, 27);
	return 0;
}

short sendTEXT(XPCSocket sendfd, char* msg, int x, int y)
{
	char buf[269] = { 0 };
	size_t msgLen = strnlen(msg, 255);
	size_t len = 14 + msgLen;
	strcpy(buf, "TEXT");
	memcpy(buf + 5, &x, sizeof(int));
	memcpy(buf + 9, &y, sizeof(int));
	buf[13] = msgLen;
	strncpy(buf + 14, msg, msgLen);

	sendUDP(sendfd, buf, len);
	return 0;
}

short sendWYPT(XPCSocket sendfd, WYPT_OP op, float points[], int numPoints)
{
	char buf[255] = "WYPT";
	//Preconditions
	//Validate operation
	if (op < xpc_WYPT_ADD || op > xpc_WYPT_CLR)
	{
		return -1;
	}
	//Validate number of points
	else if (numPoints > 19)
	{
		return -2;
	}
	//Everything checks out; send the message
	else
	{
		buf[5] = op;
		buf[6] = numPoints;
		size_t len = sizeof(float) * 3 * numPoints;
		memcpy(buf + 7, points, len);
		sendUDP(sendfd, buf, len + 7);
		return 0;
	}
}

//READ
//----------------------------------------

short readDATA(XPCSocket recfd, float dataRef[][9])
{
	char buf[5000];
	int buflen = readUDP(recfd,buf, NULL);
	
	if (buf[0]!= '\0')
	{
		return parseDATA(buf, buflen, dataRef);
	}
	return -1;
}

short readRequest(XPCSocket recfd, float *dataRef[], short arraySizes[], struct sockaddr *recvaddr)
{
	char buf[5000];
	readUDP(recfd,buf, recvaddr);
	if (buf[0]!= '\0')
	{
		return parseRequest(buf, dataRef, arraySizes);
	}
	return -1;
}

short readDREF(XPCSocket recfd, float *resultArray[], short arraySizes[])
{
	char buf[5000] = {0};
	readUDP(recfd,buf, NULL);
	
	if (buf[0]!= '\0')
	{
		return parseRequest(buf, resultArray, arraySizes);
	}
	return -1;
}

short readPOSI(XPCSocket recfd, float resultArray[], int arraySize,  float *gear)
{
	char buf[5000] = {0};
	readUDP(recfd,buf, NULL);
	
	if (buf[0]!= '\0') // Buffer is not empty
	{
		return parsePOSI(buf, resultArray, arraySize, gear);
	}
	return -1;
}

xpcCtrl readCTRL(XPCSocket recfd)
{
	xpcCtrl result;
	char buf[5000] = { 0 };
	readUDP(recfd, buf, NULL);

	if (buf[0] != '\0') // Buffer is not empty
	{
		result = parseCTRL(buf);
	}
	else
	{
		result.aircraft = -1;
	}
	return result;
}

//PARSE
//---------------------
short parseDATA(const char my_message[], short messageLength, float dataRef[][9])
{
	int i, j;
	unsigned short totalColumns = ((messageLength-5)/36);
	float tmp;
	float data[20][9];
	
	// Input Validation
	
	for (i=0;i<totalColumns;i++)
	{
		data[i][0] = my_message[5+36*i];
		for (j=1;j<9;j++)
		{
			memcpy(&tmp,&my_message[5]+4*j+36*i,4);
			data[i][j] = tmp;
		}
	}
	memcpy(dataRef,data,sizeof(data));
	return totalColumns;
}

short parseDREF(const char my_message[], char *dataRef, unsigned short *length_of_DREF, float *values, unsigned short *number_of_values)
{
	*length_of_DREF = (unsigned short) my_message[5];
	memcpy(dataRef,&(my_message[6]),*length_of_DREF);
	*number_of_values = (unsigned short) my_message[6+*length_of_DREF];
	memcpy(values,&my_message[*length_of_DREF+7],*number_of_values*sizeof(float));
	return 0;
}

int parseGETD(const char my_message[], char *DREFArray[], int DREFSizes[])
{
	int i;
	int listLength = my_message[5];
	int counter = 6;
		
	for (i=0; i<listLength;i++)
	{
		DREFSizes[i] = my_message[counter];
		memcpy(DREFArray[i],&my_message[counter+1],DREFSizes[i]);
		counter += DREFSizes[i]+1;
	}
	
	return listLength;
}

short parseRequest(const char my_message[], float *resultArray[], short arraySizes[])
{
	int i;
	short count = my_message[5];
	short place = 6;
	
	for (i=0; i<count; i++)
	{
		arraySizes[i] = my_message[place];
		//if (resultArray[i] != NULL)
		//{
		//	free(resultArray[i]);
		//}
		resultArray[i] = malloc(arraySizes[i]*sizeof(float));
		
		memcpy(resultArray[i],&my_message[place + 1],arraySizes[i]*sizeof(float));
		place += 1 + arraySizes[i]*sizeof(float);
	}
	
	return count;
}

short parsePOSI(const char my_message[], float resultArray[], int arraySize, float *gear)
{
	int i;
	
	// Input Validation
		if (arraySize < 1)  return -1;
	
	memcpy(gear,&my_message[30],4);
	
	for (i=0; (i<arraySize && i < 6); i++)
	{
		memcpy(&resultArray[i],&my_message[i*4+6],4);
	}
	
	return my_message[5]; // Aircraft
}

xpcCtrl parseCTRL(const char data[])
{
	xpcCtrl result;
	unsigned char len = data[4];
	//Preconditions
	//Validate message prefix to ensure we are looking at the right kind of packet.
	if (strncmp(data, "CTRL", 4) != 0)
	{
		result.aircraft = -1;
	}
	//Legacy packets that don't specify an aircraft number should be 22 bytes long.
	//Packets specifying an A/C num should be 24 bytes.
	else if (len != 26 && len != 27)
	{
		result.aircraft = -1;
	}
	//Everything checks out, so we can skip over the header and copy the raw data
	//into the struct.
	else
	{
		//NOTE: It's tempting to just do a single memcpy here, but we can't do that because the
		//	  compiler is allowed to add padding to the struct type.
		result.pitch = *((float*)(data + 5));
		result.roll = *((float*)(data + 9));
		result.yaw = *((float*)(data + 13));
		result.throttle = *((float*)(data + 17));
		result.gear = data[21];
		result.flaps = *((float*)(data + 22));
		if (len == 27)
		{
			result.aircraft = data[26];
		}
	}
	return result;
}

xpcWypt parseWYPT(const char data[])
{
	xpcWypt result;
	unsigned char len = data[4];
	//Preconditions
	//Validate message prefix to ensure we are looking at the right kind of packet.
	if (strncmp(data, "WYPT", 4) != 0)
	{
		result.op = -1;
	}
	//Validate operation
	else if (data[5] < xpc_WYPT_ADD || data[5] > xpc_WYPT_CLR)
	{
		result.op = -1;
	}
	//Validate number of points
	else if (data[6] > 19)
	{
		result.op = -2;
	}
	//Everything checks out; copy the points into result
	else
	{
		result.op = data[5];
		result.numPoints = data[6];
		char* ptr = data + 7;
		for (size_t i = 0; i < result.numPoints; ++i)
		{
			result.points[i].latitude = *((float*)ptr);
			result.points[i].longitude = *((float*)(ptr + 4));
			result.points[i].altitude = *((float*)(ptr + 8));
			ptr += 12;
		}
	}
	return result;
}
