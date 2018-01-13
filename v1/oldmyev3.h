void newforwardSonar(float sonarThreshold, int speed) {
    float sonarVal = get_sonar();
    if (sonarVal > sonarThreshold+10) {
        move_forever(speed, speed);
        while (sonarVal > sonarThreshold) {
            sonarVal = get_sonar();
        }
        move_forever(0, 0);
    }
}


void newbackwardSonar(float sonarThreshold) {
    float sonarVal = get_sonar();
    if (sonarVal < sonarThreshold-10) {
        move_forever(200, 200);
        while (sonarVal < sonarThreshold) {
            sonarVal = get_sonar();
        }
        move_forever(0, 0);
    }
}


void newwhereIsMyMind(int angle) {
    turn_approx(angle);
    float sonarMin = get_sonar();
    float gyroMin = get_gyro();
    float sonarVal = get_sonar();
    float gyroValInitial, gyroVal;
    gyroValInitial = get_gyro();
    gyroVal = gyroValInitial;
    move_forever(-30, 30);
    while (abs(gyroVal - gyroValInitial) < abs(2*angle)+10) {
        gyroVal = get_gyro();
        sonarVal = get_sonar();
        if (sonarMin > sonarVal) {
            sonarMin = sonarVal;
            gyroMin = gyroVal;
        }
    }
    move_forever(0, 0);
    sleep(0.5);
    turn_approx(0.95*(gyroVal-gyroMin));
    gyroVal = get_gyro();
    sonarVal = get_sonar();
}


void newforwardTimed(int seconds, int speed) {
    move_forever(speed, speed);
    sleep(seconds);
    move_forever(0, 0);
}


void newtake_object() {
    newforwardSonar(80.0, 100);
    printf("[PELLE] opening pelle\n");//--------open pelle
    set_tacho_speed_sp(sn_pelle, -80);
    set_tacho_command(sn_pelle, "run-forever");
    sleep(2);
    //set_tacho_command(sn_pelle, "stop");
    newforwardTimed(2, 100);//---------moveforward
    printf("[PELLE] closing pelle\n");//-------close pelle
    set_tacho_command(sn_pelle, "stop");
    set_tacho_speed_sp(sn_pelle, 80);
    set_tacho_command(sn_pelle, "run-forever");
    sleep(2);
    set_tacho_command(sn_pelle, "stop");
}

void newdrop_object() {
    turn_approx(180);//-------half turn
    newforwardTimed(2, 80);//---------moveforward
    printf("[PELLE] opening pelle\n");//----------open pelle
    set_tacho_speed_sp(sn_pelle, -80);
    set_tacho_command(sn_pelle, "run-forever");
    sleep(2);
    //set_tacho_command(sn_pelle, "stop");
    newforwardTimed(2, -80);//---------movebackward
    Tturn_approx(-180);//-------half turn
    printf("[PELLE] closing pelle\n");//----------close pelle
    set_tacho_command(sn_pelle, "stop");
    set_tacho_speed_sp(sn_pelle, 80);
    set_tacho_command(sn_pelle, "run-forever");
    sleep(2);
    set_tacho_command(sn_pelle, "stop");
}

void newisThisABall(float delta) {
    newforwardSonar(50.0, 100);
    turn_approx(delta);
    sleep(0.5);
    float sonarValG = get_sonar();
    turn_approx(-delta);
    sleep(0.5);
    turn_approx(-delta);
    sleep(0.5);
    float sonarValD = get_sonar();
    turn_approx(delta);
    if ((sonarValG > 150) && (sonarValD > 150)){
        printf("movable object\n");
        newtake_object();
        newdrop_object();
    }else {
        printf("UNmovable object\n");
        turn_approx(-90);
    }
}


void newkeepmoving(float sonarThreshold, int speed) {
    int i=0;
    while (i<6) {
        newforwardSonar(sonarThreshold, speed);
        newwhereIsMyMind(20);
        newisThisABall(25);
        float sonarVal = get_sonar();
        while (sonarVal<100.0) {
            newbackwardSonar();
            turn_approx(-90);
            sonarVal = get_sonar();
        }
        sleep(1);
        i+=1;
    }
}
