// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
//
// X-Plane API
// Copyright(c) 2008, Sandy Barbour and Ben Supnik All rights reserved.
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files(the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Neither the names of the authors nor that of X - Plane or Laminar Research
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission from the authors or
//     Laminar Research, respectively.
#include "Drawing.h"

#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"

#include <cmath>
#include <string>
#include <cstring>
// OpenGL includes
#if IBM
#include <windows.h>
#endif
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#else
#  include <GL/gl.h>
#endif

namespace XPC
{
	// Internal Structures
	typedef struct
	{
		double x;
		double y;
		double z;
	} LocalPoint;

	// Internal Memory
	static const size_t MSG_MAX = 1024;
	static const size_t MSG_LINE_MAX = MSG_MAX / 16;
	static bool msgEnabled = false;
	static int msgX = -1;
	static int msgY = -1;
	static char msgVal[MSG_MAX] = { 0 };
	static size_t newLineCount = 0;
	static size_t newLines[MSG_LINE_MAX] = { 0 };
	static float rgb[3] = { 0.25F, 1.0F, 0.25F };

	static const size_t WAYPOINT_MAX = 128;
	static bool routeEnabled = false;
	static size_t numWaypoints = 0;
	static Waypoint waypoints[WAYPOINT_MAX];
	static LocalPoint localPoints[WAYPOINT_MAX];

	XPLMDataRef planeXref;
	XPLMDataRef	planeYref;
	XPLMDataRef	planeZref;

	// Internal Functions

	/// Comparse two size_t integers. Used by qsort in RemoveWaypoints.
	static int cmp(const void * a, const void * b)
	{
		std::size_t sa = *(size_t*)a;
		std::size_t sb = *(size_t*)b;
		if (sa > sb)
		{
			return 1;
		}
		if (sb > sa)
		{
			return -1;
		}
		return 0;
	}

	/// Draws a cube centered at the specified OpenGL world coordinates.
	///
	/// \param x The X coordinate.
	/// \param y The Y coordinate.
	/// \param z The Z coordinate.
	/// \param d The distance from the player airplane to the center of the cube.
	static void gl_drawCube(float x, float y, float z, float d)
	{
		// tan(0.25) degrees. Should scale all markers to appear about the same size
		const float TAN = 0.00436335F;
		float h = d * TAN;

		glBegin(GL_QUAD_STRIP);
		// Top
		glVertex3f(x - h, y + h, z - h);
		glVertex3f(x + h, y + h, z - h);
		glVertex3f(x - h, y + h, z + h);
		glVertex3f(x + h, y + h, z + h);
		// Front
		glVertex3f(x - h, y - h, z + h);
		glVertex3f(x + h, y - h, z + h);
		// Bottom
		glVertex3f(x - h, y - h, z - h);
		glVertex3f(x + h, y - h, z - h);
		// Back
		glVertex3f(x - h, y + h, z - h);
		glVertex3f(x + h, y + h, z - h);

		glEnd();
		glBegin(GL_QUADS);
		// Left
		glVertex3f(x - h, y + h, z - h);
		glVertex3f(x - h, y + h, z + h);
		glVertex3f(x - h, y - h, z + h);
		glVertex3f(x - h, y - h, z - h);
		// Right
		glVertex3f(x + h, y + h, z + h);
		glVertex3f(x + h, y + h, z - h);
		glVertex3f(x + h, y - h, z - h);
		glVertex3f(x + h, y - h, z + h);

		glEnd();
	}

	/// Draws the string set by the TEXT command.
	static int MessageDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void * inRefcon)
	{
		const int LINE_HEIGHT = 16;
		XPLMDrawString(rgb, msgX, msgY, msgVal, NULL, xplmFont_Basic);
		int y = msgY - LINE_HEIGHT;
		for (size_t i = 0; i < newLineCount; ++i)
		{
			XPLMDrawString(rgb, msgX, y, msgVal + newLines[i], NULL, xplmFont_Basic);
			y -= LINE_HEIGHT;
		}
		return 1;
	}

	/// Draws waypoints.
	static int RouteDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void * inRefcon)
	{
		float px = XPLMGetDataf(planeXref);
		float py = XPLMGetDataf(planeYref);
		float pz = XPLMGetDataf(planeZref);

		// Convert to local
		for (size_t i = 0; i < numWaypoints; ++i)
		{
			Waypoint* g = &waypoints[i];
			LocalPoint* l = &localPoints[i];
			XPLMWorldToLocal(g->latitude, g->longitude, g->altitude,
				&l->x, &l->y, &l->z);
		}


		// Draw posts
		glColor3f(1.0F, 1.0F, 1.0F);
		glBegin(GL_LINES);
		for (size_t i = 0; i < numWaypoints; ++i)
		{
			LocalPoint* l = &localPoints[i];
			glVertex3f((float)l->x, (float)l->y, (float)l->z);
			glVertex3f((float)l->x, -1000.0F, (float)l->z);
		}
		glEnd();

		// Draw route
		glColor3f(1.0F, 0.0F, 0.0F);
		glBegin(GL_LINE_STRIP);
		for (size_t i = 0; i < numWaypoints; ++i)
		{
			LocalPoint* l = &localPoints[i];
			glVertex3f((float)l->x, (float)l->y, (float)l->z);
		}
		glEnd();

		// Draw markers
		glColor3f(1.0F, 1.0F, 1.0F);
		for (size_t i = 0; i < numWaypoints; ++i)
		{
			LocalPoint* l = &localPoints[i];
			float xoff = (float)l->x - px;
			float yoff = (float)l->y - py;
			float zoff = (float)l->z - pz;
			float d = sqrtf(xoff*xoff + yoff*yoff + zoff*zoff);
			gl_drawCube((float)l->x, (float)l->y, (float)l->z, d);
		}
		return 1;
	}

	// Public Functions
	void Drawing::ClearMessage()
	{
		XPLMUnregisterDrawCallback(MessageDrawCallback, xplm_Phase_Window, 0, NULL);
		msgEnabled = false;
	}

	void Drawing::SetMessage(int x, int y, char* msg)
	{
		// Determine the size of the message and clear it if it is empty.
		size_t len = strnlen(msg, MSG_MAX - 1);
		if (len == 0)
		{
			ClearMessage();
			return;
		}

		// Set the message, location, and mark new lines.
		strncpy(msgVal, msg, len + 1);
		newLineCount = 0;
		for (size_t i = 0; i < len && newLineCount < MSG_LINE_MAX; ++i)
		{
			if (msgVal[i] == '\n' || msgVal[i] == '\r')
			{
				msgVal[i] = 0;
				newLines[newLineCount++] = i + 1;
			}
		}
		msgX = x < 0 ? 10 : x;
		msgY = y < 0 ? 600 : y;

		// Enable drawing if necessary
		if (!msgEnabled)
		{
			XPLMRegisterDrawCallback(MessageDrawCallback, xplm_Phase_Window, 0, NULL);
			msgEnabled = true;
		}
	}

	void Drawing::ClearWaypoints()
	{
		numWaypoints = 0;
		if (routeEnabled)
		{
			XPLMUnregisterDrawCallback(RouteDrawCallback, xplm_Phase_Objects, 0, NULL);
		}
		return;
	}

	void Drawing::AddWaypoints(Waypoint points[], size_t numPoints)
	{
		if (numWaypoints + numPoints > WAYPOINT_MAX)
		{
			numPoints = WAYPOINT_MAX - numWaypoints;
		}
		size_t finalNumWaypoints = numPoints + numWaypoints;
		for (size_t i = 0; i < numPoints; ++i)
		{
			waypoints[numWaypoints + i] = points[i];
		}
		numWaypoints = finalNumWaypoints;

		if (!routeEnabled)
		{
			XPLMRegisterDrawCallback(RouteDrawCallback, xplm_Phase_Objects, 0, NULL);
		}
		if (!planeXref)
		{
			planeXref = XPLMFindDataRef("sim/flightmodel/position/local_x");
			planeYref = XPLMFindDataRef("sim/flightmodel/position/local_y");
			planeZref = XPLMFindDataRef("sim/flightmodel/position/local_z");
		}
	}

	void Drawing::RemoveWaypoints(Waypoint points[], size_t numPoints)
	{
		// Build a list of indices of waypoints we should delete.
		size_t delPoints[WAYPOINT_MAX];
		size_t delPointsCur = 0;
		for (size_t i = 0; i < numPoints; ++i)
		{
			Waypoint p = points[i];
			for (size_t j = 0; j < numWaypoints; ++j)
			{
				Waypoint q = waypoints[j];
				if (p.latitude == q.latitude &&
					p.longitude == q.longitude &&
					p.altitude == q.altitude)
				{
					delPoints[delPointsCur++] = j;
					break;
				}
			}
		}
		// Sort the indices so that we only have to iterate them once
		qsort(delPoints, delPointsCur, sizeof(size_t), cmp);

		// Copy the new array on top of the old array
		size_t copyCur = 0;
		size_t count = delPointsCur;
		delPointsCur = 0;
		for (size_t i = 0; i < numWaypoints; ++i)
		{
			if (i == delPoints[delPointsCur])
			{
				++delPointsCur;
				continue;
			}
			waypoints[copyCur++] = waypoints[i];
		}
		numWaypoints -= count;
		if (numWaypoints == 0)
		{
			ClearWaypoints();
		}
	}
}
