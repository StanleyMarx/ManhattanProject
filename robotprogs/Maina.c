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
	float value;
	if (ev3_search_sensor(LEGO_EV3_GYRO, &sn_gyro,0)){
		//printf("[DEBUG] reading GYRO\n");
		if ( !get_sensor_value0(sn_gyro, &value )) {
			value = 0;
		}
		//printf( "\r[DEBUG] reading GYRO: %f \n", value);
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
		//printf( "\r[DEBUG] reading COMPASS: %f \n", value);
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
		//printf( "\r[DEBUG] reading SONAR: %f \n", value);
		return value;
		fflush( stdout );
    } else {
    	return 0.5;
    }
}

//////////////// MOTORS /////////////////////

void forwardTimed(uint8_t sn_left, uint8_t sn_right, int seconds) {
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

void forwardTimed2(uint8_t sn_left, uint8_t sn_right, int ms){
	set_tacho_speed_sp(sn_right, 500);
	set_tacho_speed_sp(sn_left, 500); 
	set_tacho_time_sp(sn_left, ms);
	set_tacho_time_sp(sn_right, ms);
	printf("[TACHO] starting forwardTimed2");
	set_tacho_command(sn_left, "run-timed");
	set_tacho_command(sn_right, "run-timed");
	printf("[TACHPO function forwardTimed2 over");
}


void forwardSonar(uint8_t sn_left, uint8_t sn_right, uint8_t sn_sonar, float sonarThreshold) {
	float sonarVal = getSonar(sn_sonar);
	set_tacho_speed_sp(sn_right, 500);
	set_tacho_speed_sp(sn_left, 500);
	//printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	while (sonarVal > sonarThreshold) {
		sonarVal = getSonar(sn_sonar);
	}
	//printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
}

void turnRight(uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro) {
	float gyroVal;
    	float gyroValInitial;
	gyroValInitial = getGyro(sn_gyro);
	gyroVal = getGyro(sn_gyro);
	//printf("initial gyro value: %f\n", gyroValInitial);
	set_tacho_speed_sp(sn_left, 100);
	set_tacho_speed_sp(sn_right, -100);
	//printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	while (abs(gyroVal - gyroValInitial) < 90) {
		gyroVal = getGyro(sn_gyro);
	}
	//printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
}

void turnLeft(uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro) {
	float gyroVal;
    	float gyroValInitial;
	gyroValInitial = getGyro(sn_gyro);
	gyroVal = getGyro(sn_gyro);
	printf("initial gyro value: %f\n", gyroValInitial);
	set_tacho_speed_sp(sn_left, -100);
	set_tacho_speed_sp(sn_right, 100);
	//printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	while (abs(gyroVal - gyroValInitial) < 90) {
		gyroVal = getGyro(sn_gyro);
	}
	//printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
}

void TurnDegreeRposLneg(uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro, float angle) {
	float gyroVal;
    	float gyroValInitial;
	gyroValInitial = getGyro(sn_gyro);
	gyroVal = getGyro(sn_gyro);
	printf("initial gyro value: %f\n", gyroValInitial);
	set_tacho_speed_sp(sn_left, 100.0*angle/abs(angle));
	set_tacho_speed_sp(sn_right, -100.0*angle/abs(angle));
	printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	while (abs(gyroVal - gyroValInitial) < abs(angle)*0.95) {
		gyroVal = getGyro(sn_gyro);
	}
	printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
}
//detects if movable or else 
int detectType1(uint8_t sn_left, uint8_t sn_right, uint8_t sn_pelle, uint8_t sn_sonar, uint8_t sn_gyro, float delta) {
	int type = 0
	TurnDegreeRposLneg(sn_left, sn_right, sn_gyro, -delta);
	float sonarValG = getSonar(sn_sonar);
	TurnDegreeRposLneg(sn_left, sn_right, sn_gyro, 2*delta);
	float sonarValD = getSonar(sn_sonar);
	TurnDegreeRposLneg(sn_left, sn_right, sn_gyro, -delta);
	if (sonarValG>300 && sonarValD>300){
		printf("movable object");
		type = 1;
	} 
	else{
		type = 2;
	}
	return type; 
}
//detect non movable or fronteer 
int detectType2(uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro, uint8_t sn_sonar, float sonarTreshold){
	int type2=0;
	int i = 0
	//valeur seuil de i à tester
	while( getsonar(sn_sonar) < sonarTreshold || i < 3){	
		turnLeft(sn_left, sn_right, sn_gyro);
		forwardTimed2(sn_left, sn_right, 500);
		turnRight(sn_left, sn_right, sn_gyro);
		i=i+1;
		}
	if(i<3){
		printf("[OBSTACLE] non movable object\n");
		type2=3;
	}
	else{
		printf("[OBSTACLE] fronteer\n"); 
		type2=4;
	}
	return type; 
}
	
void take_object(uint8_t sn_pelle, uint8_t sn_left, uint8_t sn_right, uint8_t sn_sonar) {
	forwardSonar(sn_left, sn_right, sn_sonar, 150.0);
	printf("[PELLE] opening pelle\n");//--------open pelle
	set_tacho_speed_sp(sn_pelle, -80);
	set_tacho_command(sn_pelle, "run-forever");
	sleep(2);
	//set_tacho_command(sn_pelle, "stop");
	forwardTimed(sn_left, sn_right, 2, 100);//---------moveforward
	printf("[PELLE] closing pelle\n");//-------close pelle
	set_tacho_command(sn_pelle, "stop");
	set_tacho_speed_sp(sn_pelle, 80);
	set_tacho_command(sn_pelle, "run-forever");
	sleep(1);
	set_tacho_command(sn_pelle, "stop");
}



void drop_object(uint8_t sn_pelle, uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro) {
	printf("[PELLE] opening pelle\n");//----------open pelle
	set_tacho_speed_sp(sn_pelle, -80);
	set_tacho_command(sn_pelle, "run-forever");
	sleep(2);
	//set_tacho_command(sn_pelle, "stop");
	forwardTimed(sn_left, sn_right, 1, -80);//---------movebackward
	TurnDegreeRposLneg(sn_left, sn_right, sn_gyro, -180);//-------half turn
	printf("[PELLE] closing pelle\n");//----------close pelle
	set_tacho_command(sn_pelle, "stop");
	set_tacho_speed_sp(sn_pelle, 80);
	set_tacho_command(sn_pelle, "run-forever");
	sleep(1);
	set_tacho_command(sn_pelle, "stop");
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
	while(true){
		forwardSonar(sn_left, sn_right, sn_sonar, 100.0)
		int x = detectType(sn_left, sn_right, sn_gyro, sn_sonar, 150.0);
		if(x==1){
			take_object(sn_pelle, sn_left, sn_right, sn_sonar);
			TurnDegreeRposLneg(sn_left,sn_right,sn_gyro, 180);
			drop_object(sn_pelle, sn_left, sn_right, sn_gyro);	
		}
		else{
			int x = detectType2(sn_left, sn_right, sn_gyro, sn_sonar, 150.0);
			//if fronteer  
			if(x==4){
				turnRight(sn_left, sn_right, sn_gyro);
			}
		}
		}
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
