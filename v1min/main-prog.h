//----------------------- CASE 100 ---------------------
int strategy1(int arg1, int arg2, int arg3){
    // usage
    if (arg1==0 && arg2==0 && arg3==0){
        printf("usage:\n\targ1 = x pixel coordinate at the beginning\n\targ2 = y pixel coordinate at the beginning\n\targ3 = 1000 * the exploration treshold. arg3=900 -> the exploration will stop after 90%% of the map having been explored\n");
        return 1;
    }
    
    // params and variables
    float treshold_explo=(float)arg3/1000;  // exploration stops when at least 0% of the map is explored
    int nb_scan=16;                         // during the scan, measure what's in front every 360/16 degrees
    int nb_point=X_MAP_MAX+Y_MAP_MAX;       // will consider that there is at best nb_points pixels between two random pixels
    long int time0=time(NULL);
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
    while (matrix_completion(map)<treshold_explo && (time(NULL)-time0 < 210)){
        
        // scan
        printf("-map explored at %f%%-\ncurrent coordinates: (%d,%d,%f)\nscanning...\n",100*matrix_completion(map),map_x(),map_y(),fmod(T-T0,360));
        for (i=0; i<nb_scan; i++){
            angle=(T-T0)/57.29577951308232;
            get_sonar();
            get_sonar();
            d=get_sonar()/50; // sonar value is in mm, so d is in pixel. Might eplace get_sonar() by get_sonar_map() for the invisible wall stuff.
            printf("object detected, d=%f\n",d);
            dx_pointed=cos(angle)*d;
            dy_pointed=sin(angle)*d;
            
            // updates the pixels from its position to the stuff that is being pointed by the sonar 
            for (j=nb_point; j>=0; j--){
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
            sleep(1);
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

//------------------------ ROBOT ------------------------
int robot(int sw,int arg1,int arg2, int arg3){
    printf("case no: %d\n", sw);
    switch (sw){
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























