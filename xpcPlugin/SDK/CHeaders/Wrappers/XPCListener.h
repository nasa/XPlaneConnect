#ifndef _XPCListener_h_
#define _XPCListener_h_

#include <vector>
#include <algorithm>

class	XPCBroadcaster;


class	XPCListener {
public:

						XPCListener();
	virtual				~XPCListener();
	
	virtual	void		ListenToMessage(
							long			inMessage,
							void *			inParam)=0;
							
private:

	typedef	std::vector<XPCBroadcaster *>	BroadcastVector;
	
	BroadcastVector	mBroadcasters;

	friend	class	XPCBroadcaster;
	
			void		BroadcasterAdded(
							XPCBroadcaster *	inBroadcaster);

			void		BroadcasterRemoved(
							XPCBroadcaster *	inBroadcaster);
			
};			

#endif