//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#include "Log.h"

#include "XPLMUtilities.h"

#include <cstdarg>
#include <cstdio>
#include <ctime>

// Implementation note: I initial wrote this class using C++ iostreams, but I couldn't find any
// way to implement FormatLine without adding in a call to sprintf. It therefore seems more
// efficient to me to just use C-style IO and call std::fprintf directly.
namespace XPC
{
	static void WriteTime(FILE* fd)
	{
		std::time_t rawtime;
		std::tm* timeinfo;
		std::time(&rawtime);
		timeinfo = std::localtime(&rawtime);

		char buffer[16] = { 0 };
		// Format is equivalent to [%F %T], but neither of those specifiers is
		// supported on Windows as of Visual Studio 13
		std::strftime(buffer, 16, "[%H:%M:%S] ", timeinfo);
		
		std::fprintf(fd, buffer);
	}

	void Log::Initialize(std::string version)
	{
		std::FILE* fd = std::fopen("XPCLog.txt", "w");
		if (fd != NULL)
		{
			std::time_t rawtime;
			std::tm* timeinfo;
			std::time(&rawtime);
			timeinfo = std::localtime(&rawtime);

			char timeStr[16] = { 0 };
			// Format is equivalent to %F, but neither of those specifiers is
			// supported on Windows as of Visual Studio 13
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
			std::fclose(fd);
		}
	}

	void Log::WriteLine(const std::string& value)
	{
		Log::WriteLine(value.c_str());
	}

	void Log::WriteLine(const char* value)
	{
		std::FILE* fd = std::fopen("XPCLog.txt", "a");
		if (!fd)
		{
			return;
		}

		WriteTime(fd);
		std::fprintf(fd, "%s\n", value);

		std::fclose(fd);
	}

	void Log::FormatLine(const char* format, ...)
	{
		va_list args;

		std::FILE* fd = std::fopen("XPCLog.txt", "a");
		if (!fd)
		{
			return;
		}
		va_start(args, format);

		WriteTime(fd);
		std::vfprintf(fd, format, args);
		std::fprintf(fd, "\n");

		std::fclose(fd);

		va_end(args);
	}
}