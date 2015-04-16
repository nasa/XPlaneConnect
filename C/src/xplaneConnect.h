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
#ifndef xplaneConnect_h
#define xplaneConnect_h

#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h"
#ifdef _WIN32 /* WIN32 SYSTEM */
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#elif (__APPLE__ || __linux)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif
        
typedef struct
{
	char open;
	unsigned short port;
		
	// X-Plane IP and Port
	char xpIP[16];
	unsigned short xpPort;
		
	#ifdef _WIN32
	SOCKET sock;
	#else
	int sock;
	#endif
} XPCSocket;

typedef struct
{
	double latitude;
	double longitude;
	double altitude;
} Waypoint;

typedef enum
{
	xpc_WYPT_ADD = 1,
	xpc_WYPT_DEL = 2,
	xpc_WYPT_CLR = 3
} WYPT_OP;

typedef struct
{
	WYPT_OP op;
	Waypoint points[20];
	size_t numPoints;
} xpcWypt;
        
// Low Level UDP Functions
XPCSocket openUDP(unsigned short port, const char *xpIP, unsigned short xpPort);
void closeUDP(XPCSocket sock);
int sendUDP(XPCSocket sock, char buffer[], int len);
int readUDP(XPCSocket sock, char buffer[], int len, struct sockaddr* recvaddr);

// Configuration
int setCONN(XPCSocket sock);
int pauseSim(XPCSocket sock, char pause);
        
// X-Plane UDP DATA
int readDATA(XPCSocket sock, float dataRef[][9], int rows);
int sendDATA(XPCSocket sock, float dataRef[][9], int rows);

// DREF Manipulation
int setDREF(XPCSocket sock, const char* dref, float values[], int size);
int getDREF(XPCSocket sock, const char* dref, float values[], int* size);
int getDREFs(XPCSocket sock, const char* drefs[], float* values[], unsigned char count, int sizes[]);
        
// Position
int psendPOSI(XPCSocket sock, float values[], int size);
int sendPOSI(XPCSocket sock, float values[], int size, char ac);

// Controls
int psendCTRL(XPCSocket sock, float values[], int size);
int sendCTRL(XPCSocket sock, float values[], int size, char ac);

// Waypoints
xpcWypt parseWYPT(const char data[]);
short sendWYPT(XPCSocket sendfd, WYPT_OP op, float points[], int numPoints);

// Screen Text
short sendTEXT(XPCSocket sendfd, char* msg, int x, int y);

        
        
#ifdef __cplusplus
    }
#endif
#endif //ifdef _h
