#include "XPCListener.h"
#include "XPCBroadcaster.h"

XPCListener::XPCListener()
{
}

XPCListener::~XPCListener()
{
	while (!mBroadcasters.empty())
		mBroadcasters.front()->RemoveListener(this);
}
	
void		XPCListener::BroadcasterAdded(
							XPCBroadcaster *	inBroadcaster)
{
	mBroadcasters.push_back(inBroadcaster);
}							

void		XPCListener::BroadcasterRemoved(
							XPCBroadcaster *	inBroadcaster)
{
	BroadcastVector::iterator iter = std::find(mBroadcasters.begin(),
		mBroadcasters.end(), inBroadcaster);
	if (iter != mBroadcasters.end())
		mBroadcasters.erase(iter);
}							
