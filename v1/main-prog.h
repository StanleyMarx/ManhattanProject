//--------------------------- CASE_0 ---------------------------
void* display_entry(){
    int c,lval,rval;
    printf("[THREAD] displaying the position value every second\n");
    for (c=0; c<5; c++){
        sleep(1);
        get_tacho_position(sn_rwheel,&rval);
        get_tacho_position(sn_lwheel,&lval);
        printf("[THREAD] R=%d,L=%d\n",rval,lval);
    }    
}
int test_update_pos(){
    pthread_t display;
    pthread_create(&display,NULL,display_entry,NULL);
    
    printf("[MAIN] forwarding for 5 seconds then stop\n");
    move_forever(50,50);
    sleep(5);
    move_forever(0,0);
    
    pthread_join(display,NULL);
    return 0;
}

//--------------------------- CASE_1 ---------------------------
float Xdef=0.0, Ydef=0.0;
int Xpos=0, Ypos=0, XposOld=0, YposOld=0;
float pi=3.14159265;
int speedMotorL, speedMotorR;
int positionMotorR1, positionMotorR2;
float thetaCompas, thetaCompasInit;
float lambda=1/21.21*86/35;
pthread_mutex_t mutex;
int ThreadSituation=0;




void* Update_position(){
    /* get the position every secondes */
    get_sensor_value0(sn_gyro, &thetaCompasInit);
    get_tacho_position(sn_rwheel, &positionMotorR2);

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
                //printf("\nrobot is turning");
            }
        }
        printf("\n Xdef,Ydef = %f,%f       X,Y = %d,%d\n",Xdef,Ydef,Xpos,Ypos);
        if ((Xpos != XposOld) && (Ypos != YposOld)) {
            XposOld = Xpos;
            YposOld = Ypos;
            append_pos_file(Xpos, Ypos);
        }
    }
    pthread_mutex_unlock(&mutex);
    /* fin SC1 */
    pthread_exit(NULL);
}



void* test_Update_position(){
    /* get the position while moving */
    pthread_t myUpdate_position;
    pthread_create(&myUpdate_position,NULL,Update_position,NULL);

    //THE MOVEMENT FUNCTIONS___________________________________________________
    move_forever(20,20);
    sleep(5);
    move_forever(0,0);
    turn_exact_rel(90,1);
    move_forever(20,20);
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

//--------------------------- CASE_2 ---------------------------
void test_cs(){
    send_position(1,2);
    send_position(-1,-2);
    send_obstacle(1,2,3);
    send_obstacle(0,4,5);
    send_position_pos(1,2);
}

int X=0,Y=0;

//--------------------------- CASE_3 ---------------------------
void test_turn(int rat){
    printf("- testing the move_real_debug function -\n");
    move_real_debug(500,500);
    move_real_debug(500,-500);
    move_real_debug(-500,500);
    move_real_debug(-500,-500);
    printf("done\n");
    sleep(20);
    
    printf("- testing the turn_exact_gyro -\n");
    printf("current angle = %f",get_gyro());
    printf("turning to %f...\n",get_gyro()+180);
    turn_exact_gyro(180,1);
    printf("turned to %f.\n",get_gyro());
}


//--------------------------- ROBOT ---------------------------
int robot(int sw,int arg1,int arg2){
    switch (sw){
        case 0:
            test_update_pos();
            break;
        case 1:
            test_Update_position();
	    	create_map();
            break;
        case 2:
            test_cs();
            break;
        case 3:
            test_turn(arg1);
            break;
    }
	
}

/*
    0   test sur l'update des positions
    1   test sur l'update des positions en cm en continue
    2   test sur la com client/serveur
    3   test virage
*/
