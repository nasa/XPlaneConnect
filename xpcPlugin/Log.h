//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPC_LOG_H
#define XPC_LOG_H
#include <string>

#define LOG_VERBOSITY 2

namespace XPC
{
	/// Handles logging for the plugin.
	/// 
	/// \details Provides functions to write lines to the XPC log file.
	/// \author Jason Watkins
	/// \version 1.0
	/// \since 1.0
	/// \date Intial Version: 2015-04-09
	/// \date Last Updated: 2015-04-09
	class Log
	{
	public:
		/// Initializes the logging component by deleting old log files,
		/// writing header information to the log file.
		static void Initialize();

		/// Writes the C string pointed to by format, followed by a line
		/// terminator to the XPC log file. If format contains format
		/// specifiers, additional arguments following format will be formatted
		/// and inserted in the resulting string, replacing their respective
		/// specifiers.
		///
		/// \param format The format string appropriate for consumption by sprintf.
		static void FormatLine(const char* format, ...);

		/// Writes the specified string value, followed by a line terminator
		/// to the XPC log file.
		///
		/// \param value The value to write.
		static void WriteLine(const std::string& value);

		/// Writes the specified C string value, followed by a line terminator
		/// to the XPC log file.
		///
		/// \param value The value to write.
		static void WriteLine(const char* value);
	};
}
#endif
