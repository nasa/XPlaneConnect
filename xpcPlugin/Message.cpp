// Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
#include "Message.h"
#include "Log.h"

#include <iomanip>
#include <sstream>
#include <string>

namespace XPC
{
    Message::Message() {}

	Message Message::ReadFrom(const UDPSocket& sock)
	{
		Log::WriteLine(LOG_TRACE, "MESG", "Reading Message");
		Message m;
		int len = sock.Read(m.buffer, bufferSize, &m.source);
		m.size = len < 0 ? 0 : len;
		Log::FormatLine(LOG_TRACE, "MESG", "Read result: %i", len);
		return m;
	}

	unsigned long Message::GetMagicNumber() const
	{
		unsigned long val = size < 4 ? 0 : *((unsigned long*)buffer);
		Log::FormatLine(LOG_TRACE, "MESG", "Gettting magic number 0x%lX for message 0x%zX", val, this);
		return val;
	}

	std::string Message::GetHead() const
	{
		std::string val = size < 4 ? "" : std::string((char*)buffer, 4);
		Log::FormatLine(LOG_TRACE, "MESG", "Getting head %s for message 0x%zX", val, this);
		return val;
	}

	const unsigned char* Message::GetBuffer() const
	{
		const unsigned char* val = size == 0 ? NULL : buffer;
		Log::FormatLine(LOG_TRACE, "MESG", "Returning buffer 0x%lX for message 0x%zX", val, this);
		return val;
	}

	std::size_t Message::GetSize() const
	{
		Log::FormatLine(LOG_TRACE, "MESG", "Returning size %zu for message 0x%zX", size, this);
		return size;
	}

	struct sockaddr Message::GetSource() const
	{
		Log::FormatLine(LOG_TRACE, "MESG", "Returning source for message 0x%zX", this);
		return source;
	}

	void Message::PrintToLog() const
	{
		std::stringstream ss;

		// Dump raw bytes to string
		ss << std::hex << std::setfill('0');
		for (int i = 0; i < size; ++i)
		{
			ss << ' ' << std::setw(2) << static_cast<unsigned>(buffer[i]);
		}
		Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());

		ss << std::dec;
		ss.str("");
		ss << "Head:" << GetHead() << " Size:" << GetSize();
		switch (GetMagicNumber()) // Binary version of head
		{
		case 0x4E4EF443: // CONN
        case 0x54505957: // WYPT
        case 0x54584554: // TEXT
        {
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
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
            Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
            break;
        }
        case 0x41544144: // DATA
		{
			std::size_t numCols = (size - 5) / 36;
			float values[32][9];
			for (int i = 0; i < numCols; ++i)
			{
				values[i][0] = buffer[5 + 36 * i];
				std::memcpy(values[i] + 1, buffer + 9 + 36 * i, 9 * sizeof(float));
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
            break;
        }
        case 0x46455244: // DREF
		{
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
            std::string dref((char*)buffer + 6, buffer[5]);
            Log::FormatLine(LOG_DEBUG, "DBUG", "    DREF (size %i) = %s", dref.length(), dref);
            ss.str("");
            int values = buffer[6 + buffer[5]];
            ss << "    Values(size " << values << ") =";
            for (int i = 0; i < values; ++i)
            {
                ss << " " << *((float*)(buffer + values + 1 + sizeof(float) * i));
			}
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
            break;
        }
        case 0x44544547: // GETD
		{
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
            int cur = 6;
            for (int i = 0; i < buffer[5]; ++i)
            {
                std::string dref((char*)buffer + cur + 1, buffer[cur]);
                Log::FormatLine(LOG_DEBUG, "DBUG", "    #%i/%i (size:%i) %s",
					i + 1, buffer[5], dref.length(), dref);
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
			std::memcpy(pos, buffer + 6, 12);
			std::memcpy(orient, buffer + 18, 12);
            ss << " AC:" << (int)aircraft;
			ss << " Pos:(" << pos[0] << ' ' << pos[1] << ' ' << pos[2] << ") Orient:(";
			ss << orient[3] << ' ' << orient[4] << ' ' << orient[5] << ") Gear:";
			ss << gear;
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
            break;
        }
        case 0x554D4953: // SIMU
        {
			ss << ' ' << (int)buffer[5];
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
            break;
        }
        default:
        {
			ss << " UNKNOWN HEADER ";
			Log::WriteLine(LOG_DEBUG, "DBUG", ss.str());
            break;
        }
		}
	}
}
