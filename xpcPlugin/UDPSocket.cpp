// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
#include "Log.h"
#include "UDPSocket.h"

#include <cstring>
#include <cstdio>

namespace XPC
{
	const static std::string tag = "SOCK";

	UDPSocket::UDPSocket(unsigned short recvPort)
	{
		Log::FormatLine(LOG_TRACE, tag, "Opening socket (port:%d)",	recvPort);
		// Setup Port
		struct sockaddr_in localAddr;
		localAddr.sin_family = AF_INET;
		localAddr.sin_addr.s_addr = INADDR_ANY;
		localAddr.sin_port = htons(recvPort);

		// Create and bind the socket
#ifdef _WIN32
		WSADATA wsa;
		int startResult = WSAStartup(MAKEWORD(2, 2), &wsa);
		if (startResult != 0)
		{
			Log::FormatLine(LOG_FATAL, tag, "ERROR: WSAStartup failed with error code %i.", startResult);
			this->sock = ~0;
			return;
		}
		if ((this->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
		{
			int err = WSAGetLastError();
			Log::FormatLine(LOG_FATAL, tag, "ERROR: Failed to open socket. (Error code %i)", err);
			return;
		}
#elif (__APPLE__ || __linux)
		if ((this->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		{
			Log::WriteLine(LOG_FATAL, tag, "ERROR: Failed to open socket");
			return;
		}
		int optval = 1;
		setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		setsockopt(this->sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
#endif
		if (bind(this->sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) != 0)
		{
#ifdef _WIN32
			int err = WSAGetLastError();
			Log::FormatLine(LOG_FATAL, tag, "ERROR: Failed to bind socket. (Error code %i)", err);
#else
			Log::WriteLine(LOG_FATAL, tag, "ERROR: Failed to bind socket.");
#endif
			return;
		}

		// Set timeout period for SendTo to 1 millisecond
		// Without this, playback may become choppy due to process blocking
#ifdef _WIN32
		// Minimum socket timeout in Windows is 1 millisecond (0 makes it blocking)
		DWORD timeout = 1;
#else
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 1000;
#endif
		if (setsockopt(this->sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
		{
#ifdef _WIN32
			int err = WSAGetLastError();
			Log::FormatLine(LOG_ERROR, tag, "ERROR: Failed to set timeout. (Error code %i)", err);
#else
			Log::WriteLine(LOG_ERROR, tag, "ERROR: Failed to set timeout.");
#endif
		}
	}

	UDPSocket::~UDPSocket()
	{
		Log::FormatLine(LOG_TRACE, tag, "Closing socket (%d)", this->sock);
#ifdef _WIN32
		closesocket(this->sock);
#elif (__APPLE__ || __linux)
		close(this->sock);
#endif
	}

	int UDPSocket::Read(unsigned char* dst, int maxLen, sockaddr* recvAddr) const
	{
		socklen_t recvaddrlen = sizeof(*recvAddr);

		// For Read, use the select command - minimum timeout of 0 makes it polling.
		// Without this, playback may become choppy due to process blocking

		// Definitions
		fd_set stReadFDS;
		fd_set stExceptFDS;
		struct timeval timeout;

		// Setup for Select
		FD_ZERO(&stReadFDS);
		FD_SET(sock, &stReadFDS);
		FD_ZERO(&stExceptFDS);
		FD_SET(sock, &stExceptFDS);

		// Set timeout period for select to 0 to poll the socket
		timeout.tv_sec = 0;
		timeout.tv_usec = 0;

		// Select Command
		int status = select(sock+1, &stReadFDS, NULL, &stExceptFDS, &timeout);
		if (status < 0)
		{
#ifdef _WIN32
			int err = WSAGetLastError();
			Log::FormatLine(LOG_ERROR, tag, "ERROR: Select failed. (Error code %i)", err);
#else
			Log::WriteLine(LOG_ERROR, tag, "ERROR: Select failed.");
#endif
			return -1;
		}
		if (status == 0)
		{
			// No data
			return -1;
		}

		// If no error: Read Data
		status = recvfrom(sock, (char*)dst, maxLen, 0, recvAddr, &recvaddrlen);
		if (status < 0)
		{
#ifdef _WIN32
			int err = WSAGetLastError();
			Log::FormatLine(LOG_ERROR, tag, "ERROR: Receive failed. (Error code %i)", err);
#else
			Log::WriteLine(LOG_ERROR, tag, "ERROR: Receive failed.");
#endif
		}
		return status;
	}

	void UDPSocket::SendTo(const unsigned char* buffer, std::size_t len, sockaddr* remote) const
	{
		if (sendto(sock, (char*)buffer, (int)len, 0, remote, sizeof(*remote)) < 0)
		{
			Log::FormatLine(LOG_ERROR, tag, "Send failed. (remote: %s)", GetHost(remote).c_str());
		}
		else
		{
			Log::FormatLine(LOG_INFO, tag, "Send succeeded. (remote: %s)", GetHost(remote).c_str());
		}
	}
	
	sockaddr UDPSocket::GetAddr(std::string address, unsigned short port)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(address.c_str());
		addr.sin_port = htons(port);

		sockaddr* sa = reinterpret_cast<sockaddr*>(&addr);
		return *sa;
	}

	std::string UDPSocket::GetHost(sockaddr* sa)
	{
		char ip[INET6_ADDRSTRLEN + 6] = { 0 };
		switch (sa->sa_family)
		{
		case AF_INET:
		{
			sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(sa);
			inet_ntop(AF_INET, &sin->sin_addr, ip, INET6_ADDRSTRLEN);
			int len = strnlen(ip, INET6_ADDRSTRLEN);
			ip[len++] = ':';
			std::sprintf(ip + len, "%u", ntohs((*sin).sin_port));
			break;
		}
		case AF_INET6:
		{
			sockaddr_in6* sin = reinterpret_cast<sockaddr_in6*>(sa);
			inet_ntop(AF_INET6, &sin->sin6_addr, ip, INET6_ADDRSTRLEN);
			int len = strnlen(ip, INET6_ADDRSTRLEN);
			ip[len++] = ':';
			std::sprintf(ip + len, "%u", ntohs((*sin).sin6_port));
			break;
		}
		default:
			return "UNKNOWN";
		}
		return std::string(ip);
	}
}
