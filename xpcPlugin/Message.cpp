//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#include "Message.h"
#include "Log.h"

#include <iomanip>
#include <sstream>
#include <string>

namespace XPC
{
    Message::Message() {}

	Message Message::ReadFrom(UDPSocket& sock)
	{
		Message m;
		int len = sock.Read(m.buffer, bufferSize, &m.source);
		m.size = len < 0 ? 0 : len;
		return m;
	}
	
	unsigned long Message::GetMagicNumber()
	{
		if (size < 4)
		{
			return 0;
		}
		return *((unsigned long*)buffer);
	}

	std::string Message::GetHead()
	{
		if (size < 4)
		{
			return "";
		}
		return std::string((char*)buffer, 4);
	}

	const unsigned char* Message::GetBuffer()
	{
		if (size == 0)
		{
			return NULL;
		}
		return buffer;
	}

	std::size_t Message::GetSize()
	{
		return size;
	}

	struct sockaddr Message::GetSource()
	{
		return source;
	}

	void Message::PrintToLog()
	{
#if LOG_VERBOSITY > 4
		std::stringstream ss;
		ss << "[DEBUG]";

		// Dump raw bytes to string
		ss << std::hex << std::setfill('0');
		for (int i = 0; i < size; ++i)
		{
			ss << ' ' << std::setw(2) << static_cast<unsigned>(buffer[i]);
		}
		Log::WriteLine(ss.str());

		ss << std::dec;
		ss.str("");
		ss << "[" << GetHead() << "-DEBUG] (" << GetSize() << ")";
		switch (GetMagicNumber()) // Binary version of head
		{
		case 0x4E4EF443: // CONN
        case 0x54505957: // WYPT
        case 0x54584554: // TEXT
        {
            Log::WriteLine(ss.str());
            break;
        }
        case 0x4C525443: // CTRL
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
            Log::WriteLine(ss.str());
            break;
        }
        case 0x41544144: // DATA
		{
			std::size_t numCols = (size - 5) / 36;
			float values[32][9];
			for (int i = 0; i < numCols; ++i)
			{
				values[i][0] = buffer[5 + 36 * i];
				memcpy(values[i] + 1, buffer + 9 + 36 * i, 9 * sizeof(float));
			}
            ss << " (" << numCols << " lines)";
            Log::WriteLine(ss.str());
            for (int i = 0; i < numCols; ++i)
            {
                ss.str("");
				ss << "\t#" << values[i][0];
                for (int j = 1; j < 9; ++j)
                {
					ss << " " << values[i][j];
                }
                Log::WriteLine(ss.str());
            }
            break;
        }
        case 0x46455244: // DREF
        {
            Log::WriteLine(ss.str());
            std::string dref((char*)buffer + 6, buffer[5]);
            Log::FormatLine("-\tDREF  (size %i) = %s", dref.length(), dref.c_str());
            ss.str("");
            int values = buffer[6 + buffer[5]];
            ss << "\tValues(size " << values << ") =";
            for (int i = 0; i < values; ++i)
            {
                ss << " " << *((float*)(buffer + values + 1 + sizeof(float) * i));
            }
            Log::WriteLine(ss.str());
            break;
        }
        case 0x44544547: // GETD
        {
            Log::WriteLine(ss.str());
            int cur = 6;
            for (int i = 0; i < buffer[5]; ++i)
            {
                std::string dref((char*)buffer + cur + 1, buffer[cur]);
                Log::FormatLine("\t#%i/%i (size:%i) %s", i + 1, buffer[5], dref.length(), dref.c_str());
                cur += 1 + buffer[cur];
            }
            break;
        }
        case 0x49534F50: // POSI
		{
			char aircraft = buffer[5];
			float gear = *((float*)(buffer + 30));
			float pos[3];
			float orient[3];
			memcpy(pos, buffer + 6, 12);
			memcpy(orient, buffer + 18, 12);
            ss << " AC:" << (int)aircraft;
			ss << " Pos:(" << pos[0] << ' ' << pos[1] << ' ' << pos[2] << ") Orient:(";
			ss << orient[3] << ' ' << orient[4] << ' ' << orient[5] << ") Gear:";
            ss << gear;
            Log::WriteLine(ss.str());
            break;
        }
        case 0x554D4953: // SIMU
        {
            ss << ' ' << (int)buffer[5];
            Log::WriteLine(ss.str());
            break;
        }
        default:
        {
            ss << " UNKNOWN HEADER ";
            Log::WriteLine(ss.str());
            break;
        }
		}
#endif
	}
}