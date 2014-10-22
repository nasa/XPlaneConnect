#ifndef _XPCBroadcaster_h_
#define _XPCBroadcaster_h_

#include <vector>
#include <algorithm>

class	XPCListener;

class	XPCBroadcaster {
public:

						XPCBroadcaster();
	virtual				~XPCBroadcaster();
	
			void		AddListener(
							XPCListener *	inListener);
			void		RemoveListener(
							XPCListener *	inListener);
	
protected:

			void		BroadcastMessage(
							long		inMessage,
							void *		inParam=0);

private:

	typedef	std::vector<XPCListener *>	ListenerVector;
	
		ListenerVector	mListeners;

	// Reentrancy support
	
	ListenerVector::iterator *	mIterator;

};

#endif