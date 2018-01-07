int robot(){
    // just test some sensors and tachos
    
    test_sensors_verbose();
    test_tachos_verbose();

    move_forever(50,50);
    sleep(1);
    move_forever(50,-50);
    sleep(1);
    move_forever(-50,50);
    sleep(1);
    move_forever(-50,-50);
    sleep(1);
    move_forever(0,0);
    sleep(1);

    move_real(210,-210,max_speed/2);
    move_real(-210,210,max_speed/2);
    move_real(800,800,max_speed/2);
    move_real(-800,-800,max_speed/2);

    open_shovel();
    close_shovel();
}
