//
//  main.cpp
//  xpcExample
//
//  Created by Chris Teubert on 3/27/14.
//  Copyright (c) 2014 Chris Teubert. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "xplaneConnect.h"
#ifdef WIN32
#include <Windows.h>
#define sleep(n) Sleep(n * 1000)
#endif

int main() {
	int i, j;
	struct xpcSocket sendfd, readfd;
	float data[4][9] = { 0 };
	char DREF[100] = { 0 };
	char DREFArray[100][100];
	char DREFArray2[100][100];
	short DREFSizes[100];
	float *recDATA[100];
	float POSI[9] = { 0.0 };
	float CTRL[5] = { 0.0 };
	float gear;
	char IP[16] = "127.0.0.1"; //IP Address of computer running X-Plane
	short PORT = 49009;        //xpcPlugin Receiving port (usually 49009)

	printf("xplaneconnect Example Script\n- Setting up Simulation\n");

	for (i = 0; i < 100; i++) {
		recDATA[i] = (float *)malloc(40 * sizeof(float));
		memset(DREFArray[i], 0, 100);
		memset(DREFArray2[i], 0, 100);
	}

	// Open Sockets
	readfd = openUDP(49055, IP, PORT); //Open socket for receiving
	sendfd = openUDP(49077, IP, PORT); //Open socket for sending

	// Set up Connection
	setCONN(sendfd, 49055); // Setup so data will be received on port 49055

	// Set Location/Orientation (sendPOSI)
	// Set Up Position Array
	POSI[0] = 37.524;     // Lat
	POSI[1] = -122.06899; // Lon
	POSI[2] = 2500;       // Alt
	POSI[3] = 0;          // Pitch
	POSI[4] = 0;          // Roll
	POSI[5] = 0;          // Heading
	POSI[6] = 1;          // Gear

	sendPOSI(sendfd, 0, 7, POSI);
	POSI[0] = 37.52465;   // Move a second aircraft a bit North of us
	POSI[4] = 20;         // Give that aircraft a bit or right roll
	sendPOSI(sendfd, 1, 7, POSI);

	// Set Rates (sendDATA)

	for (i = 0; i < 4; i++) { // Set array to -999
		for (j = 0; j < 9; j++) data[i][j] = -999;
	}
	// Set up Data Array (first item in row is item number (example: 20=position)
	data[0][0] = 18; // Alpha
	data[0][1] = 0;
	data[0][3] = 0;

	data[1][0] = 3;//21;  //Velocity
	data[1][1] = 130;
	data[1][2] = 130;
	data[1][3] = 130;
	data[1][4] = 130;

	data[2][0] = 16; //PQR
	data[2][1] = 0;
	data[2][2] = 0;
	data[2][3] = 0;

	sendDATA(sendfd, data, 3); // Velocity/Alpha/PQR

	// Set CTRL
	CTRL[3] = 0.8; // Throttle

	sendCTRL(sendfd, 4, CTRL);

	// pauseSim
	pauseSim(sendfd, 1); // Sending 1 to pause

	// Pause for 5 seconds
	sleep(5);

	// Unpause
	pauseSim(sendfd, 0); // Sending 0 to unpause
	printf("- Resuming Simulation\n");

	// Simulate for 10 seconds
	sleep(10);

	// SendDREF (Landing Gear)
	printf("- Stowing Landing Gear\n");

	strcpy(DREF, "sim/cockpit/switches/gear_handle_status"); // Gear handle data reference
	DREFSizes[0] = strlen(DREF);
	gear = 1; // Stow gear

	sendDREF(sendfd, DREF, DREFSizes[0], &gear, 1); // Set gear to stow

	// Simulate for 10 seconds
	sleep(10);

	// Check Landing gear, Pause
	printf("- Confirming Gear Status\n");
	strcpy(DREFArray2[0], "sim/cockpit/switches/gear_handle_status");
	strcpy(DREFArray2[1], "sim/operation/override/override_planepath");
	for (i = 0; i < 2; i++) {
		DREFSizes[i] = (int)strlen(DREFArray2[i]);
	}
	requestDREF(sendfd, readfd, DREFArray2, DREFSizes, 2, recDATA, DREFSizes); // Request 2 values

	if (*(recDATA[0]) == 0) {
		printf("\tGear Stowed\n");
	}
	else {
		printf("\tERROR: Gear Stowage unsuccessful\n");
	}

	printf("---End Program---\n");

	return 0;
}
