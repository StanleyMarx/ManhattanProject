//----------------------- CASE_0 -----------------------
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
//----------------------- CASE_1 -----------------------
//dans myev3.h
int testDetectType(){
      pthread_t myUpdate_position;
      pthread_create(&myUpdate_position,NULL,Update_position2,NULL);
      forward_sonar(50);
   //   detect_type(50);
      pthread_mutex_lock(&mutex);
      ThreadSituation = 1;
      pthread_mutex_unlock(&mutex);
            /* fin SC2 */
      pthread_join(myUpdate_position,NULL);
      pthread_mutex_destroy(&mutex);
}
//----------------------- CASE_2 -----------------------
void test_cs(){
    send_position(1,2);
    send_position(-1,-2);
    send_obstacle(1,2,3);
    send_obstacle(0,4,5);
    send_position_pos(1,2);
}
//----------------------- CASE_3 -----------------------
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
//----------------------- CASE_4 -----------------------
//dans myeve3.h
//----------------------- CASE_5 -----------------------
void debug_sensors(){
    while (1){
        printf("compass %f\n",get_compass());
        printf("gyro    %f\n",get_gyro());
        printf("color   %d\n\n",get_color());
        sleep(2);
    }
}
//----------------------- CASE_8 -----------------------
float X=0, Y=0, T=0;
char UPDATE_POS_ENABLE=1;
char SEND_POS_ENABLE=1;
float SQUARE_SIZE=1; // size of a 5cm square in the units of X. TO CALIBRATE

void* update_pos_entry(){
    // updates the global variable X and Y given the input of the motors
    
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
            X+=(dr+dl)*cos(T/57.29577951308232);
            Y+=(dr+dl)*sin(T/57.29577951308232);
        } else {
            // turning
            T=get_gyro();
        }
        x_towrite=(int)(X/10);
        y_towrite=(int)(Y/10);
        if (x_towrite!=x_lastwritten || y_towrite!=y_lastwritten){
            file_pos=fopen("pos.txt","a");
            fprintf(file_pos,"%d,%d\n",x_towrite,y_towrite);
            x_lastwritten=x_towrite;
            y_lastwritten=y_towrite;            
        }
        
        right_pos_prev=right_pos;
        left_pos_prev=left_pos;
    }
}
void* send_pos_entry(){
    while(SEND_POS_ENABLE){
        send_position_pos(X,Y);
        sleep(2);
    }
}
void almost_the_real_stuff(){
    pthread_t update_pos;
    pthread_create(&update_pos,NULL,update_pos_entry,NULL);
    pthread_t send_pos;
    pthread_create(&send_pos,NULL,send_pos_entry,NULL);
    
    // does some movements
    printf("X,Y,T = %f,%f,%f\n(advancing)\n",X,Y,T);
    move_real_debug(1000,1000);
    printf("X,Y,T = %f,%f,%f\n(turning)\n",X,Y,T);
    turn_approx(90);
    printf("X,Y,T = %f,%f,%f\n(advancing)\n",X,Y,T);
    move_real_debug(1000,1000);
    printf("X,Y,T = %f,%f,%f\n(turning)\n",X,Y,T);
    turn_approx(90);
    printf("X,Y,T = %f,%f,%f\n(advancing)\n",X,Y,T);
    move_real_debug(1000,1000);
    printf("X,Y,T = %f,%f,%f\n(turning)\n",X,Y,T);
    turn_approx(90);
    printf("X,Y,T = %f,%f,%f\n(advancing)\n",X,Y,T);
    move_real_debug(1000,1000);
    printf("X,Y,T = %f,%f,%f\n(turning)\n",X,Y,T);
    turn_approx(90);
    printf("X,Y,T = %f,%f,%f\n",X,Y,T);
    
    // stop threads
    UPDATE_POS_ENABLE=0;
    SEND_POS_ENABLE=0;
    pthread_join(update_pos,NULL);
    pthread_join(send_pos,NULL);
}

//------------------------ ROBOT ------------------------
int robot(int sw,int arg1,int arg2){
    printf("case no: %d\n", sw);
    switch (sw){
        case 0:
            test_update_pos();
            break;
        case 1:
           // testDetectType();
		   
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
	    	forward_sonar(50);
	    	break;
	    case 6:
	    	printf("Going around map - test JB\n");
	    	go_around_map();
	    	printf("END OF TEST 6 JB\n");
	    	//close_shovel();
	    	break;
	    case 7:
		forward_sonar(50);
	    	printf("testing take and drop object\n");
	    	take_object();
	    	printf("just took object, about to drop it in 5sec\n");
	    	sleep(5);
	    	drop_object();
	    	break;
        case 8:
            /* à tester */
            printf("moving for a while and sending the position to the server, then sending the map\n");
            almost_the_real_stuff();
            break;
    
    }
}
