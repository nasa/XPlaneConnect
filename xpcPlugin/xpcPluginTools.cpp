// xpcPluginTools functions to support xpcPlugin
//
//  FUNCTIONS
//      buildXPLMDataRefs
//      fmini
//      almostequal
//      updateLog
//      handleDREFSIM
//      getIP
//      printBuffertoLog
//      test
//      testi
//
//  CONTACT
//      For questions email Christopher Teubert (christopher.a.teubert@nasa.gov)
//
//	CONTRIBUTORS
//		CT: Christopher Teubert (christopher.a.teubert@nasa.gov)
//
//  TO DO:
//      1. Support sending -1 length to updateLog for it to calculate intself (look for /0)
//      2. Have printbuffertolog run parse function
//      3. Builddatarefs: Fill out & test options
//
//  BEGIN CODE

#include "Log.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "xpcPluginTools.h"

int fmini(int a, int b)
{
    // Returns the minimum value of two integers
    return ((((a)-(b))&0x80000000) >> 31)? (a) : (b);
}

int almostequal(float arg1, float arg2, float tol)
{
    // Compares arg1 and arg 2. If they are within tol returns true
    return (abs(arg1-arg2)<tol);
}

unsigned short getIP(struct sockaddr recvaddr, char *IP)
{
    // Gets the IP Address from sockaddr
	struct sockaddr_in *sendaddr;
	
	sendaddr = (struct sockaddr_in *) &recvaddr;
	sprintf(IP,"%d.%d.%d.%d",(int) (*sendaddr).sin_addr.s_addr&0xFF, (int) ((*sendaddr).sin_addr.s_addr&0xFF00)>>8, (int) ((*sendaddr).sin_addr.s_addr&0xFF0000)>>16, (int) ((*sendaddr).sin_addr.s_addr&0xFF000000)>>24);
	
	return ntohs((*sendaddr).sin_port);
}

// DEBUGGING TOOLS
// --------------------------------

int test(const char *buffer)
{
    // Prints "test buffer" to log (for debugging)
    char buffer2[95] = {0};
    strncpy(buffer2, buffer, 95);
	
	XPC::Log::FormatLine("[TEST] %s", buffer2);
	
	return 0;
}

int test(int buffer)
{
    // Prints "test #[buffer]" to log (for debugging)
	XPC::Log::FormatLine("[TEST] #%i", buffer);
	
	return 0;
}
