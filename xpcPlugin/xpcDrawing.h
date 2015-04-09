#ifndef xpcDrawing_h
#define xpcDrawing_h

#include <stdlib.h>
#include "xplaneConnect.h"

void XPCClearMessage();

void XPCSetMessage(int x, int y, char* msg);

void XPCClearWaypoints();

void XPCAddWaypoints(Waypoint points[], size_t numPoints);

void XPCRemoveWaypoints(Waypoint points[], size_t numPoints);

#endif
