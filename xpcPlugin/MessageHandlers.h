//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPC_MESSAGEHANDLERS_H
#define XPC_MESSAGEHANDLERS_H
#include "Message.h"
#include "UDPSocket.h"

#include <string>
#include <unordered_map>

namespace XPC
{
	typedef void(*MessageHandler)(Message&);

	class MessageHandlers
	{
	public:
		static void HandleMessage(Message& msg);

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
			std::uint8_t id;
			std::string ip;
			std::uint16_t dstPort;
			std::uint16_t srcPort;
			std::uint8_t getdCount;
			std::string getdRequest[255];
		} ConnectionInfo;

		static std::unordered_map<std::string, ConnectionInfo> connections;
		static std::unordered_map<std::string, MessageHandler> handlers;
		static std::string connectionKey;
		static ConnectionInfo connection;
		static UDPSocket* sock;
	};
}

#endif