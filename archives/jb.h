// CONSTANTS TO MOVE ROBOTS
const int LEFT_PORT = 68;// port D
const int RIGHT_PORT = 66; //port B
const int SHOVEL_PORT = 67; // port C

// CONSTANTS FOR SENSORS AND MOTORS
uint8_t sn_left;
uint8_t sn_right;
uint8_t sn_shovel;
//uint8_t sn_color;
uint8_t sn_compass;
uint8_t sn_sonar;
//uint8_t sn_mag;
uint8_t sn_gyro;

// CONSTANTS FOR BT COMM W/ SERVER
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

void findSensors() {
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

int testSensors() {
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

uint8_t findShovelMotor() {
	// find shovel motor	
	if ( ev3_search_tacho_plugged_in(SHOVEL_PORT,0, &sn_shovel, 0 )) {
		printf( "LEGO_EV3_M_MOTOR SHOVEL is found on port %d, for sn=%d\n", SHOVEL_PORT, sn_shovel);
		set_tacho_command(sn_shovel, "reset");
		return sn_shovel;
	} else {
		printf( "LEGO_EV3_M_MOTOR SHOVEL is NOT found\n" );
		exit(-1);
	}
}

//////////////////////// SENSORS /////////////////////

float getGyro() {
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

float getCompass() {
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

float getSonar() {
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

void forwardSonar(float sonarThreshold) {
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

void turnRight() {
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

void turnLeft() {
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

///////////////////////////// SHOVEL MOTOR ///////////////////////////////////

size_t initializeShovel() {
	printf("[SHOVEL] initializing shovel with sn = %d\n", sn_shovel);
	set_tacho_position_sp(sn_shovel, 10); // OK
	set_tacho_speed_sp(sn_shovel, 50);
	int oldPos;
	get_tacho_position(sn_shovel, &oldPos);
	printf("[SHOVEL] initial position of shovel: %d\n", oldPos);
	int newPos;
	bool stuck = false;
	int pos_sp;
	/*while (!stuck) {
		get_tacho_position_sp(sn_shovel, &pos_sp);
		printf("[SHOVEL] position_sp: %d\n", pos_sp);
		set_tacho_command(sn_shovel, "run-to-rel-pos");
		get_tacho_position(sn_shovel, &newPos);
		printf("[SHOVEL] new position of shovel: %d\n", newPos);
		sleep(2);
		if (newPos==oldPos) {
			stuck = true;
		} else {
			oldPos = newPos;
		} 
	}
	set_tacho_command(sn_shovel, "stop"); // just in case
	// SHOVEL SHOULD BE DOWN RIGHT NOW
	printf("[SHOVEL] should be down right now!\n");
	sleep(2);*/	
	set_tacho_position(sn_shovel, 0); // 0 is the down position
	// GONNA GO UP
	printf("[SHOVEL] gonna try to go up!\n");
	set_tacho_position_sp(sn_shovel, -20); 
	set_tacho_speed_sp(sn_shovel, 100);
	get_tacho_position(sn_shovel, &oldPos); // should be equal to 0
	printf("[SHOVEL] initial position of shovel: %d\n", oldPos);
	stuck = false;
	while (!stuck) {
		set_tacho_command(sn_shovel, "run-to-rel-pos");
		get_tacho_position(sn_shovel, &newPos);
		printf("[SHOVEL] new position of shovel: %d\n", newPos);
		if (newPos==oldPos) {
			stuck = true;
		} else {
			oldPos = newPos;
		} 
	}
	int posUp = oldPos;
	printf("[SHOVEL] should be up right now!\n");
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
	printf("I'm updating the position in here");
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

