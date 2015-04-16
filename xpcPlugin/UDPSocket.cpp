//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#include "Log.h"
#include "UDPSocket.h"

namespace XPC
{
	UDPSocket::UDPSocket(unsigned short recvPort)
	{
#if LOG_VERBOSITY > 1
		Log::FormatLine("[SOCK] Opening socket (port:%d)",	recvPort);
#endif
		// Setup Port
		struct sockaddr_in localAddr;
		localAddr.sin_family = AF_INET;
		localAddr.sin_addr.s_addr = INADDR_ANY;
		localAddr.sin_port = htons(recvPort);
		
		//Create and bind the socket
#ifdef _WIN32
		WSADATA wsa;
		int startResult = WSAStartup(MAKEWORD(2, 2), &wsa);
		if (startResult != 0)
		{
#if LOG_VERBOSITY > 0
			Log::FormatLine("[SOCK] ERROR: WSAStartup failed with error code %i.", startResult);
#endif
			return;
		}
		if ((this->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
		{
#if LOG_VERBOSITY > 0
			int err = WSAGetLastError();
			Log::FormatLine("[SOCK] ERROR: Failed to open socket. (Error code %i)", err);
#endif
			return;
		}
#elif (__APPLE__ || __linux)
		if ((this->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		{
			Log::WriteLine("[SOCK] ERROR: Failed to open socket");
			return;
		}
		int optval = 1;
		setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		setsockopt(this->sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
#endif
		if (bind(this->sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) != 0)
		{
#ifdef _WIN32
#if LOG_VERBOSITY > 0
			int err = WSAGetLastError();
			Log::FormatLine("[SOCK] ERROR: Failed to bind socket. (Error code %i)", err);
#endif
#endif
			return;
		}

		//Set Timout
		int usTimeOut = 500;

#ifdef _WIN32
		DWORD msTimeOutWin = 1; // Minimum socket timeout in Windows is 1ms
		if(setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&msTimeOutWin, sizeof(msTimeOutWin)) != 0)
		{
#if LOG_VERBOSITY > 1
			int err = WSAGetLastError();
			Log::FormatLine("[SOCK] ERROR: Failed to set timeout. (Error code %i)", err);
#endif
		}
#else
		struct timeval tv;
		tv.tv_sec = 0;  /* Sec Timeout */
		tv.tv_usec = usTimeOut;  // Microsec Timeout
		setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
#endif
	}

	UDPSocket::~UDPSocket()
	{
#if LOG_VERBOSITY > 1
		Log::FormatLine("[SOCK] Closing socket (%d)", this->sock);
#endif
#ifdef _WIN32
		closesocket(this->sock);
#elif (__APPLE__ || __linux)
		close(this->sock);
#endif
	}

	int UDPSocket::Read(unsigned char* dst, int maxLen, sockaddr* recvAddr)
	{
		socklen_t recvaddrlen = sizeof(*recvAddr);
		int status = 0;

#ifdef _WIN32
		// Windows readUDP needs the select command- minimum timeout is 1ms.
		// Without this playback becomes choppy

		// Definitions
		FD_SET stReadFDS;
		FD_SET stExceptFDS;
		timeval tv;

		// Setup for Select
		FD_ZERO(&stReadFDS);
		FD_SET(sock, &stReadFDS);
		FD_ZERO(&stExceptFDS);
		FD_SET(sock, &stExceptFDS);
		tv.tv_sec = 0;  /* Sec Timeout */
		tv.tv_usec = 250;  // Microsec Timeout

		// Select Command
		int result = select(-1, &stReadFDS, (FD_SET *)0, &stExceptFDS, &tv);
#if LOG_VERBOSITY > 1
		if (result == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			Log::FormatLine("[SOCK] ERROR: Select failed. (Error code %i)", err);
		}
#endif
		if (result <= 0) // No Data or error
		{
			return -1;
		}

		// If no error: Read Data
		status = recvfrom(sock, (char*)dst, maxLen, 0, recvAddr, &recvaddrlen);
#else
		// For apple or linux-just read - will timeout in 0.5 ms
		status = (int)recvfrom(sock, dst, 5000, 0, recvAddr, &recvaddrlen);
#endif
		return status;
	}

	// TODO: Add IPV6 support
	void UDPSocket::SendTo(unsigned char* buffer, std::size_t len, std::string remoteHost, unsigned short remotePort)
	{
#if LOG_VERBOSITY > 4
		Log::FormatLine("[SOCK] Sending message to %s:%u (length=%u)", remoteHost.c_str(), remotePort, len);
#endif
		if (remoteHost == "localhost")
		{
			remoteHost = "127.0.0.1";
		}
		unsigned long ip;
		inet_pton(AF_INET, remoteHost.c_str(), &ip);
		SendTo(buffer, len, ip, remotePort);
	}

	void UDPSocket::SendTo(unsigned char* buffer, std::size_t len, unsigned long remoteIP, unsigned short remotePort)
	{
		struct sockaddr_in remoteAddr;
		remoteAddr.sin_family = AF_INET;
		remoteAddr.sin_port = htons(remotePort);
		remoteAddr.sin_addr.s_addr = remoteIP;

#ifdef _WIN32
		const char on = 1;
#else
		int on = 1;
#endif
		setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));

		buffer[4] = (unsigned char)len;
		if (sendto(sock, (char*)buffer, (int)len, 0, (const struct sockaddr *) &remoteAddr, sizeof(remoteAddr)) < 0)
		{
#if LOG_VERBOSITY > 0
			Log::FormatLine("[SOCK] Send failed. (remote: %s:%d)", remoteAddr, remotePort);
#endif
		}
	}
}