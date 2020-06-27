// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
//
// X-Plane API
// Copyright(c) 2008, Sandy Barbour and Ben Supnik All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files(the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//	 * Redistributions of source code must retain the above copyright notice,
//	   this list of conditions and the following disclaimer.
//	 * Neither the names of the authors nor that of X - Plane or Laminar Research
//	   may be used to endorse or promote products derived from this software
//	   without specific prior written permission from the authors or
//	   Laminar Research, respectively.

#include "MessageHandlers.h"
#include "DataManager.h"
#include "Drawing.h"
#include "Log.h"

#include "XPLMUtilities.h"
#include "XPLMScenery.h"
#include "XPLMGraphics.h"

#include <cmath>
#include <cstring>
#include <cstdint>

#define MULTICAST_GROUP "239.255.1.1"
#define MULITCAST_PORT 49710

namespace XPC
{
	std::map<std::string, MessageHandlers::ConnectionInfo> MessageHandlers::connections;
	std::map<std::string, MessageHandler> MessageHandlers::handlers;

	std::string MessageHandlers::connectionKey;
	MessageHandlers::ConnectionInfo MessageHandlers::connection;
	UDPSocket* MessageHandlers::sock;

	static sockaddr multicast_address = UDPSocket::GetAddr(MULTICAST_GROUP, MULITCAST_PORT);
	
	// define a static terrain probe handler (do not re-create probe for each query)
	XPLMProbeRef Terrain_probe = nullptr;

	void MessageHandlers::SetSocket(UDPSocket* socket)
	{
		Log::WriteLine(LOG_TRACE, "MSGH", "Setting socket");
		MessageHandlers::sock = socket;
	}

	void MessageHandlers::HandleMessage(Message& msg)
	{
		if (handlers.size() == 0)
		{
			Log::WriteLine(LOG_TRACE, "MSGH", "Initializing handlers");
			// Common messages
			handlers.insert(std::make_pair("CONN", MessageHandlers::HandleConn));
			handlers.insert(std::make_pair("CTRL", MessageHandlers::HandleCtrl));
			handlers.insert(std::make_pair("DATA", MessageHandlers::HandleData));
			handlers.insert(std::make_pair("DREF", MessageHandlers::HandleDref));
			handlers.insert(std::make_pair("GETD", MessageHandlers::HandleGetD));
			handlers.insert(std::make_pair("POSI", MessageHandlers::HandlePosi));
			handlers.insert(std::make_pair("POST", MessageHandlers::HandlePosT));
			handlers.insert(std::make_pair("SIMU", MessageHandlers::HandleSimu));
			handlers.insert(std::make_pair("TEXT", MessageHandlers::HandleText));
			handlers.insert(std::make_pair("WYPT", MessageHandlers::HandleWypt));
			handlers.insert(std::make_pair("VIEW", MessageHandlers::HandleView));
			handlers.insert(std::make_pair("GETC", MessageHandlers::HandleGetC));
			handlers.insert(std::make_pair("GETP", MessageHandlers::HandleGetP));
			handlers.insert(std::make_pair("COMM", MessageHandlers::HandleComm));
			handlers.insert(std::make_pair("GETT", MessageHandlers::HandleGetT));
			// X-Plane data messages
			handlers.insert(std::make_pair("DSEL", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("USEL", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("DCOC", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("UCOC", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("MOUS", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("CHAR", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("MENU", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("SOUN", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("FAIL", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("RECO", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("PAPT", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("VEHN", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("VEH1", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("VEHA", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("GSET", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("OBJN", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("OBJL", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("GSET", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("ISET", MessageHandlers::HandleXPlaneData));
			handlers.insert(std::make_pair("BOAT", MessageHandlers::HandleXPlaneData));
		}

		// Make sure we really have a message to handle.
		std::string head = msg.GetHead();
		if (head == "")
		{
			Log::WriteLine(LOG_WARN, "MSGH", "Warning: HandleMessage called with empty message.");
			return; // No Message to handle
		}

		// Set current connection
		sockaddr sourceaddr = msg.GetSource();
		connectionKey = UDPSocket::GetHost(&sourceaddr);
		Log::FormatLine(LOG_INFO, "MSGH", "Handling message from %s", connectionKey.c_str());
		std::map<std::string, ConnectionInfo>::iterator conn = connections.find(connectionKey);
		if (conn == connections.end()) // New connection
		{
			connection = MessageHandlers::ConnectionInfo();
			// If this is a new connection, that means we just added an elment
			// to connections. As long as we never remove elements, the size of
			// connections will serve as a unique id.
			connection.id = static_cast<unsigned char>(connections.size());
			connection.addr = sourceaddr;
			connection.getdCount = 0;
			connections[connectionKey] = connection;
			Log::FormatLine(LOG_DEBUG, "MSGH", "New connection. ID=%u, Remote=%s",
				connection.id, connectionKey.c_str());
		}
		else
		{
			connection = (*conn).second;
			Log::FormatLine(LOG_DEBUG, "MSGH", "Existing connection. ID=%u, Remote=%s",
				connection.id, connectionKey.c_str());
		}

		msg.PrintToLog();
		// Check if there is a handler for this message type. If so, execute
		// that handler. Otherwise, execute the unknown message handler.
		std::map<std::string, MessageHandler>::iterator iter = handlers.find(head);
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

	void MessageHandlers::SendBeacon(const std::string& pluginVersion, unsigned short  pluginReceivePort, int xplaneVersion) {

		unsigned char response[128] = "BECN";

		std::size_t cur = 5;

		// 2 bytes plugin port
		*((uint16_t *)(response + cur)) = pluginReceivePort;
		cur += sizeof(uint16_t);

		// 4 bytes xplane version
		*((uint32_t*)(response + cur)) = xplaneVersion;
		cur += sizeof(uint32_t);

		// plugin version
		int len = pluginVersion.length();
		memcpy(response + cur, pluginVersion.c_str(), len);
		cur += strlen(pluginVersion.c_str()) + len;

		sock->SendTo(response, cur, &multicast_address);
	}

	void MessageHandlers::HandleConn(const Message& msg)
	{
		const unsigned char* buffer = msg.GetBuffer();

		// Store new port
		unsigned short port = *((unsigned short*)(buffer + 5));
		sockaddr* sa = &connection.addr;
		switch (sa->sa_family)
		{
		case AF_INET: // IPV4 address
		{
			sockaddr_in* sin = reinterpret_cast<sockaddr_in*>(sa);
			(*sin).sin_port = htons(port);
			break;
		}
		case AF_INET6: // IPV6 addres
		{
			sockaddr_in6* sin = reinterpret_cast<sockaddr_in6*>(sa);
			(*sin).sin6_port = htons(port);
			break;
		}
		default:
			Log::WriteLine(LOG_ERROR, "CONN", "ERROR: Unknown address type.");
			return;
		}
		connections.erase(connectionKey);
		connectionKey = UDPSocket::GetHost(&connection.addr);
		connections[connectionKey] = connection;

		// Create response
		unsigned char response[6] = "CONF";
		response[5] = connection.id;

		// Update log
		Log::FormatLine(LOG_TRACE, "CONN", "ID: %u New destination port: %u",
			connection.id, port);

		// Send response
		sock->SendTo(response, 6, &connection.addr);
	}

	void MessageHandlers::HandleCtrl(const Message& msg)
	{
		// Update Log
		Log::FormatLine(LOG_TRACE, "CTRL", "Message Received (Conn %i)", connection.id);

		const unsigned char* buffer = msg.GetBuffer();
		std::size_t size = msg.GetSize();
		// Legacy packets that don't specify an aircraft number should be 26 bytes long.
		// Packets specifying an A/C num should be 27 bytes. Packets specifying a speedbrake
		// should be 31 bytes.
		if (size != 26 && size != 27 && size != 31)
		{
			Log::FormatLine(LOG_ERROR, "CTRL", "ERROR: Unexpected message length (%i)", size);
			return;
		}

		// Parse message data
		float pitch = *((float*)(buffer + 5));
		float roll = *((float*)(buffer + 9));
		float yaw = *((float*)(buffer + 13));
		float throttle = *((float*)(buffer + 17));
		char gear = buffer[21];
		float flaps = *((float*)(buffer + 22));
		unsigned char aircraftNumber = 0;
		if (size >= 27)
		{
			aircraftNumber = buffer[26];
		}
		float spdbrk = DataManager::GetDefaultValue();
		if (size >= 31)
		{
			spdbrk = *((float*)(buffer + 27));
		}


		if (!DataManager::IsDefault(pitch))
		{
			DataManager::Set(DREF_YokePitch, pitch, aircraftNumber);
		}
		if (!DataManager::IsDefault(roll))
		{
			DataManager::Set(DREF_YokeRoll, roll, aircraftNumber);
		}
		if (!DataManager::IsDefault(yaw))
		{
			DataManager::Set(DREF_YokeHeading, yaw, aircraftNumber);
		}
		if (!DataManager::IsDefault(throttle))
		{

			float throttleArray[8];
			for (int i = 0; i < 8; ++i)
			{
				throttleArray[i] = throttle;
			}
			DataManager::Set(DREF_ThrottleSet, throttleArray, 8, aircraftNumber);
			DataManager::Set(DREF_ThrottleActual, throttleArray, 8, aircraftNumber);
			if (aircraftNumber == 0)
			{
				DataManager::Set("sim/flightmodel/engine/ENGN_thro_override", throttleArray, 1);
			}
		}
		if (gear != -1)
		{
			DataManager::SetGear(gear, false, aircraftNumber);
		}
		if (!DataManager::IsDefault(flaps))
		{
			DataManager::Set(DREF_FlapSetting, flaps, aircraftNumber);
		}
		if (!DataManager::IsDefault(spdbrk))
		{
			DataManager::Set(DREF_SpeedBrakeSet, spdbrk, aircraftNumber);
		}
	}

	void MessageHandlers::HandleData(const Message& msg)
	{
		// Parse data
		const unsigned char* buffer = msg.GetBuffer();
		std::size_t size = msg.GetSize();
		std::size_t numCols = (size - 5) / 36;
		if (numCols > 0)
		{
			Log::FormatLine(LOG_TRACE, "DATA", "Message Received (Conn %i)", connection.id);
		}
		else
		{
			Log::FormatLine(LOG_WARN, "DATA", "WARNING: Empty data packet received (Conn %i)", connection.id);
			return;
		}

		if (numCols > 134) // Error. Will overflow values
		{
			Log::FormatLine(LOG_ERROR, "DATA", "ERROR: numCols to large.");
			return;
		}
		float values[134][9];
		for (int i = 0; i < numCols; ++i)
		{
			// 5 byte header + (9 * 4 = 36) bytes per row
			values[i][0] = buffer[5 + 36 * i];
			memcpy(values[i] + 1, buffer + 9 + 36 * i, 9 * sizeof(float));
		}

		// Update log

		float savedAlpha = DataManager::GetDefaultValue();
		float savedHPath = DataManager::GetDefaultValue();
		for (int i = 0; i < numCols; ++i)
		{
			unsigned char dataRef = (unsigned char)values[i][0];
			if (dataRef >= 134)
			{
				Log::FormatLine(LOG_ERROR, "DATA", "ERROR: DataRef # must be between 0 - 134 (Received: %hi)", (int)dataRef);
				continue;
			}

			switch (dataRef)
			{
			// TODO(jason-watkins): This currently overwrites the velocity several times. Should look into making this case smarter somehow.
			case 3: // Velocity
			{
				float theta = DataManager::GetFloat(DREF_Pitch);
				float alpha = DataManager::IsDefault(savedAlpha) ? savedAlpha : DataManager::GetFloat(DREF_AngleOfAttack);
				float hpath = DataManager::IsDefault(savedHPath) ? savedHPath : DataManager::GetFloat(DREF_HPath);
				if (alpha != alpha || hpath != hpath)
				{
					Log::WriteLine(LOG_ERROR, "DATA", "ERROR: Value must be a number (NaN received)");
					break;
				}
				const float deg2rad = 0.0174532925F;
				int ind[3] = { 1, 3, 4 };
				for (int j = 0; j < 3; ++j)
				{
					float v = values[i][ind[j]];
					if (!DataManager::IsDefault(v))
					{
						DataManager::Set(DREF_LocalVX, v*cos((theta - alpha)*deg2rad)*sin(hpath*deg2rad));
						DataManager::Set(DREF_LocalVY, v*sin((theta - alpha)*deg2rad));
						DataManager::Set(DREF_LocalVZ, -v*cos((theta - alpha)*deg2rad)*cos(hpath*deg2rad));
					}
				}
				break;
			}
			case 17: // Orientation
			{
				float orientation[3];
				orientation[0] = values[i][1];
				orientation[1] = values[i][2];
				orientation[2] = values[i][3];
				DataManager::SetOrientation(orientation);
				break;
			}
			case 18: // Alpha, hpath etc.
			{
				if (values[i][1] != values[i][1] || values[i][3] != values[i][3])
				{
					Log::WriteLine(LOG_ERROR, "DATA", "ERROR: Value must be a number (NaN received)");
					break;
				}
				if (!DataManager::IsDefault(values[i][1]))
				{
					savedAlpha = values[i][1];
				}
				if (DataManager::IsDefault(values[i][3]))
				{
					savedHPath = values[i][3];
				}
				break;
			}
			case 20: // Position
			{
				// TODO: loss of precision here
				double pos[3];
				pos[0] = (double)values[i][2];
				pos[1] = (double)values[i][3];
				pos[2] = (double)values[i][4];
				DataManager::SetPosition(pos);
				break;
			}
			case 25: // Throttle
			{
				if (values[i][1] != values[i][1])
				{
					Log::WriteLine(LOG_ERROR, "DATA", "ERROR: Value must be a number (NaN received)");
					break;
				}
				float throttle[8];
				for (int j = 0; j < 8; ++j)
				{
					throttle[j] = values[i][1];
				}
				DataManager::Set(DREF_ThrottleSet, throttle, 8);
				break;
			}
			default: // Non-Special dataRefs
			{
				float line[8];
				memcpy(line, values[i] + 1, 8 * sizeof(float));
				for (int j = 0; j < 8; ++j)
				{
					Log::FormatLine(LOG_ERROR, "DATA", "Setting Dataref %i.%i to %f", dataRef, j, line[j]);
					// TODO(jason-watkins): Why is this a special case?
					if (dataRef == 14 && j == 0)
					{
						DataManager::SetGear(line[0], true);
						continue;
					}

					DREF dref = XPData[dataRef][j];
					if (dref == DREF_None)
					{
						// TODO(jason): Send single line instead!
						HandleXPlaneData(msg);
					}
					else
					{
						DataManager::Set(dref, line, 8);
					}
				}
			}
			}
		}
	}

	void MessageHandlers::HandleDref(const Message& msg)
	{
		Log::FormatLine(LOG_TRACE, "DREF", "Request to set DREF value received (Conn %i)", connection.id);
		const unsigned char* buffer = msg.GetBuffer();
		std::size_t size = msg.GetSize();
		std::size_t pos = 5;
		while (pos < size)
		{
			unsigned char len = buffer[pos++];
			if (pos + len > size)
			{
				break;
			}
			std::string dref = std::string((char*)buffer + pos, len);
			pos += len;

			unsigned char valueCount = buffer[pos++];
			if (pos + 4 * valueCount > size)
			{
				break;
			}
			float* values = (float*)(buffer + pos);
			pos += 4 * valueCount;

			DataManager::Set(dref, values, valueCount);
			Log::FormatLine(LOG_DEBUG, "DREF", "Set %d values for %s", valueCount, dref.c_str());
		}
		if (pos != size)
		{
			Log::WriteLine(LOG_ERROR, "DREF", "ERROR: Command did not terminate at the expected position.");
		}
	}

	void MessageHandlers::HandleGetC(const Message& msg)
	{
		const unsigned char* buffer = msg.GetBuffer();
		std::size_t size = msg.GetSize();
		if (size != 6)
		{
			Log::FormatLine(LOG_ERROR, "GCTL", "Unexpected message length: %u", size);
			return;
		}
		unsigned char aircraft = buffer[5];
		// TODO(jason-watkins): Get proper printf specifier for unsigned char
		Log::FormatLine(LOG_TRACE, "GCTL", "Getting control information for aircraft %u", aircraft);

		float throttle[8];
		unsigned char response[31] = "CTRL";
		*((float*)(response + 5)) = DataManager::GetFloat(DREF_Elevator, aircraft);
		*((float*)(response + 9)) = DataManager::GetFloat(DREF_Aileron, aircraft);
		*((float*)(response + 13)) = DataManager::GetFloat(DREF_Rudder, aircraft);
		DataManager::GetFloatArray(DREF_ThrottleSet, throttle, 8, aircraft);
		*((float*)(response + 17)) = throttle[0];
		if (aircraft == 0)
		{
			response[21] = (char)DataManager::GetInt(DREF_GearHandle, aircraft);
		}
		else
		{
			float mpGear[10];
			DataManager::GetFloatArray(DREF_GearDeploy, mpGear, 10, aircraft);
			response[21] = mpGear[0] > 0.5 ? 1 : 0;
		}
		*((float*)(response + 22)) = DataManager::GetFloat(DREF_FlapSetting, aircraft);
		response[26] = aircraft;
		*((float*)(response + 27)) = DataManager::GetFloat(DREF_SpeedBrakeSet, aircraft);

		sock->SendTo(response, 31, &connection.addr);
	}

	void MessageHandlers::HandleGetD(const Message& msg)
	{
		const unsigned char* buffer = msg.GetBuffer();
		unsigned char drefCount = buffer[5];
		if (drefCount == 0) // Use last request
		{
			Log::FormatLine(LOG_TRACE, "GETD",
				"DATA Requested: Repeat last request from connection %i (%i data refs)",
				connection.id, connection.getdCount);
			if (connection.getdCount == 0) // No previous request to use
			{
				Log::FormatLine(LOG_ERROR, "GETD", "ERROR: No previous requests from connection %i.",
					connection.id);
				return;
			}
		}
		else // New request
		{
			Log::FormatLine(LOG_TRACE, "GETD", "DATA Requested: New Request for connection %i (%i data refs)",
				connection.id, drefCount);
			std::size_t ptr = 6;
			for (int i = 0; i < drefCount; ++i)
			{
				unsigned char len = buffer[ptr];
				connection.getdRequest[i] = std::string((char*)buffer + 1 + ptr, len);
				ptr += 1 + len;
			}
			connection.getdCount = drefCount;
			connections[connectionKey] = connection;
		}

		unsigned char response[4096] = "RESP";
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

		sock->SendTo(response, cur, &connection.addr);
	}

	void MessageHandlers::HandleGetP(const Message& msg)
	{
		const unsigned char* buffer = msg.GetBuffer();
		std::size_t size = msg.GetSize();
		if (size != 6)
		{
			Log::FormatLine(LOG_ERROR, "GPOS", "Unexpected message length: %u", size);
			return;
		}
		unsigned char aircraft = buffer[5];
		Log::FormatLine(LOG_TRACE, "GPOS", "Getting position information for aircraft %u", aircraft);

		unsigned char response[46] = "POSI";
		response[5] = (char)DataManager::GetInt(DREF_GearHandle, aircraft);
		*((double*)(response + 6)) = DataManager::GetDouble(DREF_Latitude, aircraft);
		*((double*)(response + 14)) = DataManager::GetDouble(DREF_Longitude, aircraft);
		*((double*)(response + 22)) = DataManager::GetDouble(DREF_Elevation, aircraft);
		*((float*)(response + 30)) = DataManager::GetFloat(DREF_Pitch, aircraft);
		*((float*)(response + 34)) = DataManager::GetFloat(DREF_Roll, aircraft);
		*((float*)(response + 38)) = DataManager::GetFloat(DREF_HeadingTrue, aircraft);

		float gear[10];
		DataManager::GetFloatArray(DREF_GearDeploy, gear, 10, aircraft);
		*((float*)(response + 42)) = gear[0];

		sock->SendTo(response, 46, &connection.addr);
	}

	void MessageHandlers::HandlePosi(const Message& msg)
	{
		// Update log
		Log::FormatLine(LOG_TRACE, "POSI", "Message Received (Conn %i)", connection.id);

		const unsigned char* buffer = msg.GetBuffer();
		const std::size_t size = msg.GetSize();

		char aircraftNumber = buffer[5];
		float gear;
		double posd[3];
		float orient[3];

		if (size == 34) /* lat/lon/h as 32-bit float */
		{
			float posd_32[3];
			memcpy(posd_32, buffer + 6, 12);
		/* convert float to double */
			posd[0] = posd_32[0];
			posd[1] = posd_32[1];
			posd[2] = posd_32[2];
			memcpy(orient, buffer + 18, 12);
			memcpy(&gear, buffer + 30, 4);
		}
		else if (size == 46) /* lat/lon/h as 64-bit double */
		{
			memcpy(posd, buffer + 6, 24);
			memcpy(orient, buffer + 30, 12);
			memcpy(&gear, buffer + 42, 4);
		}
		else
		{
			Log::FormatLine(LOG_ERROR, "POSI", "ERROR: Unexpected size: %i (Expected 34 or 46)", size);
			return;
		}

		DataManager::SetPosition(posd, aircraftNumber);
		DataManager::SetOrientation(orient, aircraftNumber);
		if (gear >= 0)
		{
			DataManager::SetGear(gear, true, aircraftNumber);
		}

		if (aircraftNumber > 0)
		{
			// Enable AI for the aircraftNumber we are setting
			float ai[20];
			std::size_t result = DataManager::GetFloatArray(DREF_PauseAI, ai, 20);
			if (result == 20) // Only set values if they were retrieved successfully.
			{
				ai[aircraftNumber] = 1;
				DataManager::Set(DREF_PauseAI, ai, 0, 20);
			}
		}
	}

	void MessageHandlers::HandlePosT(const Message& msg)
	{
		MessageHandlers::HandlePosi(msg);
		
		const unsigned char* buffer = msg.GetBuffer();
		char aircraftNumber = buffer[5];
		Log::FormatLine(LOG_TRACE, "POST", "Getting terrain information for aircraft %u", aircraftNumber);
		
		double pos[3];
		pos[0] = DataManager::GetDouble(DREF_Latitude, aircraftNumber);
		pos[1] = DataManager::GetDouble(DREF_Longitude, aircraftNumber);
		pos[2] = 0.0;
		MessageHandlers::SendTerr(pos, aircraftNumber);
	}

	void MessageHandlers::HandleGetT(const Message& msg)
	{
		const unsigned char* buffer = msg.GetBuffer();
		std::size_t size = msg.GetSize();
		if (size != 30)
		{
			Log::FormatLine(LOG_ERROR, "GETT", "Unexpected message length: %u", size);
			return;
		}
		unsigned char aircraft = buffer[5];
		Log::FormatLine(LOG_TRACE, "GETT", "Getting terrain information for aircraft %u", aircraft);
		
		double pos[3];
		memcpy(pos, buffer + 6, 24);
		
		if(pos[0] == -998 || pos[1] == -998 || pos[2] == -998)
		{
			// get terrain properties at aircraft location
			pos[0] = DataManager::GetDouble(DREF_Latitude, aircraft);
			pos[1] = DataManager::GetDouble(DREF_Longitude, aircraft);
			pos[2] = 0.0;
		}

		MessageHandlers::SendTerr(pos, aircraft);
	}

	void MessageHandlers::SendTerr(double pos[3], char aircraft)
	{
		double lat, lon, alt, X, Y, Z;
		
		// Init terrain probe (if required) and probe data struct
		static XPLMProbeInfo_t probe_data;
		probe_data.structSize = sizeof(XPLMProbeInfo_t);
		
		if(Terrain_probe == nullptr)
		{
			Log::FormatLine(LOG_TRACE, "TERR", "Create terrain probe for aircraft %u", aircraft);
			Terrain_probe = XPLMCreateProbe(0);
		}
		
		// terrain probe at specified location
		// Follow the process in the following post to get accurate results
		// https://forums.x-plane.org/index.php?/forums/topic/38688-how-do-i-use-xplmprobeterrainxyz/&page=2
		
		// transform probe location to local coordinates
		// Step 1. Convert lat/lon/0 to XYZ
		XPLMWorldToLocal(pos[0], pos[1], pos[2], &X, &Y, &Z);
		
		// query probe
		// Step 2. Probe XYZ to get a new Y
		int rc = XPLMProbeTerrainXYZ(Terrain_probe, X, Y, Z, &probe_data);
		if(rc > 0)
		{
			Log::FormatLine(LOG_ERROR, "TERR", "Probe failed. Return Value %u", rc);
			XPLMDestroyProbe(Terrain_probe);
			Terrain_probe = nullptr;
			return;
		}
		
		// transform probe location to world coordinates
		// Step 3. Convert that new XYZ back to LLE
		XPLMLocalToWorld(probe_data.locationX, probe_data.locationY, probe_data.locationZ, &lat, &lon, &alt);
		Log::FormatLine(LOG_TRACE, "TERR", "Conv LLA=%f, %f, %f", lat, lon, alt);

		// transform probe location to local coordinates
		// Step 4. NOW convert your original lat/lon with the elevation from step 3 to XYZ
		XPLMWorldToLocal(pos[0], pos[1], alt, &X, &Y, &Z);

		// query probe
		// Step 5. Re-probe with the NEW XYZ
		rc = XPLMProbeTerrainXYZ(Terrain_probe, X, Y, Z, &probe_data);
		if(rc == 0)
		{
		// transform probe location to world coordinates
		// Step 6. You now have a new Y, and your XYZ will be closer to correct for high elevations far from the origin.
			XPLMLocalToWorld(probe_data.locationX, probe_data.locationY, probe_data.locationZ, &lat, &lon, &alt);
			
			Log::FormatLine(LOG_TRACE, "TERR", "Probe LLA %lf %lf %lf", lat, lon, alt);
		}
		else
		{
			lat = -998;
			lon = -998;
			alt = -998;
			
			Log::FormatLine(LOG_TRACE, "TERR", "Probe failed. Return Value %u", rc);
		}

		// keep probe for next query
		// XPLMDestroyProbe(Terrain_probe);
		
		// Assemble response message
		unsigned char response[62] = "TERR";
		response[5] = aircraft;
		// terrain height over msl at lat/lon point
		memcpy(response + 6,  &lat, 8);
		memcpy(response + 14, &lon, 8);
		memcpy(response + 22, &alt, 8);
		// terrain normal vector
		memcpy(response + 30, &probe_data.normalX, 4);
		memcpy(response + 34, &probe_data.normalY, 4);
		memcpy(response + 38, &probe_data.normalZ, 4);
		// terrain velocity
		memcpy(response + 42, &probe_data.velocityX, 4);
		memcpy(response + 46, &probe_data.velocityY, 4);
		memcpy(response + 50, &probe_data.velocityZ, 4);
		// terrain type
		memcpy(response + 54, &probe_data.is_wet, 4);
		// probe status
		memcpy(response + 58, &rc, 4);

		sock->SendTo(response, 62, &connection.addr);
	}

	void MessageHandlers::HandleSimu(const Message& msg)
	{
		// Update log
		Log::FormatLine(LOG_TRACE, "SIMU", "Message Received (Conn %i)", connection.id);

		unsigned char v = msg.GetBuffer()[5];
		if (v < 0 || (v > 2 && v < 100) || (v > 119 && v < 200) || v > 219)
		{
			Log::FormatLine(LOG_ERROR, "SIMU", "ERROR: Invalid argument: %i", v);
			return;
		}

		int value[20];
		if (v == 2)
		{
			DataManager::GetIntArray(DREF_Pause, value, 20);
			for (int i = 0; i < 20; ++i)
			{
				value[i] = value[i] ? 0 : 1;
			}
		}
		else if ((v >= 100) && (v < 120))
		{
			DataManager::GetIntArray(DREF_Pause, value, 20);
			value[v - 100] = 1;
		}
		else if ((v >= 200) && (v < 220))
		{
			DataManager::GetIntArray(DREF_Pause, value, 20);
			value[v - 200] = 0;
		}
		else
		{
			for (int i = 0; i < 20; ++i)
			{
				value[i] = v;
			}
		}

		// Set DREF
		DataManager::Set(DREF_Pause, value, 20);

		if (v == 0)
		{
			Log::WriteLine(LOG_INFO, "SIMU", "Simulation resumed for all a/c");
		}
		else if (v == 1)
		{
			Log::WriteLine(LOG_INFO, "SIMU", "Simulation paused for all a/c");
		}
		else if (v == 2)
		{
			Log::FormatLine(LOG_INFO, "SIMU", "Simulation switched to %i for all a/c", value[0]);
		}
		else if ((v >= 100) && (v < 120))
		{
			Log::FormatLine(LOG_INFO, "SIMU", "Simulation paused for a/c %i", (v-100));
		}
		else if ((v >= 200) && (v < 220))
		{
			Log::FormatLine(LOG_INFO, "SIMU", "Simulation resumed for a/c %i", (v-100));
		}

	}

	void MessageHandlers::HandleText(const Message& msg)
	{
		// Update Log
		Log::FormatLine(LOG_TRACE, "TEXT", "Message Received (Conn %i)", connection.id);

		std::size_t len = msg.GetSize();
		const unsigned char*  buffer = msg.GetBuffer();

		char text[256] = { 0 };
		if (len < 14)
		{
			Log::WriteLine(LOG_ERROR, "TEXT", "ERROR: Length less than 14 bytes");
			return;
		}
		size_t msgLen = (unsigned char)buffer[13];
		if (msgLen == 0)
		{
			Drawing::ClearMessage();
			Log::WriteLine(LOG_INFO, "TEXT", "[TEXT] Text cleared");
		}
		else
		{
			int x = *((int*)(buffer + 5));
			int y = *((int*)(buffer + 9));
			strncpy(text, (char*)buffer + 14, msgLen);
			Drawing::SetMessage(x, y, text);
			Log::WriteLine(LOG_INFO, "TEXT", "[TEXT] Text set");
		}
	}

	void MessageHandlers::HandleView(const Message& msg)
	{
		// Update Log
		Log::FormatLine(LOG_TRACE, "VIEW", "Message Received(Conn %i)", connection.id);
		
		bool enable_advanced_camera = false;
		
		const std::size_t size = msg.GetSize();
		if (size == 9)
		{
			// default view switcher as before
		}
		else if (size == 49)
		{
			// Allow camera location control
			enable_advanced_camera = true;
		}
		else
		{
			Log::FormatLine(LOG_ERROR, "VIEW", "Error: Unexpected length. Message was %d bytes, expected 9 or 49.", size);
			return;
		}
		
		// get msg data
		const unsigned char* buffer = msg.GetBuffer();
		
		// get view type
		int view_type;
		memcpy(&view_type, buffer + 5, 4);
		
		// set view by calling the corresponding key stroke
		XPLMCommandKeyStroke(view_type);
		
		
		VIEW_TYPE viewRunway = VIEW_TYPE::XPC_VIEW_RUNWAY;
		VIEW_TYPE viewChase	 = VIEW_TYPE::XPC_VIEW_CHASE;
		
		// advanced runway camera view
		if(view_type == static_cast<int>(viewRunway) && enable_advanced_camera == true)
		{
			static struct CameraProperties campos; // static variable for continuous callback access
			
			memcpy(&campos, buffer+9 , sizeof(struct CameraProperties));
			
			Log::FormatLine(LOG_TRACE, "VIEW", "Cam pos %f %f %f zoom %f", campos.loc[0], campos.loc[1], campos.loc[2], campos.zoom);
			
			XPLMControlCamera(xplm_ControlCameraUntilViewChanges, CamCallback_RunwayCam, &campos);
		}
		// advanced chase camera view
		else if(view_type == static_cast<int>(viewChase) && enable_advanced_camera == true)
		{
			static struct CameraProperties campos;	// static variable for continuous callback access
			
			memcpy(&campos, buffer+9 , sizeof(struct CameraProperties));
			
			Log::FormatLine(LOG_TRACE, "VIEW", "Cam pos %f %f %f zoom %f", campos.loc[0], campos.loc[1], campos.loc[2], campos.zoom);
			
			XPLMControlCamera(xplm_ControlCameraUntilViewChanges, CamCallback_ChaseCam, &campos);
		}
	}

	void MessageHandlers::HandleComm(const Message& msg)
 	{
 		Log::FormatLine(LOG_TRACE, "COMM", "Request to execute COMM command received (Conn %i)", connection.id);
 		const unsigned char* buffer = msg.GetBuffer();
 		std::size_t size = msg.GetSize();
 		std::size_t pos = 5;
 		while (pos < size)
 		{
 			unsigned char len = buffer[pos++];
 			if (pos + len > size)
 			{
 				break;
 			}
 			std::string comm = std::string((char*)buffer + pos, len);
 			pos += len;

  			DataManager::Execute(comm);
 			Log::FormatLine(LOG_DEBUG, "COMM", "Execute command %s", comm.c_str());
 		}
 		if (pos != size)
 		{
 			Log::WriteLine(LOG_ERROR, "COMM", "ERROR: Command did not terminate at the expected position.");
 		}
 	}

	void MessageHandlers::HandleWypt(const Message& msg)
	{
		// Update Log
		Log::FormatLine(LOG_TRACE, "WYPT", "Message Received (Conn %i)", connection.id);

		// Parse data
		const unsigned char* buffer = msg.GetBuffer();
		unsigned char op = buffer[5];
		unsigned char count = buffer[6];
		Waypoint points[255];
		const unsigned char* ptr = buffer + 7;
		for (size_t i = 0; i < count; ++i)
		{
			points[i].latitude = *((float*)ptr);
			points[i].longitude = *((float*)(ptr + 4));
			points[i].altitude = *((float*)(ptr + 8));
			ptr += 12;
		}

		// Perform operation
		Log::FormatLine(LOG_INFO, "WYPT", "Performing operation %i", op);
		switch (op)
		{
		case 1:
			Drawing::AddWaypoints(points, count);
			break;
		case 2:
			Drawing::RemoveWaypoints(points, count);
			break;
		case 3:
			Drawing::ClearWaypoints();
			break;
		default:
			Log::FormatLine(LOG_ERROR, "WYPT", "ERROR: %i is not a valid operation.", op);
			break;
		}
	}

	void MessageHandlers::HandleXPlaneData(const Message& msg)
	{
		Log::WriteLine(LOG_TRACE, "MSGH", "Sending raw data to X - Plane");
		sockaddr_in loopback;
		loopback.sin_family = AF_INET;
		loopback.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		loopback.sin_port = htons(49000);
		sock->SendTo(msg.GetBuffer(), msg.GetSize(), (sockaddr*)&loopback);
	}

	void MessageHandlers::HandleUnknown(const Message& msg)
	{
		Log::FormatLine(LOG_ERROR, "MSGH", "ERROR: Unknown packet type %s", msg.GetHead().c_str());
	}
}
