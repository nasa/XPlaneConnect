//
//  xpcPluginTools.pch
//  xpcPlugin
//
//  Created by Chris Teubert on 4/14/14.
//
//
    
#ifndef xpcPlugin_xpcPluginTools_h
#define xpcPlugin_xpcPluginTools_h
    
#include <time.h>
#include "xplaneConnect.h"
        
    int almostequal(float arg1, float arg2, float tol);
    
    int test(const char *buffer);
    
    int test(int buffer);
    
    unsigned short getIP(struct sockaddr recvaddr, char *IP);
        
    int fmini(int a, int b);
    
    
#endif
