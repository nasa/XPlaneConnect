//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#include "Log.h"
#include <cstdarg>
#include <stdio.h>
#include <time.h>

// Implementation note: I initial wrote this class using C++ iostreams, but I couldn't find any
// way to implement FormatLine without adding in a call to sprintf. It therefore seems more
// efficient to me to just use C-style IO and call fprintf directly.
namespace XPC
{
	static void WriteTime(FILE* fd)
	{
		time_t rawtime;
		tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		char buffer[16] = { 0 };
		// Format is equivalent to [%F %T], but neither of those specifiers is
		// supported on Windows as of Visual Studio 13
		strftime(buffer, 16, "[%H:%M:%S] ", timeinfo);
		
		fprintf(fd, buffer);
	}

	void Log::Initialize(std::string version)
	{
		FILE* fd = fopen("XPCLog.txt", "w");
		if (fd != NULL)
		{
			time_t rawtime;
			tm* timeinfo;
			time(&rawtime);
			timeinfo = localtime(&rawtime);

			char timeStr[16] = { 0 };
			// Format is equivalent to %F, but neither of those specifiers is
			// supported on Windows as of Visual Studio 13
			strftime(timeStr, 16, "%Y-%m-%d", timeinfo);

			fprintf(fd, "X-Plane Connect [Version %s]\n", version.c_str());
			fprintf(fd, "Compiled %s %s\n", __DATE__, __TIME__);
			fprintf(fd, "Copyright (c) 2013-2015 United States Government as represented by the\n");
			fprintf(fd, "Administrator of the National Aeronautics and Space Administration.\n");
			fprintf(fd, "All Rights Reserved.\n\n");

			fprintf(fd, "This file contains debugging information about the X-Plane Connect plugin.\n");
			fprintf(fd, "If you have technical issues with the plugin, please report them by opening\n");
			fprintf(fd, "an issue on GitHub (https://github.com/nasa/XPlaneConnect/issues) or by\n");
			fprintf(fd, "emailing Christopher Teubert (christopher.a.teubert@nasa.gov).\n\n");

			fprintf(fd, "Log file generated on %s.\n", timeStr);
			fclose(fd);
		}
	}

	void Log::WriteLine(const std::string& value)
	{
		Log::WriteLine(value.c_str());
	}

	void Log::WriteLine(const char* value)
	{
		FILE* fd = fopen("XPCLog.txt", "a");
		if (!fd)
		{
			return;
		}

		WriteTime(fd);
		fprintf(fd, "%s\n", value);

		fclose(fd);
	}

	void Log::FormatLine(const char* format, ...)
	{
		va_list args;

		FILE* fd = fopen("XPCLog.txt", "a");
		if (!fd)
		{
			return;
		}
		va_start(args, format);

		WriteTime(fd);
		vfprintf(fd, format, args);
		fprintf(fd, "\n");

		fclose(fd);

		va_end(args);
	}
}