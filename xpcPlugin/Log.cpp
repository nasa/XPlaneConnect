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
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);

		char buffer[23] = { 0 };
		// Format is equivalent to [%F %T], but neither of those specifiers is
		// supported on Windows as of Visual Studio 13
		strftime(buffer, 23, "[%Y-%m-%d %H:%M:%S] ", timeinfo);
		
		fprintf(fd, buffer);
	}

	void Log::Initialize()
	{

	}

	void Log::WriteLine(const std::string& value)
	{
		Log::WriteLine(value.c_str());
	}

	void Log::WriteLine(const char* value)
	{
		FILE* fd = fopen("xpcLog.txt", "a");
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
		va_start(args, format);

		FILE* fd = fopen("xpcLog.txt", "a");
		if (!fd)
		{
			return;
		}

		WriteTime(fd);
		vfprintf(fd, format, args);
		fprintf(fd, "\n");

		fclose(fd);

		va_end(args);
	}
}