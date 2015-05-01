//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPC_MESSAGEHANDLERS_H
#define XPC_MESSAGEHANDLERS_H
#include "Message.h"

#include <string>
#include <map>

namespace XPC
{
	/// A function that handles a message.
	typedef void(*MessageHandler)(Message&);

	/// Handles incommming messages and manages connections.
	///
	/// \author Jason Watkins
	/// \version 1.0
	/// \since 1.0
	/// \date Intial Version: 2015-04-12
	/// \date Last Updated: 2015-04-12
	class MessageHandlers
	{
	public:
		/// The first stop for all messages to the plugin after they are read from the
		/// socket.
		///
		/// \details After a message is read, HandleMessage analyzes the sender's network address
		///          to determine whether the sender is a new client. It then either loads
		///          connection details for an existing client, or creates a new connection record
		///          for new clients. Finally, the message handler checks the message type and
		///          dispatches the message to the appropriate handler.
		/// \param msg The message to be processed.
		static void HandleMessage(Message& msg);

		/// Sets the socke that message handlers use to send responses.
		static void SetSocket(UDPSocket* socket);

	private:
		static void HandleConn(Message& msg);
		static void HandleCtrl(Message& msg);
		static void HandleData(Message& msg);
		static void HandleDref(Message& msg);
		static void HandleGetD(Message& msg);
		static void HandlePosi(Message& msg);
		static void HandleSimu(Message& msg);
		static void HandleText(Message& msg);
		static void HandleWypt(Message& msg);
		static void HandleXPlaneData(Message& msg);
		static void HandleUnknown(Message& msg);

		typedef struct
		{
			unsigned char id;
			sockaddr addr;
			unsigned char getdCount;
			std::string getdRequest[255];
		} ConnectionInfo;

		static std::map<std::string, ConnectionInfo> connections;
		static std::map<std::string, MessageHandler> handlers;
		static std::string connectionKey; // The current connection ip:port string
		static ConnectionInfo connection; // The current connection record
		static UDPSocket* sock; // Outgoing network socket
	};
}

#endif