// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPCPLUGIN_MESSAGEHANDLERS_H_
#define XPCPLUGIN_MESSAGEHANDLERS_H_
#include "Message.h"

#include <string>
#include <map>

#include "XPLMCamera.h"


namespace XPC
{
	/// A function that handles a message.
	typedef void(*MessageHandler)(const Message&);
    
    enum class VIEW_TYPE
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
    };

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
		///			 to determine whether the sender is a new client. It then either loads
		///			 connection details for an existing client, or creates a new connection record
		///			 for new clients. Finally, the message handler checks the message type and
		///			 dispatches the message to the appropriate handler.
		/// \param msg The message to be processed.
		static void HandleMessage(Message& msg);

		/// Sets the socket that message handlers use to send responses.
		static void SetSocket(UDPSocket* socket);
		
		static void SendBeacon(const std::string& pluginVersion, unsigned short pluginReceivePort, int xplaneVersion);
		
		static void SendTerr(double pos[3], char aircraft);

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
		static void HandleGetT(const Message& msg);
		static void HandlePosi(const Message& msg);
		static void HandlePosT(const Message& msg);
		static void HandleSimu(const Message& msg);
		static void HandleText(const Message& msg);
		static void HandleWypt(const Message& msg);
		static void HandleView(const Message& msg);
		static void HandleComm(const Message& msg);

		static void HandleXPlaneData(const Message& msg);
		static void HandleUnknown(const Message& msg);
		
        static int CamCallback_RunwayCam( XPLMCameraPosition_t * outCameraPosition, int inIsLosingControl, void *inRefcon);
        static int CamCallback_ChaseCam( XPLMCameraPosition_t * outCameraPosition, int inIsLosingControl, void *inRefcon);
		
		struct CameraProperties{
			double loc[3];
			float direction[3];
			float zoom;
		};

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
