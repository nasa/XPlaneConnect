#include "xpcDrawing.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include <stdlib.h>
#include <string.h>

//Internal Memory
static bool msgEnabled = false;
static int msgX = -1;
static int msgY = -1;
static char msgVal[256] = { 0 };
static size_t newLineCount = 0;
static size_t newLines[64] = { 0 };
static float rgb[3] = { 0.25F, 1.0F, 0.25F };

//Internal Functions
static int MessageDrawCallback(XPLMDrawingPhase inPhase, int inIsBefore, void * inRefcon)
{
	XPLMDrawString(rgb, msgX, msgY, msgVal, NULL, xplmFont_Basic);
	int y = msgY - 16;
	for (size_t i = 0; i < newLineCount; ++i)
	{
		XPLMDrawString(rgb, msgX, y, msgVal + newLines[i], NULL, xplmFont_Basic);
		y -= 16;
	}
	return 1;
}

//Public Functions
void XPCClearMessage()
{
	XPLMUnregisterDrawCallback(MessageDrawCallback, xplm_Phase_Window, 0, NULL);
	msgEnabled = false;
}

void XPCSetMessage(int x, int y, char* msg)
{
	//Determine size of message and clear instead if the message string
	//is empty.
	size_t len = strnlen(msg, 255);
	if (len == 0)
	{
		XPCClearMessage();
		return;
	}

	//Set the message, location, and mark new lines.
	strncpy(msgVal, msg, len);
	newLineCount = 0;
	for (size_t i = 0; i < len && newLineCount < 64; ++i)
	{
		if (msgVal[i] == '\n' || msgVal[i] == '\r')
		{
			msgVal[i] = 0;
			newLines[newLineCount++] = i + 1;
		}
	}
	msgX = x < 0 ? 10 : x;
	msgY = y < 0 ? 600 : y;

	//Enable drawing if necessary
	if (!msgEnabled)
	{
		XPLMRegisterDrawCallback(MessageDrawCallback, xplm_Phase_LastCockpit, 0, NULL);
		msgEnabled = true;
	}
}