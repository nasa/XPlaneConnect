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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "xpcPluginTools.h"

XPLMDataRef XPLMDataRefs[134][8];
XPLMDataRef multiplayer[20][17];
XPLMDataRef AIswitch;

void readMessage(struct xpcSocket * recSocket, struct XPCMessage * pMessage)
{
    pMessage->msglen = readUDP( *recSocket, pMessage->msg, &(pMessage->recvaddr) );
    
    if ( pMessage->msglen <= 0 ) // No Message
    {
        pMessage->msg[0] = 0;
    }
    else
    {
        strncpy( pMessage->head, pMessage->msg, 4 );
    }
    
    return;
}

void buildXPLMDataRefs()
{
    int i, j;
    char multi[50] = {0};
    
    for (i=0; i<134; i++)
    {
        for (j=0; j<8; j++)
        {
            XPLMDataRefs[i][j] = XPLMFindDataRef("sim/test/test_float");
        }
    }
    
    /* Prefetch the sim variables we will use. */
	//Row 0: Frame Rates
	
	//Row 1: Times
    XPLMDataRefs[1][1] = XPLMFindDataRef("sim/time/total_running_time_sec");
    XPLMDataRefs[1][2] = XPLMFindDataRef("sim/time/total_flight_time_sec");
    XPLMDataRefs[1][3] = XPLMFindDataRef("sim/time/timer_elapsed_time_sec");
	
    //cockpit2/clock_time/zulu_time_hours
    //cockpit2/clock_time/zulu_time_minutes
    //cockpit2/clock_time/zulu_time_seconds
	
	//cockpit2/clock_time/local_time_hours
	
	//cockpit2/clock_time/hobbs_time_hours
	
	//Row 2: Sim Stats
	
	//Row 3: Velocities
    //READ ONLY
    XPLMDataRefs[3][0] = XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed"); //Vind knots indicated airspeed TEST
	
    //Vind knots equivilent airspeed (considering compressible flow)
    XPLMDataRefs[3][2] = XPLMFindDataRef("sim/flightmodel/position/true_airspeed"); //Vtrue knots true airspeed
    XPLMDataRefs[3][3] = XPLMFindDataRef("sim/flightmodel/position/groundspeed"); //Vtrue Knots true ground speed
	
    //XPLMDataRefs[3][5] = Vind mph
    //XPLMDataRefs[3][6] = Vtrue mphas
    //XPLMDataRefs[3][7] = Vtre mphgs
	
	//Row 4: Mach, VVI, G-loads
    XPLMDataRefs[4][0] = XPLMFindDataRef("sim/flightmodel/misc/machno"); // Mach Number
    XPLMDataRefs[4][4] = XPLMFindDataRef("sim/flightmodel2/misc/gforce_normal");
    XPLMDataRefs[4][5] = XPLMFindDataRef("sim/flightmodel2/misc/gforce_axial");
    XPLMDataRefs[4][6] = XPLMFindDataRef("sim/flightmodel2/misc/gforce_side");
	
	//Row 5: Atmosphere: Weather
    XPLMDataRefs[5][0] = XPLMFindDataRef("sim/weather/barometer_sealevel_inhg");
    XPLMDataRefs[5][1] = XPLMFindDataRef("sim/weather/temperature_sealevel_c");
    XPLMDataRefs[5][3] = XPLMFindDataRef("sim/cockpit2/gauges/indicators/wind_speed_kts");
	
	//Row 6: Atmosphere: Aircraft
	
	//Row 7: System Pressures
	
	//Row 8: Joystick
    XPLMDataRefs[8][0] = XPLMFindDataRef("sim/joystick/yoke_pitch_ratio");
    XPLMDataRefs[8][1] = XPLMFindDataRef("sim/joystick/yoke_roll_ratio");
    XPLMDataRefs[8][2] = XPLMFindDataRef("sim/joystick/yoke_heading_ratio");
	
	//Row 9: Other Flight Controls
	
	//Row 10: Art stab ail/elv/rud
	
    //Row 11: Control Surfaces
    XPLMDataRefs[11][0] = XPLMFindDataRef("sim/cockpit2/controls/yoke_pitch_ratio"); //Elevator Position
    XPLMDataRefs[11][1] = XPLMFindDataRef("sim/cockpit2/controls/yoke_roll_ratio"); //Aileron Position
    XPLMDataRefs[11][2] = XPLMFindDataRef("sim/cockpit2/controls/yoke_heading_ratio"); //Rudder Position
    XPLMDataRefs[11][3] = NULL;
    XPLMDataRefs[11][4] = NULL;
    XPLMDataRefs[11][5] = NULL;
    XPLMDataRefs[11][6] = NULL;
    XPLMDataRefs[11][7] = NULL;
    
	//Row 12: Wing Sweep/Trust Vec
	
	//Row 13: trip/flap/slat/s-brakes
    XPLMDataRefs[13][3] = XPLMFindDataRef("sim/flightmodel/controls/flaprqst");
    XPLMDataRefs[13][4] = XPLMFindDataRef("sim/flightmodel/controls/flaprat");// should be equal to flap2rat
	
    //Row 14: Gear, Brakes
    XPLMDataRefs[14][0] = XPLMFindDataRef("sim/aircraft/parts/acf_gear_deploy"); //Landing Gear-SPECIAL (Float[10])
    XPLMDataRefs[14][1] = XPLMFindDataRef("sim/flightmodel/controls/parkbrake"); //Parking Brake
    XPLMDataRefs[14][2] = XPLMFindDataRef("sim/cockpit2/controls/left_brake_ratio");
    XPLMDataRefs[14][3] = XPLMFindDataRef("sim/cockpit2/controls/right_brake_ratio");
	
    XPLMDataRefs[14][7] = XPLMFindDataRef("sim/cockpit/switches/gear_handle_status"); //Landing Gear Handle
    
	//Row 15: MNR (Angular Moments)
    //READ ONLY
    XPLMDataRefs[15][0] = XPLMFindDataRef("sim/flightmodel/position/M");
    XPLMDataRefs[15][1] = XPLMFindDataRef("sim/flightmodel/position/L");
    XPLMDataRefs[15][2] = XPLMFindDataRef("sim/flightmodel/position/N");
	
    //Row 16: PQR (Angular Velocities)
    XPLMDataRefs[16][0] = XPLMFindDataRef("sim/flightmodel/position/Qrad");
    XPLMDataRefs[16][1] = XPLMFindDataRef("sim/flightmodel/position/Prad");
    XPLMDataRefs[16][2] = XPLMFindDataRef("sim/flightmodel/position/Rrad");
    XPLMDataRefs[16][3] = XPLMFindDataRef("sim/flightmodel/position/Q");
    XPLMDataRefs[16][4] = XPLMFindDataRef("sim/flightmodel/position/P");
    XPLMDataRefs[16][5] = XPLMFindDataRef("sim/flightmodel/position/R");
    
    //Row 17: Orientation: pitch, roll, yaw, heading
    XPLMDataRefs[17][0] = XPLMFindDataRef("sim/flightmodel/position/theta"); //Pitch
    XPLMDataRefs[17][1] = XPLMFindDataRef("sim/flightmodel/position/phi"); //roll
    XPLMDataRefs[17][2] = XPLMFindDataRef("sim/flightmodel/position/psi"); //true heading (where nose is pointing)
    
    //READ ONLY (always)
    XPLMDataRefs[17][3] = XPLMFindDataRef("sim/flightmodel/position/magpsi"); //magnetic heading
    
    //WRITABLE
    XPLMDataRefs[17][4] = XPLMFindDataRef("sim/flightmodel/position/q"); //Quartonian
	
	//Row 18: Orientation: alpha beta hpath vpath slip
    XPLMDataRefs[18][0] = XPLMFindDataRef("sim/flightmodel/position/alpha"); //Angle of Attack
    XPLMDataRefs[18][1] = XPLMFindDataRef("sim/cockpit2/gauges/indicators/sideslip_degrees"); //fix to beta
    XPLMDataRefs[18][2] = XPLMFindDataRef("sim/flightmodel/position/hpath"); //Heading the aircraft flies (velocity vector) TEST
    XPLMDataRefs[18][3] = XPLMFindDataRef("sim/flightmodel/position/vpath"); //VPath TEST
    XPLMDataRefs[18][7] = XPLMFindDataRef("sim/cockpit2/gauges/indicators/sideslip_degrees");
	
	//Row 19: Mag Compass
    XPLMDataRefs[19][0] = XPLMFindDataRef("sim/flightmodel/position/magpsi");
	
    //READ ONLY
    XPLMDataRefs[19][1] = XPLMFindDataRef("sim/flightmodel/position/magnetic_variation");
    
    //Row 20: Global Position
    //READ ONLY
    XPLMDataRefs[20][0] = XPLMFindDataRef("sim/flightmodel/position/latitude"); // Read Only
    XPLMDataRefs[20][1] = XPLMFindDataRef("sim/flightmodel/position/longitude"); // Read Only
    XPLMDataRefs[20][3] = XPLMFindDataRef("sim/flightmodel/position/y_agl"); // Read Only
    
    //Row 21: Local Position, Velocity
    XPLMDataRefs[21][0] = XPLMFindDataRef("sim/flightmodel/position/local_x");
    XPLMDataRefs[21][1] = XPLMFindDataRef("sim/flightmodel/position/local_y");
    XPLMDataRefs[21][2] = XPLMFindDataRef("sim/flightmodel/position/local_z");
    XPLMDataRefs[21][3] = XPLMFindDataRef("sim/flightmodel/position/local_vx");
    XPLMDataRefs[21][4] = XPLMFindDataRef("sim/flightmodel/position/local_vy");
    XPLMDataRefs[21][5] = XPLMFindDataRef("sim/flightmodel/position/local_vz");
    
	//Row 22: All Planes: Lat (READ ONLY)
    XPLMDataRefs[22][0] = XPLMDataRefs[20][0];
    XPLMDataRefs[22][1] = XPLMFindDataRef("sim/multiplayer/position/plane1_lat");
    XPLMDataRefs[22][2] = XPLMFindDataRef("sim/multiplayer/position/plane2_lat");
    XPLMDataRefs[22][3] = XPLMFindDataRef("sim/multiplayer/position/plane3_lat");
    XPLMDataRefs[22][4] = XPLMFindDataRef("sim/multiplayer/position/plane4_lat");
    XPLMDataRefs[22][5] = XPLMFindDataRef("sim/multiplayer/position/plane5_lat");
    XPLMDataRefs[22][6] = XPLMFindDataRef("sim/multiplayer/position/plane6_lat");
    XPLMDataRefs[22][7] = XPLMFindDataRef("sim/multiplayer/position/plane7_lat");
	
	//Row 23: All Planes: Lon
    XPLMDataRefs[23][0] = XPLMDataRefs[20][1];
    XPLMDataRefs[23][1] = XPLMFindDataRef("sim/multiplayer/position/plane1_lon");
    XPLMDataRefs[23][2] = XPLMFindDataRef("sim/multiplayer/position/plane2_lon");
    XPLMDataRefs[23][3] = XPLMFindDataRef("sim/multiplayer/position/plane3_lon");
    XPLMDataRefs[23][4] = XPLMFindDataRef("sim/multiplayer/position/plane4_lon");
    XPLMDataRefs[23][5] = XPLMFindDataRef("sim/multiplayer/position/plane5_lon");
    XPLMDataRefs[23][6] = XPLMFindDataRef("sim/multiplayer/position/plane6_lon");
    XPLMDataRefs[23][7] = XPLMFindDataRef("sim/multiplayer/position/plane7_lon");
	
	//Row 24: All Planes: Alt
    XPLMDataRefs[24][0] = XPLMDataRefs[20][2];
    XPLMDataRefs[24][1] = XPLMFindDataRef("sim/multiplayer/position/plane1_el");
    XPLMDataRefs[24][2] = XPLMFindDataRef("sim/multiplayer/position/plane2_el");
    XPLMDataRefs[24][3] = XPLMFindDataRef("sim/multiplayer/position/plane3_el");
    XPLMDataRefs[24][4] = XPLMFindDataRef("sim/multiplayer/position/plane4_el");
    XPLMDataRefs[24][5] = XPLMFindDataRef("sim/multiplayer/position/plane5_el");
    XPLMDataRefs[24][6] = XPLMFindDataRef("sim/multiplayer/position/plane6_el");
    XPLMDataRefs[24][7] = XPLMFindDataRef("sim/multiplayer/position/plane7_el");
	
    //Row 25: Throttle Command
    XPLMDataRefs[25][0] = XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro"); //Throttle (array 8)
	
	//Row 26: Throttle Actual
    XPLMDataRefs[26][0] = XPLMFindDataRef("sim/flightmodel2/engines/throttle_used_ratio"); // Trottle Actual (array 8) (Read Only)
	
	//Row 27:
    
    
    // Multiplayer
    for ( i = 1; i < 20; i++ )
    {
		sprintf(multi, "sim/multiplayer/position/plane%i_x", i); // X
        multiplayer[i][0] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_y", i); // Y
        multiplayer[i][1] = XPLMFindDataRef(multi);
        sprintf(multi,"sim/multiplayer/position/plane%i_z", i); // Z
        multiplayer[i][2] = XPLMFindDataRef(multi);
        sprintf(multi,"sim/multiplayer/position/plane%i_the", i); // Theta (Pitch)
        multiplayer[i][3] = XPLMFindDataRef(multi);
        sprintf(multi,"sim/multiplayer/position/plane%i_phi", i); // Phi (Roll)
        multiplayer[i][4] = XPLMFindDataRef(multi);
        sprintf(multi,"sim/multiplayer/position/plane%i_psi", i); // Psi (Heading-True)
		multiplayer[i][5] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_gear_deploy", i); // Landing Gear
		multiplayer[i][6] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_flap_ratio", i);
		multiplayer[i][7] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_flap_ratio2", i);
		multiplayer[i][8] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_spoiler_ratio", i);
		multiplayer[i][9] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_speedbrake_ratio", i);
		multiplayer[i][10] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_slat_ratio", i);
		multiplayer[i][11] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_wing_sweep", i);
		multiplayer[i][12] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_throttle", i);
		multiplayer[i][13] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_yolk_pitch", i);
		multiplayer[i][14] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_yolk_roll", i);
		multiplayer[i][15] = XPLMFindDataRef(multi);
		sprintf(multi, "sim/multiplayer/position/plane%i_yolk_yaw", i);
		multiplayer[i][16] = XPLMFindDataRef(multi);
    }
    AIswitch = XPLMFindDataRef("sim/operation/override/override_plane_ai_autopilot");
}

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

int updateLog(const char *buffer, int length)
{
    // Writes buffer to logfile (xpcLog.txt)
    
	time_t rawtime;
	struct tm * timeinfo;
	char logBuffer[523] = { 0 };
	FILE * logFile;
	
	logFile = fopen("xpcLog.txt","a");
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	// Format is equivalent to [%F %T], but neither of those specifiers is
	// supported on Windows as of Visual Studio 13
	strftime(logBuffer, 523, "[%Y-%m-%d %H:%M:%S] ", timeinfo);
	
	length = length < 500 ? length : 500;
	memcpy(&(logBuffer[22]), buffer, length);
	
	fprintf(logFile,"%s\n",logBuffer);
	
	fclose(logFile);
	
	return 1;
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
int printBufferToLog(struct XPCMessage & msg)
{
    // Prints the entire buffer to the log (for debugging)
	char logmsg[350] = "[DEBUG]";
	int i;
    
    for (i=0;i<msg.msglen && strlen(logmsg)<(sizeof(logmsg)-4);i++)
    {
        sprintf(logmsg,"%s %hhu",logmsg,msg.msg[i]);
    }
    
    updateLog(logmsg,strlen(logmsg));
    memset(logmsg,0,strlen(logmsg));
    
    sprintf(logmsg,"[%s-DEBUG](%i)",msg.head,msg.msg[4]);
	
	//Switch for header
    if (strncmp(msg.head,"CONN",4)==0)
    {// Header = CONN (Connection)
        updateLog(logmsg,strlen(logmsg));
        
    }
    else if (strncmp(msg.head,"SIMU",4)==0)
    {// Header = SIMU
        sprintf(logmsg,"%s %i",logmsg,msg.msg[5]);
        updateLog(logmsg,strlen(logmsg));
        
    }
    else if (strncmp(msg.head,"POSI",4)==0)
    {// Header = POSI (Position)
        float position[8] = {0.0};
        float pos[3],orient[3];
        short aircraft = 0;
        float gear = -1.0;
        
        // UPDATE LOG
        aircraft = parsePOSI(msg.msg,position,6, &gear);
        
        //ADD AIRCRAFT HANDLING
        
        // Position
		memcpy(pos,position,3*sizeof(float));
        
        // Orientation
		memcpy(orient,&position[3],3*sizeof(float));
        
        sprintf(logmsg,"%s %i (%f %f %f) (%f %f %f) %f",logmsg,aircraft,pos[0],pos[1],pos[2],orient[0],orient[1],orient[2],gear);
        
        updateLog(logmsg,strlen(logmsg));
    }
    else if (strncmp(msg.head,"CTRL",4)==0)
    {// Header = CTRL (Control)
        xpcCtrl ctrl = parseCTRL(msg.msg);
        
        sprintf(logmsg,"%s (%f %f %f) %f %hhi %f",logmsg, ctrl.pitch, ctrl.roll, ctrl.yaw, ctrl.throttle, ctrl.gear, ctrl.flaps);
        
        updateLog(logmsg,strlen(logmsg));
        
    }
    else if (strncmp(msg.head,"WYPT",4)==0)
    {// Header = WYPT (Waypoint Draw)
        
        updateLog(logmsg,strlen(logmsg));
        
    }
    else if (strncmp(msg.head,"GETD",4)==0)
    {// Header = GETD (Data Request)
        char DREF[100];
        int counter = 6;
        
        updateLog(logmsg,strlen(logmsg));
        memset(logmsg,0,strlen(logmsg));
        
        for (i=0;i<msg.msg[5];i++)
        {
            memcpy(DREF,&msg.msg[counter+1],msg.msg[counter]);
            sprintf(logmsg,"\t#%i/%i (size:%i) %s",i+1,msg.msg[5],msg.msg[counter],DREF);
            
            updateLog(logmsg,strlen(logmsg));
            memset(logmsg,0,strlen(logmsg));
            memset(DREF,0,sizeof(DREF));
            
            counter += msg.msg[counter]+1;
        }
    }
    else if (strncmp(msg.head,"DREF",4)==0)
    {// Header = DREF (By Data Ref) (this is slower than DATA)
        char DREF[100]={0};
        float tmp;
        updateLog(logmsg,strlen(logmsg));
        memset(logmsg,0,strlen(logmsg));
        
        memcpy(DREF,&msg.msg[6],msg.msg[5]);
        sprintf(logmsg,"-\tDREF (size %i)= %s",msg.msg[5],DREF);
        updateLog(logmsg,strlen(logmsg));
        memset(logmsg,0,strlen(logmsg));
        sprintf(logmsg,"-\tValues(Size %i)=",msg.msg[6+msg.msg[5]]);
        for (i=0;i<msg.msg[6+msg.msg[5]];i++)
        {
            memcpy(&tmp,&msg.msg[7+msg.msg[5]+sizeof(float)*i],sizeof(float));
            sprintf(logmsg,"%s %f",logmsg,tmp);
        }
        updateLog(logmsg,strlen(logmsg));
    }
    else if (strncmp(msg.head,"VIEW",4)==0)
    {// Header = VIEW (Change View)
        updateLog(logmsg,strlen(logmsg));
    }
    else if (strncmp(msg.head,"DATA",4)==0)
    {// Header = DATA (UDP Data)
        int j;
        short totalColumns = ((msg.msglen-5)/36);
        float dataRef[30][9];
        
        sprintf(logmsg,"%s (%i lines)",logmsg,totalColumns);
        updateLog(logmsg,strlen(logmsg));
        memset(logmsg,0,strlen(logmsg));
        
        totalColumns = parseDATA(msg.msg, msg.msg[4], dataRef);
        
        for (i=0; i<totalColumns; i++)
        {
            sprintf(logmsg,"\t#%i: ",(int) dataRef[i][0]);
            
            for (j=1; j<9; j++)
            {
                sprintf(logmsg,"%s %f",logmsg,dataRef[i][j]);
                if (dataRef[i][j]!=dataRef[i][j]) sprintf(logmsg,"%s (not a number)",logmsg);
            }
            updateLog(logmsg,strlen(logmsg));
            memset(logmsg,0,strlen(logmsg));
        }
    }
    else
    {
        return -1; //unrecognized header
    }
    
	return 0;
}

int test(const char *buffer)
{
    // Prints "test buffer" to log (for debugging)
	char logmsg[100] = {0};
    char buffer2[95] = {0};
    strncpy(buffer2, buffer, 95);
	sprintf(logmsg,"[TEST] %s",buffer2);
	
	updateLog(logmsg,strlen(logmsg));
	
	return 0;
}

int test(int buffer)
{
    // Prints "test #[buffer]" to log (for debugging)
	char logmsg[100] = {0};
	sprintf(logmsg,"[TEST] #%i",buffer);
	
	updateLog(logmsg,strlen(logmsg));
	
	return 0;
}
