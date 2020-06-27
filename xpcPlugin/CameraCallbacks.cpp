//
//	support.cpp
//	xpcPlugin
//
//	Created by Kai Lehmkuehler on 14/01/2019.
//

#include <stdio.h>

#include "MessageHandlers.h"
#include "DataManager.h"
#include "Log.h"

#include "XPLMUtilities.h"
#include "XPLMGraphics.h"


#include <cmath>
#include <cstring>

namespace XPC
{
	// Runway Camera Callback: Places the camera at campos.loc and points it at the aircraft, similar to a remote piloting experience.
	// Optionally, the direction of the camera can be specified by the user.
	int MessageHandlers::CamCallback_RunwayCam( XPLMCameraPosition_t * outCameraPosition, int inIsLosingControl, void *inRefcon)
	{
		if (outCameraPosition && !inIsLosingControl)
		{
			struct CameraProperties* campos = (struct CameraProperties*)inRefcon;
			
			// camera position
			double clat = campos->loc[0];
			double clon = campos->loc[1];
			double calt = campos->loc[2];
			
			double cX;
			double cY;
			double cZ;
			XPLMWorldToLocal(clat, clon, calt, &cX, &cY, &cZ);
			
			outCameraPosition->x = cX;
			outCameraPosition->y = cY;
			outCameraPosition->z = cZ;
			
			// Log::FormatLine(LOG_TRACE, "CAM", "Cam pos %f %f %f", clat, clon, calt);
			
			// calculate camera direction to point at the aircraft
			if(campos->direction[0] < -180)
			{
				// local aircraft position
				double x = XPC::DataManager::GetDouble(XPC::DREF_LocalX, 0);
				double y = XPC::DataManager::GetDouble(XPC::DREF_LocalY, 0);
				double z = XPC::DataManager::GetDouble(XPC::DREF_LocalZ, 0);
				
				// relative position vector camera to aircraft
				double dx = x - cX;
				double dy = y - cY;
				double dz = z - cZ;
				
				// Log::FormatLine(LOG_TRACE, "CAM", "Cam vect %f %f %f", dx, dy, dz);
				
				double pi = 3.141592653589793;
				
				// horizontal distance
				double dist = sqrt(dx*dx + dz*dz);
				
				outCameraPosition->pitch = atan2(dy, dist) * 180.0/pi;
				
				double angle = atan2(dz, dx) * 180.0/pi; // rel to pos right (pos X)
				
				outCameraPosition->heading = 90 + angle; // rel to north
				
				// Log::FormatLine(LOG_TRACE, "CAM", "Cam p %f hdg %f ", outCameraPosition->pitch, outCameraPosition->heading);
				
				outCameraPosition->roll = 0;
			}
			// point camera at specified direction
			else
			{
				outCameraPosition->roll		= campos->direction[0];
				outCameraPosition->pitch	= campos->direction[1];
				outCameraPosition->heading	= campos->direction[2];
			}
			
			outCameraPosition->zoom = campos->zoom;
		}
		
		return 1;
	}
	
	// Chase Camera Callback: Places the camera at campos.loc RELATIVE to and MOVING with the aircraft, pointing at the specified
	// direction.
	int MessageHandlers::CamCallback_ChaseCam( XPLMCameraPosition_t * outCameraPosition, int inIsLosingControl, void *inRefcon)
	{
		if (outCameraPosition && !inIsLosingControl)
		{
			double DTR = 3.141592653589793 / 180.0;
			
			struct CameraProperties* campos = (struct CameraProperties*)inRefcon;
			
			// Camera location relative to aircraft (local axes)
			
			// local aircraft position
			double x = XPC::DataManager::GetDouble(XPC::DREF_LocalX, 0); // +east
			double y = XPC::DataManager::GetDouble(XPC::DREF_LocalY, 0); // +up
			double z = XPC::DataManager::GetDouble(XPC::DREF_LocalZ, 0); // +south
			
			// aircraft attitude - degrees
			double phi = XPC::DataManager::GetFloat(XPC::DREF_Roll, 0);
			double the = XPC::DataManager::GetFloat(XPC::DREF_Pitch, 0);
			double psi = XPC::DataManager::GetFloat(XPC::DREF_HeadingTrue, 0);
			
			// camera position vector with respect to aircraft CG [m] (body axes)
			double c_x = campos->loc[0]; // back
			double c_y = campos->loc[1]; // right
			double c_z = campos->loc[2]; // up
			
			// camera position vector in local axes - will move with aircraft if not along principal axes
			// cLocal = Leb * cBody
			// http://www.xsquawkbox.net/xpsdk/mediawiki/ScreenCoordinates
			
			double x_phi=c_y*cos(phi*DTR) + c_z*sin(phi*DTR);
			double y_phi=c_z*cos(phi*DTR) - c_y*sin(phi*DTR);
			double z_phi=c_x;
			
			double x_the=x_phi;
			double y_the=y_phi*cos(the*DTR) - z_phi*sin(the*DTR);
			double z_the=z_phi*cos(the*DTR) + y_phi*sin(the*DTR);
			
			double x_wrl=x_the*cos(psi*DTR) - z_the*sin(psi*DTR);
			double y_wrl=y_the							;
			double z_wrl=z_the*cos(psi*DTR) + x_the*sin(psi*DTR);
			
			outCameraPosition->x = x + x_wrl;
			outCameraPosition->y = y + y_wrl;
			outCameraPosition->z = z + z_wrl;
			
			// set direction value to -998 to keep camera pointing straight along that axis
			if(campos->direction[0] < -180)
			{
				outCameraPosition->roll		= 0;
			}
			else
			{
				outCameraPosition->roll		= phi + campos->direction[0];
			}
            
			if(campos->direction[1] < -180)
			{
				outCameraPosition->pitch	= 0;
			}
			else
			{
				outCameraPosition->pitch	= the + campos->direction[1];
			}
            
			if(campos->direction[2] < -180)
			{
				outCameraPosition->heading	= 0;
			}
			else
			{
				outCameraPosition->heading	= psi + campos->direction[2];
			}
			
			outCameraPosition->zoom = campos->zoom;
		}
	
		return 1;

	}
	
}
