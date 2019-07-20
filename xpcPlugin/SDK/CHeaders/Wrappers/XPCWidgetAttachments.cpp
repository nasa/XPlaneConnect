#include "XPCWidgetAttachments.h"
#include "XPStandardWidgets.h"
#include "XPWidgetUtils.h"

static	void	XPCGetOrderedSubWidgets(
							XPWidgetID						inWidget,
							std::vector<XPWidgetID>&		outChildren);					

XPCKeyFilterAttachment::XPCKeyFilterAttachment(
								const char *	inValidKeys,
								const char *	outValidKeys) :
	mInput(inValidKeys),
	mOutput(outValidKeys)
{
}								
								
XPCKeyFilterAttachment::~XPCKeyFilterAttachment()
{
}

int		XPCKeyFilterAttachment::HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2)
{
	if (inMessage == xpMsg_KeyPress)
	{
		char&	theKey = KEY_CHAR(inParam1);
		std::string::size_type pos = mInput.find(theKey);
		if (pos == std::string::npos)
			return 1;	// Not found; eat the key!
		else {
			theKey = mOutput[pos];
			return 0;
		}	// Let it live.
	}
	return 0;
}								


XPCKeyMessageAttachment::XPCKeyMessageAttachment(
								char			inKey,
								int				inMessage,
								void *			inParam,
								bool			inConsume,
								bool			inVkey,
								XPCListener *	inListener) :
	mKey(inKey), mMsg(inMessage), mParam(inParam), mConsume(inConsume),
	mVkey(inVkey)
{
	if (inListener != NULL)
		this->AddListener(inListener);
}	

XPCKeyMessageAttachment::~XPCKeyMessageAttachment()
{
}
									
int		XPCKeyMessageAttachment::HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2)
{
	if (inMessage == xpMsg_KeyPress)
	{
		char theKey = mVkey ? KEY_VKEY(inParam1) : KEY_CHAR(inParam1);
		if (theKey != mKey)
			return 0;
		if (!(KEY_FLAGS(inParam1) & xplm_DownFlag))
			return 0;
		
		BroadcastMessage(mMsg, mParam);
		return mConsume ? 1 : 0;
	}
	return 0;
}								

XPCPushButtonMessageAttachment::XPCPushButtonMessageAttachment(
									XPWidgetID		inWidget,
									int				inMessage,
									void *			inParam,
									XPCListener *	inListener) :
	mMsg(inMessage), mParam(inParam), mWidget(inWidget)
{
	if (inListener != NULL)
		this->AddListener(inListener);
}

XPCPushButtonMessageAttachment::~XPCPushButtonMessageAttachment()
{
}

int		XPCPushButtonMessageAttachment::HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2)
{
	if ((inMessage == xpMsg_PushButtonPressed) && ((XPWidgetID) inParam1 == mWidget))
	{
		BroadcastMessage(mMsg, mParam);
		return 1;
	}

	if ((inMessage == xpMsg_ButtonStateChanged) && ((XPWidgetID) inParam1 == mWidget))
	{
		BroadcastMessage(mMsg, mParam);
		return 1;
	}
	return 0;	
}					

XPCSliderMessageAttachment::XPCSliderMessageAttachment(
									XPWidgetID		inWidget,
									int				inMessage,
									void *			inParam,
									XPCListener *	inListener) :
	mMsg(inMessage), mParam(inParam), mWidget(inWidget)
{
	if (inListener != NULL)
		this->AddListener(inListener);
}

XPCSliderMessageAttachment::~XPCSliderMessageAttachment()
{
}

int		XPCSliderMessageAttachment::HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2)
{
	if ((inMessage == xpMsg_ScrollBarSliderPositionChanged) && ((XPWidgetID) inParam1 == mWidget))
	{
		BroadcastMessage(mMsg, mParam);
		return 1;
	}

	return 0;	
}									


XPCCloseButtonMessageAttachment::XPCCloseButtonMessageAttachment(
									XPWidgetID		inWidget,
									int				inMessage,
									void *			inParam,
									XPCListener *	inListener) :
	mMsg(inMessage), mParam(inParam), mWidget(inWidget)
{
	if (inListener != NULL)
		this->AddListener(inListener);
}

XPCCloseButtonMessageAttachment::~XPCCloseButtonMessageAttachment()
{
}

int		XPCCloseButtonMessageAttachment::HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2)
{
	if ((inMessage == xpMessage_CloseButtonPushed) && ((XPWidgetID) inParam1 == mWidget))
	{
		BroadcastMessage(mMsg, mParam);
		return 1;
	}

	return 0;	
}									

XPCTabGroupAttachment::XPCTabGroupAttachment()
{
}

XPCTabGroupAttachment::~XPCTabGroupAttachment()
{
}

int		XPCTabGroupAttachment::HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2)
{
	if ((inMessage == xpMsg_KeyPress) && (KEY_CHAR(inParam1) == XPLM_KEY_TAB) &&
		((KEY_FLAGS(inParam1) & xplm_UpFlag) == 0))
	{
		bool backwards = (KEY_FLAGS(inParam1) & xplm_ShiftFlag) != 0;
		std::vector<XPWidgetID>	widgets;
		XPCGetOrderedSubWidgets(inWidget, widgets);
		int	n, index = 0;
		XPWidgetID	focusWidget = XPGetWidgetWithFocus();
		std::vector<XPWidgetID>::iterator iter = std::find(widgets.begin(), widgets.end(), focusWidget);
		if (iter != widgets.end())
		{
			index = std::distance(widgets.begin(), iter);
			if (backwards)
				index--;
			else
				index++;
			if (index < 0)
				index = widgets.size() - 1;
			if (index >= widgets.size())
				index = 0;
		}
		
		if (backwards)
		{
			for (n = index; n >= 0; --n)
			{
				if (XPGetWidgetProperty(widgets[n], xpProperty_Enabled, NULL))
				if (XPSetKeyboardFocus(widgets[n]) != NULL)
					return 1;
			}
			for (n = widgets.size() - 1; n > index; --n)
			{
				if (XPGetWidgetProperty(widgets[n], xpProperty_Enabled, NULL))
				if (XPSetKeyboardFocus(widgets[n]) != NULL)
					return 1;
			}				
		} else {
			for (n = index; n < widgets.size(); ++n)
			{
				if (XPGetWidgetProperty(widgets[n], xpProperty_Enabled, NULL))
				if (XPSetKeyboardFocus(widgets[n]) != NULL)
					return 1;
			}
			for (n = 0; n < index; ++n)
			{
				if (XPGetWidgetProperty(widgets[n], xpProperty_Enabled, NULL))
				if (XPSetKeyboardFocus(widgets[n]) != NULL)
					return 1;
			}				
		}
	} 
	return 0;
}								



static	void	XPCGetOrderedSubWidgets(
							XPWidgetID						inWidget,
							std::vector<XPWidgetID>&		outChildren)
{
	outChildren.clear();
	int	count = XPCountChildWidgets(inWidget);
	for (int n = 0; n < count; ++n)
	{
		XPWidgetID	child = XPGetNthChildWidget(inWidget, n);
		outChildren.push_back(child);
		std::vector<XPWidgetID>	grandChildren;
		XPCGetOrderedSubWidgets(child, grandChildren);
		
		outChildren.insert(outChildren.end(), grandChildren.begin(), grandChildren.end());
	}
}							
