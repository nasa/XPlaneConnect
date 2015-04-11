//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#include "Message.h"
#include "Log.h"
#include "xplaneConnect.h"

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
	
	std::uint32_t Message::GetMagicNumber()
	{
		if (size < 4)
		{
			return 0;
		}
		return *((std::uint32_t*)buffer);
	}

	std::string Message::GetHead()
	{
		if (size < 4)
		{
			return "";
		}
		return std::string((char*)buffer, 4);
	}

	const std::uint8_t* Message::GetBuffer()
	{
		if (size == 0)
		{
			return nullptr;
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
#if LOG_VERBOSITY > 2
		std::stringstream ss;
		ss << "[DEBUG]";

		// Dump raw bytes to string
		ss << std::hex << std::setfill('0');
		for (int i = 0; i < size; ++i)
		{
			ss << ' ' << std::setw(2) << static_cast<unsigned>(buffer[i]);
		}
		Log::WriteLine(ss.str());

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
            xpcCtrl ctrl = parseCTRL((char*)buffer);
            ss << " (" << ctrl.pitch << " " << ctrl.roll << " " << ctrl.yaw << ") ";
            ss << ctrl.throttle << " " << ctrl.gear << " " << ctrl.flaps;
            Log::WriteLine(ss.str());
            break;
        }
        case 0x41544144: // DATA
        {
            float dataRef[30][9];
            short numCols = parseDATA((char*)buffer, size, dataRef);
            ss << " (" << numCols << " lines)";
            Log::WriteLine(ss.str());
            for (int i = 0; i < numCols; ++i)
            {
                ss.str("");
                ss << "\t#" << dataRef[i][0];
                for (int j = 1; j < 9; ++j)
                {
                    ss << " " << dataRef[i][j];
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
            float position[6] = { 0.0 };
            short aircraft = 0;
            float gear = -1.0;
            aircraft = parsePOSI((char*)buffer, position, 6, &gear);
            ss << ' ' << aircraft;
            ss << " (" << position[0] << ' ' << position[1] << ' ' << position[2] << ") (";
            ss << position[3] << ' ' << position[4] << ' ' << position[5] << ") ";
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