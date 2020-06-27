//Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
//
//DISCLAIMERS
//    No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND,
//    EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT
//    THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF
//    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY
//    THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED,
//    WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN
//    ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS,
//    HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS RESULTING FROM USE OF THE SUBJECT
//    SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING
//    THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
//
//    Waiver and Indemnity: RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES
//    GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF
//    RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES
//    OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING
//    FROM, RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE
//    UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT,
//    TO THE EXTENT PERMITTED BY LAW.  RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE
//    IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.

#include "playback.h"

#include "chrome.h"

#include "xplaneConnect.h"

#include <stdio.h>
#include <time.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

void playbackSleep(int ms)
{
#ifdef WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

void record(char* path, int interval, int duration)
{
	FILE* fd = fopen(path, "w");
	int count = duration * 1000 / interval;
	if (!fd)
	{
		displayMsg("Unable to open output file.");
		return;
	}
	if (count < 1)
	{
		displayMsg("Duration is less than one iteration.");
		return;
	}
	displayMsg("Recording...");

	XPCSocket sock = openUDP("127.0.0.1");
	for (int i = 0; i < count; ++i)
	{
		double posi[7];
		int result = getPOSI(sock, posi, 0);
		playbackSleep(interval);
		if (result < 0)
		{
			continue;
		}
		fprintf(fd, "%f, %f, %f, %f, %f, %f, %f\n", posi[0], posi[1], posi[2], posi[3], posi[4], posi[5], posi[6]);
	}
	closeUDP(sock);
	displayMsg("Recording Complete");
}

void playback(char* path, int interval)
{
	FILE* fd = fopen(path, "r");
	if (!fd)
	{
		displayMsg("Unable to open output file.");
		return;
	}
	displayMsg("Starting Playback...");

	XPCSocket sock = openUDP("127.0.0.1");
	double posi[7];
	while (!feof(fd) && !ferror(fd))
	{
		int result = fscanf(fd, "%lf, %lf, %lf, %lf, %lf, %lf, %lf\n",
			&posi[0], &posi[1], &posi[2], &posi[3], &posi[4], &posi[5], &posi[6]);
		playbackSleep(interval);
		if (result != 7)
		{
			continue;
		}
		sendPOSI(sock, posi, 7, 0);
	}
	closeUDP(sock);
	displayMsg("Playback Complete");
}
