#include <stdio.h>
#include <stdlib.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_sensor.h"

#include "turns.h"
#include "avancer.h"
#include "avancer_sonar.h"
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

void findMotors(uint8_t sn_left, uint8_t sn_right, uint8_t sn_pelle) {
	// find left motor	
	if ( ev3_search_tacho_plugged_in(LEFT_PORT,0, &sn_left, 0 )) {
		printf( "LEGO_EV3_M_MOTOR LEFT is found\n");
	} else {
		printf( "LEGO_EV3_M_MOTOR LEFT is NOT found\n" );
	}
	// find right motor
	if ( ev3_search_tacho_plugged_in(RIGHT_PORT,0, &sn_right, 0 )) {
		printf( "LEGO_EV3_M_MOTOR RIGHT is found\n" );
	} else {
		printf( "LEGO_EV3_M_MOTOR RIGHT is NOT found\n" );
	}
	// find pelle motor
	if ( ev3_search_tacho_plugged_in(PELLE_PORT,0, &sn_pelle, 0 )) {
		printf( "LEGO_EV3_M_MOTOR PELLE is found\n" );
	} else {
		printf( "LEGO_EV3_M_MOTOR PELLE is NOT found\n" );
	}
}

//////////////////////// SENSORS /////////////////////

float getGyro(uint8_t sn_gyro) {
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

//////////////// MOTORS /////////////////////

void forward(uint8_t sn_left, uint8_t sn_right) {
	int max_speed_right;
	int max_speed_left;
	get_tacho_max_speed( sn_right, &max_speed_right );
	get_tacho_max_speed( sn_left, &max_speed_left );
	set_tacho_stop_action_inx( sn_right, TACHO_COAST );
	set_tacho_stop_action_inx( sn_left, TACHO_COAST );
	set_tacho_speed_sp( sn_right, max_speed_right * 2 / 3 );
	set_tacho_speed_sp( sn_left, max_speed_left * 2 / 3 );
	set_tacho_time_sp( sn_right, 5000 );
	set_tacho_time_sp( sn_left, 5000 );
	set_tacho_ramp_up_sp( sn_right, 1000 );
	set_tacho_ramp_up_sp( sn_left, 1000 );
	set_tacho_ramp_down_sp( sn_right, 1000 );
	set_tacho_ramp_down_sp( sn_left, 1000 );
	set_tacho_command_inx( sn_right, TACHO_RUN_TIMED );
	set_tacho_command_inx( sn_left, TACHO_RUN_TIMED );
}

void turnRight(uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro) {
    float gyroVal;
    float gyroValInitial;
    int max_speed;
    //int position = ((1*360)/(2*3.1415*2.8));
    get_tacho_max_speed(sn_left, &max_speed );
    printf("  max speed = %d\n", max_speed );
	gyroValInitial = getGyro(sn_gyro);
	gyroVal = getGyro(sn_gyro);
	printf("initial gyro value: %f\n", gyroValInitial);
	//int pos = (5.8)/(2.8); // tiny rotation - see if we can change this value (or actually calculate it?)
	set_tacho_speed_sp(sn_left, max_speed / 2 );
	set_tacho_ramp_up_sp( sn_left, 0 );
	set_tacho_ramp_down_sp( sn_left, 0 );
	set_tacho_position_sp( sn_left, 90 );

	set_tacho_speed_sp(sn_right, - max_speed / 2 );
	set_tacho_ramp_up_sp( sn_right, 0 );
	set_tacho_ramp_down_sp( sn_right, 0 );
	set_tacho_position_sp( sn_right, 90 );
	while (abs(gyroVal - gyroValInitial) < 90) {
		/*ev3_set_speed_sp(sn_left, 300);
		ev3_set_speed_sp(sn_right, 300);
		ev3_set_position_sp(sn_left, pos);
		ev3_set_position_sp(sn_right, -pos));*/
		//ev3_set_speed_sp(sn_right, -50);
		//ev3_set_speed_sp(sn_left, 50);
		//set_tacho_stop_action_inx( sn, TACHO_COAST );
		//set_tacho_speed_sp(sn_left, max_speed * 2 / 3 );
		//set_tacho_speed_sp(sn_right, - max_speed * 2 / 3 );
		//set_tacho_command(sn_left, "TACHO_RUN_FOREVER");
		//set_tacho_command(sn_left, "TACHO_RUN_FOREVER");
		//set_tacho_command(sn_right, "TACHO_RUN_FOREVER");
		/*ev3_set_speed_sp(sn_left, max_speed);
        ev3_set_speed_sp(sn_right, -max_speed);
        ev3_set_position_sp(sn_left, position);
        ev3_set_position_sp(sn_right, position);
        ev3_command_motor_by_name(sn_right, "run-to-rel-pos");
        ev3_command_motor_by_name(sn_left, "run-to-rel-pos");*/
		set_tacho_command_inx(sn_left, TACHO_RUN_TO_REL_POS );
		set_tacho_command_inx(sn_right, TACHO_RUN_TO_REL_POS );
		gyroVal = getGyro(sn_gyro);
		/*ev3_command_motor_by_name(motor1, "run-to-rel-pos");
        ev3_command_motor_by_name(motor2, "run-to-rel-pos");*/
	}
	//set_tacho_command(sn_left, "TACHO_STOP");
	//set_tacho_command(sn_right, "TACHO_STOP");
}


///////////////////////////////////// MMMMAAAAIIIIINNNNNN ///////////////////////////////////

int main(void) {
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
	findMotors(sn_left, sn_right, sn_pelle);

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
	forward(sn_left, sn_right);
	/*
	// TURN RIGHT
	printf("turning right\n");
	turnRight(sn_left, sn_right, sn_gyro);
	printf("finished the turn\n");*/

	// ENDS MAIN
	ev3_uninit();
	printf( "*** ( EV3 ) Bye! ***\n" );
	return ( 0 );
}
