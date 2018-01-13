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
    switch (sw){
        case 0:
            test_update_pos();
            break;
        case 1:
            test_Update_position2();
	    create_map();
            break;
        case 2:
            test_cs();
            break;
        case 3:
            test_turn(arg1);
            break;
        case 4:
            test_Update_position2();
	    create_map();
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

