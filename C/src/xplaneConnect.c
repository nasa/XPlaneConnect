//
//  xPlaneConnect.c Beta
//
//  DESCRIPTION
//      XPLANECONNECT (XPC) Facilitates Communication to and from the XPCPlugin
//
//  REQUIREMENTS
//      1. X-Plane Version 9.0 or newer (untested on previous versions)
//      2. XPCPlugin.xpl-must be placed in [X-Plane Directory]/Resources/plugins
//      3. OS X 10.8 or newer (untested on previous versions)
//
//  INSTRUCTIONS
//      1. Include the header "xplaneConnect.h"
//          EXAMPLE: #include "xplaneConnect.h"
//      2. Open UDP Socket using openUDP
//          EXAMPLE: struct xpcSocket theSocket = openUDP(port_number);
//          NOTE: Do not send/receive on the same socket
//      3. Use functions described below
//      4. Close UDP Socket using closeUDP
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
//  TO DO
//      1. Update SelectData
//      2. RequestDREF: look into removing DREFSizes
//
// BEGIN CODE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "xplaneConnect.h"

#ifdef _WIN32 /* WIN32 SYSTEM */
	#include <WS2tcpip.h>
    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
	#define snprintf sprintf_s
	#define socklen_t int

    void usleep(__int64 usec)
    { // From http://www.c-plusplus.de/forum/109539-full
        HANDLE timer;
        LARGE_INTEGER ft;
        
        ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time
        
        timer = CreateWaitableTimer(NULL, TRUE, NULL);
        SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
        WaitForSingleObject(timer, INFINITE);
        CloseHandle(timer);
    }
#endif

short errorReport(char *functionName, char *errorMessage);
short sendRequest(struct xpcSocket recfd, char DREFArray[][100], short DREFSizes[], short listLength);

short errorReport(char *functionName, char *errorMessage)
{
	printf("ERROR: %s-%s\n", functionName, errorMessage);
	return -1;
}

struct xpcSocket openUDP(unsigned short port_number, const char *xpIP, unsigned short xpPort)
{
    struct xpcSocket theSocket;
    struct sockaddr_in server;

#if (__APPLE__ || __linux)
    struct timeval tv;
    int optval = 1;
#endif
    
    // Setup Port
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(port_number);
    
    // Set X-Plane Port and IP
        if (strcasecmp(xpIP,"localhost") == 0) // IP
        {
            strncpy(theSocket.xpIP,"127.0.0.1",9); // Default
        }
        else
        {
            memcpy(theSocket.xpIP,xpIP,(size_t) fminl(strlen(xpIP),16));
        }
    
        if (xpPort <= 0) // Default Port
        {
            theSocket.xpPort = 49009; // Default
        }
        else
        {
            theSocket.xpPort = xpPort;
        }
    
#ifdef _WIN32
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        perror("ERROR: openUDP- ");
        return theSocket;
        //ERROR IN Socket Message
    }
    
    if ((theSocket.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
        perror("ERROR: openUDP- ");
        return theSocket;
        //ERROR IN Socket Message
    }
    
    if (bind(theSocket.sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        perror("ERROR: openUDP- ");
        return theSocket;
        //ERROR IN Socket Message
    }
#elif (__APPLE__ || __linux)
    // Create a SOCKET
    if ((theSocket.sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror((char*) "ERROR: openUDP- ");
        return theSocket;
    }
    
    // Options
        setsockopt(theSocket.sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        setsockopt(theSocket.sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
    
    //Bind
    if ( bind(theSocket.sock, (struct sockaddr *) &server, sizeof(server)) == -1)
    {
        perror( (char*) "ERROR: openUDP- ");
        return theSocket;
    }
#endif
    
    //Set Timout
	int usTimeOut = 500;

#ifdef _WIN32
	DWORD msTimeOutWin = 1; // Minimum socket timeout in Windows is 1ms
	setsockopt(theSocket.sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&msTimeOutWin, sizeof(msTimeOutWin));
#else
	tv.tv_sec = 0;  /* Sec Timeout */
	tv.tv_usec = usTimeOut;  // Microsec Timeout
    setsockopt(theSocket.sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval));
#endif
    
    return theSocket;
}

void closeUDP(struct xpcSocket socketNumber)
{
#ifdef _WIN32
	closesocket(socketNumber.sock);
#elif (__APPLE__ || __linux)
    close(socketNumber.sock);
#endif
}

short sendUDP(struct xpcSocket recfd,  char my_message[], short messageLength)
{
    struct sockaddr_in servaddr;
    
    my_message[4] = (char) messageLength;
    
    // Preconditions
	if (messageLength <= 0)// Positive Message Length
	{
		return errorReport("sendUDP", "message length must be positive >0");
	}
    
    // Code
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(recfd.xpPort);
    servaddr.sin_addr.s_addr = inet_addr(recfd.xpIP);

#ifdef _WIN32
	const char on = 1;
#else
    int on=1;
#endif

    setsockopt(recfd.sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    
    if (sendto(recfd.sock, my_message, (int) messageLength, 0, (const struct sockaddr *) &servaddr, sizeof(servaddr))<0)
    {
        perror("ERROR: sendUDP-");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

short sendDATA(struct xpcSocket recfd, float dataRef[][9], unsigned short rows)
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

short sendDREF(struct xpcSocket recfd, const char *dataRef, unsigned short length_of_DREF, float *values, unsigned short number_of_values) {
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

short sendRequest(struct xpcSocket recfd, char DREFArray[][100], short DREFSizes[], short listLength)
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

short requestDREF(struct xpcSocket sendfd, struct xpcSocket recfd, char DREFArray[][100], short DREFSizes[], short listLength,  float *resultArray[], short arraySizes[])
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

short pauseSim(struct xpcSocket recfd, short pause)
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

short setCONN(struct xpcSocket recfd,  unsigned short recPort)
{
    char message[10] = {0};
    int length;
    strncpy(message,"CONN",4);
    
    memcpy(&message[5],&recPort,sizeof(recPort));
    
    length = 5 + sizeof(recPort);
    
    sendUDP(recfd,message, length);
    
    return 0;
}

short sendPOSI(struct xpcSocket recfd, short ACNum, short numArgs, float valueArray[])
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

short sendCTRL(struct xpcSocket recfd, short numArgs, float valueArray[])
{
	return sendpCTRL(recfd, numArgs, valueArray, 0);
}

short sendpCTRL(struct xpcSocket recfd, short numArgs, float valueArray[], char acNum)
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

//READ
//----------------------------------------
short readUDP(struct xpcSocket recfd, char *dataRef, struct sockaddr *recvaddr)
{
    socklen_t recvaddrlen = sizeof(*recvaddr);
    int status = 0;

#ifdef _WIN32
    // Windows readUDP needs the select command- minimum timeout is 1ms.
    // Without this playback becomes choppy
    
    // Definitions
        FD_SET stReadFDS;
        FD_SET stExceptFDS;
        struct timeval tv;
        struct sockaddr_in testaddr;
        int error = 0;
    
    // Setup for Select
        FD_ZERO(&stReadFDS);
        FD_SET(recfd.sock, &stReadFDS);
        FD_ZERO(&stExceptFDS);
        FD_SET(recfd.sock, &stExceptFDS);
        tv.tv_sec = 0;  /* Sec Timeout */
        tv.tv_usec = 250;  // Microsec Timeout
    
    // Select Command
        int tmp = select(-1, &stReadFDS, (FD_SET *)0, &stExceptFDS, &tv);
        if (tmp <= 0) // No Data or error
        {
            return -1;
        }
    
    // If no error: Read Data
        recvaddrlen = sizeof(testaddr);
        status = (int) recvfrom(recfd.sock,dataRef,5000,0,(SOCKADDR *) &testaddr,&recvaddrlen);
        if (status == SOCKET_ERROR)
        {
            error = WSAGetLastError();
        }
#else
    // For apple or linux-just read - will timeout in 0.5 ms
    status = (int) recvfrom(recfd.sock,dataRef,5000,0,recvaddr,&recvaddrlen);
#endif

	return status;
}

short readDATA(struct xpcSocket recfd, float dataRef[][9])
{
    char buf[5000];
    int buflen = readUDP(recfd,buf, NULL);
    
    if (buf[0]!= '\0')
    {
        return parseDATA(buf, buflen, dataRef);
    }
    return -1;
}

short readRequest(struct xpcSocket recfd, float *dataRef[], short arraySizes[], struct sockaddr *recvaddr)
{
    char buf[5000];
    readUDP(recfd,buf, recvaddr);
    if (buf[0]!= '\0')
    {
        return parseRequest(buf, dataRef, arraySizes);
    }
    return -1;
}

short readDREF(struct xpcSocket recfd, float *resultArray[], short arraySizes[])
{
    char buf[5000] = {0};
    readUDP(recfd,buf, NULL);
    
    if (buf[0]!= '\0')
    {
        return parseRequest(buf, resultArray, arraySizes);
    }
    return -1;
}

short readPOSI(struct xpcSocket recfd, float resultArray[], int arraySize,  float *gear)
{
    char buf[5000] = {0};
    readUDP(recfd,buf, NULL);
    
    if (buf[0]!= '\0') // Buffer is not empty
    {
        return parsePOSI(buf, resultArray, arraySize, gear);
    }
    return -1;
}

xpcCtrl readCTRL(struct xpcSocket recfd)
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
        //    free(resultArray[i]);
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
		//      compiler is allowed to add padding to the struct type.
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
