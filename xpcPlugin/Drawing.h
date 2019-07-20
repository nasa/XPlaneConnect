// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPCPLUGIN_DRAWING_H_
#define XPCPLUGIN_DRAWING_H_

#include <cstdlib>

namespace XPC
{
	typedef struct
	{
		double latitude;
		double longitude;
		double altitude;
	} Waypoint;

	/// Handles tasks that involve drawing to the screen in X-Plane.
	///
	/// \author Jason Watkins
	/// \version 1.0
	/// \since 1.0
	/// \date Intial Version: 2015-04-10
	/// \date Last Updated: 2015-04-10
	class Drawing
	{
	public:
		/// Clears the current message on the screen if any and unregisters the
		/// draw callback for message drawing.
		static void ClearMessage();

		/// Sets the message to be drawn on the screen.
		///
		/// \param x   The x coordinate of the message relative to the left edge
		///            of the screen.
		/// \param y   The y coordinate of the message relative to the bottom
		///            edge of the screen
		/// \param msg A C string containing the message to display. The message
		///            value is copied into a local buffer.
		static void SetMessage(int x, int y, char* msg);

		/// Adds the given waypoints to the list of waypoints to draw.
		///
		/// \param points    A pointer to an array of waypoints.
		/// \param numPoints The number of points in the array.
		static void AddWaypoints(Waypoint points[], size_t numPoints);

		/// Removes all waypoints and unregisters the callback for waypoint
		/// drawing.
		static void ClearWaypoints();

		/// Removes the given waypoints from the list of waypoints to draw.
		/// If all waypoints are removed as a result of this action, unregisters
		/// the callback for waypoint drawing.
		///
		/// \param points    A pointer to an array of waypoints.
		/// \param numPoints The number of points in the array.
		static void RemoveWaypoints(Waypoint points[], size_t numPoints);
	};
}
#endif
