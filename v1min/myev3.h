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
const float SONAR_THRESHOLD = 90.0;
const int POS_THRESHOLD = -1000;
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
float X=0, Y=0, T=0;
float T0=0;
float T0_COMPASS=0;
float SQUARE_SIZE=200; // size of a 5cm square in the units of X


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

// Simple Movement Functions
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
void turn_gyro(float delta){
    // turns untill the gyro measures the right value
    float t0=get_gyro();
    if (delta>0){
        move_forever(17,-17);
        while(get_gyro()<t0+delta-1.6);
        move_forever(0,0);
    } else {
        move_forever(-17,17);
        while(get_gyro()>t0+delta+1.6);
        move_forever(0,0);
    }
}
void turn_gyro_abs(float agl){
    // turns to the absolute position T0+agl
    float delta=fmod(agl-get_gyro()+T0,360);
    if (delta>180){
        delta=delta-360;
    }
    turn_gyro(delta);
}
void turn_exact_gyro(float delta,float prec){
    float anglCurr=get_gyro();
    float anglDest=anglCurr+delta;
    while (abs(delta)>prec){
        turn_approx(delta);
        anglCurr=get_gyro();
        delta=anglDest-anglCurr;
    }
}

// ------------------------------ Shovel Functions
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
    usleep(10000);
}
void send_mapdata(int16_t x,int16_t y,char r,char g,char b){
    char str[58];
    *((uint16_t*)str)=msgId++;
    str[2]=TEAM_ID;
    str[3]=0xff;
    str[4]=5;
    str[5]=x;
    if (x<0){str[6]=0xff;}else{str[6]=0x00;};
    str[7]=y;
    if (y<0){str[8]=0xff;}else{str[8]=0x00;};
    str[9]=r;
    str[10]=g;
    str[11]=b;
    write(s,str,12);
    usleep(10000);
}
void send_mapdone(){
    char str[58];
    *((uint16_t*)str)=msgId++;
    str[2]=TEAM_ID;
    str[3]=0xff;
    str[4]=6;
    write(s,str,5);
    usleep(10000);
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
    usleep(10000);
}


/* STRATEGY 1 */
int X_MAP_MAX=24;
int Y_MAP_MAX=40;
int map_x(){
    return round(X/SQUARE_SIZE); 
}
int map_y(){
    return round(Y/SQUARE_SIZE); 
}
void matrix_print(int mat[Y_MAP_MAX][X_MAP_MAX]){
    for (int y=0; y<Y_MAP_MAX; y++){
        for (int x=0; x<X_MAP_MAX; x++){
            switch(mat[y][x]){
                case 0:
                    printf("??");
                    break;
                case 1:
                    printf(". ");
                    break;
                case 2:
                    printf("NN");
                    break;
                case 3:
                    printf("MM");
                    break;
            }
        }
        printf("\n");
    }
}
int matrix_nb_zeros(int mat[Y_MAP_MAX][X_MAP_MAX]){
    int res=0;
    for (int y=0; y<Y_MAP_MAX; y++){
        for (int x=0; x<X_MAP_MAX; x++){
            if (mat[y][x]==0){
                res++;
            }
        }
    }
    return res;
}
float matrix_completion(int mat[Y_MAP_MAX][X_MAP_MAX]){
    float total=X_MAP_MAX*Y_MAP_MAX;
    float explo=total-matrix_nb_zeros(mat);
    float res=explo/total;
    return res;
}

char is_freespace_large(int mat[Y_MAP_MAX][X_MAP_MAX],int x, int y, int width){
    int xx,yy;
    for (yy=y-width; yy<=y+width; yy++){
        for (xx=x-width; xx<=x+width; xx++){
            if (xx<0 && xx>=X_MAP_MAX && yy<0 && yy>=Y_MAP_MAX){
                // (x,y) trop près du bord
                return 0;
            } else {
                if (mat[yy][xx]!=1){
                    // (x,y) trop près d'un obstacle
                    return 0;
                }
            }
        }
    }
    return 1;
}
char is_path_empty(int mat[Y_MAP_MAX][X_MAP_MAX],int x, int y){
    int i,max=Y_MAP_MAX*X_MAP_MAX,xx,yy,dx=x-map_x(),dy=y-map_y();
    if (x>=0 && x<X_MAP_MAX && y>=0 && y<Y_MAP_MAX){
        for (i=0; i<=max; i++){
            xx=map_x()+dx*i/max;
            yy=map_y()+dy*i/max;
            if (!is_freespace_large(mat,xx,yy,1)){
                return 0;
            }
        }
    }
    return 1;
}
int chose_next_point(int mat[Y_MAP_MAX][X_MAP_MAX],int* x_ptr, int* y_ptr){
    printf("(chose_next_point) currently at (%d,%d)\n",map_x(),map_y());
    int x=rand()%X_MAP_MAX;
    int y;
    
    if (map_y()>Y_MAP_MAX*0.75){
        // goes toward Y=0;
        y=2;
        while(1){
            for (x=2; x<X_MAP_MAX-2; x++){
                if (is_path_empty(mat,x,y)){
                    *x_ptr=x;
                    *y_ptr=y;
                    return 0;
                }
            }
            y++;
            if (y==map_y()+5){
                y=2;
                printf("(chose_next_point) [WARNING] y reset to 2. Not an error but suspicious behavior\n");
            }
        }
    }
    else {
        // goes toward Y=0;
        y=Y_MAP_MAX-3;
        while(1){
            for (x=2; x<X_MAP_MAX-2; x++){
                if (is_path_empty(mat,x,y)){
                    *x_ptr=x;
                    *y_ptr=y;
                    return 0;
                }
            }
            y--;
            if (y==map_y()-5){
                y=Y_MAP_MAX-3;
                printf("(chose_next_point) [WARNING] y reset to Y_MAP_MAX-3. Not an error but suspicious behavior\n");
            }
        }        
    }
    
    *x_ptr=x;
    *y_ptr=y;
    printf("(chose_next_point) chose (%d,%d)\n",x,y);
}

float get_sonar_map(){
    float d=get_sonar();
    float t=fmod(T-T0+180,360);
    if (abs(t)<90){
        float dd=Y/cos(t);
        if (dd<d){
            return dd;
        }
    }
    return d;
}
void go_to_approx(float x, float y){
    // given (X,Y,T), moves straight to (x,y,[complicated]) in one way
    // tested - it should work properly
    float ratio=0.5;
    float dx=x-X;
    float dy=y-Y;
    turn_gyro_abs(atan2(dy,dx)*57.29577951308232);
    float d=sqrt(dx*dx+dy*dy);
    move_real_debug(d*ratio,d*ratio);
}
void go_to(float x, float y, float prec){
    // makes the robot move from its original position to a point in the disk of center (x,y) and of radius prec
    // tested, should work, a precision of 80 (=2cm) is already pretty fine
    float d=sqrt(pow((x-X),2)+pow((y-Y),2));
    while (d>prec){
        go_to_approx(x,y);
        d=sqrt(pow((x-X),2)+pow((y-Y),2));
    }
}
void go_to_map(int x, int y){
    // goes to (x,y) on the map, so x and y are pixel coordinates
    // not tested but should work
    go_to(round(x*SQUARE_SIZE),round(y*SQUARE_SIZE),100);
}
void send_map_from_var(int mat[Y_MAP_MAX][X_MAP_MAX]){
    printf("will send the following map:\n");
    matrix_print(mat);
    printf("color code:\n");
    printf("\tgrey          not explored\n");
    printf("\tlight grey    empty\n");
    printf("\tblack         non-movable or frontier\n");
    printf("\tred           movable object\n");
    
    for (int y=0; y<Y_MAP_MAX; y++){
        for (int x=0; x<X_MAP_MAX; x++){
            switch (mat[y][x]){
                case 0:
                    send_mapdata(x,y,128,128,128);
                    break;
                case 1:
                    send_mapdata(x,y,192,192,192);
                    break;
                case 2:
                    send_mapdata(x,y,0,0,0);
                    break;
                case 3:
                    send_mapdata(x,y,255,0,0);
                    break;
            }
        }
    }
    send_mapdone();
}

char UPDATE_POS_ENABLE=1;
void* update_pos_entry(){
    // updates the global variable X and Y given the input of the motors
    // writes it to a file
    // uses the gyro
    
    int right_pos, left_pos, right_pos_prev, left_pos_prev, dr, dl;
    get_tacho_position(sn_rwheel,&right_pos_prev);
    get_tacho_position(sn_lwheel,&left_pos_prev);
    T=get_gyro();
    right_pos=right_pos_prev;
    left_pos=left_pos_prev;
    FILE* file_pos;
    int x_towrite;
    int y_towrite;
    int x_lastwritten=0;
    int y_lastwritten=0;
    
    while (UPDATE_POS_ENABLE){
        get_tacho_position(sn_rwheel,&right_pos);
        get_tacho_position(sn_lwheel,&left_pos);
        dr=right_pos-right_pos_prev;
        dl=left_pos-left_pos_prev;
        
        if (dr*dl>0){
            // advancing
            X+=(dr+dl)*cos((T-T0)/57.29577951308232);
            Y+=(dr+dl)*sin((T-T0)/57.29577951308232);
        } else {
            // turning
            T=get_gyro();
        }
        
        right_pos_prev=right_pos;
        left_pos_prev=left_pos;
    }
}
char SEND_POS_ENABLE=1;
void* send_pos_entry(){
    while(SEND_POS_ENABLE){
        send_position(map_x(),map_y());
        sleep(2);
    }
}
