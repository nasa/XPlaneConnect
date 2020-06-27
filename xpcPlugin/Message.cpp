// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
#include "Message.h"
#include "Log.h"

#include <cstring>

#include <iomanip>
#include <sstream>
#include <string>

namespace XPC
{
	Message::Message() {}

	Message Message::ReadFrom(const UDPSocket& sock)
	{
		Message m;
		int len = sock.Read(m.buffer, bufferSize, &m.source);
		m.size = len < 0 ? 0 : len;
		if (len > 0)
		{
			Log::FormatLine(LOG_TRACE, "MESG", "Read message with length %i", len);
		}
		return m;
	}

	std::string Message::GetHead() const
	{
		std::string val = size < 4 ? "" : std::string((char*)buffer, 4);
		return val;
	}

	const unsigned char* Message::GetBuffer() const
	{
		const unsigned char* val = size == 0 ? NULL : buffer;
		return val;
	}

	std::size_t Message::GetSize() const
	{
		return size;
	}

	struct sockaddr Message::GetSource() const
	{
		return source;
	}

	void Message::PrintToLog() const
	{
		using namespace std;
		stringstream ss;

		// Dump raw bytes to string
		ss << std::hex << setfill('0');
		for (int i = 0; i < size; ++i)
		{
			ss << ' ' << setw(2) << static_cast<unsigned>(buffer[i]);
		}
		Log::WriteLine(LOG_TRACE, "DBUG", ss.str());

		std::string head = GetHead();
		ss.str("");
		ss << "Head: " << head << std::dec << " Size: " << GetSize();
		if (head == "CONN" || head == "WYPT" || head == "TEXT")
		{
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
		}
		else if (head == "CTRL")
		{
			// Parse message data
			float pitch = *((float*)(buffer + 5));
			float roll = *((float*)(buffer + 9));
			float yaw = *((float*)(buffer + 13));
			float thr = *((float*)(buffer + 17));
			char gear = buffer[21];
			float flaps = *((float*)(buffer + 22));
			unsigned char aircraft = 0;
			if (size == 27)
			{
				aircraft = buffer[26];
			}
			ss << " Attitude:(" << pitch << " " << roll << " " << yaw << ")";
			ss << " Thr:" << thr << " Gear:" << (int)gear << " Flaps:" << flaps;
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
		}
		else if (head == "DATA")
		{
			size_t numCols = (size - 5) / 36;
			float values[32][9];
			for (int i = 0; i < numCols; ++i)
			{
				values[i][0] = buffer[5 + 36 * i];
				memcpy(values[i] + 1, buffer + 9 + 36 * i, 9 * sizeof(float));
			}
			ss << " (" << numCols << " lines)";
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
			for (int i = 0; i < numCols; ++i)
			{
				ss.str("");
				ss << "    #" << values[i][0];
				for (int j = 1; j < 9; ++j)
				{
					ss << " " << values[i][j];
				}
				Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
			}
		}
		else if (head == "DREF")
		{
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
			string dref((char*)buffer + 6, buffer[5]);
			Log::FormatLine(LOG_DEBUG, "DBUG", "    DREF (size %i) = %s", dref.length(), dref.c_str());
			ss.str("");
			int values = buffer[6 + buffer[5]];
			ss << "    Values(size " << values << ") =";
			for (int i = 0; i < values; ++i)
			{
				ss << " " << *((float*)(buffer + values + 1 + sizeof(float) * i));
			}
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
		}
		else if (head == "GETC" || head == "GETP" || head == "GETT")
		{
			ss << " Aircraft:" << (int)buffer[5];
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
		}
		else if (head == "GETD")
		{
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
			int cur = 6;
			for (int i = 0; i < buffer[5]; ++i)
			{
				string dref((char*)buffer + cur + 1, buffer[cur]);
				Log::FormatLine(LOG_DEBUG, "DBUG", "    #%i/%i (size:%i) %s",
					i + 1, buffer[5], dref.length(), dref.c_str());
				cur += 1 + buffer[cur];
			}
		}
		else if (head == "POSI" || head == "POST")
		{
			char aircraft = buffer[5];
			float gear;
			double pos[3];
			float orient[3];
			if (size == 34) /* lat/lon/h as 32-bit float */
			{
				float posd_32[3];
				memcpy(posd_32, buffer + 6, 12);
				pos[0] = posd_32[0];
				pos[1] = posd_32[1];
				pos[2] = posd_32[2];
				memcpy(orient, buffer + 18, 12);
				memcpy(&gear, buffer + 30, 4);
			}
			else if (size == 46) /* lat/lon/h as 64-bit double */
			{
				memcpy(pos, buffer + 6, 24);
				memcpy(orient, buffer + 30, 12);
				memcpy(&gear, buffer + 42, 4);
			}
			ss << " AC:" << (int)aircraft;
			ss << " Pos:(" << pos[0] << ' ' << pos[1] << ' ' << pos[2] << ") Orient:(";
			ss << orient[0] << ' ' << orient[1] << ' ' << orient[2] << ") Gear:";
			ss << gear;
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
		}
		else if (head == "SIMU")
		{
			ss << ' ' << (int)buffer[5];
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
		}
		else if (head == "VIEW")
		{
			ss << "Type:" << *((unsigned long*)(buffer + 5));
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
		}
		else if (head == "COMM")
 		{
 			ss << "Type:" << *((unsigned long*)(buffer + 5));
 			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
 		}
		else
		{
			ss << " UNKNOWN HEADER ";
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
		}
	}
}
