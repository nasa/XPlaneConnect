// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPCPLUGIN_LOG_H_
#define XPCPLUGIN_LOG_H_
#include <string>

// LOG_VERBOSITY determines the level of logging throughout the plugin.
//     OFF: No logging at all will be performed.
//   FATAL: Critical errors that would normally result in termination of the program. Because XPC
//          operates in the X-Plane process, we try to never actually crash. As a result, we this
//          level of logging may be the only indication of a problem.
//   ERROR: All errors not covered by FATAL
//    WARN: Potentially, but not definitely, incorrect behavior
//    INFO: Information about normal actions taken by the plugin.
//   DEBUG: More verbose information usefull for debugging.
//   TRACE: Log all the things!
#define LOG_OFF   0
#define LOG_FATAL 1
#define LOG_ERROR 2
#define LOG_WARN  3
#define LOG_INFO  4
#define LOG_DEBUG 5
#define LOG_TRACE 6

#define LOG_LEVEL LOG_TRACE

namespace XPC
{
	/// Handles logging for the plugin.
	///
	/// \details Provides functions to write lines to the XPC log file.
	/// \author Jason Watkins
	/// \version 1.1
	/// \since 1.0
	/// \date Intial Version: 2015-04-09
	/// \date Last Updated: 2015-05-11
	class Log
	{
	public:
		/// Initializes the logging component by deleting old log files,
		/// writing header information to the log file.
		static void Initialize(const std::string& header);

		/// Closes the log file.
		static void Close();

		/// Writes the string pointed to by format, followed by a line
		/// terminator to the XPC log file. If format contains format
		/// specifiers, additional arguments following format will be formatted
		/// and inserted in the resulting string, replacing their respective
		/// specifiers.
		///
		/// \param format The format string appropriate for consumption by sprintf.
		///
		/// \remarks Note that Visual C++ silently fails va_start when the last non-varargs
		///          argument is a reference, so we need a value-type format here.
		static void FormatLine(int level, const std::string& tag, const std::string format, ...);

		/// Writes the specified string value, followed by a line terminator
		/// to the XPC log file.
		///
		/// \param value The value to write.
		static void WriteLine(int level, const std::string& tag, const std::string& value);
	};
}
#endif
