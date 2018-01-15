#define PORT_RIGHTWHEEL 66
#define PORT_SHOVEL 67
#define PORT_LEFTWHEEL 68

// ---------------------------------- Global Variables
// tachos and sensors
uint8_t sn;
uint8_t sn_compass;
uint8_t sn_gyro;
uint8_t sn_sonar;
uint8_t sn_rwheel;
uint8_t sn_lwheel;
uint8_t sn_shovel;
uint8_t sn;
uint8_t sn_color;
int max_speed;
// for the map
int minX = 0;
int minY = 0;
int maxX = 0;
int maxY = 0;
FILE* posFile = NULL;
// robot movements
const int CHECK_TIMER = 2;
const float SONAR_THRESHOLD = 70.0;
const int POS_THRESHOLD = 0;
// shovel
int count_take = 1; //begin with an obstacle in the shovel
int count_drop = 0;
// for robot position
float Xdef, Ydef; 
int Xpos, Ypos, XposOld, YposOld;
float pi=3.14159265; 
int speedMotorL, speedMotorR; 
int positionMotorR1, positionMotorR2; 
float thetaCompas, thetaCompasInit; 
float lambda=1/21.21*86/35; 
pthread_mutex_t mutex;
int ThreadSituation=0;

// ---------------------------------- Signatures
// Testing Functions
void test_tachos_verbose();
void test_sensors_verbose();
// Get Sensor Values
float get_gyro();
float get_compass();
float get_compass_slow();
float get_sonar();
int get_color();
// Simple Movement Functions
void move_forever(int rcycle,int lcycle);
void move_real(int r,int l,int speed);
void move_real_debug(int r,int l);
void turn_approx(float angle);
void turn_exact_abs(float anglDest,float prec);
void turn_exact_rel(float delta,float prec);
void turn_exact_gyro(float delta,float prec);
void forwardTimed(int seconds, int speed);
int forward_sonar(float sonarThreshold);
int forward_sonar_timed(int rcycle, int lcycle, float sonarThreshold, int sec, int delta);
int forward_Sonar2(int rcycle, int lcycle, float sonarThreshold, int msec, int delta);
void turn_right();
void turn_left();
int forward_timed();
int forward_sonar_jb();
// Shovel Functions
void open_shovel();
void close_shovel();
void take_object();
void drop_object();
// Communication with Server
void send_position(int16_t x,int16_t y);
void send_mapdata(int16_t x,int16_t y,char r,char g,char b);
void send_mapdone();
void send_obstacle(int act,uint16_t x,uint16_t y);
void send_position_pos(int16_t x,int16_t y);
void send_mapdata_pos(int16_t x,int16_t y,char r,char g,char b);
void send_obstacle_pos(int act,uint16_t x,uint16_t y);
// Robot Position
void* Update_position2();
void* test_Update_position2();
int get_X_position();
int get_Y_position();
// Detection Functions
int detect_movable();
void position_in_front();
int isThereSomethingInFront();
int checkSides();
int detect_nonmovable(int sonarThreshold);
// More complex Movements
int forward_while_checking_left();
int go_around_map();
int explore_mountain();
// Mapping 
void find_corners();
int create_map(int x0, int y0);
int append_pos_file(int x, int y, int nature);
int known_point(int checkX, int checkY);
void set_initial_coordinates(int x, int y);
// Nino map ?
void print_int_list(int* list, int len);
int pos_exists(int x, int y, int* x_list, int* y_list, int len);
int send_map_from_file();

// ------------------------------ Testing Functions
void test_tachos_verbose(){
    int8_t sn;
    int allPlugged=1;
    int i;

    if (ev3_search_tacho_plugged_in(PORT_RIGHTWHEEL,0,&sn,0)){
        printf("[ OK  ] right wheel tacho found at default port (B), sn=%d\n",sn);
    }
    else {
        printf("[ERROR] right wheel tacho not found at default port (B)\n");
        allPlugged=0;
    }
    if (ev3_search_tacho_plugged_in(PORT_SHOVEL,0,&sn,0)){
        printf("[ OK  ] shovel tacho found at default port (C), sn=%d\n",sn);
    }
    else {
        printf("[ERROR] shovel not found at default port (C)\n");
        allPlugged=0;
    }
    if (ev3_search_tacho_plugged_in(PORT_LEFTWHEEL,0,&sn,0)){
        printf("[ OK  ] left wheel tacho found at default port (D), sn=%d\n",sn);
    }
    else {
        printf("[ERROR] left wheel not found at default port (D)\n");
        allPlugged=0;
    }
    if (!allPlugged){
        printf("Ports with tachos plugged in: ");
        for (i=65; i<69; i++){
            if (ev3_search_tacho_plugged_in(i,0,&sn,0)){
                printf("(port=%d, sn=%d) ",i,sn);
            }
        }
        printf("\n");
    }
}
void test_sensors_verbose(){
    uint8_t sn_touch;
    uint8_t sn_color;
    uint8_t sn_compass;
    uint8_t sn_sonar;
    uint8_t sn_mag;
    uint8_t sn_gyro;
    float value;

    ev3_sensor_init();
    if (ev3_search_sensor(LEGO_EV3_TOUCH,&sn_touch,0)){
        printf("[ OK  ] touch sensor is found at %d\n",sn_touch);
    }
    else {
        printf("[ERROR] touch sensor is not found\n");
    }
    if (ev3_search_sensor(LEGO_EV3_COLOR,&sn_color,0)){
        printf("[ OK  ]color sensor is found at %d\n",sn_color);
        set_sensor_mode(sn_color, "COL-COLOR");
    }
    else {
        printf("[ERROR] color sensor is not found\n");
    }
    if (ev3_search_sensor(HT_NXT_COMPASS,&sn_compass,0)){
        printf("[ OK  ] compass is found at %d\n",sn_color);
        get_sensor_value0(sn_compass,&value);
        printf("\tvalue = %f\n",value);
    }
    else {
        printf("[ERROR] compass is not found\n");
    }
    if (ev3_search_sensor(LEGO_EV3_US,&sn_sonar,0)){
        printf("[ OK  ] sonar is found at %d\n",sn_sonar);
        get_sensor_value0(sn_sonar,&value);
        printf("\tvalue = %f\n",value);
    }
    else {
        printf("[ERROR] sonar is not found\n");
    }
    if (ev3_search_sensor(NXT_ANALOG,&sn_mag,0)){
        printf("[ OK  ] magnetic sensor is found at %d\n",sn_mag);
        get_sensor_value0(sn_mag,&value);
        printf("\tvalue = %f\n",value);
    }
    else {
        printf("[ERROR] magnetic sensor is not found\n");
    }
    if (ev3_search_sensor(LEGO_EV3_GYRO,&sn_mag,0)){
        printf("[ OK  ] gyro is found at %d\n",sn_mag);
        get_sensor_value0(sn_gyro,&value);
        printf("\tvalue = %f\n",value);
    }
    else {
        printf("[ERROR] gyro is not found\n");
    }
}

// ------------------------------ Get Sensor Values
float get_gyro(){
    float ret;
    get_sensor_value0(sn_gyro,&ret);
    return -ret;
}
float get_compass(){
    float ret;
    get_sensor_value0(sn_compass,&ret);
    return ret;
}

//unused? 
float get_compass_slow(){
    sleep(1);
    move_forever(0,0);
    return get_compass();
}
float get_sonar(){
    float ret;
    get_sensor_value0(sn_sonar,&ret);
    return ret;
}
int get_color(){
	float ret;
	get_sensor_value0(sn_color, &ret);
	return (int) ret; // { "?", "BLACK", "BLUE", "GREEN", "YELLOW", "RED", "WHITE", "BROWN" };
}

// ------------------------------ Simple Movement Functions
//used
void move_forever(int rcycle,int lcycle){
    set_tacho_duty_cycle_sp(sn_rwheel,rcycle);
    set_tacho_duty_cycle_sp(sn_lwheel,lcycle);
    set_tacho_command_inx(sn_rwheel,TACHO_RUN_DIRECT);
    set_tacho_command_inx(sn_lwheel,TACHO_RUN_DIRECT);
}
//unused? 
void move_real(int r,int l,int speed){
    set_tacho_stop_action_inx(sn_rwheel,TACHO_BRAKE);
    set_tacho_stop_action_inx(sn_lwheel,TACHO_BRAKE);
    set_tacho_speed_sp(sn_rwheel,speed);
    set_tacho_speed_sp(sn_lwheel,speed);
    set_tacho_ramp_up_sp(sn_rwheel,0);
    set_tacho_ramp_up_sp(sn_lwheel,0);
    set_tacho_ramp_down_sp(sn_rwheel,0);
    set_tacho_ramp_down_sp(sn_lwheel,0);
    set_tacho_position_sp(sn_rwheel,r);
    set_tacho_position_sp(sn_lwheel,l);
    set_tacho_command_inx(sn_rwheel,TACHO_RUN_TO_REL_POS);
    set_tacho_command_inx(sn_lwheel,TACHO_RUN_TO_REL_POS);
}

//unused? 
void move_real_debug(int r,int l){
    int time_ratio=4500;
    move_real(r,l,100);
    usleep(time_ratio*(abs(r)+abs(l)));
}

void turn_approx(float angle){
    float ratio=2.66;
    move_real_debug(ratio*angle,-ratio*angle);
}

//unused? 
void turn_exact_abs(float anglDest,float prec){
    float ratio=2.5;
    float anglCurr=get_compass();
    float delta=anglDest-anglCurr;
    delta=fmod(delta,360);
    if (delta>180){delta=delta-360;}
    while(abs(delta)>prec){
        move_real(-ratio*delta,ratio*delta,max_speed/5);
        sleep(1);
        anglCurr=get_compass();
        delta=fmod(anglDest-anglCurr,360);
        if (delta>180){delta=delta-360;}
        if (delta<-180){delta=delta+180;}
    }
}

//unused? 
void turn_exact_rel(float delta,float prec){
    float t0=get_compass();
    turn_exact_abs(fmod(t0+delta,360),prec);
}

//unused? 
void turn_exact_gyro(float delta,float prec){
    float anglCurr=-get_gyro();
    float anglDest=anglCurr+delta;
    while (abs(delta)>prec){
        turn_approx(delta);
        anglCurr=-get_gyro();
        delta=anglDest-anglCurr;
    }
}

//used
void forwardTimed(int seconds, int speed) {
	/* by Alix
	forwardTimed(1,200) : 10cm */
	set_tacho_speed_sp(sn_rwheel, speed);
	set_tacho_speed_sp(sn_lwheel, speed);
	//printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_lwheel, "run-forever");
	set_tacho_command(sn_rwheel, "run-forever");
	sleep(seconds);
	//printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_lwheel, "stop");
	set_tacho_command(sn_rwheel, "stop");
	//printf("[TACHO] function forward is over!\n");
}

int forward_sonar(float sonarThreshold){
	// old forward_sonar by JB
	float sonarVal = get_sonar();
	set_tacho_speed_sp(sn_rwheel, 400);
	set_tacho_speed_sp(sn_lwheel, 400);
	set_tacho_command(sn_lwheel, "run-forever");
	set_tacho_command(sn_rwheel, "run-forever");
	while (sonarVal > sonarThreshold) {
		sonarVal = get_sonar();
	}
	set_tacho_command(sn_lwheel, "stop");
	set_tacho_command(sn_rwheel, "stop");
}

int forward_sonar_timed(int rcycle, int lcycle, float sonarThreshold, int sec, int delta) {
	/* by Alix and JB 
	moves forward until the sonar detects an object. 
	If during sec seconds there was no object found, it stops, checks around if there are any.
	forward_sonar_timed(50, 50, 50.0, 2, 20);
	*/
	printf("in forward sonar \n");
	int timeup = 0;
    	float sonarVal = get_sonar();
	double C_PER_SECS;
	clock_t previous, current;
	previous = clock();
	printf("previous %lf \n", previous);
	sleep(1);
	current = clock();
	printf("current %lf \n", current);
	if (sonarVal > sonarThreshold) {
        	move_forever(rcycle, lcycle);
		while (sonarVal > sonarThreshold) {
			current = clock();
			printf("current %lf \n", current);
			sonarVal = get_sonar();
			timeup =(((double)(current - previous)/ CLOCKS_PER_SEC ) > sec);
			if (timeup) {
				printf("in2");
				move_forever(0,0);
				turn_approx(-delta);
				if (get_sonar() < sonarThreshold){
					return 0;
				} else {
					turn_approx(2*delta);
					if (get_sonar() < sonarThreshold){
						return 0;
					}
				}
				turn_approx(-delta);
				previous = clock();
				}
			
			move_forever(rcycle, lcycle);
			printf("previous %lf \n", previous);


		}
		
	}
	move_forever(0,0);
	return 0;
}

//not used 
int forward_Sonar2(int rcycle, int lcycle, float sonarThreshold, int msec, int delta) {
    // moves forward until it is close enough to an object
    int i=0;

    float sonarVal = get_sonar();
    if (sonarVal > sonarThreshold) {
        move_forever(rcycle, lcycle);
        while (sonarVal > sonarThreshold) {
            sonarVal = get_sonar();
            if (i>msec) {
		i=0;
                move_forever(0,0);
                turn_approx(90);
                sonarVal = get_sonar();
                if (sonarVal < sonarThreshold){
                    return 0;
                } else {
                    turn_approx(-180);
                    sonarVal = get_sonar();
                    if (sonarVal < sonarThreshold){
                        return 0;
                    }
                }
                move_forever(rcycle, lcycle);
            }
            i+=20;
        }
        move_forever(0,0);
    }
}

void turn_right() {
	// by JB
	printf("[TURNING] RIGHT\n");
	float gyroVal;
    float gyroValInitial;
	gyroValInitial = get_gyro();
	gyroVal = get_gyro();
	//printf("initial gyro value: %f\n", gyroValInitial);
	set_tacho_speed_sp(sn_lwheel, 50);
	set_tacho_speed_sp(sn_rwheel, -50);
	//printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_lwheel, "run-forever");
	set_tacho_command(sn_rwheel, "run-forever");
	while (abs(gyroVal - gyroValInitial) < 90) {
		gyroVal = get_gyro();
	}
	//printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_lwheel, "stop");
	set_tacho_command(sn_rwheel, "stop");
}

void turn_left() {
	// by JB
	printf("[TURNING] LEFT\n");
	float gyroVal;
    float gyroValInitial;
	gyroValInitial = get_gyro();
	gyroVal = get_gyro();
	//printf("initial gyro value: %f\n", gyroValInitial);
	set_tacho_speed_sp(sn_lwheel, -50);
	set_tacho_speed_sp(sn_rwheel, 50);
	//printf("[TACHO] starting tachos\n");
	set_tacho_command(sn_lwheel, "run-forever");
	set_tacho_command(sn_rwheel, "run-forever");
	while (abs(gyroVal - gyroValInitial) < 90) {
		gyroVal = get_gyro();
	}
	//printf("[TACHO] stopping tachos\n");
	set_tacho_command(sn_lwheel, "stop");
	set_tacho_command(sn_rwheel, "stop");
}

int forward_timed() {
	/*
		by JB
		robot goes forward until:
			1/ timer runs out (CHECK_TIMER seconds)
			2/ obstacle in front of robot
	*/
	clock_t start_t, check_t;
	start_t = clock();
	int timeIsUp = 0;
	//int initialGyro = get_gyro();
	//int checkGyro;
	int obstacleInFront = 0;
	//int localRCycle=50;
	//int localLCycle=50;
	int yPos = get_Y_position();
	int xPos = get_X_position();
	move_forever(35, 35);//localRCycle, localLCycle); //25
	while (!timeIsUp && !obstacleInFront && yPos>POS_THRESHOLD && xPos>POS_THRESHOLD) {
		yPos = get_Y_position();
		xPos = get_X_position();
		/*checkGyro = get_gyro();	
		if (checkGyro<initialGyro) {
			localRCycle++;
		}
		if (checkGyro>initialGyro) {
			localRCycle--;
		}
		if (checkGyro!=initialGyro) {
			move_forever(localRCycle, localLCycle);
		}*/
		check_t = clock();
		timeIsUp = ( ((double)(check_t - start_t) / CLOCKS_PER_SEC) > CHECK_TIMER); // should we use another timer ?
		obstacleInFront = isThereSomethingInFront();
	}
	move_forever(0,0);
	if (timeIsUp) {
		printf("[TIME] time is up, gotta check left\n");
		return 0;
	} else if (obstacleInFront) {
		printf("[OBSTACLES] FRONT is NOT clear, out of forward_timed\n"); // (or reached y=0
		return 1;
	} else {
		printf("[POSITION] reached one axis\n");
		forwardTimed(2, -100);
		return 2;
	}
}

int forward_sonar_jb() {
	/*
		by JB
		return 0 if obstacle in front
		return 1 if obstacle to the right/left
	*/
	clock_t start_t, check_t;
	start_t = clock();
	//check_t = clock();
	int timeIsUp = 0;
	int frontClear = 1;
	int sidesClear = 1;
	int sidesCheck;
	int yPos = get_Y_position();
	int xPos = get_X_position();
	/*int initialGyro = get_gyro();
	int checkGyro;
	int localRCycle=50;
	int localLCycle=50;*/
	printf("[MOTORS] starting motors\n");
	while (frontClear && sidesClear && yPos>POS_THRESHOLD && xPos>POS_THRESHOLD) {
		yPos = get_Y_position();
		xPos = get_X_position();
		/*checkGyro = get_gyro();	
		if (checkGyro<initialGyro) {
			localRCycle++;
		}
		if (checkGyro>initialGyro) {
			localRCycle--;
		}*/
		move_forever(35, 35);
		check_t = clock();
		timeIsUp = (((double)(check_t - start_t) / CLOCKS_PER_SEC) > CHECK_TIMER);
		if (timeIsUp) {
			printf("[TIME] time is up, gotta check the sides\n");
			sidesCheck = checkSides();
			if (sidesCheck!=1) sidesClear=0;
			timeIsUp = 0;
			start_t = clock();
		}
		frontClear = isThereSomethingInFront();
		frontClear = (!frontClear);
	}
	move_forever(0,0);
	if (!frontClear) {
		printf("[OBSTACLES] FRONT is NOT clear, out of forward_sonar_jb\n");	
		return 0; // obstacle in front
	} else if (!sidesClear) {
		printf("[OBSTACLES] OBSTACLE to the LEFT/RIGHT\n");
		return 1; // obstacle to the right/left
	}
	printf("[POSITION] reached one axis\n");
	forwardTimed(2,-100);
	return 0;
}

// ------------------------------ Shovel Functions

//not used
void open_shovel(){
    set_tacho_stop_action_inx(sn_shovel,TACHO_COAST);
    set_tacho_speed_sp(sn_shovel,200);
    set_tacho_ramp_up_sp(sn_shovel,0);
    set_tacho_ramp_down_sp(sn_shovel,0);
    set_tacho_position_sp(sn_shovel,-150);
    set_tacho_command_inx(sn_shovel,TACHO_RUN_TO_REL_POS);
}

//not used
void close_shovel(){
    set_tacho_stop_action_inx(sn_shovel,TACHO_COAST);
    set_tacho_speed_sp(sn_shovel,100);
    set_tacho_ramp_up_sp(sn_shovel,0);
    set_tacho_ramp_down_sp(sn_shovel,0);
    set_tacho_position_sp(sn_shovel,150);
    set_tacho_command_inx(sn_shovel,TACHO_RUN_TO_REL_POS);
}

void take_object(){
	/* by Henri and Alix
	When the robot is in front of a movable object (at dist 5cm)
	check if the robot has already taken 3 objects (maximum drop allowed) before taking it. 
	
	*/
	if (count_take < 3){
		count_take = count_take + 1;
		printf("[PELLE] opening pelle\n");//--------open pelle
		set_tacho_speed_sp(sn_shovel, -200);
		set_tacho_command(sn_shovel, "run-forever");
		sleep(2);
    		forwardTimed(1,100);
		printf("[PELLE] closing pelle\n");//-------close pelle
		set_tacho_command(sn_shovel, "stop");
		set_tacho_speed_sp(sn_shovel, 0);
		set_tacho_command(sn_shovel, "run-forever");
		sleep(2);
		set_tacho_command(sn_shovel, "stop");
	}
    	
}

void drop_object() {
	/* by Henri and Alix
	check if the robot has already dropped two objects (maximum drop allowed) and places the object behind it so it doesn't bump into it
	*/
	if (count_drop < 3){
		count_drop = count_drop + 1;
		turn_approx(-180);
    		forwardTimed(1,200);
		printf("[PELLE] opening pelle\n");//----------open pelle
		set_tacho_speed_sp(sn_shovel, -200);
		set_tacho_command(sn_shovel, "run-forever");
		sleep(2);
		forwardTimed(1,-200);//---------movebackward
    		turn_approx(90); //-------half turn
		printf("[PELLE] closing pelle\n");//----------close pelle
		set_tacho_command(sn_shovel, "stop");
		set_tacho_speed_sp(sn_shovel, 200);
		set_tacho_command(sn_shovel, "run-forever");
		sleep(2);
		set_tacho_command(sn_shovel, "stop");
	}
}

// ------------------------------ Communication with Server

void send_position(int16_t x,int16_t y){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=MSG_POSITION;
	str[5]=x;
    if (x<0){str[6]=0xff;} else {str[6]=0x00;}
	str[7]=y;
    if (y<0){str[8]=0xff;} else {str[8]=0x00;}
	write(s,str,9);
	Sleep(1000);
}
void send_mapdata(int16_t x,int16_t y,char r,char g,char b){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=5;
    str[5]=x;
    if (x<0){str[6]=0xff;}else{str[6]=0x00;};
    str[7]=7;
    if (y<0){str[8]=0xff;}else{str[8]=0x00;};
	str[9]=r;
	str[10]=g;
	str[11]=b;
	write(s,str,12);
	Sleep(1000);
}
void send_mapdone(){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=6;
	write(s,str,5);
	Sleep(1000);
}
void send_obstacle(int act,uint16_t x,uint16_t y){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=7;
    str[5]=act;
	str[6]=x;
    if (x<0){str[7]=0xff;} else {str[7]=0x00;}
	str[8]=y;
    if (y<0){str[9]=0xff;} else {str[9]=0x00;}
	write(s,str,10);
	Sleep(1000);
}
void send_position_pos(int16_t x,int16_t y){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=MSG_POSITION;
    str[5]=x;
    str[6]=0x00;
    str[7]=y;
    str[8]=0x00;
	write(s,str,9);
	Sleep(1000);
}
void send_mapdata_pos(int16_t x,int16_t y,char r,char g,char b){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=5;
    str[5]=x;
    str[6]=0x00;
    str[7]=y;
    str[8]=0x00;
	str[9]=r;
	str[10]=g;
	str[11]=b;
	write(s,str,12);
	Sleep(100);
}
void send_obstacle_pos(int act,uint16_t x,uint16_t y){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=7;
    str[5]=act;
    str[6]=x;
    str[7]=0x00;
    str[8]=y;
    str[9]=0x00;
	write(s,str,10);
	Sleep(1000);
}

// ------------------------------ Robot Position

void* Update_position2(){
	// by Henri
	// function with the thread in charge of updating the robot position
	int timeIsUp = 0;
    /* get the position every seconds */
    get_sensor_value0(sn_gyro, &thetaCompasInit);
    sleep(0.3);
    get_sensor_value0(sn_gyro, &thetaCompasInit);
    get_tacho_position(sn_rwheel, &positionMotorR2);
    append_pos_file(Xpos, Ypos, 0);
	clock_t start_t = clock();
	clock_t check_t;
    /* debut SC1 */
    pthread_mutex_lock(&mutex);
    while(ThreadSituation == 0){
        pthread_mutex_unlock(&mutex);
        /* fin SC1 */

        sleep(0.3);
        get_tacho_speed(sn_lwheel, &speedMotorL);
        get_tacho_speed(sn_rwheel, &speedMotorR);
        positionMotorR1 = positionMotorR2;
        get_tacho_position(sn_rwheel, &positionMotorR2);
        get_sensor_value0(sn_gyro, &thetaCompas);
        thetaCompas = (thetaCompas-thetaCompasInit)*pi/180;
	//printf("\n           thetaCompas = %f",thetaCompas/pi*180);

        /* debut SC1 */
        pthread_mutex_lock(&mutex);
        if ((abs(speedMotorR) > 5) && (abs(speedMotorL) > 5)) {
            if (speedMotorR/speedMotorL > 0) {
                /*printf("\nrobot is moving");*/
                Xdef=Xdef+sin(thetaCompas)*(positionMotorR2-positionMotorR1)*lambda;
                Ydef=Ydef+cos(thetaCompas)*(positionMotorR2-positionMotorR1)*lambda;
                Xpos=(int) round(Xdef/5);
                Ypos=(int) round(Ydef/5);
            } else {
                //printf("           robot is turning");
            }
        }
        check_t = clock();
        timeIsUp = ( ((double)(check_t - start_t) / CLOCKS_PER_SEC) > 2);
        if (timeIsUp) {
        	printf("[SERVER] Xpos = %d, Ypos = %d\n", Xpos, Ypos);
        	start_t = clock();
        } 
        //printf("\n Xdef,Ydef = %f,%f       X,Y = %d,%d\n",Xdef,Ydef,Xpos,Ypos);
        if ((Xpos != XposOld) || (Ypos != YposOld)) {
            XposOld = Xpos;
            YposOld = Ypos;
            append_pos_file(Xpos, Ypos, 0);
        }
    }
    pthread_mutex_unlock(&mutex);
    /* fin SC1 */
    pthread_exit(NULL);
}



int get_X_position() {
	// by Henri
	// returns X without conflit with the thread updating the robot position
	int X;
	// debut SC1
    	pthread_mutex_lock(&mutex);
    	X=Xpos;
        pthread_mutex_unlock(&mutex);
        // fin SC1
	return X;
}
int get_Y_position() {
	// by Henri
	// returns X without conflit with the thread updating the robot position
	int Y;
	// debut SC1
    	pthread_mutex_lock(&mutex);
    	Y=Ypos;
        pthread_mutex_unlock(&mutex);
        // fin SC1
	return Y;
}

// ------------------------------ Detection Functions

int detect_movable() {
	// by JB
	// returns 1 if obect is movable (ie color = red) and 0 otherwise
	int color = get_color();
	if (color==5) {
		return 1;
	} else {
		return 0;
	}
}

void position_in_front() {
	/*
		by JB
		supposed to return the position right in front of the robot but it keeps on making some mistakes...
	*/
	float theta;
	int xFront, yFront;
	int actualX = get_X_position();
	int actualY = get_Y_position();
	theta = get_gyro();
	theta = (theta-thetaCompasInit)*pi/180;
	xFront = (int) round((Xdef/5)+sin(theta));
	yFront = (int) round((Ydef/5)+cos(theta));
	printf("[OBJECT POSITION] front is at x=%d, y=%d (robot is at x=%d, y=%d)\n", xFront, yFront, actualX, actualY);
	append_pos_file(xFront, yFront, 1);
}

int isThereSomethingInFront() {
	/*
		by JB
		returns 1 if an obstacle is close enough to the sonar sensor, 0 otherwise
	*/
	float sonarVal;
	clock_t start_t = clock();
	clock_t check_t;
	int timeIsUp = 0;
	sonarVal = get_sonar();
	//int res = (sonarVal < SONAR_THRESHOLD);
	//printf("[SONAR] sonarVal = %f ; so res = %d\n", sonarVal, res);
	if (sonarVal < SONAR_THRESHOLD) {
		printf("[OBSTACLES] there is something in FRONT\n");
		move_forever(20, 20);
		while (sonarVal > 40) {
			sonarVal = get_sonar();
			check_t = clock();
			timeIsUp = ( ((double)(check_t - start_t) / CLOCKS_PER_SEC) > 3); 
			if (timeIsUp) {
				printf("[OBJECT] unidentified object, took to long to detect it\n");
				return 1;
			}
		}
		int movable = detect_movable();
		if (!movable) {
			printf("[OBJECT] non movable\n");
			position_in_front();
			return 1;
		}
		printf("[OBJECT] movable\n");
		position_in_front();
		return 1;
	}
	return 0;//res;
}

int checkSides() {
	/*
		by JB
		return 1 if sides are clear
		return 0 if obstacle left
		return 2 if obstacle right
	*/
	int front;
	move_forever(0,0);
	turn_approx(10); // left ?
	front = isThereSomethingInFront();
	if (front) {
		printf("[CHECKSIDES] obstacle to the left\n");
		return 0;
	}
	turn_approx(10); // left ?
	front = isThereSomethingInFront();
	if (front) {
		printf("[CHECKSIDES] obstacle to the left\n");
		return 0;
	}
	turn_approx(10); // left ?
	front = isThereSomethingInFront();
	if (front) {
		printf("[CHECKSIDES] obstacle to the left\n");
		return 0;
	}
	// right
	turn_approx(-40); // right?
	front = isThereSomethingInFront();
	if (front) {
		printf("[CHECKSIDES] obstacle to the right\n");
		return 2;
	}
	turn_approx(-10); // right?
	front = isThereSomethingInFront();
	if (front) {
		printf("[CHECKSIDES] obstacle to the right\n");
		return 2;
	}
	turn_approx(-10); // right?
	front = isThereSomethingInFront();
	if (front) {
		printf("[CHECKSIDES] obstacle to the right\n");
		return 2;
	}
	turn_approx(30); // now facing back in front
	return 1; // sides are clear
}

int detect_nonmovable(int sonarThreshold){
	/* by Alix
	detects non movable objects. To get the width of the object, it turns around it until it gets back to its first position
	*/
	int x = get_X_position();
	int y = get_Y_position();
	printf(" init pos %d %d \n", x, y);
	float sonarVal;
	float sonarValF;
	turn_left();
	sonarVal = get_sonar();
	if (sonarVal > sonarThreshold){
		forwardTimed(1,200);
		turn_right();
	}
	sonarVal = get_sonar();
	int a = get_X_position();
	int b = get_Y_position();
	printf(" pos %d %d \n", a, b);
	while (a!=x || b!=y){ 
		printf("in1\n");
		if (sonarVal < sonarThreshold) {
			printf("in2\n");
			turn_left();
			sonarValF = get_sonar();
			if (sonarValF > sonarThreshold){
				printf("etape2\n");
				forwardTimed(1,300);
				turn_right();
				printf("fin etape2\n");
			}
			printf("fin etape2\n");
		}
		else{
			printf("else2\n");
			forwardTimed(1,300);
			turn_right();
		}
		printf("out2\n");
		sonarVal = get_sonar();
		a = get_X_position();
		b = get_Y_position();
	}
	printf("out1, a fait le tour ");
	return 0;
}

// ------------------------------ More complex Movements

int forward_while_checking_left() {
	/*
		by JB
		robot goes forward for a specific amount of time (CHECK_TIMER) then checks its left side.
	*/
	//int blocked = 0;
	int obstacleInFront = 0;
	int obstacleLeft = 0;
	while (1) { //!blocked) {
		obstacleInFront = forward_timed(); // 0: timeIsUp, 1: obstacleInFront, 2: y<0
		if (obstacleInFront==2) {
			return 0; // meaning y<0
		}
		turn_left();
		obstacleLeft = isThereSomethingInFront();
		turn_right();
		if (!obstacleLeft) {
			printf("[OBSTACLES] LEFT is CLEAR\n");
			return 2; // left is free
		} else if (obstacleInFront==1) {
			printf("[OBSTACLES] FRONT is NOT clear, out of forward_while_checking_left\n");
			return 1; // obstacle in front
		}
		printf("[DEBUG] should not be in here!\n");
		//blocked = (obstacleInFront && obstacleLeft);
	}
	//return 1;
}

int go_around_map() {
	/*
		by JB
		while the robot is not back in the start area (y = 0), it keeps on going around the map starting in the bottom left corner.
		To do so, it always tries to go to the left, forward otherwise (and right if both front and left are blocked).
	*/
	turn_left();
	forward_sonar_jb();
	printf("[POSITION] should be in bottom left corner (x=0, y=0), facing towards negative y values\n");
	turn_right();
	printf("[POSITION] should be in bottom left corner (x=0, y=0), facing towards positive x values\n");
	forward_sonar_jb();
	printf("[POSITION] should be in either top left corner or in front of an obstacle\n");
	int yPos = get_Y_position();
	int xPos = get_X_position();
	int obstacleDir;
	while (yPos>POS_THRESHOLD && xPos>POS_THRESHOLD) {
		obstacleDir = forward_while_checking_left();
		if (obstacleDir==2) turn_left();
		if (obstacleDir==1) turn_right();
		yPos = get_Y_position();
		xPos = get_X_position();
	}
	printf("x = %d, y = %d\n", xPos, yPos);
	return 0;
}

int explore_mountain() {
	/*
		by JB
		robot will explore the map like this: 
		 __    __
		|  |  |  |
		|  |  |  |
		|  |  |  |
		|  |  |  |
		|  |  |  |  ^
		|  |  |  |  |
		|  |__|  |__|
		o  
	*/
	int stuck = 0;
	int towardsNegX = 1;
	int frontNotClear = 0;
	while (!stuck) {
		forward_sonar_jb();
		if (towardsNegX) {
			turn_right();
			frontNotClear = isThereSomethingInFront();
			while (frontNotClear) {
				turn_right();
				forwardTimed(2,200); //10cm
				turn_left();
				frontNotClear = isThereSomethingInFront();
			}
			forwardTimed(2,200); //10cm
			turn_right();
		} else {
			turn_left();
			frontNotClear = isThereSomethingInFront();
			while (frontNotClear) {
				turn_left();
				forwardTimed(2,200); //10cm
				turn_right();
				frontNotClear = isThereSomethingInFront();
			}
			forwardTimed(2,200); //10cm
			turn_left();
		}
		towardsNegX = !towardsNegX;
	}
	return 0;
}

// ------------------------------ Mapping 

void find_corners() {
	/*
		by JB
		recovers all of the past coordinates of the robots from the Position text file and deduce the possible corners of a rectangular map.
	*/
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int x; int y;
    char *token;

  	posFile = fopen("pos.txt", "r");
    if (posFile == NULL)
        exit(1);

    while ((read = getline(&line, &len, posFile)) != -1) {
		x = -1000;
		y = -1000;
		token = strtok(line, ",");
    	while(token) {
        	if (x==-1000) {
        		x = atoi(token);
        	} else if (y==-1000) {
        		y = atoi(token);
        	}
        	token = strtok(NULL, ",");
   		}
   		if (x<minX) minX=x;
		if (x>maxX) maxX=x;
		if (y<minY) minY=y;
		if (y>maxY) maxY=y;
    }
    fclose(posFile);
    if (line) free(line);
	printf("Bottom left : %d, %d\n", minX, minY);
	printf("Bottom right: %d, %d\n", maxX, minY);
	printf("Top left    : %d, %d\n", minX, maxY);
	printf("Top right   : %d, %d\n", maxX, maxY);
	printf("\n");
}

int create_map(int x0, int y0) {
	/*
		by JB
		Given the map corners and all of the robot's past coordinates, this script creates a map of where the robot went and where it did not.
		We need to add the send_map_data_message function in here so that it sends the right map messages to the server.
	*/
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int x; int y;
    int xFile; int yFile;
    int nature;
	int found = 0;
	char *token;
	find_corners();
  	posFile = fopen("pos.txt", "r");
    if (posFile == NULL){
        printf("[ERROR] create_map(): couldn't open pos.txt\n");
        exit(1);
    }
    for (y=maxY; y>minY-1; y--) {
    	for (x=minX; x<maxX+1; x++) {
    		found = 0;
    		posFile = fopen("pos.txt", "r");
			while (!found && ((read = getline(&line, &len, posFile)) != -1)) {
				xFile = -1000; // value that should never be reached
				yFile = -1000; // value that should never be reached
				nature = -1;
				token = strtok(line, ",");
				while(token) {
					if (xFile==-1000) {
						xFile = atoi(token);
					} else if (yFile==-1000) {
						yFile = atoi(token);
					} else if (nature==-1) {
						nature = atoi(token);
					}
					token = strtok(NULL, ",");
		   		}
				if (x==xFile && y==yFile) {
					found = 1;
				}
			}
			if (x==xFile && y==yFile){
				if (x==x0 && y==y0) {
					printf(" 0 ");
				} else if (nature==0) {
					printf(" X ");// send_mapdata_pos((int16_t) x, (int16_t) y, 255, 255, 255); //### WHITE: %d, %d\n", x, y);
				} else {
					printf(" + "); //this is an object
				}
			} else {
				printf(" . ");// send_mapdata_pos((int16_t) x, (int16_t) y, 0, 0, 0); //BLACK: %d, %d\n", x, y);
			}
			fclose(posFile);
		}
		printf("\n");
	}
	printf("\n");
    return 0;
}

int append_pos_file(int x, int y, int nature) {
	/*
		by JB and Alix
		When the robot is at (x,y), this script writes the coordinates in the Position text file that will be used to build the map at the end of the exploration. 
		nature = 0 is it is robot position, 1 if it is an object position
	*/
   	posFile = fopen("pos.txt", "a");
    if (posFile != NULL){
    	fprintf(posFile, "%d,%d,%d\n",x,y,nature);
        fclose(posFile);
    }
    else {
        printf("[ERROR] append_pos_file(%d,%d): couldn't open pos.txt",x,y);
    }
    return 0;
}

int known_point(int checkX, int checkY) {
	/*
		by JB
		checks if the robot already went through this (x, y) coordinates.
		returns 1 if coordinates in file already, 0 otherwise.
	*/
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int x; int y;
    char *token;
    int count=0;

  	posFile = fopen("pos.txt", "r");
    if (posFile == NULL) exit(1);

    while ((read = getline(&line, &len, posFile)) != -1) {
  		x = -1000;
  		y = -1000;
  		token = strtok(line, ",");
    	while(token) {
        	if (x==-1000) {
        		x = atoi(token);
        	} else if (y==-1000) {
        		y = atoi(token);
        	}
        	token = strtok(NULL, ",");
   		}
      if (x==checkX && y==checkY && count>0) return 1;
      if (x==checkX && y==checkY) count++;
    }
    fclose(posFile);
    if (line) free(line);
  	return 0;
}

void set_initial_coordinates(int x, int y) {
	Xdef= (float) x * 5.0;
	Ydef= ((float) y) * 5.0;
	Xpos=x, Ypos=y, XposOld=x, YposOld=y; //0
	printf("[POSITION] just changed the initial coordinates to (%d,%d)\n", x, y);
}

// Nino map ?
void print_int_list(int* list, int len){
	// by Nino
    printf("[");
    for (int i=0; i<len-1; i++){
        printf("%d,",list[i]);
    }
    printf("%d]\n",list[len-1]);
}
int pos_exists(int x, int y, int* x_list, int* y_list, int len){
	// by Nino
    int i=0;
    while (i<len){
        if (x==x_list[i] && y==y_list[i]){
            return 1;
        }
        i++;
    }
    return 0;
}
int send_map_from_file(){
	// by Nino
    FILE* f=fopen("pos.txt","r");
    int nb_lines=0;
    char c=getc(f);
    while(c!=EOF){
        if (c=='\n'){
            nb_lines++;
        }
        c=getc(f);
    }
    fclose(f);
    printf("number of lines: %d\n",nb_lines);
    
    int x_list[nb_lines];
    int y_list[nb_lines];
    char x[10];
    char y[10];
    int xx=0,yy=0,x_i=0,y_i=0;
    char is_x=1;
    
    f=fopen("pos.txt","r");
    c=getc(f);
    while(c!=EOF){
        switch(c){
            case '\n':
                is_x=1;
                y_list[y_i]=atoi(y);
                strcpy(y,"");
                yy=0;
                break;
            case ',':
                is_x=0;
                x_list[x_i]=atoi(x);
                strcpy(x,"");
                xx=0;
                break;
            default:
                if (is_x){
                    x[xx]=c;
                    xx++;
                }
                else {
                    y[yy]=c;
                    yy++;
                }
        }
        c=getc(f);
    }
    
    // DEBUG
    printf("list of coordinates:\n");
    print_int_list(x_list,nb_lines);
    print_int_list(y_list,nb_lines);
    
    int xmin=x_list[0],xmax=x_list[0],ymin=y_list[0],ymax=y_list[0];
    for (int i=1; i<nb_lines; i++){
        if (x_list[i]<xmin){
            xmin=x_list[i];
        }
        if (x_list[i]>xmax){
            xmax=x_list[i];
        }
        if (y_list[i]<ymin){
            ymin=y_list[i];
        }
        if (y_list[i]>ymax){
            ymax=y_list[i];
        }
    }
    
    // DEBUG
    printf("xmin,xmax,ymin,ymax = %d,%d,%d,%d\n",xmin,xmax,ymin,ymax);
    
    printf("map:\n");
    for (int y=ymin; y<=ymax; y++){
        for (int x=xmin; x<=xmax; x++){
            if (pos_exists(x,y,x_list,y_list,nb_lines)){
                send_mapdata(x,y,0,0,0);
                printf(" X ");
            }
            else {
                send_mapdata(x,y,255,255,255);
                printf(" . ");
            }
        }
        printf("\n");
    }
    send_mapdone();
    
    printf("map sent to the server\n");
    return 0;
}







//********************

void newforwardSonar(float sonarThreshold, int speed) {
    float sonarVal = get_sonar();
    if (sonarVal > sonarThreshold+10) {
        move_forever(speed, speed);
        while (sonarVal > sonarThreshold) {
            sonarVal = get_sonar();
        }
        move_forever(0, 0);
    }
}

void newbackwardSonar(float sonarThreshold, float speed) {
    float sonarVal = get_sonar();
    if (sonarVal < sonarThreshold-10) {
        move_forever(speed, speed);
        while (sonarVal < sonarThreshold) {
            sonarVal = get_sonar();
        }
        move_forever(0, 0);
    }
}

void newforwardTimed(int seconds, int speed) {
    move_forever(speed, speed);
    sleep(seconds);
    move_forever(0, 0);
}

void newtake_object() {
    newforwardSonar(80.0, 100);
    printf("[PELLE] opening pelle\n");//--------open pelle
    set_tacho_speed_sp(sn_shovel, -80);
    set_tacho_command(sn_shovel, "run-forever");
    sleep(2);
    //set_tacho_command(sn_shovel, "stop");
    newforwardTimed(2, 100);//---------moveforward
    printf("[PELLE] closing pelle\n");//-------close pelle
    set_tacho_command(sn_shovel, "stop");
    set_tacho_speed_sp(sn_shovel, 80);
    set_tacho_command(sn_shovel, "run-forever");
    sleep(2);
    set_tacho_command(sn_shovel, "stop");
}

void newdrop_object() {
    turn_approx(180);//-------half turn
    newforwardTimed(2, 80);//---------moveforward
    printf("[PELLE] opening pelle\n");//----------open pelle
    set_tacho_speed_sp(sn_shovel, -80);
    set_tacho_command(sn_shovel, "run-forever");
    sleep(2);
    //set_tacho_command(sn_shovel, "stop");
    newforwardTimed(2, -80);//---------movebackward
    turn_approx(-180);//-------half turn
    printf("[PELLE] closing pelle\n");//----------close pelle
    set_tacho_command(sn_shovel, "stop");
    set_tacho_speed_sp(sn_shovel, 80);
    set_tacho_command(sn_shovel, "run-forever");
    sleep(2);
    set_tacho_command(sn_shovel, "stop");
}

void newisThisABall(float delta) {
    newforwardSonar(50.0, 100);
    turn_approx(delta);
    sleep(0.5);
    float sonarValG = get_sonar();
    turn_approx(-delta);
    sleep(0.5);
    turn_approx(-delta);
    sleep(0.5);
    float sonarValD = get_sonar();
    turn_approx(delta);
    if ((sonarValG > 150) && (sonarValD > 150)){
        printf("movable object\n");
        newtake_object();
        newdrop_object();
    }else {
        printf("UNmovable object\n");
        turn_approx(-90);
    }
}

void deplacement(int sonarThreshold , int speed ) {
    int Xinit=get_X_position();
    int Yinit=get_Y_position();
    //int sonarThreshold = 60;
    //int speed = 200;

    turn_approx(90);
    newforwardSonar(sonarThreshold, speed);

    int lastMove = 50;
    int lastTurn = 90;
    int firstOrientation = 90;

    while(get_X_position()!=Xinit && get_Y_position()!=Yinit) {
        if (lastMove == 50) {
            //do newforward_sonar
            newforwardSonar(sonarThreshold, speed);
            //newisThisABall(25);

            lastTurn*=-1;
            turn_approx(lastTurn);
            if (isThereSomethingInFront()) {
                lastTurn*=-1;
                turn_approx(lastTurn);
                if (isThereSomethingInFront()) {
                    turn_approx(lastTurn);
                } else {
                    lastMove=01;
                }
            } else {
                lastMove=01;
            }
        }else {
            //do move_a_bit
            newforwardTimed(2.5, 80);

            turn_approx(lastTurn);
            if (isThereSomethingInFront()) {
                lastTurn*=-1;
                turn_approx(2*lastTurn);
                if (isThereSomethingInFront()) {
                    lastTurn*=-1;
                    turn_approx(lastTurn);
                    if (isThereSomethingInFront()) {
                        turn_approx(2*lastTurn);
                    }
                } else {
                    lastMove=50;
                }
            } else {
                lastMove=50;
            }
        }

    }
}



void* test_Update_position2(){
	// by Henri
    /* get the position while moving */
    pthread_t myUpdate_position;
    pthread_create(&myUpdate_position,NULL,Update_position2,NULL);

    //THE MOVEMENT FUNCTIONS___________________________________________________
    deplacement(70, 200);
    //THE END OF THE INITIALISATION____________________________________________

    /* debut SC2 */
    pthread_mutex_lock(&mutex);
    ThreadSituation = 1;
    pthread_mutex_unlock(&mutex);
    /* fin SC2 */

    pthread_join(myUpdate_position,NULL);
    pthread_mutex_destroy(&mutex);
}

//**********************
