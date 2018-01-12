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
float pi=3.14159265;
float Xdef=0.0,Ydef=0.0;
int Xpos=0,Ypos=0;
int speedMotorL, speedMotorR;
int positionMotorL1, positionMotorR1, positionMotorL2, positionMotorR2;
float thetaCompas, thetaCompasInit;
float lambda=1/21.21*86/35;
int ThreadDisplay=0;
pthread_mutex_t lock;

void* Update_position(){
        /* affiche la position toutes les secondes */
    /* debut SC1 */
        pthread_mutex_lock(&lock);
    
    get_sensor_value0(sn_gyro, &thetaCompasInit);
        while(ThreadDisplay == 0){
                pthread_mutex_unlock(&lock);
                /* fin SC1 */

                get_tacho_position(sn_lwheel, &positionMotorL1);
                get_tacho_position(sn_rwheel, &positionMotorR1);
                sleep(0.3);
                get_tacho_speed(sn_lwheel, &speedMotorL);
                get_tacho_speed(sn_rwheel, &speedMotorR);
                get_tacho_position(sn_lwheel, &positionMotorL2);
                get_tacho_position(sn_rwheel, &positionMotorR2);
                get_sensor_value0(sn_gyro, &thetaCompas);
        thetaCompas = (thetaCompas-thetaCompasInit)*pi/180;
                //printf("\n speedMotorL,speedMotorR = %d,%d",speedMotorL,speedMotorR);
                
        if ((abs(speedMotorR) > 5) && (abs(speedMotorL) > 5)) {
            if ((speedMotorR > 0) && (speedMotorL > 0)) {
                /*printf("\nrobot is moving");
                printf("\nsin(thetaCompas) %f",sin(thetaCompas));
                printf("             diffPosition %d",positionMotorR2-positionMotorR1);
                printf("             lambda %f",lambda);*/
		/* debut SC1 */
        	pthread_mutex_lock(&lock);
                Xdef=Xdef+sin(thetaCompas)*(positionMotorR2-positionMotorR1)*lambda;
                Ydef=Ydef+cos(thetaCompas)*(positionMotorR2-positionMotorR1)*lambda;
                Xpos=(int) round(Xdef/5);
                Ypos=(int) round(Ydef/5);
            } else {
                //printf("\nrobot is turning");
            }
        }
                printf("\n Xdef,Ydef = %f,%f       X,Y = %d,%d\n",Xdef,Ydef,Xpos,Ypos);
		append_pos_file(Xpos, Ypos);
  
        }
        pthread_mutex_unlock(&lock);
        /* fin SC1 */
}
void* test_Update_position(){
    
    
    pthread_t display;
    pthread_create(&display,NULL,Update_position,NULL);

    //THE END OF THE INITIALISATION____________________________________________
    //THE MOVEMENT FUNCTIONS___________________________________________________

    
    /*forward_sonar(200,200,200);
    turn_exact_rel(90,3);
    forward_sonar(200,200,200);*/
    
    
    
    move_forever(20,20);
    sleep(5);
    move_forever(0,0);
    turn_exact_rel(90,1);
    move_forever(20,20);
    sleep(5);
    move_forever(0,0);
    

    //THE END OF THE INITIALISATION____________________________________________
    //THE MOVEMENT FUNCTIONS___________________________________________________

    /* debut SC2 */
    pthread_mutex_lock(&lock);
    ThreadDisplay = 1;
    pthread_mutex_unlock(&lock);
    /* fin SC2 */

    pthread_join(display,NULL);
    pthread_mutex_destroy(&lock);
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

//--------------------------- ROBOT ---------------------------
int robot(int sw){
    switch (sw){
        case 0:
            test_update_pos();
            break;
        case 1:
            test_Update_position();
            find_corners();
			printf("Bottom left : %d, %d\n", minX, minY);
		    printf("Bottom right: %d, %d\n", maxX, minY);
		    printf("Top left    : %d, %d\n", minX, maxY);
		    printf("Top right   : %d, %d\n", maxX, maxY);
		    printf("\n");
	    	create_map();
            break;
        case 2:
            test_cs();
            break;	
    }
	
}

/*
    0
        test sur l'update des positions
    1
        test sur l'update des positions en cm en continue
    2
        test sur la com client/serveur
*/
