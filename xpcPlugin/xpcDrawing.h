#ifndef xpcDrawing_h
#define xpcDrawing_h

#include <stdlib.h>

typedef struct
{
	double lattitude;
	double longitude;
	double altitude;
} Waypoint;

void XPCClearMessage();

void XPCSetMessage(int x, int y, char* msg);

void XPCClearWaypoints();

void XPCAddWaypoints(Waypoint points[], size_t numPoints);

void XPCRemoveWaypoints(Waypoint points[], size_t numPoints);

#endif
