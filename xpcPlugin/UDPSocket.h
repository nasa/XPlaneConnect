// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPCPLUGIN_SOCKET_H_
#define XPCPLUGIN_SOCKET_H_

#include <cstdlib>
#include <string>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // Winsock Library
#elif (__APPLE__ || __linux)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif


namespace XPC
{
	/// Represents a UDP datagram socket used for reading data from and sending
	/// data to XPC clients.
	///
	/// \author Jason Watkins
	/// \version 1.1
	/// \since 1.0
	/// \date Intial Version: 2015-04-10
	/// \date Last Updated: 2015-05-11
	class UDPSocket
	{
	public:
		/// Initializes a new instance of the XPCSocket class bound to the
		/// specified receive port.
		///
		/// \param recvPort The port on which this instance will receive data.
		explicit UDPSocket(unsigned short recvPort);

		/// Closes the underlying socket for this instance.
		~UDPSocket();

		/// Reads the specified number of bytes into the data buffer and stores
		/// the remote endpoint.
		///
		/// \param buffer	 The array to copy the data into.
		/// \param size	   The number of bytes to read.
		/// \param remoteAddr When at least one byte is read, contains the address
		///                   of the remote host.
		/// \returns		  The number of bytes read, or a negative number if
		///                   an error occurs.
		int Read(unsigned char* buffer, int size, sockaddr* remoteAddr) const;

		/// Sends data to the specified remote endpoint.
		///
		/// \param data   The data to be sent.
		/// \param len    The number of bytes to send.
		/// \param remote The destination socket.
		void SendTo(const unsigned char* buffer, std::size_t len, sockaddr* remote) const;
		
		/// Gets a string containing the IP address and port contained in the given sockaddr.
		///
		/// \param addr The socket address to parse.
		/// \returns    A string representation of the socket address.
		static std::string GetHost(sockaddr* addr);
		
		static sockaddr GetAddr(std::string address, unsigned short port);
	private:
#ifdef _WIN32
		SOCKET sock;
#else
		int sock;
#endif
	};
}
#endif
