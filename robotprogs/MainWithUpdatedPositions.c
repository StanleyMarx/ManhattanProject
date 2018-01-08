#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include "ev3.h"
#include "ev3_port.h"
#include "ev3_tacho.h"
#include "ev3_sensor.h"
#include "turns.h"
#include "avancer.h"

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

//////////////// MOTORS WHEELS /////////////////////


void forwardSonar(uint8_t sn_left, uint8_t sn_right, uint8_t sn_sonar, float sonarThreshold, int speed) {
	float sonarVal = getSonar(sn_sonar);
	if (sonarVal > sonarThreshold+10) {
		set_tacho_speed_sp(sn_right, speed);
		set_tacho_speed_sp(sn_left, speed);
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
}




void TurnDegreeRposLneg(uint8_t sn_left, uint8_t sn_right, uint8_t sn_sonar, uint8_t sn_gyro, float speed, float angle) {
	float gyroVal;
    float gyroValInitial;
	float sonarVal = getSonar(sn_sonar);
	gyroValInitial = getGyro(sn_gyro);
	gyroVal = getGyro(sn_gyro);
	printf("initial gyro value: %f\n", gyroValInitial);
	set_tacho_speed_sp(sn_left, speed*angle/abs(angle));
	set_tacho_speed_sp(sn_right, -speed*angle/abs(angle));
	printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	while (abs(gyroVal - gyroValInitial) < abs(angle)) {
		gyroVal = getGyro(sn_gyro);
		sonarVal = getSonar(sn_sonar);
		printf("\n");
	}
	printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
}		




///////////////////////////////////// UPDATE_POSITIONS ///////////////////////////////////


int pi=3.14159265;
int Xdef=0,Ydef=0;
int speedMotorL, speedMotorR, positionMotorL1, positionMotorR1, positionMotorL2, positionMotorR2;
float thetaCompas;
float lambda=1/21.21;
int ThreadDisplay=0;

pthread_mutex_t lock;

void* display_entry(){
        /* affiche la position toutes les secondes */
	/* debut SC1 */
        pthread_mutex_lock(&lock);
        while(ThreadDisplay == 0){
                pthread_mutex_unlock(&lock);
                /* fin SC1 */

        		get_tacho_position_sp(sn_left, &positionMotorL1);
        		get_tacho_position_sp(sn_rigth, &positionMotorR1);
        		spleep(0.2);
        		get_tacho_speed_sp(sn_left, &speedMotorL);
        		get_tacho_speed_sp(sn_rigth, &speedMotorR);
        		get_tacho_position_sp(sn_left, &positionMotorL2);
        		get_tacho_position_sp(sn_rigth, &positionMotorR2);
        		get_sensor_value0(sn_compass, &thetaCompas);

        		if (speedMotorR != 0) and (speedMotorL != 0) {
        			if (speedMotorL/speedMotorR > 0) {
        				printf("\nrobot is moving");
        				Xdef=Xdef-sin(thetaCompas)*(positionMotorR2-positionMotorR1)*lambda;
        				Ydef=Ydef+cos(thetaCompas)*(positionMotorR2-positionMotorR1)*lambda;
        			} else {
        				printf("\nrobot is turning");
        			}
        		}
                printf("x,y = %d,%d\n",Xdef,Ydef);

                /* debut SC1 */
                pthread_mutex_lock(&lock);
        }
        pthread_mutex_unlock(&lock);
        /* fin SC1 */
        return NULL;
}






///////////////////////////////////// MMMMAAAAIIIIINNNNNN ///////////////////////////////////

int main(void) {
	uint8_t sn_left;
	uint8_t sn_right;
	uint8_t sn_pelle;
	uint8_t sn_compass;
	uint8_t sn_sonar;
	uint8_t sn_gyro;
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






    pthread_t display;
    pthread_create(&display,NULL,display_entry,NULL);

	//THE END OF THE INITIALISATION____________________________________________
	//THE MOVEMENT FUNCTIONS___________________________________________________

	
	forwardSonar(sn_left, sn_right, sn_sonar, sonarThreshold, 200);
	TurnDegreeRposLneg(sn_left, sn_right, sn_sonar, sn_gyro, 100.0, 90);
	forwardSonar(sn_left, sn_right, sn_sonar, sonarThreshold, 200);
	TurnDegreeRposLneg(sn_left, sn_right, sn_sonar, sn_gyro, 100.0, 90);
	forwardSonar(sn_left, sn_right, sn_sonar, sonarThreshold, 200);
	

	//THE END OF THE INITIALISATION____________________________________________
	//THE MOVEMENT FUNCTIONS___________________________________________________

	/* debut SC2 */
    pthread_mutex_lock(&lock);
    ThreadDisplay = 1;
    pthread_mutex_unlock(&lock);
    /* fin SC2 */

    pthread_join(display,NULL);
    pthread_mutex_destroy(&lock);






	ev3_uninit();
	printf( "*** ( EV3 ) Bye! ***\n" );
	return ( 0 );
}