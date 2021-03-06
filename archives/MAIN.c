#include <stdio.h>
#include <stdlib.h>
#include "ev3.h"
//#include "ev3c.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include "ev3_sensor.h"

#include "turns.h"
#include "avancer.h"
//#include "avancer_sonar.h"
//#include "test-gyro.h"
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

////////////////// INITIALISATION ///////////////////////

void findSensors(uint8_t sn_compass, uint8_t sn_sonar, uint8_t sn_gyro) {
	// from the OS GitHub, modified by JB
	int i, val;
	uint32_t n, ii;
	float value;
	char s[ 256 ];
	printf( "Found sensors:\n" );
	for ( i = 0; i < DESC_LIMIT; i++ ) {
		if ( ev3_sensor[ i ].type_inx != SENSOR_TYPE__NONE_ ) {
			printf( "  type = %s\n", ev3_sensor_type( ev3_sensor[ i ].type_inx ));
			printf( "  port = %s\n", ev3_sensor_port_name( i, s ));
			if ( get_sensor_mode( i, s, sizeof( s ))) {
				printf( "  mode = %s\n", s );
			}
			if ( get_sensor_num_values( i, &n )) {
				for ( ii = 0; ii < n; ii++ ) {
					if ( get_sensor_value( ii, i, &val )) {
						printf( "  value%d = %d\n", ii, val );
					}
				}
			}
		}
	}
    if (ev3_search_sensor(HT_NXT_COMPASS, &sn_compass,0)){
		printf("COMPASS found, reading angle...\n");
	 	if ( !get_sensor_value0(sn_compass, &value )) {
			value = 0;
		}
		printf( "\r(%f) \n", value);
		fflush( stdout );
    }
	if (ev3_search_sensor(LEGO_EV3_US, &sn_sonar,0)){
		printf("SONAR found, reading distance...\n");
		if ( !get_sensor_value0(sn_sonar, &value )) {
			value = 0;
		}
		printf( "\r(%f) \n", value);
		fflush( stdout );
    }
	/*if (ev3_search_sensor(NXT_ANALOG, &sn_mag,0)){
		printf("Magnetic sensor found, reading magnet...\n");
		if ( !get_sensor_value0(sn_mag, &value )) {
			value = 0;
		}
		printf( "\r(%f) \n", value);
		fflush( stdout );
    }*/
    if (ev3_search_sensor(LEGO_EV3_GYRO, &sn_gyro,0)){
		printf("GYRO found, reading angle...\n");
		if ( !get_sensor_value0(sn_gyro, &value )) {
			value = 0;
		}
		printf( "\r(%f) \n", value);
		fflush( stdout );
    }

}

uint8_t findLeftMotor(uint8_t sn_left) {
	// from the OS GitHub, modified by JB
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

uint8_t findRightMotor(uint8_t sn_right) {
	// from the OS GitHub, modified by JB
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

uint8_t findPelleMotor(uint8_t sn_pelle) {
	// from the OS GitHub, modified by JB
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

//////////////////////// SENSORS /////////////////////

float getGyro(uint8_t sn_gyro) {
	// from the OS GitHub, modified by JB
	float value;
	if (ev3_search_sensor(LEGO_EV3_GYRO, &sn_gyro,0)){
		//printf("[DEBUG] reading GYRO\n");
		if ( !get_sensor_value0(sn_gyro, &value )) {
			value = 0;
		}
		printf( "\r[DEBUG] reading GYRO: %f \n", value);
		return value;
		fflush( stdout );
    } else {
    	return 0.5;
    }
}

float getCompass(uint8_t sn_compass) {
	// from the OS GitHub, modified by JB
	float value;
	if (ev3_search_sensor(HT_NXT_COMPASS, &sn_compass,0)){
		//printf("[DEBUG] reading COMPASS\n");
		if ( !get_sensor_value0(sn_compass, &value )) {
			value = 0;
		}
		printf( "\r[DEBUG] reading COMPASS: %f \n", value);
		return value;
		fflush( stdout );
    } else {
    	return 0.5;
    }
}

float getSonar(uint8_t sn_sonar) {
	// from the OS GitHub, modified by JB
	float value;
	if (ev3_search_sensor(LEGO_EV3_US, &sn_sonar,0)){
		//printf("[DEBUG] reading SONAR\n");
		if ( !get_sensor_value0(sn_sonar, &value )) {
			value = 0;
		}
		printf( "\r[DEBUG] reading SONAR: %f \n", value);
		return value;
		fflush( stdout );
    } else {
    	return 0.5;
    }
}

//////////////// MOTORS WHEELS /////////////////////

void forwardTimed(uint8_t sn_left, uint8_t sn_right, int seconds) {
	// by JB Henri and Alix
	set_tacho_speed_sp(sn_right, 500);
	set_tacho_speed_sp(sn_left, 500);
	printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	sleep(seconds);
	printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
	printf("[TACHO] function forward is over!\n");
}

void forwardSonar(uint8_t sn_left, uint8_t sn_right, uint8_t sn_sonar, float sonarThreshold) {
	// by JB and Alix
	float sonarVal = getSonar(sn_sonar);
	set_tacho_speed_sp(sn_right, 500);
	set_tacho_speed_sp(sn_left, 500);
	printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	while (sonarVal > sonarThreshold) {
		sonarVal = getSonar(sn_sonar);
	}
	printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
}

void turnRight(uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro) {
	// by JB
	float gyroVal;
    	float gyroValInitial;
	gyroValInitial = getGyro(sn_gyro);
	gyroVal = getGyro(sn_gyro);
	printf("initial gyro value: %f\n", gyroValInitial);
	set_tacho_speed_sp(sn_left, 100);
	set_tacho_speed_sp(sn_right, -100);
	printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	while (abs(gyroVal - gyroValInitial) < 90) {
		gyroVal = getGyro(sn_gyro);
	}
	printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
}

void turnLeft(uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro) {
	//  by JB
	float gyroVal;
    	float gyroValInitial;
	gyroValInitial = getGyro(sn_gyro);
	gyroVal = getGyro(sn_gyro);
	printf("initial gyro value: %f\n", gyroValInitial);
	set_tacho_speed_sp(sn_left, -100);
	set_tacho_speed_sp(sn_right, 100);
	printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	while (abs(gyroVal - gyroValInitial) < 90) {
		gyroVal = getGyro(sn_gyro);
	}
	printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
}

///////////////////////////// PELLE MOTOR ///////////////////////////////////

size_t initializePelle(uint8_t sn_pelle) {
	// by JB - not working
	printf("[PELLE] initializing pelle with sn = %d\n", sn_pelle);
	set_tacho_position_sp(sn_pelle, 10); // OK
	set_tacho_speed_sp(sn_pelle, 50);
	int oldPos;
	get_tacho_position(sn_pelle, &oldPos);
	printf("[PELLE] initial position of pelle: %d\n", oldPos);
	int newPos;
	bool stuck = false;
	int pos_sp;
	/*while (!stuck) {
		get_tacho_position_sp(sn_pelle, &pos_sp);
		printf("[PELLE] position_sp: %d\n", pos_sp);
		set_tacho_command(sn_pelle, "run-to-rel-pos");
		get_tacho_position(sn_pelle, &newPos);
		printf("[PELLE] new position of pelle: %d\n", newPos);
		sleep(2);
		if (newPos==oldPos) {
			stuck = true;
		} else {
			oldPos = newPos;
		} 
	}
	set_tacho_command(sn_pelle, "stop"); // just in case
	// PELLE SHOULD BE DOWN RIGHT NOW
	printf("[PELLE] should be down right now!\n");
	sleep(2);*/	
	set_tacho_position(sn_pelle, 0); // 0 is the down position
	// GONNA GO UP
	printf("[PELLE] gonna try to go up!\n");
	set_tacho_position_sp(sn_pelle, -20); 
	set_tacho_speed_sp(sn_pelle, 100);
	get_tacho_position(sn_pelle, &oldPos); // should be equal to 0
	printf("[PELLE] initial position of pelle: %d\n", oldPos);
	stuck = false;
	while (!stuck) {
		set_tacho_command(sn_pelle, "run-to-rel-pos");
		get_tacho_position(sn_pelle, &newPos);
		printf("[PELLE] new position of pelle: %d\n", newPos);
		if (newPos==oldPos) {
			stuck = true;
		} else {
			oldPos = newPos;
		} 
	}
	int posUp = oldPos;
	printf("[PELLE] should be up right now!\n");
	return posUp;
}


///////////////////////////////////// MMMMAAAAIIIIINNNNNN ///////////////////////////////////

int main(void) {
	// by JB to test the basic functions
	uint8_t sn_left;
	uint8_t sn_right;
	uint8_t sn_pelle;
	//uint8_t sn_color;
	uint8_t sn_compass;
	uint8_t sn_sonar;
	//uint8_t sn_mag;
	uint8_t sn_gyro;
	//char s[ 256 ];
	//int val;
	//float value;
	float gyroVal;
	float compassVal;
	float sonarVal;
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
	sn_left = findLeftMotor(sn_left);	
	sn_right = findRightMotor(sn_right);
	sn_pelle = findPelleMotor(sn_pelle);
	printf("%d %d %d\n", sn_left, sn_right, sn_pelle);
	
	// GETS READY TO READ SENSORS
	ev3_sensor_init();

	// FIND SENSORS
	findSensors(sn_compass, sn_sonar, sn_gyro);
	
	// GET GYRO ANGLE
	gyroVal = getGyro(sn_gyro);
	if(gyroVal==0.5) {
		printf("[ERROR] Could not find GYRO!");
	}
	printf("GYRO val: %f\n", gyroVal);

	// GET COMPASS ANGLE
	compassVal = getCompass(sn_compass);
	if(compassVal==0.5) {
		printf("[ERROR] Could not find COMPASS!");
	}
	printf("COMPASS val: %f\n", compassVal);

	// GET SONAR DISTANCE
	sonarVal = getSonar(sn_sonar);
	if(sonarVal==0.5) {
		printf("[ERROR] Could not find SONAR!");
	}
	printf("SONAR val: %f\n", sonarVal);

	// TEST MOTORS
	//forwardTimed(sn_left, sn_right, 2);
	//forwardSonar(sn_left, sn_right, sn_sonar, 100.0);
	size_t pellePosUp = initializePelle(sn_pelle);
	printf("[PELLE] pellePosUp: %d\n", pellePosUp);
	/*printf("turning right\n");
	turnRight(sn_left, sn_right, sn_gyro);
	forwardTimed(sn_left, sn_right, 2);
	turnLeft(sn_left, sn_right, sn_gyro);
	forwardTimed(sn_left, sn_right, 2);
	*/

	// ENDS MAIN
	ev3_uninit();
	printf( "*** ( EV3 ) Bye! ***\n" );
	return ( 0 );
}
