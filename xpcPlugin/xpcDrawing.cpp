#include "xpcDrawing.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include <stdlib.h>
#include <string.h>

//Internal Memory
static int msgEnabled = 0;
static int msgX = -1;
static int msgY = -1;
static char* msgVal = NULL;
static float rgb[4] = { 0.5F, 1.0F, 0.5F };

//Internal Functions
static int MessageDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void * inRefcon)
{
	if (msgVal)
	{
		XPLMDrawString(rgb, msgX, msgY, msgVal, NULL, xplmFont_Basic);
	}
	return 1;
}

//Public Functions
void XPCClearMessage()
{
	if (msgVal != NULL)
	{
		free(msgVal);
	}
	msgVal = NULL;
	if (msgEnabled)
	{
		XPLMUnregisterDrawCallback(MessageDrawCallback, xplm_Phase_Window, 0, NULL);
		msgEnabled = 0;
	}
}

void XPCSetMessage(int x, int y, char* msg)
{
	//Determine size of message and clear instead if the message string
	//is empty.
	size_t len = strlen(msg);
	if (len == 0)
	{
		XPCClearMessage();
		return;
	}

	//Try to size msgVal to fit the new message. If we fail for any
	//reason, clear the message and bail.
	if (msgVal != NULL)
	{
		msgVal = (char*)realloc(msgVal, len);
	}
	else
	{
		msgVal = (char*)malloc(len);
	}
	if (!msgVal)
	{
		XPCClearMessage();
		return;
	}

	//Set the message, location, and enable drawing if necessary.
	strcpy(msgVal, msg);
	msgX = x < 0 ? 10 : x;
	msgY = y < 0 ? 600 : y;
	if (!msgEnabled)
	{
		XPLMRegisterDrawCallback(MessageDrawCallback, xplm_Phase_LastCockpit, 0, NULL);
		msgEnabled = 1;
	}
}