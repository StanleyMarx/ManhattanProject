#define PORT_RIGHTWHEEL 66
#define PORT_SHOVEL 67
#define PORT_LEFTWHEEL 68

uint8_t sn;
uint8_t sn_compass;
uint8_t sn_gyro;
uint8_t sn_sonar;
uint8_t sn_rwheel;
uint8_t sn_lwheel;
uint8_t sn_shovel;
uint8_t sn_color;
int max_speed;
const char const *colors[] = { "?", "BLACK", "BLUE", "GREEN", "YELLOW", "RED", "WHITE", "BROWN" };
int minX = 0;
int minY = 0;
int maxX = 0;
int maxY = 0;

FILE* posFile = NULL;

const int CHECK_TIMER = 3;
const float SONAR_THRESHOLD = 50.0;

int count_take = 1; //begin with an obstacle in the shovel
int count_drop = 0;

float Xdef=0.0, Ydef=0.0;
int Xpos=0, Ypos=0, XposOld=0, YposOld=0; //0
float pi=3.14159265;
int speedMotorL, speedMotorR;
int positionMotorR1, positionMotorR2;
float thetaCompas, thetaCompasInit;
float lambda=1/21.21*86/35;
pthread_mutex_t mutex;
int ThreadSituation=0;

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

void move_forever(int rcycle,int lcycle){
    set_tacho_duty_cycle_sp(sn_rwheel,rcycle);
    set_tacho_duty_cycle_sp(sn_lwheel,lcycle);
    set_tacho_command_inx(sn_rwheel,TACHO_RUN_DIRECT);
    set_tacho_command_inx(sn_lwheel,TACHO_RUN_DIRECT);
}
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
void move_real_debug(int r,int l){
    int time_ratio=4500;
    move_real(r,l,100);
    usleep(time_ratio*(abs(r)+abs(l)));
}

void open_shovel(){
    set_tacho_stop_action_inx(sn_shovel,TACHO_COAST);
    set_tacho_speed_sp(sn_shovel,200);
    set_tacho_ramp_up_sp(sn_shovel,0);
    set_tacho_ramp_down_sp(sn_shovel,0);
    set_tacho_position_sp(sn_shovel,-150);
    set_tacho_command_inx(sn_shovel,TACHO_RUN_TO_REL_POS);
}
void close_shovel(){
    set_tacho_stop_action_inx(sn_shovel,TACHO_COAST);
    set_tacho_speed_sp(sn_shovel,100);
    set_tacho_ramp_up_sp(sn_shovel,0);
    set_tacho_ramp_down_sp(sn_shovel,0);
    set_tacho_position_sp(sn_shovel,150);
    set_tacho_command_inx(sn_shovel,TACHO_RUN_TO_REL_POS);
}

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

void turn_approx(float angle){
    float ratio=2.66;
    move_real_debug(ratio*angle,-ratio*angle);
}

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
void turn_exact_rel(float delta,float prec){
    float t0=get_compass();
    turn_exact_abs(fmod(t0+delta,360),prec);
}
void turn_exact_gyro(float delta,float prec){
    float anglCurr=-get_gyro();
    float anglDest=anglCurr+delta;
    while (abs(delta)>prec){
        turn_approx(delta);
        anglCurr=-get_gyro();
        delta=anglDest-anglCurr;
    }
}

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

/* communication client/serveur */
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

void print_int_list(int* list, int len){
    printf("[");
    for (int i=0; i<len-1; i++){
        printf("%d,",list[i]);
    }
    printf("%d]\n",list[len-1]);
}
int pos_exists(int x, int y, int* x_list, int* y_list, int len){
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
    // file: pos.txt
    
    int x_list[10000];
    int y_list[10000];
    
    FILE* pos_file=fopen("pos.txt","r");
    if (!pos_file){
        printf("[ERROR] print_map(): couldn't open pos.txt in read mode\n");
    }
    /*debug*/printf("file opened. reading file...\n");
    
    char is_x=1;
    int i_x=0;
    int i_y=0;
    char c=getc(pos_file);
    while (c!=EOF){
        switch(c){
            case ',':
                is_x=0;
                break;
            case '\n':
                is_x=1;
                break;
            default:
                if (is_x){
                    x_list[i_x]=c-48;
                    i_x++;
                } else {
                    y_list[i_y]=c-48;
                    i_y++;
                }
                break;
        }
        c=getc(pos_file);
    }
    
    /*debug*/printf("file read: x_list and y_list now have a length of %d and %d\n",i_x,i_y);
    fclose(pos_file);
    /* at this point, x_list and y_list contains the list of coordinates, and i_x is the length of these lists */
    if (i_x==0){
        printf("no position to send - aborting send_map_from_file()\n");
        return 1;
    }
    
    // DEBUG
    printf("list of coordinates:\n");
    print_int_list(x_list,i_x);
    print_int_list(y_list,i_y);
    
    int xmin=x_list[0],xmax=x_list[0],ymin=y_list[0],ymax=y_list[0];
    for (int i=1; i<i_x; i++){
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
            if (pos_exists(x,y,x_list,y_list,i_x)){
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
int create_map() {
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
	int found = 0;
	char *token;
	//char map[maxY-minY][maxX-minX];
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
				token = strtok(line, ",");
				while(token) {
					if (xFile==-1000) {
						xFile = atoi(token);
					} else if (yFile==-1000) {
						yFile = atoi(token);
					}
					token = strtok(NULL, ",");
		   		}
				if (x==xFile && y==yFile) {
					found = 1;
				}
				//free(line);
			}
			if (x==xFile && y==yFile){
				if (x==0 && y==0) {
					printf(" 0 ");
				} else {
					printf(" X ");//### WHITE: %d, %d\n", x, y);
				}
			} else {
				printf(" . ");//BLACK: %d, %d\n", x, y);
			}
			fclose(posFile);
		}
		printf("\n");
	}
	printf("\n");
    return 0;
}
int append_pos_file(int x, int y) {
	/*
		by JB and Alix
		When the robot is at (x,y), this script writes the coordinates in the Position text file that will be used to build the map at the end of the exploration.
	*/
   	posFile = fopen("pos.txt", "a");
    if (posFile != NULL){
    	fprintf(posFile, "%d,%d\n",x,y);
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

void* Update_position2(){
    /* get the position every seconds */
    get_sensor_value0(sn_gyro, &thetaCompasInit);
    sleep(0.3);
    get_sensor_value0(sn_gyro, &thetaCompasInit);
    get_tacho_position(sn_rwheel, &positionMotorR2);
    append_pos_file(Xpos, Ypos);

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
        //printf("\n Xdef,Ydef = %f,%f       X,Y = %d,%d\n",Xdef,Ydef,Xpos,Ypos);
        if ((Xpos != XposOld) || (Ypos != YposOld)) {
            XposOld = Xpos;
            YposOld = Ypos;
            append_pos_file(Xpos, Ypos);
        }
    }
    pthread_mutex_unlock(&mutex);
    /* fin SC1 */
    pthread_exit(NULL);
}

void* test_Update_position2(){
    /* get the position while moving */
    pthread_t myUpdate_position;
    pthread_create(&myUpdate_position,NULL,Update_position2,NULL);

    //THE MOVEMENT FUNCTIONS___________________________________________________
    move_forever(40,40);
    printf("\nmoving\n");
    sleep(5);
    move_forever(0,0);
    printf("\turning\n");
    turn_approx(90);
    move_forever(40,40);
    printf("\nmoving\n");
    sleep(5);
    move_forever(0,0);
    //THE END OF THE INITIALISATION____________________________________________

    /* debut SC2 */
    pthread_mutex_lock(&mutex);
    ThreadSituation = 1;
    pthread_mutex_unlock(&mutex);
    /* fin SC2 */

    pthread_join(myUpdate_position,NULL);
    pthread_mutex_destroy(&mutex);
}

int get_X_position() {
	int X;
	// debut SC1
    	pthread_mutex_lock(&mutex);
    	X=Xpos;
        pthread_mutex_unlock(&mutex);
        // fin SC1
	return X;
}
int get_Y_position() {
	int Y;
	// debut SC1
    	pthread_mutex_lock(&mutex);
    	Y=Ypos;
        pthread_mutex_unlock(&mutex);
        // fin SC1
	return Y;
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

int detect_movable() {
	// returns 1 if obect is movable (ie color = red) and 0 otherwise
	int color = get_color();
	if (color==5) {
		return 1;
	} else {
		return 0;
	}
}

int detect_nonmovable(int sonarThreshold){
	int x = get_X_position();
	int y = get_Y_position();
	printf(" init pos %d %d \n", x, y);
	float sonarVal;
	float sonarValF;
	turn_approx(90);
	sonarVal = get_sonar();
	if (sonarVal > sonarThreshold){
		forwardTimed(1,200);
		turn_approx(-90);
	}
	sonarVal = get_sonar();
	int a = get_X_position();
	int b = get_Y_position();
	printf(" pos %d %d \n", a, b);
	while (1){ 
		printf("in1");
		if (sonarVal < sonarThreshold) {
			printf("in2");
			turn_approx(90);
			sonarValF = get_sonar();
			if (sonarValF > sonarThreshold){
				printf("etape2\n");
				forwardTimed(1,200);
				turn_approx(-90);
				printf("fin etape2\n");
			}
			printf("fin etape2\n");
			sonarVal = get_sonar();
		}
		printf("out2");
		turn_approx(-90);
		a = get_X_position();
		b = get_Y_position();
	}
	printf("out1, a fait le tour ");
	return 0;
}
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

// SIGNATURES
/*
int forward_timed();
int forward_while_checking_left();
int go_around_map();
int isThereSomethingInFront();
int forward_sonar_jb();
int checkSides();
*/

// GO AROUND MAP
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


int isThereSomethingInFront() {
	/*
		by JB
		returns 1 if an obstacle is close enough to the sonar sensor, 0 otherwise
	*/
	float sonarVal;
	sonarVal = get_sonar();
	//int res = (sonarVal < SONAR_THRESHOLD);
	//printf("[SONAR] sonarVal = %f ; so res = %d\n", sonarVal, res);
	if (sonarVal < SONAR_THRESHOLD) {
		printf("[OBSTACLES] there is something in FRONT\n");
	}
	return (sonarVal < SONAR_THRESHOLD);//res;
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
	turn_approx(30); // left ?
	front = isThereSomethingInFront();
	if (front) {
		printf("[CHECKSIDES] obstacle to the left\n");
		return 0;
	}
	turn_approx(-60); // right?
	front = isThereSomethingInFront();
	if (front) {
		printf("[CHECKSIDES] obstacle to the right\n");
		return 2;
	}
	turn_approx(30); // now facing back in front
	return 1; // sides are clear
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
	move_forever(25, 25);//localRCycle, localLCycle);
	while (!timeIsUp && !obstacleInFront && yPos>-3) {
		yPos = get_Y_position();
		/*checkGyro = get_gyro();	
		if (checkGyro<initialGyro) {
			localRCycle++;
		}
		if (checkGyro>initialGyro) {
			localLCycle++;
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
	}
	printf("[OBSTACLES] FRONT is NOT clear, out of forward_timed (or reached y=0)\n");
	return 1; // there is something in front of the robot
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
	/*int initialGyro = get_gyro();
	int checkGyro;
	int localRCycle=50;
	int localLCycle=50;*/
	printf("[MOTORS] starting motors\n");
	while (frontClear && sidesClear && yPos>-3) {
		yPos = get_Y_position();
		/*checkGyro = get_gyro();	
		if (checkGyro<initialGyro) {
			localRCycle++;
		}
		if (checkGyro>initialGyro) {
			localLCycle++;
		}*/
		move_forever(25, 25);//localRCycle, localLCycle);
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
	}
	printf("[OBSTACLES] OBSTACLE to the LEFT/RIGHT (or reached y=0)\n");
	return 1; // obstacle to the right/left
}

int forward_while_checking_left() {
	/*
		by JB
		robot goes forward for a specific amount of time (CHECK_TIMER) then checks its left side.
	*/
	//int blocked = 0;
	int obstacleInFront = 0;
	int obstacleLeft = 0;
	while (1) { //!blocked) {
		obstacleInFront = forward_timed(); // ARGUMENTS
		turn_left();
		obstacleLeft = isThereSomethingInFront();
		turn_right();
		if (!obstacleLeft) {
			printf("[OBSTACLES] LEFT is CLEAR\n");
			return 2; // left is free
		} else if (obstacleInFront) {
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
	int obstacleDir;
	while (yPos>-3) {
		yPos = get_Y_position();
		obstacleDir = forward_while_checking_left();
		if (obstacleDir==2) turn_left();
		if (obstacleDir==1) turn_right();
	}
	printf("y = %d\n", yPos);
	int x = get_X_position();
	printf("x = %d\n", x);
	return x;
}
