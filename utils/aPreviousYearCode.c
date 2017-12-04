#include "ev3c.h"
#include ‹stdio.h›
#include ‹stdlib.h›
#include ‹unistd.h›
#include ‹stdarg.h›
#include ‹sys/socket.h›
#include ‹bluetooth/bluetooth.h›
#include ‹bluetooth/rfcomm.h›
#include ‹pthread.h›
#include ‹math.h›
#define PI 3.141592653589793
#define LONG 240
#define LARG 208

#define SERV_ADDR   "00:1E:0A:00:02:E1"     /* Whatever the address of the server is */
#define TEAM_ID     7                       /* Your team ID */

#define MSG_ACTION  0
#define MSG_ACK     1
#define MSG_LEAD    2
#define MSG_START   3
#define MSG_STOP    4
#define MSG_WAIT    5
#define MSG_CUSTOM  6
#define MSG_KICK    7
#define MSG_CANCEL  8

int valGyro;
int coin=0; //as we are looking for the ball in different corners, the variable idetifies the current corner (the one we are searching)
int x; //the robot may move in order to catch the ball in the corner, so these variables enable to keep state of its position
int y;
int gyroInit; //the initial value of the gyroscope

ev3_motor_ptr motor0;
ev3_motor_ptr motor1;
ev3_motor_ptr motor2;

ev3_sensor_ptr sensorUS;
ev3_sensor_ptr gyro;
ev3_sensor_ptr color;

void simpleTurn(int speed, int angle);
void moveX(int speed, int x);
void* borderThread(void* arg);
void resetAllMotors();
void find(int speed, int angle);
int sign(int x);
void straight(int speed, int angle);
void runDistance(int speed, int distance);
void scanColor(int speed);
void catchBall(int speed, int position);
void backRightCorner();
void backLeftCorner();
void frontLeftCorner();
void frontRightCorner();
void runAbsolute(int angle,  int distance, int  speed);
void begin();
void trueRun(int angle, float distance, int speed);
void trueTurn(int angle);
void returnHome();
void sendActionMessage(int angle, int distance,int vitesse);

//resets all motors, putting their stop command to hold. Do not call this in a loop on the motor state ! (infinite)
void resetAllMotors(){
        //Reset, it's important to reset motors at the end and at the beginning of the program because it resets the different values 
	//(if we don't the robot is unpredictable)
        ev3_command_motor_by_name( motor0, "reset"); 
        ev3_command_motor_by_name( motor1, "reset");
        ev3_command_motor_by_name( motor2, "reset");

        //Set stop command, hold command enables more precise stop (because the motors are holding one position) than others command
        ev3_stop_command_motor_by_name( motor0, "hold");
        ev3_stop_command_motor_by_name( motor1, "hold");
        ev3_stop_command_motor_by_name( motor2, "hold");
}

//Once the ball caught, return home
void returnHome(){
        trueTurn(0); 
        switch(coin){
                case 2: //the robot is in the backyard left corner
			sendActionMessage(180, LONG - 200, 150); 
                        trueTurn(-90);
                        usleep(50000);
                        trueRun(0, y - 30, 150);
                        usleep(50000);
                        trueTurn(-90);
                        usleep(50000);
			sendActionMessage(270, LARG/2 - 80, 150); 
                        trueRun(0, abs(x), 150);
                        usleep(50000);
                        trueTurn(0);
                        usleep(50000);
                        break;
                case 3: //the robot is in the front left corner
                        trueTurn(90);
                        usleep(50000);
                        if (y > 30) trueRun(0, y-30, 150); //if the robot is too far away from the "starting fence"
                        usleep(50000);
                        trueTurn(-90);
                        usleep(50000);
			sendActionMessage(270, LARG/2 - 80, 150); 
                        trueRun(0, x, 150);
                        usleep(50000);
                        break;
                case 4://the robot is in the front right corner
                        trueTurn(120);
                        usleep(50000);
                        if (y > 30) trueRun(0, y-30, 150);
                        usleep(50000);
                        trueTurn(90);
                        usleep(50000);
			sendActionMessage(90, LARG/2 - 80, 150); 
                        trueRun(0, abs(x), 150);
                        usleep(50000);
                        printf("retour a la case depart !");
                        break;
                case 1: //the robot is in the back right corner
                        trueTurn(90);
                        usleep(50000);
			sendActionMessage(180, LONG - 200, 150); 
                        trueRun(0, y - 30, 150);
                        usleep(50000);
                        trueTurn(90);
                        usleep(50000);
			sendActionMessage(90, LARG/2 - 80, 150); 
                        trueRun(0, abs(x), 150);
                        usleep(50000);
                        printf("retour a la case depart");
                        break;
        }
}

//function executed by the thread in trueTurn
void* turningLeft(void* arg){
        int* p = arg;
        int obj = *p; //the gyro value we want to reach
        ev3_update_sensor_val(gyro);
        int curValGyro = gyro->val_data[0].s32;
        printf("curValGyro before while = %d\n", curValGyro);
        while (curValGyro > obj){ //gyro decreases when turning left
                 ev3_update_sensor_val(gyro);
                 curValGyro = gyro->val_data[0].s32;
        }
        printf("curValGyro after while = %d\n", curValGyro);
        ev3_command_motor_by_name(motor1, "stop");
        ev3_command_motor_by_name(motor2, "stop");
        usleep(50000); //all along the program, the numerous sleep were added so that the robot speed has time to reach 0 before going on with the next movement  
        return NULL;
}

void* turningRight(void* arg){
        int *p = arg;
        int obj = *p;
        ev3_update_sensor_val(gyro);
        int curValGyro = gyro->val_data[0].s32;
        printf("curValGyro before while = %d\n", curValGyro);
        while (curValGyro < obj){ //gyro increases when turning right
                 ev3_update_sensor_val(gyro);
                 curValGyro = gyro->val_data[0].s32;
                //printf("curValGyro = %d\n", curValGyro);
        }
        printf("curValGyro after while = %d\n", curValGyro);
        ev3_command_motor_by_name(motor1, "stop");
        ev3_command_motor_by_name(motor2, "stop");
        printf("Stopped motors\n");
        usleep(50000);
        return NULL;
}

//turns of angle, where angle is a relative angle, expressed in degree with a sign convention (the same as the gyro) opposite to trigonometry
void trueTurn(int angle){
        printf("----TrueTurn----\n");
        usleep(500000);
        ev3_update_sensor_val(gyro);
        int curValGyro = gyro->val_data[0].s32;
        int objectif = angle + valGyro; //the gyro value we want our robot to have at the end of its movement 
        int *p = &objectif;
        pthread_t gyroThread;
        printf("valGyro = %d, curValGyro = %d, objectif = %d\n", valGyro, curValGyro, objectif);
        if (abs(curValGyro-objectif) > 1){ // we don't move if the current gyro value is too close to the objectives because, due to the gyro instability, 
					//we may enter an infinite loop
                if (curValGyro < objectif){
                        ev3_set_speed_sp(motor1, 50);
                        ev3_set_speed_sp(motor2, -50);//we use speed regulation for our motors, thus they put more power in order to balance frictions 
							//and keep a constant speed
                        if (pthread_create(&gyroThread, NULL, turningRight, p)){
                                perror("pthread_create");
                                //return EXIT_FAILURE;
                        }
                }else{
                        ev3_set_speed_sp(motor1, -50);
                        ev3_set_speed_sp(motor2, 50);
                        if (pthread_create(&gyroThread, NULL, turningLeft, p)) {
                                perror("pthread_create");
                                //return EXIT_FAILURE;
                        }
                }
                ev3_command_motor_by_name(motor1, "run-forever");
                ev3_command_motor_by_name(motor2, "run-forever");
                while (
                        ((ev3_motor_state(motor1) != 5) || (ev3_motor_state(motor2) != 5)) //waiting for the motors to stop, 5 represents HOLDING position
                );
                usleep(500000);
                valGyro = objectif;
                usleep(50000);
        }
}


//run straight in the direction of  angle (relative value), on a distance distance (in cm) at a speed speed (in mm/s)
void runAbsolute(int angle, int distance, int speed){
        printf("-----Run Absolute-----\n");
        int evspeed = speed*300/148; //empirical formula, found out by testing
        ev3_set_speed_sp(motor1, evspeed);
        ev3_set_speed_sp(motor2, evspeed);
        int position = ((distance*360)/(2*PI*2.8));
        ev3_set_position_sp(motor1, position);
        ev3_set_position_sp(motor2, position);
        ev3_command_motor_by_name(motor1, "run-to-rel-pos");
        ev3_command_motor_by_name(motor2, "run-to-rel-pos");
        while ((ev3_motor_state(motor1) != 5) || (ev3_motor_state(motor2) != 5)){
        }
        usleep(50000);
        trueTurn(0); //the robot put himself in the begining direction (in case he deviated)
}

//like runAbsolute but more precise on large distance (uses runAbsolute)
void trueRun(int angle, float distance, int speed){
        printf("-----True Run-----\n");
        int pent = floor(distance);
        int nbAbsolute = pent/40; //we will run by 40 cm steps, in order to go more straight 
        int rest = pent %40;
        float remain = rest + (distance - pent);
        int i;
        trueTurn(angle);
        runAbsolute(0, remain, speed);
        for(i = 0 ; i < nbAbsolute ; i++){
                runAbsolute(0, 40, speed);
        }
}

//running but without trueTurn(previously known as straight), less precise, to be used with small distances, for fast small step per small step running
void runSansStraight(int distance, int speed){
        int evspeed = speed*300/148;
        ev3_set_speed_sp(motor1, evspeed);
        ev3_set_speed_sp(motor2, evspeed);
        int position = ((distance*360)/(2*PI*2.8));
        ev3_set_position_sp(motor1, position);
        ev3_set_position_sp(motor2, position);
        ev3_command_motor_by_name(motor1, "run-to-rel-pos");
        ev3_command_motor_by_name(motor2, "run-to-rel-pos");
        while ((ev3_motor_state(motor1) != 5) || (ev3_motor_state(motor2) != 5)){
        }
        usleep(50000);
}

int sign(int x) {
        if(x > 0){
                return 1;
        }
        else {
                if(x < 0){
                        return -1;
                }
                else {
                        return 0;
                }
        }
}

//simple, quick but less precise turning function
void simpleTurn(int speed, int angle) {

        printf("-----simpleTurn-----\n");

        int pos = abs(angle)*(5.8)/(2.8);
        ev3_set_speed_sp(motor1, 300);
        ev3_set_speed_sp(motor2, 300);
        ev3_set_position_sp(motor1, sign(angle)*pos);
        ev3_set_position_sp(motor2, -(sign(angle)*pos));

        ev3_command_motor_by_name(motor1, "run-to-rel-pos");
        ev3_command_motor_by_name(motor2, "run-to-rel-pos");
        while (
                (ev3_motor_state(motor1) !=5) || (ev3_motor_state(motor2) !=5)
        );
        usleep(50000);
}

struct border
{
        int x;
};

//function called by the thread in MoveX, stop the motors when the sensorUS is at less than x from the wall
void* borderThread(void* arg) {
        //pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        struct border *q = arg;
        int x = q->x;  // en mm

        //ev3_mode_sensor(sensorUS, 0);
        ev3_update_sensor_val(sensorUS);
        int valUS = sensorUS->val_data[0].s32;

        printf("thread before while, valUS = %d\n", valUS);

        while(valUS > x){
                //printf("while\n");
                ev3_update_sensor_val(sensorUS);
                valUS = sensorUS->val_data[0].s32;
        }
        printf("thread after while valUS=%d\n", valUS);
        ev3_command_motor_by_name(motor1, "stop");
        ev3_command_motor_by_name(motor2, "stop");
        //printf("Stopped motors\n");
        usleep(50000);
        return NULL;
}

//Move until x mm of the border
void moveX(int speed, int x) {

        printf("-----movX-----\n");
        //trueTurn(0);
        ev3_update_sensor_val(sensorUS);
        int valUS = sensorUS->val_data[0].s32;

        if (valUS > (x+3)){

                struct border arg = {x};

                pthread_t threadBorder;

                ev3_set_speed_sp(motor1, 200);
                ev3_set_speed_sp(motor2, 200);

                if (pthread_create(&threadBorder, NULL, borderThread, &arg)) {
                        perror("pthread_create");
                        //return EXIT_FAILURE;
                }

                ev3_command_motor_by_name(motor1, "run-forever");
                ev3_command_motor_by_name(motor2, "run-forever");

                while (
                        ((ev3_motor_state(motor1) != 5) || (ev3_motor_state(motor2) != 5))
                );
                usleep(50000);

                ev3_update_sensor_val(sensorUS);
                int valUS = sensorUS->val_data[0].s32;
                /*if(valUS > (x+3)) {
                        printf("------moveX AGAIN-------\n");
                        moveX(speed, x);
                }*/
        }
        usleep(50000);
}

//searches for the ball in the current corner, if it's not there calls the function going to the next corner
void find(int speed, int angle) {
        printf("-----find-----\n");
        ev3_update_sensor_val(sensorUS);
        int valUS = sensorUS->val_data[0].s32;
        printf("valUS=%d\n", valUS);
        ev3_update_sensor_val(sensorUS);
        int valUSnext = sensorUS->val_data[0].s32;
        int i=0;
        while((i<(abs(angle))) && ((valUS - valUSnext < 300) || (valUSnext > 350))){
                simpleTurn(speed, sign(angle));
                usleep(50000);
                valUS=valUSnext;
                ev3_update_sensor_val(sensorUS);
                valUSnext = sensorUS->val_data[0].s32;
                //printf("valUS=%d et valUSnext=%d\n", valUS, valUSnext);
                usleep(20000);
                i++;
        }
        printf("valUS finale =%d\n", valUSnext);
        if(ival_data[0].s32;
                sleep(2);
                ev3_set_led(RIGHT_LED,RED_LED,127);
                ev3_set_led(LEFT_LED,RED_LED,127);
                ev3_set_led(RIGHT_LED,GREEN_LED,127);
                ev3_set_led(LEFT_LED,GREEN_LED,127);
                if(valUSnext > 80){
                        ev3_update_sensor_val(gyro);
                        int curValGyro = gyro->val_data[0].s32;
                        int angle = (curValGyro - gyroInit) %360 ;
                        float multx = sin(angle);
                        float multy = cos(angle);
                        printf("valUS > 80\n");
                        i=0;
                        while((i < ((valUSnext-30)/10)) && (valColor!=5) && (valColor!=2)){
                                runSansStraight(1, 150);
                                ev3_update_sensor_val(color);
                                valColor = color->val_data[0].s32;
                                usleep(20000);
                                i++;
                        }
                        x += i*multx;
                        y += i*multy;
                        ev3_update_sensor_val(color);
                        valColor = color->val_data[0].s32;
                        if((valColor==5) || (valColor==2)){
                                catchBall(30, -180);
                        }
                        else {
                                trueTurn(90);
                                scanColor(20);
                        }
                }
                else{
                        catchBall(30, -180);
                }
        }
        else{
                printf("Nothing in this corner\n");
                switch(coin){
                        case 1:
                                backLeftCorner();
                                break;
                        case 2:
                                frontLeftCorner();
                                break;
                        case 3 :
                                frontRightCorner();
                                break;
                        default :
                                trueTurn(0);
                                sleep(1);
                                printf("fin de frontRightCorner\n");
				sendActionMessage(0, 100, 100); 
                                trueRun(-60, 100, 150);
                                backRightCorner();
                                break;
                }
        }
}

//if the USsensor detected the ball in the corner , but is not close enough to catch it, the find function has it run in the direction of the ball and 
//calls the scanColor function, in order to replace the robot in front of the ball and catch it
void scanColor(int speed){
        printf("-----scannColor-----\n");
        ev3_update_sensor_val(color);
        int valColor = color->val_data[0].s32;
        int i=0;
        while((i<150) && (valColor!=5) && (valColor!=2)){
                simpleTurn(speed, -1);
                usleep(20000);
                ev3_update_sensor_val(color);
                valColor = color->val_data[0].s32;
                usleep(20000);
                i++;
        }
        if((valColor==5)||(valColor==2)){
                ev3_set_led(RIGHT_LED,GREEN_LED,0);
                ev3_set_led(LEFT_LED,GREEN_LED,0);
                ev3_set_led(RIGHT_LED,RED_LED,255);
                ev3_set_led(LEFT_LED,RED_LED,255);
                catchBall(30, -180);
                sleep(2);
        }
        else{
                switch(coin){
                        case 1:
                                backLeftCorner();
                                break;
                        case 2:
                                frontLeftCorner();
                                break;
                        case 3 :
                                frontRightCorner();
                                break;
                        default :
                                trueTurn(0);
                                sleep(1);
				sendActionMessage(0, 100, 100); 
                                trueRun(-60, 100, 150);
                                backRightCorner();
                                break;
                }
        }
}

//grabs the ball and calls the returnHome function
void catchBall(int speed, int position) {
        ev3_set_speed_sp(motor0, 350);
        ev3_set_position_sp(motor0, -170);
        ev3_command_motor_by_name( motor0, "run-to-rel-pos");
         while (
                (ev3_motor_state(motor0)!= 5)

         );
        sleep(1);
        returnHome();
}

//goes to the backyard right corner and search the ball there
void backRightCorner(){
        printf("-----backRightCorner-----\n");
        coin=1;
        sleep(1);
        sendActionMessage(0, LONG - 200, 100); 
        moveX(30, 2300);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 2000);
        usleep(50000);
        trueTurn(0);
        usleep(5000);
        moveX(30, 1750);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 1500);
        trueTurn(0);
        usleep(50000);
        moveX(30, 1250);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 1000);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 600);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 600);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        trueTurn(90);
        y = LONG - 60;
        find(40, -180);
}

//goes to the backyard left corner and searches the ball corner
void backLeftCorner(){
        printf("-----backLeftCorner-----\n");
        coin=2;
        trueTurn(-180);
        usleep(50000);
        sendActionMessage(90, LARG - 170, 100);
        moveX(30, 750);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 500);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 500);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        x = 50 - (LARG/2);
        find(40, 120);
}

void frontLeftCorner(){
        printf("-----frontLeftCorner-----\n");
        coin=3;
        trueTurn(0);
        usleep(50000);
        trueRun(-90, 80, 150);
        sendActionMessage(180, LONG - 210, 100);
        moveX(30, 2300);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 2000);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 1750);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 1500);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 1250);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 1000);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 750);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 750);
        usleep(50000);
        trueTurn(90);
        usleep(50000);
        y = 75;
        find(40, -120);
}

//goes to the frontyard right corner
void frontRightCorner(){
        coin=4;
        printf("-----frontRightCorner----\n");
        trueTurn(-180);
        usleep(50000);
        sendActionMessage(270, LARG - 160, 100);
        moveX(30, 750);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 500);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        moveX(30, 500);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        x = (LARG/2) - 50;
        trueTurn(-30);
        find(40, 120);
}

//begining of the leader function
void begin(){
        printf("-----begin-----\n");
        sendActionMessage(0, 100, 110);
        trueRun(0, 100, 150);
        usleep(50000);
        trueTurn(90);
        usleep(50000);
        moveX(30, 600);
        usleep(50000);
        trueTurn(0);
        usleep(50000);
        x = (LARG/2) - 60;
        trueTurn(-90);
        usleep(50000);
        trueTurn(0);
        backRightCorner();
}

void debug (const char *fmt, ...) {
    va_list argp;

    va_start (argp, fmt);

    vprintf (fmt, argp);

    va_end (argp);
}

unsigned char rank = 0;
unsigned char length = 0;
unsigned char previous = 0xFF;
unsigned char next = 0xFF;

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

void sendActionMessage(int angle, int distance,int vitesse){
    char string[58];

    /* Send ACTION message */
    *((uint16_t *) string) = msgId++;
    string[2] = TEAM_ID;
    string[3] = next;
    string[4] = MSG_ACTION;

    if (angle < 256){
        string[5] = angle;
        string[6] = 0x00;
    }else{
        int petit = angle - 256;
        string[5] = petit;
        string[6] = 1;
    }

    string[7] = distance;

    string[8] = vitesse;          /*we will always send speed smaller than 250*/
    string[9] = 0x00;

    write(s, string, 10);
}

void leader () {
    printf ("I'm the leader...\n");
    begin();
}


void follower(){
        char string[58];
        printf ("I'm a follower...\n");
        char type;
        int angle;
        int distance;
        int speed;
        while (1){
                read_from_server (s, string, 58);
                if (string[3] == TEAM_ID) {
                        type = string[4];
                        switch (type) {
                                case MSG_ACTION:
                                        // Send ACK message
                                        angle = 256 * string[6] + string[5];
                                        distance = string[7];
                                        speed = string[8]; //won't go faster than 255 mm/s
                                        char sender = string[2];
                                        char idR0 = string[0];
                                        char idR1 = string[1];
                                        string[3] = next; // transmit the message
                                        string[2] = TEAM_ID;
                                        *((uint16_t *) string) = msgId++;
                                        write(s, string, 8);

                                        string[3] = sender; // reply to sender
                                        string[4] = MSG_ACK;
                                        string[5] = idR0; // ID of the mess$
                                        string[6] = idR1;
                                        *((uint16_t *) string) = msgId++;
                                        string[7] = 0x00; // status OK
                                        write(s, string, 8);
										
					if (curValGyro - gyroInit + angle > 0){ 
                                                diff = (curValGyro - gyroInit + angle) % 360 ;
                                                if (diff > 180) diff = 360 - diff;
                                                else diff = -diff; 
                                        } else {
                                                diff = (- curValGyro + gyroInit - angle) % 360 ;
                                                if (diff > 180) diff = diff - 360;
                                        }
										
                                        trueRun(diff, distance, speed); 
										
                                        break; 
                                case MSG_STOP:
                                        return;
					break;
                                case MSG_LEAD:
                                        leader ();
                                        break;
                                default:
                                        printf ("Ignoring message %d\n", type);
                        }
                }
        }
}


int main(int argc, char **argv) {
    struct sockaddr_rc addr = { 0 };
    int status;

    /* allocate a socket */
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    /* set the connection parameters (who to connect to) */
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba (SERV_ADDR, &addr.rc_bdaddr);

    /* connect to server */
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    ev3_init_led();
    ev3_set_led(LEFT_LED,RED_LED, 127);
    ev3_set_led(RIGHT_LED,RED_LED,127);
    ev3_set_led(LEFT_LED,GREEN_LED,127);
    ev3_set_led(RIGHT_LED,GREEN_LED,127);

    //Loading all motors
    ev3_motor_ptr motor = ev3_load_motors();

    //A copy to be used to play with motors
    ev3_motor_ptr motors = motor;

    //Loading all sensors
    ev3_sensor_ptr sensor = ev3_load_sensors();

    color = ev3_search_sensor_by_port(sensor,3);
    gyro = ev3_search_sensor_by_port(sensor,1);
    sensorUS = ev3_search_sensor_by_port(sensor,4);

    ev3_open_sensor(color);
    ev3_open_sensor(gyro);
    ev3_open_sensor(sensorUS);

    ev3_mode_sensor(sensorUS, 0);
    ev3_mode_sensor(gyro, 0);
    ev3_mode_sensor(color, 2);

    //Open all the motors
    while (motors){
            ev3_open_motor(motors);
            motors = motors->next;
    }

    //All the motors
    motor0 = ev3_search_motor_by_port(motor,'A');
    motor1 = ev3_search_motor_by_port(motor,'B');
    motor2 = ev3_search_motor_by_port(motor,'C');

    //Reset all the motors
    resetAllMotors();

    ev3_update_sensor_val(gyro);
    valGyro = gyro->val_data[0].s32;
    gyroInit = valGyro;

    /* if connected */
    if( status == 0 ) {
        char string[58];

        /* Wait for START message */
        read_from_server (s, string, 9);
        if (string[4] == MSG_START) {
            printf ("Received start message!\n");
            rank = (unsigned char) string[5];
            length = (unsigned char) string[6];
            previous = (unsigned char) string[7];
            next = (unsigned char) string[8];
        }

        if (rank == 0)
            leader ();
        else
            follower ();

        close (s);

        sleep (5);

    } else {
        fprintf (stderr, "Failed to connect to server...\n");
        sleep (2);
        exit (EXIT_FAILURE);
    }

    close(s);

    ev3_set_led(LEFT_LED,RED_LED,0);
    ev3_set_led(RIGHT_LED,RED_LED,0);
    ev3_set_led(LEFT_LED,GREEN_LED,0);
    ev3_set_led(RIGHT_LED,GREEN_LED,0);

    ev3_quit_led();

    ev3_command_motor_by_name( motor0, "reset");
    ev3_command_motor_by_name( motor1, "reset");
    ev3_command_motor_by_name( motor2, "reset");

    ev3_close_motor(motor1);
    ev3_close_motor(motor2);
										
    ev3_delete_motors(motor);

    ev3_close_sensor(color);
    ev3_close_sensor(gyro);
    ev3_close_sensor(sensorUS);

    ev3_delete_sensors(sensor);

    return 0;
}
