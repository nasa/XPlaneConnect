//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
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

#include "chrome.h"

#include <stdio.h>

void displayStart(char* version)
{
	printf("X-Plane Connect Playback Example  [Version %s]\n", version);
	printf("(c) 2013-2015 United States Government as represented by the Administrator\n");
	printf("of the National Aeronautics and Space Administration. All Rights Reserved.\n");
}

int displayMenu(char* title, char* opts[], size_t count)
{
	printf("\n");
	printf("+---------------------------------------------- +\n");
	printf("|  %-42s   |\n", title);
	printf("+---------------------------------------------- +\n");
	for (size_t i = 0; i < count; i++)
	{
		printf("| %2u. %-40s  |\n", i + 1, opts[i]);
	}
	printf("+---------------------------------------------- +\n");
	printf("Please select an option: ");

	int opt;
	scanf("%d", &opt);
	return opt;
}

void displayMsg(char* msg)
{
	printf("%s\n", msg);
}

void getString(char* prompt, char buffer[255])
{
	printf("%s: ", prompt);
	scanf("%255s", buffer);
}

int getInt(char* prompt)
{
	printf("%s: ", prompt);
	int result;
	scanf("%d", &result);
	return result;
}