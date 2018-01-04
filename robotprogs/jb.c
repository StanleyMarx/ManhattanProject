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

// CONSTANTS TO MOVE ROBOTS
const int LEFT_PORT = 68;// port D
const int RIGHT_PORT = 66; //port B
const int PELLE_PORT = 67; // port C

int X=0, Y=0;
int Xdef=0, Ydef=0;
pthread_mutex_t lock; // THREADS

////////////////// INITIALISATION BLUETOOTH ///////////////////////
int s;
uint16_t msgId = 0;

int read_from_server (int sock, char *buffer, size_t maxSize) {
    int bytes_read = read (sock, buffer, maxSize);

    if (bytes_read <= 0) {
        fprintf (stderr, "Server unexpectedly closed connection...\n");
        close (s);
        exit (EXIT_FAILURE);
    }

    printf ("[DEBUG] received %d bytes\n", bytes_read);

    return bytes_read;
}

////////////////// INITIALISATION FUNCTIONS ///////////////////////

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

int testSensors(uint8_t sn_compass, uint8_t sn_sonar, uint8_t sn_gyro) {
	float gyroVal;
	float compassVal;
	float sonarVal;
	int successfulSensors = 0;
	// GET GYRO ANGLE
	gyroVal = getGyro(sn_gyro);
	if(gyroVal==0.5) {
		printf("[ERROR] Could not find GYRO!");
	} else {
		printf("GYRO val: %f\n", gyroVal);
		successfulSensors++;
	}
	// GET COMPASS ANGLE
	compassVal = getCompass(sn_compass);
	if(compassVal==0.5) {
		printf("[ERROR] Could not find COMPASS!");
	} else {
		printf("COMPASS val: %f\n", compassVal);
		successfulSensors++;
	}
	// GET SONAR DISTANCE
	sonarVal = getSonar(sn_sonar);
	if(sonarVal==0.5) {
		printf("[ERROR] Could not find SONAR!");
	} else {
		printf("SONAR val: %f\n", sonarVal);
		successfulSensors++;
	}
	return successfulSensors;
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

//////////////// MOTORS FOR BOTH WHEELS /////////////////////

void forwardSonar(uint8_t sn_left, uint8_t sn_right, uint8_t sn_sonar, float sonarThreshold) {
	float sonarVal = getSonar(sn_sonar);
	set_tacho_speed_sp(sn_right, 500);
	set_tacho_speed_sp(sn_left, 500);
	printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	while (sonarVal > sonarThreshold) {
		sonarVal = getSonar(sn_sonar);
		// updatePos() -- not coded yet!
		/*
		printf("[INFO] locking display\n");
        pthread_mutex_lock(&lock);
		printf("[INFO] changing Xdef and Ydef\n");
        Xdef=X;
        Ydef=Y;
		printf("[INFO] unlocking display\n");
        pthread_mutex_unlock(&lock);
        */
	}
	printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
}

void turnRight(uint8_t sn_left, uint8_t sn_right, uint8_t sn_gyro) {
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
		// updatePos() -- not coded yet!
		/*
		printf("[INFO] locking display\n");
        pthread_mutex_lock(&lock);
		printf("[INFO] changing Xdef and Ydef\n");
        Xdef=X;
        Ydef=Y;
		printf("[INFO] unlocking display\n");
        pthread_mutex_unlock(&lock);
        */
	}
	printf("[TACHO] stopping tachos\n");
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
	printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_left, "run-forever");
	set_tacho_command(sn_right, "run-forever");
	while (abs(gyroVal - gyroValInitial) < 90) {
		gyroVal = getGyro(sn_gyro);
		// updatePos() -- not coded yet!
		/*
		printf("[INFO] locking display\n");
        pthread_mutex_lock(&lock);
		printf("[INFO] changing Xdef and Ydef\n");
        Xdef=X;
        Ydef=Y;
		printf("[INFO] unlocking display\n");
        pthread_mutex_unlock(&lock);
        */
	}
	printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_left, "stop");
	set_tacho_command(sn_right, "stop");
}

///////////////////////////// PELLE MOTOR ///////////////////////////////////

size_t initializePelle(uint8_t sn_pelle) {
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

////////////////////////////////// SENDING MESSAGES TO SERVER ///////////////////////////////////

void sendMapMsg(int x, int y, int red, int green, int blue) {
	char string[58];
	*((uint16_t *) string) = msgId++;
	string[2] = TEAM_ID;
	string[3] = 0xFF;
	string[4] = MSG_MAPDATA;
    string[5] = x;
	string[6] = 0x00;
    string[7] = y;
	string[8]= 0x00;
	string[9]= red;
	string[10]= green;
	string[11]= blue;
	write(s, string, 12);
	printf("[DEBUG] Just sent a MAP message to server");
	Sleep(100);
}

int sendMapDoneMsg() {
	char type;
	*((uint16_t *) string) = msgId++;
	string[2] = TEAM_ID;
	string[3] = 0xFF;
	string[4] = MSG_MAPDONE;
	write(s, string, 5);
	printf("[DEBUG] Just sent MAP_DONE message to server");
	Sleep(100);
	/*
	printf("I'm waiting for the stop message");
	while(1){
		//Wait for stop message
		read_from_server (s, string, 58);
      		type = string[4];
		if (type ==MSG_STOP){
			return;
		}
	}
	*/
	return 0;	
}

void sendPosMsg(int x, int y) {
	char string[58];
	*((uint16_t *) string) = msgId++;
 	string[2] = TEAM_ID;
    string[3] = 0xFF;
    string[4] = MSG_POSITION;
    string[5] = x;          /* x */
	string[6] = 0x00;
    string[7] = y;		/* y */
	string[8]= 0x00;
	write(s, string, 9);
	printf("[DEBUG] Just sent a POSITION message to server");
	Sleep(100);	
}

void* sendPosToServer() {
    while (true) { // not sure if that's a good idea...
    	/* debut SC1 */
        pthread_mutex_lock(&lock);
        sendPosMsg(Xdef, Ydef);
        pthread_mutex_unlock(&lock);
        /* fin SC1 */
        sleep(2);
    }
    return NULL;
}

///////////////////////////////////// UPDATE POSITION ///////////////////////////////////
void updatePos() {
	printf("idk what to do here");
	/*
	get pos for both wheels and deduce (dx,dy) of robot: 
		wheel radius, 
		angle, 
		dx in cm
		dy in cm
	how to use gyro_angle as well..?
	
	1- use compass to set x-axis and y-axis
	2- use compass_angle, compare it with initial_compass_angle to measure angle with x-axis and y-axis
	3- use changes of wheel position and angle to measure dx and dy (w/ cos() and sin())
	4- x+=1 if dx+=5cm, same for y and dy.
	5- update X and Y
	*/
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
	sn_left = findLeftMotor(sn_left);	
	sn_right = findRightMotor(sn_right);
	sn_pelle = findPelleMotor(sn_pelle);
	printf("%d %d %d\n", sn_left, sn_right, sn_pelle);
	
	// GETS READY TO READ SENSORS
	ev3_sensor_init();

	// FIND SENSORS
	findSensors(sn_compass, sn_sonar, sn_gyro);
	int nbrSensors;
	nbrSensors = testSensors(sn_compass, sn_sonar, sn_gyro);
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
			forwardSonar(sn_left, sn_right, sn_sonar, 100.0);
			side = rand() % 2;
			if (side==0) {
				turnRight(sn_left, sn_right, sn_gyro);
			} else {
				turnLeft(sn_left, sn_right, sn_gyro);
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
		//forwardTimed(sn_left, sn_right, 2);
		//forwardSonar(sn_left, sn_right, sn_sonar, 100.0);
		//size_t pellePosUp = initializePelle(sn_pelle);
		//printf("[PELLE] pellePosUp: %d\n", pellePosUp);
		/*printf("turning right\n");
		turnRight(sn_left, sn_right, sn_gyro);
		forwardTimed(sn_left, sn_right, 2);
		turnLeft(sn_left, sn_right, sn_gyro);
		forwardTimed(sn_left, sn_right, 2);
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
