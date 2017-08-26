#ifndef _XPCWidget_h_
#define _XPCWidget_h_

#include <vector>
#include <algorithm>
#include "XPWidgets.h"

class	XPCWidget;

class	XPCWidgetAttachment {
public:

	virtual	int			HandleWidgetMessage(
								XPCWidget *		inObject,
								XPWidgetMessage	inMessage,
								XPWidgetID		inWidget,
								intptr_t		inParam1,
								intptr_t		inParam2)=0;
								
};

class	XPCWidget {
public:

						XPCWidget(
								int						inLeft,
								int						inTop,
								int						inRight,
								int						inBottom,
								bool					inVisible,
								const char *			inDescriptor,
								bool					inIsRoot,
								XPWidgetID				inParent,
								XPWidgetClass			inClass);
						XPCWidget(
								XPWidgetID				inWidget,
								bool					inOwnsWidget);
	virtual				~XPCWidget();
	
			void		SetOwnsWidget(
								bool 					inOwnsWidget);
			void		SetOwnsChildren(
								bool 					inOwnsChildren);

			operator XPWidgetID () const;
			
			XPWidgetID	Get(void) const;

			void		AddAttachment(
								XPCWidgetAttachment * 	inAttachment, 
								bool 					inOwnsAttachment,
								bool					inPrefilter);
			void		RemoveAttachment(
								XPCWidgetAttachment * 	inAttachment);

	virtual	int			HandleWidgetMessage(
								XPWidgetMessage			inMessage,
								XPWidgetID				inWidget,
								intptr_t				inParam1,
								intptr_t				inParam2);
		
private:

	static	int			WidgetCallback(
								XPWidgetMessage			inMessage,
								XPWidgetID				inWidget,
								intptr_t				inParam1,
								intptr_t				inParam2);
		
	typedef	std::pair<XPCWidgetAttachment *, bool>	AttachmentInfo;
	typedef	std::vector<AttachmentInfo>				AttachmentVector;
		
		AttachmentVector		mAttachments;
		XPWidgetID				mWidget;
		bool					mOwnsChildren;
		bool					mOwnsWidget;
	
	XPCWidget();							
	XPCWidget(const XPCWidget&);
	XPCWidget& operator=(const XPCWidget&);

};

#endif