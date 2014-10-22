#ifndef _XPCDisplay_h_
#define _XPCDisplay_h_

#include "XPLMDisplay.h"

class	XPCKeySniffer {
public:

					XPCKeySniffer(int inBeforeWindows);
	virtual			~XPCKeySniffer();
	
	virtual	int		HandleKeyStroke(
							char 			inCharKey, 
							XPLMKeyFlags	inFlags,
							char			inVirtualKey)=0;

private:

		int		mBeforeWindows;

	static	int		KeySnifferCB(
							char 			inCharKey, 
							XPLMKeyFlags	inFlags,
							char			inVirtualKey,
							void * 			inRefCon);
};



class	XPCWindow {
public:

					XPCWindow(
						int					inLeft,
						int					inTop,
						int					inRight,
						int					inBottom,
						int					inIsVisible);
	virtual			~XPCWindow();

	virtual	void	DoDraw(void)=0;
	virtual	void	HandleKey(char inKey, XPLMKeyFlags inFlags, char inVirtualKey)=0;
	virtual	void	LoseFocus(void)=0;
	virtual	int		HandleClick(int x, int y, XPLMMouseStatus inMouse)=0;
	
			void	GetWindowGeometry(
						int	*				outLeft,
						int	*				outTop,
						int	*				outRight,
						int	*				outBottom);
			void	SetWindowGeometry(
						int					inLeft,
						int					inTop,
						int					inRight,
						int					inBottom);
			int		GetWindowIsVisible(void);
			void	SetWindowIsVisible(
						int					inIsVisible);
			void	TakeKeyboardFocus(void);
			void	BringWindowToFront(void);
			int		IsWindowInFront(void);
		
private:

		XPLMWindowID	mWindow;

	static	void	DrawCB(XPLMWindowID inWindowID, void * inRefcon);
	static	void	HandleKeyCB(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey, void * inRefcon, int losingFocus);
	static	int		MouseClickCB(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void * inRefcon);

};

#endif