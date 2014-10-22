#include "XPCWidget.h"

XPCWidget::XPCWidget(
		int						inLeft,
		int						inTop,
		int						inRight,
		int						inBottom,
		bool					inVisible,
		const char *			inDescriptor,
		bool					inIsRoot,
		XPWidgetID				inParent,
		XPWidgetClass			inClass) :
	mWidget(NULL),
	mOwnsChildren(false),
	mOwnsWidget(true)
{
	mWidget = XPCreateWidget(
		inLeft, inTop, inRight, inBottom,
		inVisible ? 1 : 0,
		inDescriptor,
		inIsRoot ? 1 : 0,
		inIsRoot ? NULL : inParent,
		inClass);

	XPSetWidgetProperty(mWidget, xpProperty_Object, reinterpret_cast<long>(this));		
	XPAddWidgetCallback(mWidget, WidgetCallback);
}								
						
XPCWidget::XPCWidget(
	XPWidgetID				inWidget,
	bool					inOwnsWidget) :
	mWidget(inWidget),
	mOwnsChildren(false),
	mOwnsWidget(inOwnsWidget)
{
	XPSetWidgetProperty(mWidget, xpProperty_Object, reinterpret_cast<long>(this));		
	XPAddWidgetCallback(mWidget, WidgetCallback);
}

XPCWidget::~XPCWidget()
{
	if (mOwnsWidget)
		XPDestroyWidget(mWidget, mOwnsChildren ? 1 : 0);
}
	
void		XPCWidget::SetOwnsWidget(
					bool 					inOwnsWidget)
{
	mOwnsWidget = inOwnsWidget;
}

void		XPCWidget::SetOwnsChildren(
					bool 					inOwnsChildren)
{
	mOwnsChildren = inOwnsChildren;
}					

XPCWidget::operator XPWidgetID () const
{
	return mWidget;
}

XPWidgetID XPCWidget::Get(void) const
{
	return mWidget;
}

void		XPCWidget::AddAttachment(
								XPCWidgetAttachment * 	inAttachment, 
								bool 					inOwnsAttachment,
								bool					inPrefilter)
{
	if (inPrefilter)
	{
		mAttachments.insert(mAttachments.begin(), AttachmentInfo(inAttachment, inOwnsAttachment));
	} else {
		mAttachments.push_back(AttachmentInfo(inAttachment, inOwnsAttachment));
	}
}								

void		XPCWidget::RemoveAttachment(
								XPCWidgetAttachment * 	inAttachment)
{
	for (AttachmentVector::iterator iter = mAttachments.begin();
			iter != mAttachments.end(); ++iter)
	{
		if (iter->first == inAttachment)
		{
			mAttachments.erase(iter);
			return;
		}
	}
}								

int			XPCWidget::HandleWidgetMessage(
								XPWidgetMessage			inMessage,
								XPWidgetID				inWidget,
								long					inParam1,
								long					inParam2)
{
	return 0;
}								
		
int			XPCWidget::WidgetCallback(
								XPWidgetMessage			inMessage,
								XPWidgetID				inWidget,
								long					inParam1,
								long					inParam2)
{
	XPCWidget * me = reinterpret_cast<XPCWidget *>(XPGetWidgetProperty(inWidget, xpProperty_Object, NULL));
	if (me == NULL)
		return 0;
	
	for (AttachmentVector::iterator iter = me->mAttachments.begin(); iter != 
		me->mAttachments.end(); ++iter)
	{
		int result = iter->first->HandleWidgetMessage(me, inMessage, inWidget, inParam1, inParam2);
		if (result != 0)
			return result;
	}

	return me->HandleWidgetMessage(inMessage, inWidget, inParam1, inParam2);
}								
