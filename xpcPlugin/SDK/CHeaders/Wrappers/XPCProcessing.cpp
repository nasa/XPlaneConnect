#include "XPCProcessing.h"
#include "XPLMUtilities.h"

XPCProcess::XPCProcess() :
	mInCallback(false),
	mCallbackTime(0)
{
	XPLMRegisterFlightLoopCallback(FlightLoopCB, 0, reinterpret_cast<void *>(this));
}

XPCProcess::~XPCProcess()
{
	XPLMUnregisterFlightLoopCallback(FlightLoopCB, reinterpret_cast<void *>(this));
}
	
void		XPCProcess::StartProcessTime(float	inSeconds)
{
	mCallbackTime = inSeconds;
	if (!mInCallback)
		XPLMSetFlightLoopCallbackInterval(
			FlightLoopCB, mCallbackTime, 1/*relative to now*/, reinterpret_cast<void *>(this));
}

void		XPCProcess::StartProcessCycles(int	inCycles)
{
	mCallbackTime = -inCycles;
	if (!mInCallback)
		XPLMSetFlightLoopCallbackInterval(
			FlightLoopCB, mCallbackTime, 1/*relative to now*/, reinterpret_cast<void *>(this));
}

void		XPCProcess::StopProcess(void)
{
	mCallbackTime = 0;
	if (!mInCallback)
		XPLMSetFlightLoopCallbackInterval(
			FlightLoopCB, mCallbackTime, 1/*relative to now*/, reinterpret_cast<void *>(this));
}


float	XPCProcess::FlightLoopCB(
						float 				inElapsedSinceLastCall, 
						float				inElapsedTimeSinceLastFlightLoop,
						int 				inCounter, 
						void * 				inRefcon)
{
	XPCProcess * me = reinterpret_cast<XPCProcess *>(inRefcon);
	me->mInCallback = true;
	me->DoProcessing(inElapsedSinceLastCall, inElapsedTimeSinceLastFlightLoop, inCounter);
	me->mInCallback = false;
	return me->mCallbackTime;
}