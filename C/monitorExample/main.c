// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
//
// DISCLAIMERS
//     No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND,
//     EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT
//     THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF
//     MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY
//     THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED,
//     WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN
//     ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS,
//     HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT
//     SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING
//     THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
//
//     Waiver and Indemnity: RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES
//     GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF
//     RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES
//     OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING
//     FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE
//     UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT,
//     TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE
//     IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.

#include "../src/xplaneConnect.h"
#include <sys/time.h>
#include "stdio.h"

struct timeval tv;

#ifdef WIN32
HANDLE hStdIn = NULL;
INPUT_RECORD buffer;
int waitForInput()
{
	if (hStdIn == NULL)
	{
		hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	}
	FlushConsoleInputBuffer(hStdIn);
	DWORD result = WaitForSingleObject(hStdIn, 100);
	if (result == WAIT_OBJECT_0)
	{
		DWORD eventsRead;
		PeekConsoleInput(hStdIn, &buffer, 1, &eventsRead);
		if (eventsRead > 0)
		{
			return buffer.EventType == KEY_EVENT;
		}
	}
	return FALSE;
}
#else
int fdstdin = 0;
fd_set fds;

int waitForInput()
{

	FD_ZERO(&fds);
	FD_SET(fdstdin, &fds);
	select(1, &fds, NULL, NULL, &tv);
	return FD_ISSET(fdstdin, &fds);
}
#endif

int main(void)
{
	XPCSocket client = openUDP("127.0.0.1");
	const int aircraftNum = 0;
	tv.tv_usec = 100 * 1000;
	while (1)
	{
		double posi[7];
		int result = getPOSI(client, posi, aircraftNum);
		if (result < 0) // Error in getPOSI
		{
			break;
		}

		float ctrl[7];
		result = getCTRL(client, ctrl, aircraftNum);
		if (result < 0) // Error in getCTRL
		{
			break;
		}

		printf("Loc: (%4f, %4f, %4f) Aileron:%2f Elevator:%2f Rudder:%2f\n",
			posi[0], posi[1], posi[2], ctrl[1], ctrl[0], ctrl[2]);

		// Check if any key has been pressed and break
		if (waitForInput())
		{
			break;
		}
	}


	printf("\n\nPress Any Key to exit...");
	getchar();
	return 0;
}
