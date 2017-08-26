#ifndef _XPCWidgetAttachments_h_
#define _XPCWidgetAttachments_h_

#include <string>

#include "XPCWidget.h"
#include "XPCBroadcaster.h"

class	XPCKeyFilterAttachment : public XPCWidgetAttachment {
public:

					XPCKeyFilterAttachment(
								const char *	inValidKeys,
								const char *	outValidKeys);
	virtual			~XPCKeyFilterAttachment();

	virtual	int		HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2);

private:

	std::string		mInput;
	std::string		mOutput;

};


class	XPCKeyMessageAttachment : public XPCWidgetAttachment, public XPCBroadcaster {
public:

					XPCKeyMessageAttachment(
								char			inKey,
								int				inMessage,
								void *			inParam,
								bool			inConsume,
								bool			inVkey,
								XPCListener *	inListener);
	virtual			~XPCKeyMessageAttachment();
									
	virtual	int		HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2);

private:

		char	mKey;
		bool	mVkey;
		int		mMsg;
		void *	mParam;
		bool	mConsume;
	
};

class	XPCPushButtonMessageAttachment : public XPCWidgetAttachment, XPCBroadcaster {
public:

					XPCPushButtonMessageAttachment(
								XPWidgetID		inWidget,
								int				inMessage,
								void *			inParam,
								XPCListener *	inListener);
	virtual			~XPCPushButtonMessageAttachment();

	virtual	int		HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2);

private:
		XPWidgetID	mWidget;
		int			mMsg;
		void *		mParam;
};

class	XPCSliderMessageAttachment : public XPCWidgetAttachment, XPCBroadcaster {
public:

					XPCSliderMessageAttachment(
								XPWidgetID		inWidget,
								int				inMessage,
								void *			inParam,
								XPCListener *	inListener);
	virtual			~XPCSliderMessageAttachment();

	virtual	int		HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2);

private:
		XPWidgetID	mWidget;
		int			mMsg;
		void *		mParam;
};


class	XPCCloseButtonMessageAttachment : public XPCWidgetAttachment, XPCBroadcaster {
public:

					XPCCloseButtonMessageAttachment(
								XPWidgetID		inWidget,
								int				inMessage,
								void *			inParam,
								XPCListener *	inListener);
	virtual			~XPCCloseButtonMessageAttachment();

	virtual	int		HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2);

private:
		XPWidgetID	mWidget;
		int			mMsg;
		void *		mParam;
};

class	XPCTabGroupAttachment : public XPCWidgetAttachment {
public:

					XPCTabGroupAttachment();
	virtual			~XPCTabGroupAttachment();

	virtual	int		HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2);

};

#endif