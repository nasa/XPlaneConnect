//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPC_LOG_H
#define XPC_LOG_H
#include <string>

// LOG_VERBOSITY determines the level of logging throughout the plugin.
//   0: Minimum logging. Only plugin manager events will be logged.
//   1: Critical errors. When an error that prevents correct operation of the
//      plugin, attempt to write useful information to the log. Note that since
//      XPC runs inside the X-Plane executable, we try very hard no to crash.
//      As a result, these log messages may be the only indication of failure.
//   2: All errors. Any time something unexpected happens, log it.
//   3: Significant actions. Any time something happens outside of normal
//      command processing, log it.
//   5: Everything. Log nearly every single action the plugin takes. This may
//      have a detrimental impact on X-Plane performance.
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
		static void Initialize(std::string header);

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
