// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
#include "Log.h"

#include "XPLMUtilities.h"

#include <cstdarg>
#include <cstdio>
#include <ctime>

#ifndef LIN
#include <chrono>
#endif
#include <iomanip>
#include <sstream>

// Implementation note: I initially wrote this class using C++ iostreams, but I couldn't find any
// way to implement FormatLine without adding in a call to sprintf. It therefore seems more
// efficient to me to just use C-style IO and call std::fprintf directly.
namespace XPC
{
	static std::FILE* fd;
	static void WriteTime(FILE* fd)
	{
#ifdef LIN
		// Can't provide high resolution logging on Linux because C++11 doesn't work with X-Plane.
		time_t rawtime;
		tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		char buffer[16] = { 0 };
		// Format is equivalent to [%F %T], but neither of those specifiers is
		// supported on Windows as of Visual Studio 13
		strftime(buffer, 16, "[%H:%M:%S] ", timeinfo);

		fprintf(fd, buffer);
#else
		using namespace std::chrono;

		system_clock::time_point now = system_clock::now();
		std::time_t now_tt = system_clock::to_time_t(now);
		system_clock::time_point now_sec = system_clock::from_time_t(now_tt);
		milliseconds ms = duration_cast<milliseconds>(now - now_sec);
		std::tm * tm = std::localtime(&now_tt);

		std::stringstream ss;
		ss << std::setfill('0')
			<< std::setw(2) << tm->tm_hour << ":"
			<< std::setw(2) << tm->tm_min << ":"
			<< std::setw(2) << tm->tm_sec << "."
			<< std::setw(3) << ms.count() << "|";

		std::fprintf(fd, ss.str().c_str());
#endif
	}

	static void WriteLevel(FILE* fd, int level)
	{
		const char* str;
		switch (level)
		{
		case LOG_OFF:
			str = "  OFF|";
			break;
		case LOG_FATAL:
			str = "FATAL|";
			break;
		case LOG_ERROR:
			str = "ERROR|";
			break;
		case LOG_WARN:
			str = " WARN|";
			break;
		case LOG_INFO:
			str = " INFO|";
			break;
		case LOG_DEBUG:
			str = "DEBUG|";
			break;
		case LOG_TRACE:
			str = "TRACE|";
			break;
		default:
			str = "  UNK|";
			break;
		}
		std::fprintf(fd, str);
	}

	void Log::Initialize(const std::string& version)
	{
		if (LOG_LEVEL == LOG_OFF)
		{
			return;
		}

		// Note: Mode "w" deletes an existing file with the same name. This means that we only
		//       ever get the log from the last run. This matches the way that X-Plane treats its
		//       log.
		fd = std::fopen("XPCLog.txt", "w");
		if (fd != NULL)
		{
			std::time_t rawtime;
			std::tm* timeinfo;
			std::time(&rawtime);
			timeinfo = std::localtime(&rawtime);

			char timeStr[16] = { 0 };
			std::strftime(timeStr, 16, "%Y-%m-%d", timeinfo);

			std::fprintf(fd, "X-Plane Connect [Version %s]\n", version.c_str());
			std::fprintf(fd, "Compiled %s %s\n", __DATE__, __TIME__);
			std::fprintf(fd, "Copyright (c) 2013-2018 United States Government as represented by the\n");
			std::fprintf(fd, "Administrator of the National Aeronautics and Space Administration.\n");
			std::fprintf(fd, "All Rights Reserved.\n\n");

			std::fprintf(fd, "This file contains debugging information about the X-Plane Connect plugin.\n");
			std::fprintf(fd, "If you have technical issues with the plugin, please report them by opening\n");
			std::fprintf(fd, "an issue on GitHub (https://github.com/nasa/XPlaneConnect/issues) or by\n");
			std::fprintf(fd, "emailing Christopher Teubert (christopher.a.teubert@nasa.gov).\n\n");

			int xpVer;
			int xplmVer;
			XPLMHostApplicationID hostID;
			XPLMGetVersions(&xpVer, &xplmVer, &hostID);
			std::fprintf(fd, "X-Plane Version: %d\n", xpVer);
			std::fprintf(fd, "Plugin Manager Version: %d\n", xplmVer);
			std::fprintf(fd, "Host Application ID: %d\n", hostID);
			std::fprintf(fd, "Log file generated on %s.\n", timeStr);
			std::fflush(fd);
		}
	}

	void Log::Close()
	{
		if (fd)
		{
			std::fclose(fd);
		}
	}

	void Log::WriteLine(int level, const std::string& tag, const std::string& value)
	{
		if (level > LOG_LEVEL || !fd)
		{
			return;
		}

		WriteTime(fd);
		WriteLevel(fd, level);
		std::fprintf(fd, "%s|%s\n", tag.c_str(), value.c_str());
		std::fflush(fd);
	}

	void Log::FormatLine(int level, const std::string& tag, std::string format, ...)
	{
		va_list args;

		if (level > LOG_LEVEL || !fd)
		{
			return;
		}
		va_start(args, format);

		WriteTime(fd);
		WriteLevel(fd, level);
		std::fprintf(fd, "%s|", tag.c_str());
		std::vfprintf(fd, format.c_str(), args);
		std::fprintf(fd, "\n");
		std::fflush(fd);

		va_end(args);
	}
}
