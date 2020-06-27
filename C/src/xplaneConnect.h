//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
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

typedef struct xpcSocket
{
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

typedef enum
{
	XPC_WYPT_ADD = 1,
	XPC_WYPT_DEL = 2,
	XPC_WYPT_CLR = 3
} WYPT_OP;

typedef enum
{
	XPC_VIEW_FORWARDS = 73,
	XPC_VIEW_DOWN,
	XPC_VIEW_LEFT,
	XPC_VIEW_RIGHT,
	XPC_VIEW_BACK,
	XPC_VIEW_TOWER,
	XPC_VIEW_RUNWAY,
	XPC_VIEW_CHASE,
	XPC_VIEW_FOLLOW,
	XPC_VIEW_FOLLOWWITHPANEL,
	XPC_VIEW_SPOT,
	XPC_VIEW_FULLSCREENWITHHUD,
	XPC_VIEW_FULLSCREENNOHUD,
} VIEW_TYPE;

// Low Level UDP Functions

/// Opens a new connection to XPC on an OS chosen port.
///
/// \param xpIP   A string representing the IP address of the host running X-Plane.
/// \returns      An XPCSocket struct representing the newly created connection.
XPCSocket openUDP(const char *xpIP);

/// Opens a new connection to XPC on the specified port.
///
/// \param xpIP   A string representing the IP address of the host running X-Plane.
/// \param xpPort The port of the X-Plane Connect plugin is listening on. Usually 49009.
/// \param port   The local port to use when sending and receiving data from XPC.
/// \returns      An XPCSocket struct representing the newly created connection.
XPCSocket aopenUDP(const char *xpIP, unsigned short xpPort, unsigned short port);

/// Closes the specified connection and releases resources associated with it.
///
/// \param sock The socket to close.
void closeUDP(XPCSocket sock);

// Configuration

/// Sets the port on which the socket sends and receives data.
///
/// \param sock A pointer to the socket to change.
/// \param port The new port to use.
/// \returns    0 if successful, otherwise a negative value.
int setCONN(XPCSocket* sock, unsigned short port);

/// Pause or unpause the simulation.
///
/// \param sock  The socket to use to send the command.
/// \param pause 0 to unpause the sim; 1 to pause, 100:119 to pause a/c 0:19, 200:219 to unpause a/c 0:19.
/// \returns     0 if successful, otherwise a negative value.
int pauseSim(XPCSocket sock, char pause);

// X-Plane UDP DATA

/// Reads X-Plane data from the specified socket.
///
/// \details This command is compatible with the X-Plane data API.
/// \param sock    The socket to use to send the command.
/// \param data    A 2D array of data rows to read into.
/// \param rows    The number of rows in dataRef.
/// \returns       0 if successful, otherwise a negative value.
int readDATA(XPCSocket sock, float data[][9], int rows);

/// Sends X-Plane data on the specified socket.
///
/// \details This command is compatible with the X-Plane data API.
/// \param sock    The socket to use to send the command.
/// \param data    A 2D array of data rows to send.
/// \param rows    The number of rows in dataRef.
/// \returns       0 if successful, otherwise a negative value.
int sendDATA(XPCSocket sock, float data[][9], int rows);

// DREF Manipulation

/// Sets the specified dataref to the specified value.
///
/// \details dref names and their associated data types can be found on the XPSDK wiki at
///          http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html. The size of values should match
///          the size given on that page. XPC currently sends all values as floats regardless of
///          the type described on the wiki. This doesn't cause any data loss for most datarefs.
/// \param sock   The socket to use to send the command.
/// \param dref   The name of the dataref to set.
/// \param values An array of values representing the data to set.
/// \param size   The number of elements in values.
/// \returns      0 if successful, otherwise a negative value.
int sendDREF(XPCSocket sock, const char* dref, float values[], int size);

/// Sets the specified datarefs to the specified values.
///
/// \details dref names and their associated data types can be found on the XPSDK wiki at
///          http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html. The size of values should match
///          the size given on that page. XPC currently sends all values as floats regardless of
///          the type described on the wiki. This doesn't cause any data loss for most datarefs.
/// \param sock   The socket to use to send the command.
/// \param drefs  The names of the datarefs to set.
/// \param values A 2D array containing the values for each dataref to set.
/// \param sizes  The number of elements in each array in values
/// \param count  The number of datarefs being set.
/// \returns      0 if successful, otherwise a negative value.
int sendDREFs(XPCSocket sock, const char* drefs[], float* values[], int sizes[], int count);

/// Gets the value of the specified dataref.
///
/// \details dref names and their associated data types can be found on the XPSDK wiki at
///          http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html. The size of values should match
///          the size given on that page. XPC currently sends all values as floats regardless of
///          the type described on the wiki. This doesn't cause any data loss for most datarefs.
/// \param sock   The socket to use to send the command.
/// \param dref   The name of the dataref to get.
/// \param values The array in which the values of the dataref will be stored.
/// \param size   The number of elements in values. The actual number of elements copied in will
///               be set when the function returns.
/// \returns      0 if successful, otherwise a negative value.
int getDREF(XPCSocket sock, const char* dref, float values[], int* size);

/// Gets the values of the specified datarefs.
///
/// \details dref names and their associated data types can be found on the XPSDK wiki at
///          http://www.xsquawkbox.net/xpsdk/docs/DataRefs.html. The size of values should match
///          the size given on that page. XPC currently sends all values as floats regardless of
///          the type described on the wiki. This doesn't cause any data loss for most datarefs.
/// \param sock   The socket to use to send the command.
/// \param drefs  The names of the datarefs to get.
/// \param values A 2D array in which the values of the datarefs will be stored.
/// \param count  The number of datarefs being requested.
/// \param sizes  The number of elements in each row of values. The size of each row will be set
///               to the actual number of elements copied in for that row.
/// \returns      0 if successful, otherwise a negative value.
int getDREFs(XPCSocket sock, const char* drefs[], float* values[], unsigned char count, int sizes[]);

// Position

/// Gets the position and orientation of the specified aircraft.
///
/// \param sock   The socket used to send the command and receive the response.
/// \param values An array to store the position information returned by the
///               plugin. The format of values is [Lat, Lon, Alt, Pitch, Roll, Yaw, Gear]
/// \param ac     The aircraft number to get the position of. 0 for the main/user's aircraft.
/// \returns      0 if successful, otherwise a negative value.
int getPOSI(XPCSocket sock, double values[7], char ac);

/// Sets the position and orientation of the specified aircraft.
///
/// \param sock   The socket to use to send the command.
/// \param values A double array representing position data about the aircraft. The format of values is
///               [Lat, Lon, Alt, Pitch, Roll, Yaw, Gear]. If less than 7 values are specified,
///               the unspecified values will be left unchanged.
/// \param size   The number of elements in values.
/// \param ac     The aircraft number to set the position of. 0 for the main/user's aircraft.
/// \returns      0 if successful, otherwise a negative value.
int sendPOSI(XPCSocket sock, double values[], int size, char ac);

// Terrain

/// Sets the position and orientation and gets the terrain information of the specified aircraft.
///
/// \param sock   The socket to use to send the command.
/// \param posi   A double array representing position data about the aircraft. The format of values is
///               [Lat, Lon, Alt, Pitch, Roll, Yaw, Gear]. If less than 7 values are specified,
///               the unspecified values will be left unchanged.
/// \param size   The number of elements in posi.
/// \param values A double array with the information for the terrain output. The format of values is
///               [Lat, Lon, Alt, Nx, Ny, Nz, Vx, Vy, Vz, wet, result]. The first three are for output of
///               the Lat and Lon of the aircraft with the terrain height directly below. The next three
///               represent the terrain normal. The next three represent the velocity of the terrain.
///               The wet variable is 0.0 if the terrain is dry and 1.0 if wet.
///               The last output is the terrain probe result parameter.
/// \param ac     The aircraft number to set the position of. 0 for the main/user's aircraft.
/// \returns      0 if successful, otherwise a negative value.
int sendPOST(XPCSocket sock, double posi[], int size, double values[11], char ac);

/// Gets the terrain information of the specified aircraft.
///
/// \param sock   The socket to use to send the command.
/// \param posi   A double array representing position data about the aircraft. The format of values is
///               [Lat, Lon, Alt].
///               -998 used for [Lat, Lon, Alt] to request terrain info at the current aircraft position.
/// \param values A double array with the information for the terrain output. The format of values is
///               [Lat, Lon, Alt, Nx, Ny, Nz, Vx, Vy, Vz, wet, result]. The first three are for output of
///               the Lat and Lon of the aircraft with the terrain height directly below. The next three
///               represent the terrain normal. The next three represent the velocity of the terrain.
///               The wet variable is 0.0 if the terrain is dry and 1.0 if wet.
///               The last output is the terrain probe result parameter.
/// \param ac     The aircraft number to get the terrain data of. 0 for the main/user's aircraft.
/// \returns      0 if successful, otherwise a negative value.
int getTERR(XPCSocket sock, double posi[3], double values[11], char ac);

// Controls

/// Gets the control surface information for the specified aircraft.
///
/// \param sock   The socket used to send the command and receive the response.
/// \param values An array to store the position information returned by the
///               plugin. The format of values is [Elevator, Aileron, Rudder,
///               Throttle, Gear, Flaps, Speed Brakes]
/// \param ac     The aircraft to get the control surfaces of. 0 is the main/user's aircraft.
/// \returns      0 if successful, otherwise a negative value.
int getCTRL(XPCSocket sock, float values[7], char ac);

/// Sets the control surfaces of the specified aircraft.
///
/// \param sock   The socket to use to send the command.
/// \param values An array representing position data about the aircraft. The format of values is
///               [Elevator, Aileron, Rudder, Throttle, Gear, Flaps, Speed Brakes]. If less than
///               6 values are specified, the unspecified values will be left unchanged.
/// \param size   The number of elements in values.
/// \param ac     The aircraft to set the control surfaces of. 0 for the main/user's aircraft.
/// \returns      0 if successful, otherwise a negative value.
int sendCTRL(XPCSocket sock, float values[], int size, char ac);

// Drawing

/// Sets a string to be printed on the screen in X-Plane.
///
/// \param sock The socket to use to send the command.
/// \param msg  The message to print of the screen.
/// \param x    The distance in pixels from the left edge of the screen to print the text.
/// \param y    The distance in pixels from the bottom edge of the screen to print the top line of text.
/// \returns    0 if successful, otherwise a negative value.
int sendTEXT(XPCSocket sock, char* msg, int x, int y);

/// Sets the camera view in X-Plane.
///
/// \param sock The socket to use to send the command.
/// \param view The view to use.
/// \returns    0 if successful, otherwise a negative value.
int sendVIEW(XPCSocket sock, VIEW_TYPE view);

/// Adds, removes, or clears a set of waypoints. If the command is clear, the points are ignored
/// and all points are removed.
///
/// \param sock   The socket to use to send the command.
/// \param op     The operation to perform. 1=add, 2=remove, 3=clear.
/// \param points An array of values representing points. Each triplet in the array will be
///               interpreted as a (Lat, Lon, Alt) point.
/// \param count  The number of points. There should be 3 * count elements in points.
/// \returns      0 if successful, otherwise a negative value.
int sendWYPT(XPCSocket sock, WYPT_OP op, float points[], int count);

/// Sends commands.
///
/// \param sock The socket to use to send the command.
/// \param comm The command string.
/// \returns    0 if successful, otherwise a negative value.
int sendCOMM(XPCSocket sock, const char* comm);

#ifdef __cplusplus
    }
#endif
#endif
