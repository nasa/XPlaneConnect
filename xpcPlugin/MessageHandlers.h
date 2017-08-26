// Copyright (c) 2013-2017 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPCPLUGIN_MESSAGEHANDLERS_H_
#define XPCPLUGIN_MESSAGEHANDLERS_H_
#include "Message.h"

#include <string>
#include <map>

namespace XPC
{
	/// A function that handles a message.
	typedef void(*MessageHandler)(const Message&);

	/// Handles incoming messages and manages connections.
	///
	/// \author Jason Watkins
	/// \version 1.1
	/// \since 1.0
	/// \date Intial Version: 2015-04-12
	/// \date Last Updated: 2015-05-11
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

		/// Sets the socket that message handlers use to send responses.
		static void SetSocket(UDPSocket* socket);

	private:
		// One handler per message type. Message types are descripbed on the
		// wiki at https://github.com/nasa/XPlaneConnect/wiki/Network-Information
		static void HandleConn(const Message& msg);
		static void HandleCtrl(const Message& msg);
		static void HandleData(const Message& msg);
		static void HandleDref(const Message& msg);
		static void HandleGetC(const Message& msg);
		static void HandleGetD(const Message& msg);
		static void HandleGetP(const Message& msg);
		static void HandlePosi(const Message& msg);
		static void HandleSimu(const Message& msg);
		static void HandleText(const Message& msg);
		static void HandleWypt(const Message& msg);
		static void HandleView(const Message& msg);
		static void HandleComm(const Message& msg);

		static void HandleXPlaneData(const Message& msg);
		static void HandleUnknown(const Message& msg);

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
