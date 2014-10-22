#ifndef _XPCProcessing_h_
#define _XPCProcessing_h_

#include "XPLMProcessing.h"

class	XPCProcess {
public:

						XPCProcess();
	virtual				~XPCProcess();
	
			void		StartProcessTime(float	inSeconds);
			void		StartProcessCycles(long	inCycles);
			void		StopProcess(void);

	virtual	void		DoProcessing(
							float 				inElapsedSinceLastCall, 
							float				inElapsedTimeSinceLastFlightLoop,
							int 				inCounter)=0;

private:

	static	float	FlightLoopCB(
							float 				inElapsedSinceLastCall, 
							float				inElapsedTimeSinceLastFlightLoop,
							int 				inCounter, 
							void * 				inRefcon);
						
		bool		mInCallback;
		float		mCallbackTime;
		
	XPCProcess(const XPCProcess&);
	XPCProcess& operator=(const XPCProcess&);

};

#endif
