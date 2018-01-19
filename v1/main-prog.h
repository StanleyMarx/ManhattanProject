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
    detect_nonmovable(60);
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
    for (uint16_t x=0; x<32; x++){
        for (uint16_t y=0; y<32; y++){
            printf("sending (%d,%d)...\n",x,y);
            send_mapdata(x,y,8*x,0,8*y);
        }
    }
    printf("sending mapdone...\n");
    send_mapdone();
    printf("all done!\n");
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
//--------------------------- CASE 6 ----------------
int test_explore_mountain(int x0, int y0) {
	set_sensor_mode(sn_gyro, "GYRO-G&A");
	set_sensor_mode(sn_gyro, "GYRO-ANG");
	T = get_gyro();
	pthread_t update_pos;
    pthread_create(&update_pos,NULL,update_pos_entry,NULL);
    pthread_t send_pos;
    pthread_create(&send_pos,NULL,send_pos_entry,NULL);
    move_forever(35, 20);
    sleep(3);
    move_forever(0,0);
    
	explore_mountain();

	create_map(x0, y0);
    // stop threads
    printf("stopping threads...\n");
    UPDATE_POS_ENABLE=0;
    SEND_POS_ENABLE=0;
    printf("joining threads...\n");
    pthread_join(update_pos,NULL);
    pthread_join(send_pos,NULL);
    printf("- done -");
    return 0;
}
// ---------------------- CASE 7 ----------------------
int test_go_around_map(int x0, int y0){
      pthread_t myUpdate_position;
      pthread_create(&myUpdate_position,NULL,Update_position2,NULL);
      
      go_around_map(x0, y0);
      
      pthread_mutex_lock(&mutex);
      ThreadSituation = 1;
      pthread_mutex_unlock(&mutex);
      
      pthread_join(myUpdate_position,NULL);
      pthread_mutex_destroy(&mutex);
      return 0;
}
// ---------------------- CASE 10 ---------------------
int go_random(int x0, int y0) {
	/*
		by JB
		Robot is supposed to go forward in random directions while avoiding obstacles, x axis and y axis
	*/
	set_sensor_mode(sn_gyro, "GYRO-G&A");
	set_sensor_mode(sn_gyro, "GYRO-ANG");
	T = get_gyro();
	pthread_t update_pos;
    pthread_create(&update_pos,NULL,update_pos_entry,NULL);
    pthread_t send_pos;
    pthread_create(&send_pos,NULL,send_pos_entry,NULL);
    /*
    move_forever(35, 20);
    sleep(1);
    move_forever(20, 35);
    sleep(1);
    move_forever(0,0);
    */
    //printf("x = %d, y = %d\n");
    int side;
    int i;
    for (i=0; i<3; i++) {
    	forward_sonar_jb();
    	side = rand() % 2;
		if (side==0) {
			turn_right();
		} else {
			turn_left();
		}
    }
    create_map(x0, y0);
    // stop threads
    printf("stopping threads...\n");
    UPDATE_POS_ENABLE=0;
    SEND_POS_ENABLE=0;
    printf("joining threads...\n");
    pthread_join(update_pos,NULL);
    pthread_join(send_pos,NULL);
    printf("- done -");
    return 0;
}
//----------------------- CASE_8 -----------------------
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
    printf("stopping threads...\n");
    UPDATE_POS_ENABLE=0;
    SEND_POS_ENABLE=0;
    printf("joining threads...\n");
    pthread_join(update_pos,NULL);
    pthread_join(send_pos,NULL);
    printf("sending map...\n");
    send_map_from_file();
    printf("- done -");
}
//----------------------- CASE 100 ---------------------
int strategy1(int arg1, int arg2, int arg3){
    // usage
    if (arg1==0 && arg2==0 && arg3==0){
        printf("usage:\n\targ1 = x pixel coordinate at the beginning\n\targ2 = y pixel coordinate at the beginning\n\targ3 = 1000 * the exploration treshold. arg3=900 -> the exploration will stop after 90%% of the map having been explored\n");
        return 1;
    }
    
    // params and variables
    float treshold_explo=(float)arg3/1000;  // exploration stops when at least 0% of the map is explored
    int nb_scan=32;                         // during the scan, measure what's in front every 360/16 degrees
    int nb_point=X_MAP_MAX+Y_MAP_MAX;       // will consider that there is at best nb_points pixels between two random pixels
    int i,j,x,y;
    float d,angle,dx_pointed,dy_pointed;
    
    printf("--- STRATEGY 1 ---\n");
    
    printf("creating threads...\n");
    UPDATE_POS_ENABLE=1;
    SEND_POS_ENABLE=1;
    pthread_t update_pos;
    pthread_create(&update_pos,NULL,update_pos_entry,NULL);
    pthread_t send_pos;
    pthread_create(&send_pos,NULL,send_pos_entry,NULL);
    
    printf("initiating the position...\n");
    X=SQUARE_SIZE*arg1;
    Y=SQUARE_SIZE*arg2;
    T0=get_gyro();
    T=get_gyro();
    T0_COMPASS=get_compass();
    
    printf("starting the exploration!\nwill finish when %f%% of the map will have been explored.\n",treshold_explo*100);
    int map[Y_MAP_MAX][X_MAP_MAX];
    for (y=0; y<Y_MAP_MAX; y++){
        for (x=0; x<X_MAP_MAX; x++){
            map[y][x]=0;
        }
    }
    map[map_y()][map_x()]=1;
    while (matrix_completion(map)<treshold_explo){
        
        // scan
        printf("-map explored at %f%%-\ncurrent coordinates: (%d,%d,%f)\nscanning...\n",100*matrix_completion(map),map_x(),map_y(),fmod(T-T0,360));
        for (i=0; i<nb_scan; i++){
            angle=(T-T0)/57.29577951308232;
            usleep(100000);
            d=get_sonar()/50; // sonar value is in mm, so d is in pixel. Might eplace get_sonar() by get_sonar_map() for the invisible wall stuff.
            printf("object detected, d=%f\n",d);
            dx_pointed=cos(angle)*d;
            dy_pointed=sin(angle)*d;
            
            // updates the pixels from its position to the stuff that is being pointed by the sonar 
            for (j=1; j<=nb_point; j++){
                y=round(map_y()+dy_pointed*j/nb_point);
                x=round(map_x()+dx_pointed*j/nb_point);
                if (x>=0 && y>=0 && x<X_MAP_MAX && y<Y_MAP_MAX){
                    // (x,y) is in the arena
                    if (j==nb_point){
                        if (map[y][x]==0){
                            // (x,y) is the object that made the sonar's ultrasound stop
                            map[y][x]=2;
                        }
                    } else {
                        if (map[y][x]==0){
                            // (x,y) is empty
                            map[y][x]=1;
                        }
                    }
                }
            }
            turn_gyro(360/nb_scan);
        }
        printf("scan finished, map updated:\n");
        matrix_print(map);
        
        // choses the next point to explore and goes there
        chose_next_point(map,&x,&y);
        printf("moving to (%d,%d)...\n",x,y);
        go_to_map(x,y);
        printf("moved. current position: (%d,%d)\n",map_x(),map_y());
    }
    printf("finished the exploration!\nmap explored at %f%%\nmap:\n",100*matrix_completion(map));
    matrix_print(map);
    
    
    printf("stopping and joining the threads...\n");
    UPDATE_POS_ENABLE=0;
    SEND_POS_ENABLE=0;
    pthread_join(update_pos,NULL);
    pthread_join(send_pos,NULL);
    printf("sending map...\n");
    send_map_from_var(map);
    
    printf("--- EXITING : SUCCESS ---\n ");
    return 0;
}
/*
content of variable map
    0 = unexplored
    1 = empty
    2 = unmovable
    3 = movable
*/

//------------------------ ROBOT ------------------------
int robot(int sw,int arg1,int arg2, int arg3){
    printf("case no: %d\n", sw);
    switch (sw){
        case 0:
            test_update_pos();
            break;
        case 1:
         //  testDetectType();
	//	
		    //forward_sonar_timed(50, 50, 50.0, 2, 20);
		    testDetectType();
		        break;
        case 2:
            printf("test the CS communication. Sends a few coordinates and a red/blue gradient map.\n");
            test_cs();
            break;
        case 3:
            test_turn(arg1);
            break;
        case 4:
            /*
        	set_initial_coordinates(arg1, arg2);
            test_Update_position2();
            create_map(arg1, arg2);
            int x=get_X_position();
            int y=get_Y_position();
            printf("\nX,Y = %d,%d\n",x,y);
            break;*/
        case 5:
	    	forward_sonar(50);
	    	break;
	    case 6:
	    	printf("TEST JB - explore mountain\n");
	    	//set_initial_coordinates(arg1, arg2);
	    	test_explore_mountain(arg1, arg2);
	    	//create_map(arg1, arg2);
	    	printf("END OF TEST 6 JB\n");
	    	break;
	    case 7:
			printf("TEST JB - changing initial coordinates\n");
			set_initial_coordinates(arg1, arg2);
			test_go_around_map(arg1, arg2);
	    	printf("END OF TEST 7 JB\n");
	    	break;
        case 8:
            printf("moving for a while and sending the position to the server, then sending the map\n");
            almost_the_real_stuff();
            break;
        case 9:
            printf("sending map from pos.txt...\n");
            send_map_from_file();
            break;
        case 10:
        	printf("TEST JB - random going forward and turning, avoiding obstacles, x axis and y axis\n");
        	set_initial_coordinates(arg1, arg2);
        	go_random(arg1, arg2);
        	//create_map(arg1, arg2);
        	printf("END OF TEST 10 JB\n");
        	break;
        case 11:
            printf("displays the sensor values every 2 seconds\n");
            while (1){
                printf("sonar:   %f\n",get_sonar());
                printf("gyro:    %f\n",get_gyro());
                printf("compass: %f\n",get_compass());
                printf("color:   %d\n",get_color());
                sleep(2);
            }
            break;
        case 12:
            printf("test the turn_gyro and turn_gyro_abs functions\n");
            
            T0=get_gyro();
            printf("current angle: %f\n",get_gyro());
            turn_gyro(90);
            printf("made a 90° turn, now at %f\n",get_gyro());
            turn_gyro_abs(0);
            printf("went back to T0=%f, now at %f\n",T0,get_gyro());
            
            break;
        case 13: 
        	printf("[CASE 13] about to send map from pos.txt\n");
        	send_map_jb();
        	break;
        
            
        // STRATEGY 1
        case 100:
            printf("implements the strategy n°1\n");
            strategy1(arg1,arg2,arg3);
            break;
        case 101:
            printf("goes to (arg1,arg2) in a precision of arg3\n");
            UPDATE_POS_ENABLE=1;
            pthread_t update_pos;
            pthread_create(&update_pos,NULL,update_pos_entry,NULL);

            T0=get_gyro();
            T=get_gyro();
            X=0;
            Y=0;
            
            printf("(X,Y,T)=(%f,%f,%f)\n",X,Y,T);
            go_to(arg1,arg2,arg3);
            printf("(X,Y,T)=(%f,%f,%f)\n",X,Y,T);
            
            UPDATE_POS_ENABLE=0;
            pthread_join(update_pos,NULL);
            break;
        case 102:
            printf("tests a simple send map\n");
            
            for (int y=0; y<32; y++){
                for (int x=0; x<32; x++){
                    send_mapdata(x,y,8*x,0,8*y);
                    printf("sent %d %d %d 0 %d",x,y,8*x,8*y);
                }
            }
            send_mapdone();
            printf("sent mapdone\n");
    }
        
}























