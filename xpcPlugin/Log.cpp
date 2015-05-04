//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#include "Log.h"

#include "XPLMUtilities.h"

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

// Implementation note: I initial wrote this class using C++ iostreams, but I couldn't find any
// way to implement FormatLine without adding in a call to sprintf. It therefore seems more
// efficient to me to just use C-style IO and call std::fprintf directly.
namespace XPC
{
	static std::FILE* fd;
	static void WriteTime(FILE* fd)
	{
		using namespace std::chrono;

		system_clock::time_point now = system_clock::now();
		std::time_t now_tt = system_clock::to_time_t(now);
		system_clock::time_point now_sec = system_clock::from_time_t(now_tt);
		milliseconds ms = duration_cast<milliseconds>(now - now_sec);
		std::tm * tm = std::localtime(&now_tt);

		std::stringstream ss;
		ss << std::setfill('0') << "["
			<< std::setw(2) << tm->tm_hour << ":"
			<< std::setw(2) << tm->tm_min << ":"
			<< std::setw(2) << tm->tm_sec << "."
			<< std::setw(3) << ms.count() << "]";
		
		std::fprintf(fd, ss.str().c_str());
	}

	void Log::Initialize(std::string version)
	{
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
			std::fprintf(fd, "Copyright (c) 2013-2015 United States Government as represented by the\n");
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

	void Log::WriteLine(const std::string& value)
	{
		Log::WriteLine(value.c_str());
	}

	void Log::WriteLine(const char* value)
	{
		if (!fd)
		{
			return;
		}

		WriteTime(fd);
		std::fprintf(fd, "%s\n", value);
		std::fflush(fd);
	}

	void Log::FormatLine(const char* format, ...)
	{
		va_list args;

		if (!fd)
		{
			return;
		}
		va_start(args, format);

		WriteTime(fd);
		std::vfprintf(fd, format, args);
		std::fprintf(fd, "\n");
		std::fflush(fd);

		va_end(args);
	}
}
