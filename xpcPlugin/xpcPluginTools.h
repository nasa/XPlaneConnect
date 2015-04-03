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
#include "XPLMDataAccess.h"
    
    extern XPLMDataRef XPLMDataRefs[134][8];
    extern XPLMDataRef multiplayer[19][8];
    extern XPLMDataRef AIswitch;
    
    struct XPCMessage
    {
        short					connectionID;
        char					head[5];
        char					msg[5000];
        int                     msglen;
        struct sockaddr         recvaddr;
    };
    
    void readMessage(struct xpcSocket * recSocket, struct XPCMessage * pMessage);
    
    void buildXPLMDataRefs(void);
        
    int almostequal(float arg1, float arg2, float tol);
    
    int test(const char *buffer);
    
    int test(int buffer);
    
    int updateLog(const char *buffer, int length);
    
    unsigned short getIP(struct sockaddr recvaddr, char *IP);
    
    int printBufferToLog(struct XPCMessage & msg);
        
    int fmini(int a, int b);
    
    
#endif
