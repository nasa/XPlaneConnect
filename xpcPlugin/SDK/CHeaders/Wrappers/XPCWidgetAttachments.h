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
								long			inParam1,
								long			inParam2);

private:

	std::string		mInput;
	std::string		mOutput;

};


class	XPCKeyMessageAttachment : public XPCWidgetAttachment, public XPCBroadcaster {
public:

					XPCKeyMessageAttachment(
								char			inKey,
								long			inMessage,
								void *			inParam,
								bool			inConsume,
								bool			inVkey,
								XPCListener *	inListener);
	virtual			~XPCKeyMessageAttachment();
									
	virtual	int		HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								long			inParam1,
								long			inParam2);

private:

		char	mKey;
		bool	mVkey;
		long	mMsg;
		void *	mParam;
		bool	mConsume;
	
};

class	XPCPushButtonMessageAttachment : public XPCWidgetAttachment, XPCBroadcaster {
public:

					XPCPushButtonMessageAttachment(
								XPWidgetID		inWidget,
								long			inMessage,
								void *			inParam,
								XPCListener *	inListener);
	virtual			~XPCPushButtonMessageAttachment();

	virtual	int		HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								long			inParam1,
								long			inParam2);

private:
		XPWidgetID	mWidget;
		long		mMsg;
		void *		mParam;
};

class	XPCSliderMessageAttachment : public XPCWidgetAttachment, XPCBroadcaster {
public:

					XPCSliderMessageAttachment(
								XPWidgetID		inWidget,
								long			inMessage,
								void *			inParam,
								XPCListener *	inListener);
	virtual			~XPCSliderMessageAttachment();

	virtual	int		HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								long			inParam1,
								long			inParam2);

private:
		XPWidgetID	mWidget;
		long		mMsg;
		void *		mParam;
};


class	XPCCloseButtonMessageAttachment : public XPCWidgetAttachment, XPCBroadcaster {
public:

					XPCCloseButtonMessageAttachment(
								XPWidgetID		inWidget,
								long			inMessage,
								void *			inParam,
								XPCListener *	inListener);
	virtual			~XPCCloseButtonMessageAttachment();

	virtual	int		HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								long			inParam1,
								long			inParam2);

private:
		XPWidgetID	mWidget;
		long		mMsg;
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
								long			inParam1,
								long			inParam2);

};

#endif