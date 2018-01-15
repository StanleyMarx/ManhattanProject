#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> // -lpthreads during compilation
#include <stdarg.h>
#include <time.h>
#include <sys/socket.h>
#include <math.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

#include "ev3.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include "ev3_sensor.h"
#include "jb.h"

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__

#include <windows.h>

// UNIX //////////////////////////////////////////
#else

#include <unistd.h>
#define Sleep( msec ) usleep(( msec ) * 1000 )

//////////////////////////////////////////////////
#endif

////////////////// CONSTANTS ///////////////////////////

// CONSTANTS FOR COMMUNICATING WITH SERVER
#define SERV_ADDR "30:3a:64:ea:bf:0e" // might change

#define TEAM_ID 12

#define MSG_ACK 0
#define MSG_START 1
#define MSG_STOP 2
#define MSG_KICK 3
#define MSG_POSITION 4
#define MSG_MAPDATA 5
#define MSG_MAPDONE 6
#define Sleep( msec ) usleep(( msec ) * 1000 )


///////////////////////////////////// MAIN ///////////////////////////////////

int main(void) {

	//char s[ 256 ];
	//int val;
	//float value;
	float gyroVal;
	float compassVal;
	float sonarVal;
	// for Bluetooth communication
	struct sockaddr_rc addr = { 0 };
	int status;
	pthread_t sendPosition;
	pthread_create(&sendPosition,NULL,sendPosToServer,NULL);
    	
#ifndef __ARM_ARCH_4T__
	/* Disable auto-detection of the brick (you have to set the correct address below) */
	ev3_brick_addr = "192.168.0.204";

#endif
	if ( ev3_init() == -1 ) return ( 1 );

#ifndef __ARM_ARCH_4T__
	printf( "The EV3 brick auto-detection is DISABLED,\nwaiting %s online with plugged tacho...\n", ev3_brick_addr );

#else
	printf( "Waiting tacho is plugged...\n" );

#endif
	while ( ev3_tacho_init() < 1 ) Sleep( 1000 );

	printf( "*** ( EV3 ) Hello! ***\n" );
	
	// FIND ALL THREE MOTORS
	sn_left = findLeftMotor();	
	sn_right = findRightMotor();
	sn_shovel = findShovelMotor();
	printf("%d %d %d\n", sn_left, sn_right, sn_shovel);
	
	// GETS READY TO READ SENSORS
	ev3_sensor_init();

	// FIND SENSORS
	findSensors();
	int nbrSensors;
	nbrSensors = testSensors();
	// check if nbrSensors == 3 ?
	
	// CONNECTION TO SERVER	
	/* allocate a socket */
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	
	/* set the connection parameters (who to connect to) */
	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t) 1;
	str2ba (SERV_ADDR, &addr.rc_bdaddr);

	/* connect to server */
	status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

	// SHOULD NOW BE CONNECTED TO SERVER WITH SENSORS AND MOTORS READY TO GO
	/* if connected */
    	if( status == 0 ) {
	       	char string[58];
	    	/* Wait for START message */
		read_from_server (s, string, 9);
		if (string[4] == MSG_START) {
		    printf ("Received start message!\n");
		}
       		/* DO WHAT YOU HAVE TO DO */
		
		int j = 0; 
		int side;
		while(true){
			forwardSonar(100.0);
			side = rand() % 2;
			if (side==0) {
				turnRight();
			} else {
				turnLeft();
			}
			j = j+1;
		}
		
		//updatePos(); // changes X and Y
        	/* debut SC2 */
		printf("[INFO] locking display\n");
	        pthread_mutex_lock(&lock);
		printf("[INFO] changing Xdef and Ydef\n");
        	Xdef=X;
        	Ydef=Y;
		printf("[INFO] unlocking display\n");
        	pthread_mutex_unlock(&lock);
        	/* fin SC2 */

        	pthread_join(display,NULL);
        	pthread_mutex_destroy(&lock);
        
        	// TEST MOTORS
		//forwardTimed(2);
		//forwardSonar(100.0);
		//size_t shovelPosUp = initializeShovel();
		//printf("[SHOVEL] shovelPosUp: %d\n", shovelPosUp);
		/*printf("turning right\n");
		turnRight();
		forwardTimed(2);
		turnLeft();
		forwardTimed(2);
		*/
		//robot();
    	} else { /* unable to connect to server! */
        	fprintf (stderr, "Failed to connect to server...\n");
        	sleep (2);
        	exit (EXIT_FAILURE);
    	}

	// ENDS MAIN
	close(s); //closes connection w/ server
	ev3_uninit();
	printf( "*** ( EV3 ) Bye! ***\n" );
	return ( 0 );
}
