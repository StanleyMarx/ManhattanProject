#include <stdio.h>
#include <stdlib.h>
#include "ev3.h"
//#include "ev3c.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include "ev3_sensor.h"

// WIN32 /////////////////////////////////////////
#ifdef __WIN32__

#include <windows.h>

// UNIX //////////////////////////////////////////
#else

#include <unistd.h>
#define Sleep( msec ) usleep(( msec ) * 1000 )

//////////////////////////////////////////////////
#endif

const int LEFT_PORT = 68;// port D
const int RIGHT_PORT = 66; //port B
const int PELLE_PORT = 67; // port C

uint8_t sn_left;
uint8_t sn_right;
uint8_t sn_pelle;


uint8_t findLeftMotor() {
	// find left motor	
	if ( ev3_search_tacho_plugged_in(LEFT_PORT,0, &sn_left, 0 )) {
		printf( "LEGO_EV3_M_MOTOR LEFT is found on port %d, for sn=%d\n", LEFT_PORT, sn_left);
		set_tacho_command(sn_left, "reset");
		return sn_left;
	} else {
		printf( "LEGO_EV3_M_MOTOR LEFT is NOT found\n" );
		return -1;
	}
}

uint8_t findRightMotor() {
	// find right motor	
	if ( ev3_search_tacho_plugged_in(RIGHT_PORT,0, &sn_right, 0 )) {
		printf( "LEGO_EV3_M_MOTOR RIGHT is found on port %d, for sn=%d\n", RIGHT_PORT, sn_right);
		set_tacho_command(sn_right, "reset");
		return sn_right;
	} else {
		printf( "LEGO_EV3_M_MOTOR RIGHT is NOT found\n" );
		exit(-1);
	}
}

uint8_t findPelleMotor() {
	// find pelle motor	
	if ( ev3_search_tacho_plugged_in(PELLE_PORT,0, &sn_pelle, 0 )) {
		printf( "LEGO_EV3_M_MOTOR PELLE is found on port %d, for sn=%d\n", PELLE_PORT, sn_pelle);
		set_tacho_command(sn_pelle, "reset");
		return sn_pelle;
	} else {
		printf( "LEGO_EV3_M_MOTOR PELLE is NOT found\n" );
		exit(-1);
	}
}


///////////////////////////////////// MMMMAAAAIIIIINNNNNN ///////////////////////////////////

int main(void) {
	
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
	sn_pelle = findPelleMotor();
	printf("%d %d %d\n", sn_left, sn_right, sn_pelle);
	
	// GETS READY TO READ SENSORS
	ev3_sensor_init();

	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
	set_tacho_command(sn_pelle, "stop");

	// ENDS MAIN
	ev3_uninit();
	printf( "*** ( EV3 ) Bye! ***\n" );
	return ( 0 );
}
