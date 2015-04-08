//
//  xplaneConnect.h
//
//  Created by Chris Teubert on 12/2/13.
//

#ifdef __cplusplus
    extern "C" {
#endif

    #ifndef xplaneConnect_h
    #define xplaneConnect_h

    #ifdef _WIN32 /* WIN32 SYSTEM */
        #include <winsock2.h>
        #include <ws2tcpip.h>
        #pragma comment(lib,"ws2_32.lib") //Winsock Library
    #elif (__APPLE__ || __linux)
        #include <sys/socket.h>
        #include <netinet/in.h>
        #include <arpa/inet.h>
        #include <unistd.h>
    #endif
        
    struct xpcSocket {
        short open;
        unsigned short port;
        
        // X-Plane IP and Port
            char xpIP[16];
            unsigned short xpPort;
        
        #ifdef _WIN32 /* WIN32 SYSTEM */
            SOCKET sock;
        #elif (__APPLE__ || __linux) //OS X/Linux
            int sock;
        #endif
    };

	typedef struct
	{
		float pitch;
		float roll;
		float yaw;
		float throttle;
		char gear;
		float flaps;
		char aircraft;
	} xpcCtrl;
        
    // Basic Functions
        struct xpcSocket openUDP(unsigned short port, const char *xpIP, unsigned short xpPort);
        void closeUDP(struct xpcSocket);
        short sendUDP(struct xpcSocket recfd, char my_message[], short messageLength);
        short readUDP(struct xpcSocket recfd, char *dataRef, struct sockaddr *recvaddr);
    
    // Configuration
        short setCONN(struct xpcSocket recfd, unsigned short recPort);
        short pauseSim(struct xpcSocket recfd, short);
        
    // UDP DATA
        short parseDATA(const char my_message[], short messageLength, float dataRef[][9]);
        short readDATA(struct xpcSocket recfd, float dataRef[][9]);
        short sendDATA(struct xpcSocket recfd, float dataRef[][9], unsigned short rows);
        
    // Position
        short parsePOSI(const char my_message[], float resultArray[], int arraySize,  float *gear);
        short readPOSI(struct xpcSocket recfd, float resultArray[], int arraySize, float *gear);
        short sendPOSI(struct xpcSocket recfd, short ACNum, short numArgs, float valueArray[]);
        
    // Controls
        xpcCtrl parseCTRL(const char data[]);
		xpcCtrl readCTRL(struct xpcSocket recfd);
		short sendCTRL(struct xpcSocket recfd, short numArgs, float valueArray[]);
		short sendpCTRL(struct xpcSocket recfd, short numArgs, float valueArray[], char acNum);
    
    // DREF Manipulation
        short readDREF(struct xpcSocket recfd, float *resultArray[], short arraySizes[]);
        short parseDREF(const char my_message[], char *dataRef, unsigned short *length_of_DREF, float *values, unsigned short *number_of_values);
        short sendDREF(struct xpcSocket recfd, const char *dataRef, unsigned short length_of_DREF, float *values, unsigned short number_of_values);
        short requestDREF(struct xpcSocket sendfd, struct xpcSocket recfd, char DREFArray[][100], short DREFSizes[], short listLength,  float *resultArray[], short arraySizes[]);
        int parseGETD(const char my_message[], char *DREFArray[], int DREFSizes[]);
        short parseRequest(const char my_message[], float *resultArray[], short arraySizes[]);
		short readRequest(struct xpcSocket recfd, float *dataRef[], short arraySizes[], struct sockaddr *recvaddr);

	// Screen Text
        short sendTEXT(struct xpcSocket sendfd, char* msg, int x, int y);

        
    #endif //ifdef _h
        
#ifdef __cplusplus
    }
#endif
