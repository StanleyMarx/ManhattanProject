#define PORT_RIGHTWHEEL 66
#define PORT_SHOVEL 67
#define PORT_LEFTWHEEL 68

uint8_t sn;
uint8_t sn_compass;
uint8_t sn_sonar;
uint8_t sn_rwheel;
uint8_t sn_lwheel;
uint8_t sn_shovel;
uint8_t sn_color;
int max_speed;
const char const *colors[] = { "?", "BLACK", "BLUE", "GREEN", "YELLOW", "RED", "WHITE", "BROWN" };

void test_tachos_verbose(){
    /*
        checks that tachos are plugged in at the default port
        right wheel = 66 (B)
        shovel      = 67 (C)
        left wheel  = 66 (D)
        if one is not found, makes the list of all plugged ports
    */
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
    /*
        search for the touch, color, compass, sonar and magnetic sensors.
        if found, displays the value related to each of them.
    */
    uint8_t sn_touch;
    uint8_t sn_color;
    uint8_t sn_compass;
    uint8_t sn_sonar;
    uint8_t sn_mag;
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
}

void move_forever(int rcycle,int lcycle){
    /*
        set the robot in motion
        rcycle and lcycle must be in [-100,100]:
            rcycle,lcycle = 50,50 : forward at 50% capacity
            rcycle,lcycle = -50,-50 : forward at 50% capacity
            rcycle,lcycle = -10,10 : turn right at 10% capacity
            rcycle,lcycle = 0,0 : stops
    */
    set_tacho_duty_cycle_sp(sn_rwheel,rcycle);
    set_tacho_duty_cycle_sp(sn_lwheel,lcycle);
    set_tacho_command_inx(sn_rwheel,TACHO_RUN_DIRECT);
    set_tacho_command_inx(sn_lwheel,TACHO_RUN_DIRECT);
}
void move_real(int r,int l,int speed){
    /*
        forward:
            356.4cm move_real(8000,8000,400)
            Xcm     move_real(X*22.447,X*22.447,400)
        turn:
            360° move_real(815,-815,max_speed/2) 2.26
            180° move_real(415,-415,max_speed/2) 2.3
             90° move_real(210,-210,max_speed/2) 2.33
    */
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

float get_compass(){
    float ret;
    get_sensor_value0(sn_compass,&ret);
    return ret;
}
float get_compass_slow(){
    move_forever(0,0);
    sleep(1);
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

void turn_exact_rel(float delta,float prec){
    float ratio=2.3;
    delta=fmod(delta,360);
    if (delta>180){delta=delta-360;}
    float anglCurr=get_compass();
    float anglDest=fmod(anglCurr+delta,360);
    while(abs(delta)>prec){
        printf("delta = %f\n");
        move_real(-ratio*delta,ratio*delta,max_speed/5);
        sleep(1);
        anglCurr=get_compass();
        delta=fmod(anglDest-anglCurr,360);
        if (delta>180){delta=delta-360;}
    }
}
void turn_exact_abs(float anglDest,float prec){
    float ratio=2.3;
    float anglCurr=get_compass();
    float delta=anglDest-anglCurr;
    delta=fmod(delta,360);
    if (delta>180){delta=delta-360;}
    while(abs(delta)>prec){
        printf("delta = %f - ",delta);
        move_real(-ratio*delta,ratio*delta,max_speed/5);
        sleep(1);
        anglCurr=get_compass();
        delta=fmod(anglDest-anglCurr,360);
        printf("delta = %f - ",delta);
        if (delta>180){delta=delta-360;}
        if (delta<-180){delta=delta+180;}
        printf("delta = %f\n",delta);
    }
}

int forward_sonar(int rcycle, int lcycle, float sonarThreshold, int msec, int delta) {
	// moves forward until it is close enough to an object 
    	float sonarVal = get_sonar();
	double C_PER_SECS;
	C_PER_SECS = 1000000.0;
	clock_t previous, current;
	previous = clock();
	sleep(1);
	current = clock();
	if (sonarVal > sonarThreshold) {
        	move_forever(rcycle, lcycle);
		while (sonarVal > sonarThreshold) {
			current = clock();
			sonarVal = get_sonar();
				if ((current - previous)/C_PER_SECS > msec) { 
					move_forever(0,0);
					turn_exact_rel(-delta,2);
					if (get_sonar() < sonarThreshold){
						return 0;
					} else {
						turn_exact_rel(2*delta, 2);
						if (get_sonar() < sonarThreshold){
							return 0;
						}
					}
				}
				previous = current;
			
				
		}
		move_forever(0,0);
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

void detectBall(int delta){
    // detect if movable or non movable object
	turn_exact_rel(-delta,2);
	sleep(0.5);
	float sonarValG = get_sonar();
    	turn_exact_rel(delta+5,2);
	sleep(0.5);
    	turn_exact_rel(delta+5,2);
	sleep(0.5);
	float sonarValD = get_sonar();
    	turn_exact_rel(-delta-5,2);
	if (sonarValG>150 && sonarValD>150){
		printf("movable object\n");
	}else {
		printf("non movable object\n");
        turn_exact_rel(90,2);
	}
}

void take_object(){
    	forward_sonar(50, 50, 80.0, 1000, 20);
	printf("[PELLE] opening pelle\n");//--------open pelle
	set_tacho_speed_sp(sn_shovel, -80);
	set_tacho_command(sn_shovel, "run-forever");
	sleep(2);
    	move_real(8*22.447,8*22.447,400);
	printf("[PELLE] closing pelle\n");//-------close pelle
	set_tacho_command(sn_shovel, "stop");
	set_tacho_speed_sp(sn_shovel, 80);
	set_tacho_command(sn_shovel, "run-forever");
	sleep(2);
	set_tacho_command(sn_shovel, "stop");
}

void drop_object() {
	turn_exact_rel(-180,2);
    	move_real(8*22.447,8*22.447,400);
	printf("[PELLE] opening pelle\n");//----------open pelle
	set_tacho_speed_sp(sn_shovel, -80);
	set_tacho_command(sn_shovel, "run-forever");
	sleep(2);
	move_real(8*22.447,8*22.447,-400);//---------movebackward
    	turn_exact_rel(90,2); //-------half turn
	printf("[PELLE] closing pelle\n");//----------close pelle
	set_tacho_command(sn_shovel, "stop");
	set_tacho_speed_sp(sn_shovel, 80);
	set_tacho_command(sn_shovel, "run-forever");
	sleep(2);
	set_tacho_command(sn_shovel, "stop");
}
// à tester !! sera à utiliser en thread 
void Detect_timed(int delta, int msec, int sonarTreshold){
	/*if the robot hasn't detected any obstacles in the time msec, 
	it stops and looks around  with an angle delta*/
	double C_PER_SECS;
	C_PER_SECS = 1000000.0;
	clock_t previous, current;
	previous = clock();
	sleep(1);
	current = clock();
	if ((current - previous)/C_PER_SECS > msec) { 
		move_forever(0,0);
		turn_exact_rel(-delta,2);
		if (get_sonar() < sonarTreshold){
			// revenir au main
			
		}
		else {
			turn_exact_rel(2*delta, 2);
			if (get_sonar() < sonarTreshold){
				// revenir au main
			}
		}
		previous = current;
	}
}

/* à tester */
float width_object(){
    /*
        return the angular width of the object in front of him
        end position: at the right of it
    */
    int speed_scan=20;
    float treshold=200;
    float al,ar;
    
    move_forever(speed_scan,-speed_scan);
    while(get_sonar()<treshold);
    move_forever(0,0);
    sleep(1);
    al=get_compass();
    move_forever(-speed_scan,speed_scan);
    while(get_sonar()<treshold);
    move_forever(0,0);
    sleep(1);
    ar=get_compass();
    return al-ar;
}
float width_object2(){
    int speed_scan=20;
    float treshold_drop=50;
    float d0=get_sonar(),d1=d0;
    float al,ar;
    
    move_forever(speed_scan,-speed_scan);
    while(abs(d1-d0)<treshold_drop){
        d0=d1;
        d1=get_sonar();
    }
    al=get_compass_slow();
    move_forever(-speed_scan,speed_scan);
    while(abs(d1-d0)<treshold_drop){
        d0=d1;
        d1=get_sonar();
    }
    ar=get_compass_slow();
    
    return al-ar;
}
void scan_object(){
    int speed_scan=20;
    float treshold=200;
    int prec=256;
    int scan[prec];
    float ar,al;
    int i;
    
    move_forever(speed_scan,-speed_scan);
    while(get_sonar()<treshold);
    move_forever(0,0);
    sleep(1);
    al=get_compass();
    move_forever(-speed_scan,speed_scan);
    for (i=0; i<256; i++){
        scan[i]=get_sonar();
    }
    move_forever(0,0);
    sleep(1);
    get_compass();
    
    // max
    int imax=0;
    for (i=0; i<prec; i++){
        if (scan[i]>scan[imax]){
            imax=i;
        }
    }
    float aobjmax=al+(ar-al)*imax/prec;
    
    // boundaries via scan'
    float scanp,scanpmin=scan[1]-scan[0],scanpmax=scanpmin;
    int ipmin,ipmax;
    for (i=0; i<prec-1; i++){
        scanp=scan[i+1]-scan[i];
        if (scanpmin>scanp){
            scanpmin=scanp;
            ipmin=i;
        }
        if (scanpmax<scanp){
            scanpmax=scanp;
            ipmax=i;
        } 
    }
    float aobjl=al+(ar-al)*ipmin/prec;
    float aobjr=al+(ar-al)*ipmax/prec;    
}

/* communicatio client/serveur */
void send_position(int16_t x,int16_t y){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=MSG_POSITION;
	*((int16_t*)&str[5])=x;
    str[5]=0xff;
	*((int16_t*)&str[7])=y;
    str[7]=0xff;
	write(s,str,9);
	Sleep(1000);
    
    /*debug*/
    
}
void send_mapdata(int16_t x,int16_t y,char r,char g,char b){
    char str[58];
	*((uint16_t*)str)=msgId++;
	str[2]=TEAM_ID;
	str[3]=0xff;
	str[4]=5;
	*((int16_t*)&str[5])=x;	
	*((int16_t*)&str[7])=y;
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
	*((int16_t*)&str[6])=x;	
	*((int16_t*)&str[8])=y;
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
