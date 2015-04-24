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




void printError(char *functionName, char *format, ...)
{
	va_list args;
	va_start(args, format);

	printf("[%s] ERROR: ", functionName);
	vprintf(format, args);
	printf("\n");

	va_end(args);
}

/*****************************************************************************/
/****                       Low Level UDP functions                       ****/
/*****************************************************************************/
XPCSocket aopenUDP(const char *xpIP, unsigned short xpPort)
{
	return openUDP(xpIP, xpPort, 0);
}

XPCSocket openUDP(const char *xpIP, unsigned short xpPort, unsigned short port)
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
	timeout.tv_sec = 0;
	timeout.tv_usec = 500;
#endif
	if (setsockopt(sock.sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
	{
		printError("OpenUDP", "Failed to set timeout");
	}	
	return sock;
}

void closeUDP(XPCSocket sock)
{
#ifdef _WIN32
	int result = closesocket(sock.sock);
#else
	int result = close(sock.sock);
#endif
	if (result < 0)
	{
		printError("closeUDP", "Failed to close socket");
		exit(EXIT_FAILURE);
	}
}

/// Sends the given data to the X-Plane plugin.
///
/// \param sock   The socket to use to send the data.
/// \param buffer A pointer to the data to send.
/// \param len    The number of bytes to send.
/// \returns      If an error occurs, a negative number. Otehrwise, the number of bytes sent.
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

/// Reads a datagram from the specified socket.
///
/// \param sock   The socket to read from.
/// \param buffer A pointer to the location to store the data.
/// \param len    The number of bytes to read.
/// \returns      If an error occurs, a negative number. Otehrwise, the number of bytes read.
int readUDP(XPCSocket sock, char buffer[], int len)
{
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
	status = recv(sock.sock, buffer, len, 0);
#else
	// For apple or linux-just read - will timeout in 0.5 ms
	int status = (int)recv(sock.sock, buffer, len, 0);
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

/*****************************************************************************/
/****                      Configuration functions                        ****/
/*****************************************************************************/
int setCONN(XPCSocket* sock, unsigned short port)
{
	// Set up command
	char buffer[32] = "CONN";
	memcpy(&buffer[5], &port, 2);

	// Send command
	if (sendUDP(*sock, buffer, 7) < 0)
	{
		printError("setCONN", "Failed to send command");
		return -1;
	}

	// Switch socket
	closeUDP(*sock);
	*sock = openUDP(sock->xpIP, sock->xpPort, port);

	// Read response
	int result;
	for (int i = 0; i < 64; ++i)
	{
		result = readUDP(*sock, buffer, 32);
		if (result != 0)
		{
			break;
		}
	}
	if (result <= 0)
	{
		printError("setCONN", "Failed to read response");
		return -2;
	}
	if (strncmp(buffer, "CONF", 4) == 0)
	{
		// Response received succesfully.
		return 0;
	}
	// Response incorrect
	return -3;
}

int pauseSim(XPCSocket sock, char pause)
{
	// Setup command
	char buffer[6] = "SIMU";
	buffer[5] = pause == 0 ? 0 : 1;
	// Send command
	if (sendUDP(sock, buffer, 6) < 0)
	{
		printError("pauseSim", "Failed to send command");
		return -1;
	}
	return 0;
}
/*****************************************************************************/
/****                    End Configuration functions                      ****/
/*****************************************************************************/

/*****************************************************************************/
/****                    X-Plane UDP Data functions                       ****/
/*****************************************************************************/
int sendDATA(XPCSocket sock, float data[][9], int rows)
{
	// Preconditions
	// There are only 134 DATA rows in X-Plane. Realistically, clients probably
	// shouldn't be trying to set nearly this much data at once anyway.
	if (rows > 134)
	{
		printError("sendDATA", "Too many rows.");
		return -1;
	}

	// Setup command
	// 5 byte header + 134 rows * 9 values * 4 bytes per value => 4829 byte max length.
	char buffer[4829] = "DATA"; 
	int len = 5 + rows * 9 * sizeof(float);
	unsigned short step = 9 * sizeof(float);	
	for (int i=0;i<rows;i++)
	{
		buffer[5 + i * step] = (char)data[i][0];
		memcpy(&buffer[9 + i*step], &data[i][1], 8 * sizeof(float));
	}
	// Send command
	if (sendUDP(sock, buffer, len ) < 0)
	{
		printError("sendDATA", "Failed to send command");
		return -2;
	}
	return 0;
}

int readDATA(XPCSocket sock, float data[][9], int rows)
{
	// Preconditions
	// There are only 134 DATA rows in X-Plane. Realistically, clients probably
	// shouldn't be trying to read nearly this much data at once anyway.
	if (rows > 134)
	{
		printError("sendDATA", "Too many rows.");
		// Read as much as we can anyway
		rows = 134;
	}

	// Read data
	char buffer[4829] = { 0 };
	int result = readUDP(sock, buffer, 5120);
	if (result <= 0)
	{
		printError("readDATA", "Failed to read from socket.");
		return -1;
	}
	// Validate data
	int readRows = (result - 5) / 36;
	if (readRows > rows)
	{
		printError("readDATA", "Read more rows than will fit in dataRef.");
		// Copy as much data as we can anyway
		rows = readRows;
	}

	// Parse data
	for (int i = 0; i < rows; ++i)
	{
		data[i][0] = buffer[5 + i * 36];
		memcpy(&data[i][1], &buffer[9 + i * 36], 8 * sizeof(float));
	}
	return rows;
}
/*****************************************************************************/
/****                  End X-Plane UDP Data functions                     ****/
/*****************************************************************************/

/*****************************************************************************/
/****                          DREF functions                             ****/
/*****************************************************************************/
int setDREF(XPCSocket sock, const char* dref, float value[], int size)
{
	// Setup command
	// 5 byte header + max 255 char dref name + max 255 values * 4 bytes per value = 1279
	unsigned char buffer[1279] = "DREF";
	int drefLen = strnlen(dref, 256);
	if (drefLen > 255)
	{
		printError("setDREF", "dref length is too long. Must be less than 256 characters.");
		return -1;
	}
	if (size > 255)
	{
		printError("setDREF", "size is too big. Must be less than 256.");
		return -2;
	}
	int len = 7 + drefLen + size * sizeof(float);
	
	// Copy dref to buffer
	buffer[5] = (unsigned char)drefLen;
	memcpy(buffer + 6, dref, drefLen);

	// Copy values to buffer
	buffer[6 + drefLen] = (unsigned char)size;
	memcpy(buffer + 7 + drefLen, value, size * sizeof(float));

	// Send command
	if (sendUDP(sock, buffer, len) < 0)
	{
		printError("setDREF", "Failed to send command");
		return -3;
	}
	return 0;
}

int sendDREFRequest(XPCSocket sock, const char* drefs[], unsigned char count)
{
	// Setup command
	// 6 byte header + potentially 255 drefs, each 256 chars long.
	// Easiest to just round to an even 2^16.
	unsigned char buffer[65536] = "GETD";
	buffer[5] = count;
	int len = 6;
	for (int i = 0; i < count; ++i)
	{
		size_t drefLen = strnlen(drefs[i], 256);
		if (drefLen > 255)
		{
			printError("getDREFs", "dref %d is too long.", i);
			return -1;
		}
		buffer[len++] = (unsigned char)drefLen;
		strncpy(buffer + len, drefs[i], drefLen);
		len += drefLen;
	}
	// Send Command
	if (sendUDP(sock, buffer, len) < 0)
	{
		printError("getDREFs", "Failed to send command");
		return -2;
	}
	return 0;
}

int getDREFResponse(XPCSocket sock, float* values[], unsigned char count, int sizes[])
{
	unsigned char buffer[65536];
	// Read data. Try 40 times to read, then give up.
	// TODO: Why not just set the timeout to 40ms?
	int result;
	for (int i = 0; i < 512; ++i)
	{
		result = readUDP(sock, buffer, 65536);
		if (result > 0)
		{
			break;
		}
		if (result < 0)
		{
#ifdef _WIN32
			printError("getDREFs", "Read operation failed. (%d)", WSAGetLastError());
#else
			printError("getDREFs", "Read operation failed.");
#endif
			return -1;
		}
	}

	if (result < 6)
	{
		printError("getDREFs", "Response was too short. Expected at least 6 bytes, but only got %d.", result);
		return -2;
	}
	if (buffer[5] != count)
	{
		printError("getDREFs", "Unexpected response size. Expected %d rows, got %d instead.", count, buffer[5]);
		return -3;
	}

	int cur = 6;
	for (int i = 0; i < count; ++i)
	{
		int l = buffer[cur++];
		if (l > sizes[i])
		{
			printError("getDREFs", "values is too small. Row had %d values, only room for %d.", l, sizes[i]);
			// Copy as many values as we can anyway
			memcpy(values[i], buffer + cur, sizes[i] * sizeof(float));
		}
		else
		{
			memcpy(values[i], buffer + cur, l * sizeof(float));
			sizes[i] = l;
		}
		cur += l * sizeof(float);
	}
	return 0;
}

int getDREF(XPCSocket sock, const char* dref, float values[], int* size)
{
	return getDREFs(sock, &dref, &values, 1, size);
}

int getDREFs(XPCSocket sock, const char* drefs[], float* values[], unsigned char count, int sizes[])
{
	// Send Command
	int result = sendDREFRequest(sock, drefs, count);
	if (result < 0)
	{
		// A error ocurred while sending.
		// sendDREFRequest will print an error message, so just return.
		return -1;
	}

	// Read Response
	if (getDREFResponse(sock, values, count, sizes) < 0)
	{
		// A error ocurred while reading the response.
		// getDREFResponse will print an error message, so just return.
		return -2;
	}
	return 0;
}
/*****************************************************************************/
/****                        End DREF functions                           ****/
/*****************************************************************************/

/*****************************************************************************/
/****                          POSI functions                             ****/
/*****************************************************************************/
int psendPOSI(XPCSocket sock, float values[], int size)
{
	return sendPOSI(sock, values, size, 0);
}

int sendPOSI(XPCSocket sock, float values[], int size, char ac)
{
	// Validate input
	if (ac < 0 || ac > 20)
	{
		printError("sendPOSI", "aircraft should be a value between 0 and 20.");
		return -1;
	}
	if (size < 1 || size > 7)
	{
		printError("sendPOSI", "size should be a value between 1 and 7.");
		return -2;
	}

	// Setup command
	// 5 byte header + up to 7 values * 5 bytes each
	unsigned char buffer[40] = "POSI";
	buffer[5] = ac;
	for (int i = 0; i < 7; i++)
	{
		float val = -998;

		if (i < size)
		{
			val = values[i];
		}
		*((float*)(buffer + 6 + i * 4)) = val;
	}

	// Send Command
	if (sendUDP(sock, buffer, 40) < 0)
	{
		printError("sendPOSI", "Failed to send command");
		return -3;
	}
	return 0;
}
/*****************************************************************************/
/****                        End POSI functions                           ****/
/*****************************************************************************/

/*****************************************************************************/
/****                          CTRL functions                             ****/
/*****************************************************************************/
int psendCTRL(XPCSocket sock, float values[], int size)
{
	return sendCTRL(sock, values, size, 0);
}

int sendCTRL(XPCSocket sock, float values[], int size, char ac)
{
	// Validate input
	if (ac < 0 || ac > 20)
	{
		printError("sendCTRL", "aircraft should be a value between 0 and 20.");
		return -1;
	}
	if (size < 1 || size > 7)
	{
		printError("sendCTRL", "size should be a value between 1 and 6.");
		return -2;
	}

	// Setup Command
	// 5 byte header + 5 float values * 4 + 2 byte values
	unsigned char buffer[27] = "CTRL";
	int cur = 5;
	for (int i = 0; i < 6; i++)
	{
		float val = -998;

		if (i < size)
		{
			val = values[i];
		}
		if (i == 4)
		{
			buffer[cur++] = val == -998 ? -1 : (unsigned char)val;
		}
		else
		{
			*((float*)(buffer + cur)) = val;
			cur += sizeof(float);
		}
	}
	buffer[26] = ac;

	// Send Command
	if (sendUDP(sock, buffer, 27) < 0)
	{
		printError("sendCTRL", "Failed to send command");
		return -3;
	}
	return 0;
}
/*****************************************************************************/
/****                        End CTRL functions                           ****/
/*****************************************************************************/

/*****************************************************************************/
/****                        Drawing functions                            ****/
/*****************************************************************************/
int sendTEXT(XPCSocket sock, char* msg, int x, int y)
{
	size_t msgLen = strnlen(msg, 255);
	// Input Validation
	if (x < -1)
	{
		printError("sendTEXT", "x should be positive (or -1 for default).");
		// Technically, this should work, and may print something to the screen.
	}
	if (y < -1)
	{
		printError("sendTEXT", "y should be positive (or -1 for default).");
		// Negative y will never result in text being displayed.
		return -1;
	}
	if (msgLen > 255)
	{
		printError("sendTEXT", "msg must be less than 255 bytes.");
		return -2;
	}

	// Setup command
	// 5 byte header + 8 byte position + up to 256 byte message
	char buffer[269] = "TEXT";
	size_t len = 14 + msgLen;
	memcpy(buffer + 5, &x, sizeof(int));
	memcpy(buffer + 9, &y, sizeof(int));
	buffer[13] = msgLen;
	strncpy(buffer + 14, msg, msgLen);
	
	// Send Command
	if (sendUDP(sock, buffer, len) < 0)
	{
		printError("sendTEXT", "Failed to send command");
		return -3;
	}
	return 0;
}

int sendWYPT(XPCSocket sock, WYPT_OP op, float points[], int count)
{
	// Input Validation
	if (op < XPC_WYPT_ADD || op > XPC_WYPT_CLR)
	{
		printError("sendWYPT", "Unrecognized operation.");
		return -1;
	}
	if (count > 255)
	{
		printError("sendWYPT", "Too many points. Must be less than 256.");
		return -2;
	}

	// Setup Command
	// 7 byte header + 12 bytes * count
	char buffer[3067] = "WYPT";
	buffer[5] = (unsigned char)op;
	buffer[6] = (unsigned char)count;
	size_t ptLen = sizeof(float) * 3 * count;
	memcpy(buffer + 7, points, ptLen);

	// Send Command
	if (sendUDP(sock, buffer, 40) < 0)
	{
		printError("sendWYPT", "Failed to send command");
		return -2;
	}
	return 0;
}
/*****************************************************************************/
/****                      End Drawing functions                          ****/
/*****************************************************************************/
