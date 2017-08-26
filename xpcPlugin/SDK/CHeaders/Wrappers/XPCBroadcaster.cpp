#include "XPCBroadcaster.h"
#include "XPCListener.h"

XPCBroadcaster::XPCBroadcaster() :
	mIterator(NULL)
{
}

XPCBroadcaster::~XPCBroadcaster()
{
	ListenerVector::iterator iter;
	mIterator = &iter;
	for (iter = mListeners.begin(); iter != mListeners.end(); ++iter)
	{
		(*iter)->BroadcasterRemoved(this);
	}
}

void		XPCBroadcaster::AddListener(
				XPCListener *	inListener)
{
	mListeners.push_back(inListener);
	inListener->BroadcasterAdded(this);
}				

void		XPCBroadcaster::RemoveListener(
				XPCListener *	inListener)
{
	ListenerVector::iterator iter = std::find
		(mListeners.begin(), mListeners.end(), inListener);
	if (iter == mListeners.end())
		return;
		
	if (mIterator != NULL)
	{
		if (*mIterator >= iter)
			(*mIterator)--;
	}
	
	mListeners.erase(iter);
	inListener->BroadcasterRemoved(this);
}				

void		XPCBroadcaster::BroadcastMessage(
				int			inMessage,
				void *		inParam)
{
	ListenerVector::iterator iter;
	mIterator = &iter;
	for (iter = mListeners.begin(); iter != mListeners.end(); ++iter)
	{
		(*iter)->ListenToMessage(inMessage, inParam);
	}
	mIterator = NULL;
}				

