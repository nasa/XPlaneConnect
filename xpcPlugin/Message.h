//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPC_MESSAGE_H
#define XPC_MESSAGE_H

#include "UDPSocket.h"

namespace XPC
{
	/// Represents a message received from an XPC client.
	///
	/// \author Jason Watkins
	/// \version 1.0
	/// \since 1.0
	/// \date Intial Version: 2015-04-11
	/// \date Last Updated: 2015-04-11
	class Message
	{
	public:
		/// Reads a datagram from the specified socket and interprets it as a
		/// message.
		///
		/// \param sock The socket to read from.
		/// \returns    A message parsed from the data read from sock. If no
		///             data was read or an error occurs, returns a message
		///             with the size set to 0.
		static Message ReadFrom(UDPSocket& sock);

		/// Gets the message header in binary form.
		unsigned long GetMagicNumber();

		/// Gets the message header.
		std::string GetHead();

		/// Gets the buffer underlying the message.
		const unsigned char* GetBuffer();

		/// Gets the size of the message in bytes.
		std::size_t GetSize();

		/// Gets the address this message was read from.
		struct sockaddr GetSource();

		/// Prints the contents of the message to the XPC log.
		void PrintToLog();

	private:
		Message();

		static const std::size_t bufferSize = 4096;
		unsigned char buffer[bufferSize];
		std::size_t size;
		struct sockaddr source;
	};
}

#endif