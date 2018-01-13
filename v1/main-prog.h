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
//dans myev3.h
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



//--------------------------- CASE_4 ---------------------------
//dans myeve3.h
//--------------------------- CASE_5 ---------------------------

void debug_sensors(){
    while (1){
        printf("compass %f\n",get_compass());
        printf("gyro    %f\n",get_gyro());
        printf("color   %d\n\n",get_color());
        sleep(2);
    }
}

//--------------------------- ROBOT ---------------------------
int robot(int sw,int arg1,int arg2){
    printf("case no: %d", sw);
    switch (sw){
        case 0:
            test_update_pos();
            break;
        case 1:
            pthread_t myUpdate_position;
            pthread_create(&myUpdate_position,NULL,Update_position2,NULL);
            forward_sonar(50, 50, 50, 3000, 20);
            detect_type(50);
            pthread_mutex_lock(&mutex);
    	    ThreadSituation = 1;
            pthread_mutex_unlock(&mutex);
            /* fin SC2 */
    	    pthread_join(myUpdate_position,NULL);
            pthread_mutex_destroy(&mutex);
        case 2:
            test_cs();
            break;
        case 3:
            test_turn(arg1);
            break;
        case 4:
            test_Update_position2();
            create_map();
            int x=get_X_position();
            int y=get_Y_position();
            printf("\nX,Y = %d,%d\n",x,y);
            break;
        case 5:
	    	forward_sonar(50, 50, 50, 10000, 20);
	    	break;
	    case 6:
	    	printf("testing basic opening/closing shovel functions\n");
	    	open_shovel();
	    	close_shovel();
	    	break;
	    case 7:
	    	printf("testing take and drop object\n");
	    	take_object();
	    	printf("just took object, about to drop it in 5sec\n");
	    	sleep(5);
	    	drop_object();
	    	break;
    
    }
	
}

/*
    0   test sur l'update des positions
    1   test sur l'update des positions en cm en continu
    2   test sur la com client/serveur
    3   test virage
    4   test l'update des positions sur un parcours en L
    5   affiche la valeur des capteurs toutes les 2 secs
*/

