//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#include "MessageHandlers.h"
#include "DataManager.h"
#include "DataMaps.h"
#include "Drawing.h"
#include "Log.h"

namespace XPC
{
	std::unordered_map<std::string, MessageHandlers::ConnectionInfo> MessageHandlers::connections;
	std::unordered_map<std::string, MessageHandler> MessageHandlers::handlers(
	{
		// Common messages
		{ "CONN", MessageHandlers::HandleConn },
		{ "CTRL", MessageHandlers::HandleCtrl },
		{ "DATA", MessageHandlers::HandleData },
		{ "DREF", MessageHandlers::HandleDref },
		{ "GETD", MessageHandlers::HandleGetD },
		{ "POSI", MessageHandlers::HandlePosi },
		{ "SIMU", MessageHandlers::HandleSimu },
		{ "TEXT", MessageHandlers::HandleText },
		{ "WYPT", MessageHandlers::HandleWypt },
		// Not implemented messages
		{ "VIEW", MessageHandlers::HandleUnknown },
		// X-Plane data messages
		{ "DSEL", MessageHandlers::HandleXPlaneData },
		{ "USEL", MessageHandlers::HandleXPlaneData },
		{ "DCOC", MessageHandlers::HandleXPlaneData },
		{ "UCOC", MessageHandlers::HandleXPlaneData },
		{ "MOUS", MessageHandlers::HandleXPlaneData },
		{ "CHAR", MessageHandlers::HandleXPlaneData },
		{ "MENU", MessageHandlers::HandleXPlaneData },
		{ "SOUN", MessageHandlers::HandleXPlaneData },
		{ "FAIL", MessageHandlers::HandleXPlaneData },
		{ "RECO", MessageHandlers::HandleXPlaneData },
		{ "PAPT", MessageHandlers::HandleXPlaneData },
		{ "VEHN", MessageHandlers::HandleXPlaneData },
		{ "VEH1", MessageHandlers::HandleXPlaneData },
		{ "VEHA", MessageHandlers::HandleXPlaneData },
		{ "GSET", MessageHandlers::HandleXPlaneData },
		{ "OBJN", MessageHandlers::HandleXPlaneData },
		{ "OBJL", MessageHandlers::HandleXPlaneData },
		{ "GSET", MessageHandlers::HandleXPlaneData },
		{ "ISET", MessageHandlers::HandleXPlaneData },
		{ "BOAT", MessageHandlers::HandleXPlaneData },
	});

	std::string MessageHandlers::connectionKey;
	MessageHandlers::ConnectionInfo MessageHandlers::connection;
	UDPSocket* MessageHandlers::sock;

	void MessageHandlers::SetSocket(UDPSocket* socket)
	{
		MessageHandlers::sock = socket;
	}

	static std::string getIP(sockaddr* sa)
	{
		char ip[INET6_ADDRSTRLEN + 6] = { 0 };
		switch (sa->sa_family)
		{
		case AF_INET:
		{
			sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(sa);
			inet_ntop(AF_INET, &sin->sin_addr, ip, INET6_ADDRSTRLEN);
			break;
		}
		case AF_INET6:
		{
			sockaddr_in6* sin = reinterpret_cast<sockaddr_in6*>(sa);
			inet_ntop(AF_INET6, &sin->sin6_addr, ip, INET6_ADDRSTRLEN);
			break;
		}
		default:
			return "UNKNOWN";
		}
		return std::string(ip);
	}

	static std::uint16_t getPort(sockaddr* sa)
	{
		switch (sa->sa_family)
		{
		case AF_INET:
		{
			sockaddr_in *sin = reinterpret_cast<sockaddr_in*>(sa);
			return ntohs((*sin).sin_port);
		}
		case AF_INET6:
		{
			sockaddr_in6 *sin = reinterpret_cast<sockaddr_in6*>(sa);
			return ntohs((*sin).sin6_port);
		}
		default:
			return ~0;
		}
	}

	void MessageHandlers::HandleMessage(Message& msg)
	{
		// Make sure we really have a message to handle.
		std::string head = msg.GetHead();
		if (head == "")
		{
			return;
		}
		msg.PrintToLog();

		// Set current connection
		sockaddr sourceaddr = msg.GetSource();
		std::string ip = getIP(&sourceaddr);
		std::uint16_t port = getPort(&sourceaddr);
		connectionKey = ip + ":" + std::to_string(port);
#if LOG_VERBOSITY > 4
		Log::FormatLine("[MSGH] Handling message from %s", connectionKey.c_str());
#endif
		auto conn = connections.find(connectionKey);
		if (conn == connections.end()) // New connection
		{
			connection = MessageHandlers::ConnectionInfo
			{
				// If this is a new connection, that means we just added an elment
				// to connections. As long as we never remove elements, the size of
				// connections will serve as a unique id.
				connections.size(),
				ip,
				49008, // By default, send information to the client on this port.
				port,
				0
			};
			connections[connectionKey] = connection;
#if LOG_VERBOSITY > 4
			Log::FormatLine("[MSGH] New connection. ID=%u, Remote=%s:%u", connection.id, ip.c_str(), port);
#endif
		}
		else
		{
			connection = (*conn).second;
#if LOG_VERBOSITY > 4
			Log::FormatLine("[MSGH] Existing connection. ID=%u, Remote=%s:%u",
				connection.id, connection.ip.c_str(), connection.srcPort);
#endif
		}
		
		// Check if there is a handler for this message type. If so, execute
		// that handler. Othewwise, execute the unknown message handler.
		auto iter = handlers.find(head);
		if (iter != handlers.end())
		{
			MessageHandler handler = (*iter).second;
			handler(msg);
		}
		else
		{
			MessageHandlers::HandleUnknown(msg);
		}
	}

	void MessageHandlers::HandleConn(Message& msg)
	{
		const std::uint8_t* buffer = msg.GetBuffer();

		// Store new port
		connection.dstPort = *((std::uint16_t*)(buffer + 5));
		connections[connectionKey] = connection;

		// Create response
		std::uint8_t response[6] = "CONF";
		response[5] = connection.id;

		// Update log
#if LOG_VERBOSITY > 0
		Log::FormatLine("[CONN] ID: %u New destination port: %u",
			connection.id, connection.dstPort);
#endif

		// Send response
		sock->SendTo(response, 6, connection.ip, connection.dstPort);
	}

	void MessageHandlers::HandleCtrl(Message& msg)
	{
		// Update Log
#if LOG_VERBOSITY > 0
		Log::FormatLine("[CTRL] Message Received (Conn %i)", connection.id);
#endif

		const std::uint8_t* buffer = msg.GetBuffer();
		std::size_t size = msg.GetSize();
		//Legacy packets that don't specify an aircraft number should be 26 bytes long.
		//Packets specifying an A/C num should be 27 bytes.
		if (size != 26 && size != 27)
		{
#if LOG_VERBOSITY > 1
			Log::FormatLine("[CTRL] ERROR: Unexpected message length (%i)", size);
#endif
			return;
		}

		// Parse message data
		float pitch = *((float*)(buffer + 5));
		float roll = *((float*)(buffer + 9));
		float yaw = *((float*)(buffer + 13));
		float thr = *((float*)(buffer + 17));
		std::int8_t gear = buffer[21];
		float flaps = *((float*)(buffer + 22));
		std::uint8_t aircraft = 0;
		if (size == 27)
		{
			aircraft = buffer[26];
		}

		float thrArray[8];
		for (int i = 0; i < 8; ++i)
		{
			thrArray[i] = thr;
		}

		DataManager::Set(DREF::YokePitch, pitch, aircraft);
		DataManager::Set(DREF::YokeRoll, roll, aircraft);
		DataManager::Set(DREF::YokeHeading, yaw, aircraft); 
		DataManager::Set(XPC::DREF::ThrottleSet, thrArray, 8, aircraft);
		DataManager::Set(XPC::DREF::ThrottleActual, thrArray, 8, aircraft);
		if (aircraft == 0)
		{
			DataManager::Set("sim/flightmodel/engine/ENGN_thro_override", thrArray, 1);
		}
		if (gear != -1)
		{
			DataManager::SetGear(gear, false, aircraft);
		}
		if (flaps < -999.5 || flaps > -997.5)
		{
			DataManager::Set(DREF::FlapSetting, flaps, aircraft);
		}
	}

	void MessageHandlers::HandleData(Message& msg)
	{
		// Parse data
		const std::uint8_t* buffer = msg.GetBuffer();
		std::size_t size = msg.GetSize();
		std::size_t numCols = (size - 5) / 36;
		if (numCols > 0)
		{
#if LOG_VERBOSITY > 3
			Log::FormatLine("[DATA] Message Received (Conn %i)", connection.id);
#endif
		}
		if (numCols > 32) // Error. Will overflow values
		{
#if LOG_VERBOSITY > 2
			Log::FormatLine("[DATA] ERROR numCols to large.");
#endif
			return;
		}
		else
		{
#if LOG_VERBOSITY > 2
			Log::FormatLine("[DATA] WARNING: Empty data packet received (Conn %i)", connection.id);
#endif
			return;
		}
		float values[32][9];
		for (int i = 0; i < numCols; ++i)
		{
			values[i][0] = buffer[5 + 36 * i];
			memcpy(values[i] + 1, buffer + 9 + 36 * i, 9 * sizeof(float));
		}

		// Update log

		float savedAlpha = -998;
		float savedHPath = -998;
		for (int i = 0; i < numCols; ++i)
		{
			std::uint8_t dataRef = (std::uint8_t)values[i][0];
			if (dataRef > 134)
			{
				Log::FormatLine("[DATA] ERROR: DataRef # must be between 0 - 134 (Received: %hi)", (int)dataRef);
			}

			switch (dataRef)
			{
			case 3: // Velocity
			{
				float theta = DataManager::GetFloat(DREF::Pitch);
				float alpha = savedAlpha != -998 ? savedAlpha : DataManager::GetFloat(DREF::AngleOfAttack);
				float hpath = savedHPath != -998 ? savedHPath : DataManager::GetFloat(DREF::HPath);
				if (alpha != alpha || hpath != hpath)
				{
#if LOG_VERBOSITY > 0
					Log::WriteLine("[DATA] ERROR: Value must be a number (NaN received)");
#endif
					break;
				}
				const float deg2rad = (float) 0.0174532925;
				int ind[3] = { 1, 3, 4 };
				for (int j = 0; j < 3; ++j)
				{
					float v = values[i][ind[j]];
					if (v != -998)
					{
						DataManager::Set(DREF::LocalVX, v*cos((theta - alpha)*deg2rad)*sin(hpath*deg2rad));
						DataManager::Set(DREF::LocalVY, v*sin((theta - alpha)*deg2rad));
						DataManager::Set(DREF::LocalVZ, -v*cos((theta - alpha)*deg2rad)*cos(hpath*deg2rad));
					}
				}
				break;
			}
			case 17: // Orientation
			{
				float orient[3]
				{
					values[i][1] == -998 ? DataManager::GetFloat(DREF::Pitch) : values[i][1],
					values[i][2] == -998 ? DataManager::GetFloat(DREF::Roll) : values[i][2],
					values[i][3] == -998 ? DataManager::GetFloat(DREF::HeadingTrue) : values[i][3]
				};
				DataManager::SetOrientation(orient);
				break;
			}
			case 18: // Alpha, hpath etc.
			{
				if (values[i][1] != values[i][1] || values[i][3] != values[i][3])
				{
#if LOG_VERBOSITY > 0
					Log::WriteLine("[DATA] ERROR: Value must be a number (NaN received)");
#endif
					break;
				}
				if (values[i][1] != -998)
				{
					savedAlpha = values[i][1];
				}
				if (values[i][3] != -998)
				{
					savedHPath = values[i][3];
				}
				break;
			}
			case 20: // Position
			{
				float pos[3]
				{
					values[i][2] == -998 ? DataManager::GetFloat(DREF::Latitude) : values[i][2],
					values[i][3] == -998 ? DataManager::GetFloat(DREF::Longitude) : values[i][3],
					values[i][4] == -998 ? DataManager::GetFloat(DREF::AGL) : values[i][4]
				};
				DataManager::SetPosition(pos);
				break;
			}
			case 25: // Throttle
			{
				if (values[i][1] != values[i][1])
				{
#if LOG_VERBOSITY > 0
					Log::WriteLine("[DATA] ERROR: Value must be a number (NaN received)");
#endif
					break;
				}
				float thr[8];
				for (int j = 0; j < 8; ++j)
				{
					thr[j] = values[i][1];
				}
				DataManager::Set(DREF::ThrottleSet, thr, 8);
				break;
			}
			default: // Non-Special dataRefs
			{
				float line[8];
				memcpy(line, values[i] + 1, 8 * sizeof(float));
				for (int j = 0; j < 8; ++j)
				{
#if LOG_VERBOSITY > 1
					Log::FormatLine("Setting Dataref %i.%i to %f", dataRef, j, line[j]);
#endif

					if (dataRef == 14 && j == 0)
					{
						DataManager::SetGear(line[0], true);
						continue;
					}

					XPC::DREF dref = XPC::XPData[dataRef][j];
					if (dref == XPC::DREF::None)
					{
						HandleXPlaneData(msg);
					}
					else
					{
						XPC::DataManager::Set(dref, line, 8);
					}
				}
			}
			}
		}
	}

	void MessageHandlers::HandleDref(Message& msg)
	{
		const std::uint8_t* buffer = msg.GetBuffer();
		std::uint8_t len = buffer[5];
		std::string dref = std::string((char*)buffer + 6, len);

		std::uint8_t valueCount = buffer[6 + len];
		float values[40];
		memcpy(values, buffer + len + 7, valueCount * sizeof(float));

#if LOG_VERBOSITY > 1
		Log::FormatLine("[DREF] Request to set DREF value received (Conn %i): %s", connection.id, dref.c_str());
#endif

		DataManager::Set(dref, values, valueCount);
	}

	void MessageHandlers::HandleGetD(Message& msg)
	{
		const std::uint8_t* buffer = msg.GetBuffer();
		std::uint8_t drefCount = buffer[5];
		if (drefCount == 0) // Use last request
		{
#if LOG_VERBOSITY > 0
			Log::FormatLine("[GETD] DATA Requested: Repeat last request from connection %i (%i data refs)",
				connection.id, connection.getdCount);
#endif
			if (connection.getdCount == 0) // No previous request to use
			{
#if LOG_VERBOSITY > 1
				Log::FormatLine("[GETD] ERROR: No previous requests from connection %i.", connection.id);
#endif
				return;
			}
		}
		else // New request
		{
#if LOG_VERBOSITY > 0
			Log::FormatLine("[GETD] DATA Requested: New Request for connection %i (%i data refs)",
				connection.id, drefCount);
#endif
			std::size_t ptr = 6;
			for (int i = 0; i < drefCount; ++i)
			{
				std::uint8_t len = buffer[ptr];
				connection.getdRequest[i] = std::string((char*)buffer + 1 + ptr, len);
				ptr += 1 + len;
			}
			connection.getdCount = drefCount;
			connections[connectionKey] = connection;
		}

		std::uint8_t response[4096] = "RESP";
		response[5] = drefCount;
		std::size_t cur = 6;
		for (int i = 0; i < drefCount; ++i)
		{
			float values[255];
			int count = DataManager::Get(connection.getdRequest[i], values, 255);
			response[cur++] = count;
			memcpy(response + cur, values, count * sizeof(float));
			cur += count * sizeof(float);
		}

		sock->SendTo(response, cur, connection.ip, connection.dstPort);
	}

	void MessageHandlers::HandlePosi(Message& msg)
	{
		// Update log
#if LOG_VERBOSITY > 0
		Log::FormatLine("[POSI] Message Received (Conn %i)", connection.id);
#endif

		const std::uint8_t* buffer = msg.GetBuffer();
		const std::size_t size = msg.GetSize();
		if (size < 34)
		{
#if LOG_VERBOSITY > 1
			Log::FormatLine("[POSI] ERROR: Unexpected size: %i (Expected at least 34)", size);
#endif
			return;
		}

		std::int8_t aircraft = buffer[5];
		float gear = *((float*)(buffer + 30));
		float pos[3];
		float orient[3];
		memcpy(pos, buffer + 6, 12);
		memcpy(orient, buffer + 18, 12);

		if (aircraft > 0)
		{
			// Enable AI for the aircraft we are setting
			float ai[20];
			std::size_t result = DataManager::GetFloatArray(DREF::PauseAI, ai, 20);
			if (result == 20) // Only set values if they were retrieved successfully.
			{
				ai[aircraft] = 1;
				DataManager::Set(DREF::PauseAI, ai, 0, 20);
			}
		}

		DataManager::SetPosition(pos, aircraft);
		DataManager::SetOrientation(orient, aircraft);
		if (gear != -1)
		{
			DataManager::SetGear(gear, true, aircraft);
		}
	}

	void MessageHandlers::HandleSimu(Message& msg)
	{
		// Update log
#if LOG_VERBOSITY > 0
		Log::FormatLine("[SIMU] Message Received (Conn %i)", connection.id);
#endif

		const std::uint8_t* buffer = msg.GetBuffer();

		// Set DREF
		int value = buffer[5];
		DataManager::Set(DREF::Pause, &value, 1);

#if LOG_VERBOSITY > 2
		if (buffer[5] == 0)
		{
			Log::FormatLine("[SIMU] Simulation Resumed (Conn %i)", connection.id);
		}
		else
		{
			Log::FormatLine("[SIMU] Simulation Paused (Conn %i)", connection.id);
		}
#endif
	}

	void MessageHandlers::HandleText(Message& msg)
	{
		// Update Log
#if LOG_VERBOSITY > 0
		Log::FormatLine("[TEXT] Message Received (Conn %i)", connection.id);
#endif

		std::size_t len = msg.GetSize();
		const std::uint8_t*  buffer = msg.GetBuffer();

		char text[256] = { 0 };
		if (len < 14)
		{
#if LOG_VERBOSITY > 1
			Log::WriteLine("[TEXT] ERROR: Length less than 14 bytes");
#endif
			return;
		}
		size_t msgLen = (unsigned char)buffer[13];
		if (msgLen == 0)
		{
			Drawing::ClearMessage();
#if LOG_VERBOSITY > 2
			Log::WriteLine("[TEXT] Text cleared");
#endif
		}
		else
		{
			int x = *((int*)(buffer + 5));
			int y = *((int*)(buffer + 9));
			strncpy(text, (char*)buffer + 14, msgLen);
			Drawing::SetMessage(x, y, text);
#if LOG_VERBOSITY > 2
			Log::WriteLine("[TEXT] Text set");
#endif
		}
	}

	void MessageHandlers::HandleWypt(Message& msg)
	{
		// Update Log
#if LOG_VERBOSITY > 0
		Log::FormatLine("[WYPT] Message Received (Conn %i)", connection.id);
#endif

		// Parse data
		const std::uint8_t* buffer = msg.GetBuffer();
		std::uint8_t op = buffer[5];
		std::uint8_t count = buffer[6];
		Waypoint points[255];
		const std::uint8_t* ptr = buffer + 7;
		for (size_t i = 0; i < count; ++i)
		{
			points[i].latitude = *((float*)ptr);
			points[i].longitude = *((float*)(ptr + 4));
			points[i].altitude = *((float*)(ptr + 8));
			ptr += 12;
		}

		// Perform operation
#if LOG_VERBOSITY > 2
		Log::FormatLine("[WYPT] Performing operation %i", op);
#endif
		switch (op)
		{
		case xpc_WYPT_ADD:
			Drawing::AddWaypoints(points, count);
			break;
		case xpc_WYPT_DEL:
			Drawing::RemoveWaypoints(points, count);
			break;
		case xpc_WYPT_CLR:
			Drawing::ClearWaypoints();
			break;
		default:
#if LOG_VERBOSITY > 1
			Log::FormatLine("[WYPT] ERROR: %i is not a valid operation.", op);
#endif
			break;
		}
	}

	void MessageHandlers::HandleXPlaneData(Message& msg)
	{
#if LOG_VERBOSITY > 1
		Log::WriteLine("[MSGH] Sending raw data to X-Plane");
#endif
		sock->SendTo((std::uint8_t*)msg.GetBuffer(), msg.GetSize(), "127.0.0.1", 49000);
	}

	void MessageHandlers::HandleUnknown(Message& msg)
	{
		// UPDATE LOG
#if LOG_VERBOSITY > 0
		XPC::Log::FormatLine("[EXEC] ERROR: Unknown packet type %s", msg.GetHead().c_str());
#endif
	}
}