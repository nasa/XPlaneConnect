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

#ifdef _WIN32
#include <time.h>
#else
#include <sys/time.h>
#endif

int sendUDP(XPCSocket sock, char buffer[], int len);
int readUDP(XPCSocket sock, char buffer[], int len);
int sendDREFRequest(XPCSocket sock, const char* drefs[], unsigned char count);
int getDREFResponse(XPCSocket sock, float* values[], unsigned char count, int sizes[]);

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
XPCSocket openUDP(const char *xpIP)
{
	return aopenUDP(xpIP, 49009, 0);
}

XPCSocket aopenUDP(const char *xpIP, unsigned short xpPort, unsigned short port)
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

	// Set socket timeout period for sendUDP to 1 millisecond
	// Without this, playback may become choppy due to process blocking
#ifdef _WIN32
	// Minimum socket timeout in Windows is 1 millisecond (0 makes it blocking)
	DWORD timeout = 1;
#else
	// Set socket timeout to 1 millisecond = 1,000 microseconds to make it the same as Windows (0 makes it blocking)
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;
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
/// \returns      If an error occurs, a negative number. Otherwise, the number of bytes read.
int readUDP(XPCSocket sock, char buffer[], int len)
{
	// For readUDP, use the select command - minimum timeout of 0 makes it polling.
	// Without this, playback may become choppy due to process blocking

	// Definitions
	fd_set stReadFDS;
	fd_set stExceptFDS;
	struct timeval timeout;

	// Setup for Select
	FD_ZERO(&stReadFDS);
	FD_SET(sock.sock, &stReadFDS);
	FD_ZERO(&stExceptFDS);
	FD_SET(sock.sock, &stExceptFDS);

	// Set timeout period for select to 0.05 sec = 50 milliseconds = 50,000 microseconds (0 makes it polling)
	// TO DO - This could be set to 0 if a message handling system were implemented, like in the plugin.
	timeout.tv_sec = 0;
	timeout.tv_usec = 50000;

	// Select Command
	int status = select(sock.sock+1, &stReadFDS, NULL, &stExceptFDS, &timeout);
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

	// If no error: Read Data
	status = recv(sock.sock, buffer, len, 0);
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
	*sock = aopenUDP(sock->xpIP, sock->xpPort, port);

	// Read response
	int result = readUDP(*sock, buffer, 32);

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
	// Validte input
	if (pause < 0 || (pause > 2 && pause < 100) || (pause > 119 && pause < 200) || pause > 219)
	{
		printError("pauseSim", "Invalid argument: %i", pause);
		return -2;
	}

	// Setup command
	char buffer[6] = "SIMU";
	buffer[5] = pause;

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
    int i; // iterator
	for (i = 0; i < rows; i++)
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
		printError("readDATA", "Too many rows.");
		// Read as much as we can anyway
		rows = 134;
	}

	// Read data
	char buffer[4829] = { 0 };
	int result = readUDP(sock, buffer, 4829);
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
	}
	else if (readRows < rows)
	{
		printError("readDATA", "Read fewer rows than expected.");
		// Copy as much data as we read anyway
		rows = readRows;
	}

	// Parse data
    int i; // iterator
	for (i = 0; i < rows; ++i)
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
int sendDREF(XPCSocket sock, const char* dref, float values[], int size)
{
	return sendDREFs(sock, &dref, &values, &size, 1);
}

int sendDREFs(XPCSocket sock, const char* drefs[], float* values[], int sizes[], int count)
{
	// Setup command
	// Max size is technically unlimited.
	char buffer[65536] = "DREF";
	int pos = 5;
    int i; // Iterator
	for (i = 0; i < count; ++i)
	{
		int drefLen = strnlen(drefs[i], 256);
		if (pos + drefLen + sizes[i] * 4 + 2 > 65536)
		{
			printError("sendDREF", "About to overrun the send buffer!");
			return -4;
		}
		if (drefLen > 255)
		{
			printError("sendDREF", "dref %d is too long. Must be less than 256 characters.", i);
			return -1;
		}
		if (sizes[i] > 255)
		{
			printError("sendDREF", "size %d is too big. Must be less than 256.", i);
			return -2;
		}
		// Copy dref to buffer
		buffer[pos++] = (unsigned char)drefLen;
		memcpy(buffer + pos, drefs[i], drefLen);
		pos += drefLen;

		// Copy values to buffer
		buffer[pos++] = (unsigned char)sizes[i];
		memcpy(buffer + pos, values[i], sizes[i] * sizeof(float));
		pos += sizes[i] * sizeof(float);
	}

	// Send command
	if (sendUDP(sock, buffer, pos) < 0)
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
	char buffer[65536] = "GETD";
	buffer[5] = count;
	int len = 6;
    int i; // iterator
	for (i = 0; i < count; ++i)
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
	char buffer[65536];
	int result = readUDP(sock, buffer, 65536);

    if (result < 0)
    {
#ifdef _WIN32
        printError("getDREFs", "Read operation failed. (%d)", WSAGetLastError());
#else
        printError("getDREFs", "Read operation failed.");
#endif
        return -1;
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
    int i; // Iterator
	for (i = 0; i < count; ++i)
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
		// An error ocurred while sending.
		// sendDREFRequest will print an error message, so just return.
		return -1;
	}

	// Read Response
	if (getDREFResponse(sock, values, count, sizes) < 0)
	{
		// An error ocurred while reading the response.
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
int getPOSI(XPCSocket sock, double values[7], char ac)
{
	// Setup send command
	char buffer[6] = "GETP";
	buffer[5] = ac;

	// Send command
	if (sendUDP(sock, buffer, 6) < 0)
	{
		printError("getPOSI", "Failed to send command.");
		return -1;
	}

	// Get response
	char readBuffer[46];
	float f[7];
	int readResult = readUDP(sock, readBuffer, 46);

	// Copy response into values
	if (readResult < 0)
	{
		printError("getPOSI", "Failed to read response.");
		return -2;
	}
	else if (readResult == 34) /* lat/lon/h as 32-bit float */
	{
		memcpy(f, readBuffer + 6, 7 * sizeof(float));
		values[0] = (double)f[0];
		values[1] = (double)f[1];
		values[2] = (double)f[2];
		values[3] = (double)f[3];
		values[4] = (double)f[4];
		values[5] = (double)f[5];
		values[6] = (double)f[6];
	}
	else if (readResult == 46) /* lat/lon/h as 64-bit double */
	{
		memcpy(values, readBuffer + 6, 3 * sizeof(double));
		memcpy(f, readBuffer + 30, 4 * sizeof(float));
		values[3] = (double)f[0];
		values[4] = (double)f[1];
		values[5] = (double)f[2];
		values[6] = (double)f[3];
	}
	else
	{
		printError("getPOSI", "Unexpected response length.");
		return -3;
	}
	return 0;
}

int sendPOSI(XPCSocket sock, double values[], int size, char ac)
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
	char buffer[46] = "POSI";
	buffer[4] = 0xff; //Placeholder for message length
	buffer[5] = ac;
    int i; // iterator

	for (i = 0; i < 7; i++) // double for lat/lon/h
	{
		double val = -998;

		if (i < size)
		{
			val = values[i];
		}
		if (i < 3) /* lat/lon/h */
		{
			memcpy(&buffer[6 + i*8], &val, sizeof(double));
		}
		else /* attitude and gear */
		{
            float f = (float)val;
            memcpy(&buffer[18 + i*4], &f, sizeof(float));
		}
	}

	// Send Command
	if (sendUDP(sock, buffer, 46) < 0)
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
/****                          TERR functions                             ****/
/*****************************************************************************/
int sendTERRRequest(XPCSocket sock, double posi[3], char ac)
{
	// Setup send command
	char buffer[30] = "GETT";
	buffer[5] = ac;
	memcpy(&buffer[6], posi, 3 * sizeof(double));

	// Send command
	if (sendUDP(sock, buffer, 30) < 0)
	{
		printError("getTERR", "Failed to send command.");
		return -1;
	}
	return 0;
}

int getTERRResponse(XPCSocket sock, double values[11], char ac)
{
	// Get response
	char readBuffer[62];
	int readResult = readUDP(sock, readBuffer, 62);
	if (readResult < 0)
	{
		printError("getTERR", "Failed to read response.");
		return -2;
	}
	if (readResult != 62)
	{
		printError("getTERR", "Unexpected response length.");
		return -3;
	}

	// Copy response into outputs
	float f[8];
	ac = readBuffer[5];
	memcpy(values, readBuffer + 6, 3 * sizeof(double));
	memcpy(f, readBuffer + 30, 8 * sizeof(float));
	values[ 3] = (double)f[0];
	values[ 4] = (double)f[1];
	values[ 5] = (double)f[2];
	values[ 6] = (double)f[3];
	values[ 7] = (double)f[4];
	values[ 8] = (double)f[5];
	values[ 9] = (double)f[6];
	values[10] = (double)f[7];

	return 0;
}

int sendPOST(XPCSocket sock, double posi[], int size, double values[11], char ac)
{
	// Validate input
	if (ac < 0 || ac > 20)
	{
		printError("sendPOST", "aircraft should be a value between 0 and 20.");
		return -1;
	}
	if (size < 1 || size > 7)
	{
		printError("sendPOST", "size should be a value between 1 and 7.");
		return -2;
	}

	// Setup command
	char buffer[46] = "POST";
	buffer[4] = 0xff; //Placeholder for message length
	buffer[5] = ac;
	int i; // iterator

	for (i = 0; i < 7; i++) // double for lat/lon/h
	{
		double val = -998;

		if (i < size)
		{
			val = posi[i];
		}
		if (i < 3) /* lat/lon/h */
		{
			memcpy(&buffer[6 + i*8], &val, sizeof(double));
		}
		else /* attitude and gear */
		{
			float f = (float)val;
			memcpy(&buffer[18 + i*4], &f, sizeof(float));
		}
	}

	// Send Command
	if (sendUDP(sock, buffer, 46) < 0)
	{
		printError("sendPOST", "Failed to send command");
		return -3;
	}
	
	// Read Response
	int result = getTERRResponse(sock, values, ac);
	if (result < 0)
	{
		// A error ocurred while reading the response.
		// getTERRResponse will print an error message, so just return.
		return result;
	}
	return 0;
}

int getTERR(XPCSocket sock, double posi[3], double values[11], char ac)
{
	// Send Command
	int result = sendTERRRequest(sock, posi, ac);
	if (result < 0)
	{
		// An error ocurred while sending.
		// sendTERRRequest will print an error message, so just return.
		return result;
	}

	// Read Response
	result = getTERRResponse(sock, values, ac);
	if (result < 0)
	{
		// An error ocurred while reading the response.
		// getTERRResponse will print an error message, so just return.
		return result;
	}
	return 0;
}
/*****************************************************************************/
/****                        End TERR functions                           ****/
/*****************************************************************************/

/*****************************************************************************/
/****                          CTRL functions                             ****/
/*****************************************************************************/
int getCTRL(XPCSocket sock, float values[7], char ac)
{
	// Setup send command
	char buffer[6] = "GETC";
	buffer[5] = ac;

	// Send command
	if (sendUDP(sock, buffer, 6) < 0)
	{
		printError("getCTRL", "Failed to send command.");
		return -1;
	}

	// Get response
	char readBuffer[31];
	int readResult = readUDP(sock, readBuffer, 31);
	if (readResult < 0)
	{
		printError("getCTRL", "Failed to read response.");
		return -2;
	}
	if (readResult != 31)
	{
		printError("getCTRL", "Unexpected response length.");
		return -3;
	}

	// Copy response into values
	memcpy(values, readBuffer + 5, 4 * sizeof(float));
	values[4] = readBuffer[21];
	values[5] = *((float*)(readBuffer + 22));
	values[6] = *((float*)(readBuffer + 27));
	return 0;
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
		printError("sendCTRL", "size should be a value between 1 and 7.");
		return -2;
	}

	// Setup Command
	// 5 byte header + 5 float values * 4 + 2 byte values
	char buffer[31] = "CTRL";
	int cur = 5;
    int i; // iterator
	for (i = 0; i < 6; i++)
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
	*((float*)(buffer + 27)) = size == 7 ? values[6]: -998;

	// Send Command
	if (sendUDP(sock, buffer, 31) < 0)
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
	if (msg == NULL)
	{
		msg = "";
	}
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
	buffer[13] = (unsigned char)msgLen;
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
	if (sendUDP(sock, buffer, 7 + 12 * count) < 0)
	{
		printError("sendWYPT", "Failed to send command");
		return -2;
	}
	return 0;
}
/*****************************************************************************/
/****                      End Drawing functions                          ****/
/*****************************************************************************/

/*****************************************************************************/
/****                          View functions                             ****/
/*****************************************************************************/
int sendVIEW(XPCSocket sock, VIEW_TYPE view)
{
	// Validate Input
	if (view < XPC_VIEW_FORWARDS || view > XPC_VIEW_FULLSCREENNOHUD)
	{
		printError("sendVIEW", "Unrecognized view");
		return -1;
	}

	// Setup Command
	char buffer[9] = "VIEW";
	*((int*)(buffer + 5)) = view;

	// Send Command
	if (sendUDP(sock, buffer, 9) < 0)
	{
		printError("sendVIEW", "Failed to send command");
		return -2;
	}
	return 0;
}
/*****************************************************************************/
/****                        End View functions                           ****/
/*****************************************************************************/

/*****************************************************************************/
/****                          Comm functions                             ****/
/*****************************************************************************/
int sendCOMM(XPCSocket sock, const char* comm) {
	// Setup command
	// Max size is technically unlimited.
	unsigned char buffer[65536] = "COMM";
	int pos = 5;

	int commLen = strnlen(comm, 256);
	if (pos + commLen + 2 > 65536)
	{
		printError("sendCOMM", "About to overrun the send buffer!");
		return -4;
	}
	if (commLen > 255)
	{
		printError("sendCOMM", "comm is too long. Must be less than 256 characters.");
		return -1;
	}
	// Copy comm to buffer
	buffer[pos++] = (unsigned char)commLen;
	memcpy(buffer + pos, comm, commLen);
	pos += commLen;

	// Send command
	if (sendUDP(sock, buffer, pos) < 0)
	{
		printError("setDREF", "Failed to send command");
		return -3;
	}
	return 0;
}
/*****************************************************************************/
/****                        End Comm functions                           ****/
/*****************************************************************************/