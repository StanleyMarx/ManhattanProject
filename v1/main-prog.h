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

void* Update_position(){
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
int test_Update_position(){
    int pi=3.14159265;
    int Xdef=0,Ydef=0;
    int speedMotorL, speedMotorR, positionMotorL1, positionMotorR1, positionMotorL2, positionMotorR2;
    float thetaCompas;
    float lambda=1/21.21;
    int ThreadDisplay=0;
    pthread_mutex_t lock;
    
    
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
    return NULL;
}

void test_cs(){
    send_position(1,2);
    send_position(-1,-2);
    send_obstacle(1,2,3);
    send_obstacle(0,4,5);
    send_position_pos(1,2);
}

int X=0,Y=0;

int robot(int sw){
    switch (sw){
        case 0:
            test_update_pos();
            break;
        case 1:
            test_Update_position();
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
